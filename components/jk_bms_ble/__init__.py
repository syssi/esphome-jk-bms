import logging

import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@syssi", "@txubelaxu"]
DEPENDENCIES = ["ble_client"]
AUTO_LOAD = [
    "binary_sensor",
    "button",
    "number",
    "select",
    "sensor",
    "switch",
    "text_sensor",
]
MULTI_CONF = True


def deprecated_renames(renames: dict[str, str]):
    def validator(config):
        config = config.copy()
        for old, new in renames.items():
            if old in config:
                _LOGGER.warning(
                    "'%s' is deprecated, use '%s' instead. Will be removed in a future release.",
                    old,
                    new,
                )
                config[new] = config.pop(old)
        return config

    return validator


CONF_JK_BMS_BLE_ID = "jk_bms_ble_id"
CONF_PROTOCOL_VERSION = "protocol_version"
CONF_ERROR_OVERRIDES = "error_overrides"

DEFAULT_ERRORS_JK02 = [
    "Wire resistance",  # bit 0
    "MOSFET overtemperature",  # bit 1
    "Cell count is not equal to settings",  # bit 2
    "",  # bit 3 (Previously: "Current sensor anomaly")
    "Battery is fully charged",  # bit 4
    "Battery pack overvoltage",  # bit 5
    "Charge overcurrent",  # bit 6
    "Charge short circuit",  # bit 7
    "Charge overtemperature",  # bit 8
    "Charge undertemperature",  # bit 9
    "Coprocessor communication error",  # bit 10
    "Cell undervoltage",  # bit 11
    "Battery pack undervoltage",  # bit 12
    "Discharge overcurrent",  # bit 13
    "Discharge short circuit",  # bit 14
    "Discharge overtemperature",  # bit 15
    "Charging MOSFET abnormal",  # bit 16
    "Discharging MOSFET abnormal",  # bit 17
    "GPS disconnected",  # bit 18
    "Modify password in time",  # bit 19
    "Discharge on failed",  # bit 20
    "Battery overtemperature",  # bit 21
    "Temperature sensor anomaly",  # bit 22
    "PL module anomaly",  # bit 23
    "SCP release failed",  # bit 24
    "Discharge OCP II",  # bit 25
    "Discharge OCP III",  # bit 26
    "Discharge undertemperature alarm",  # bit 27
    "GPS remote lock",  # bit 28
    "",  # bit 29
    "",  # bit 30
    "",  # bit 31
]

# Maps a bit position to the label replacing the DEFAULT_ERRORS_JK02 entry. An empty
# label suppresses the bit, so it shows up in the raw bitmask sensor only.
ERROR_OVERRIDES_SCHEMA = cv.Schema(
    {cv.int_range(0, len(DEFAULT_ERRORS_JK02) - 1): cv.string_strict}
)


def apply_error_overrides(overrides):
    """Return the JK02 error labels with the given {bit: label} overrides applied."""
    errors = DEFAULT_ERRORS_JK02.copy()
    for bit, label in overrides.items():
        errors[bit] = label
    return errors


jk_bms_ble_ns = cg.esphome_ns.namespace("jk_bms_ble")
JkBmsBle = jk_bms_ble_ns.class_(
    "JkBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

ProtocolVersion = jk_bms_ble_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "JK04": ProtocolVersion.PROTOCOL_VERSION_JK04,
    "JK02_24S": ProtocolVersion.PROTOCOL_VERSION_JK02_24S,
    "JK02_32S": ProtocolVersion.PROTOCOL_VERSION_JK02_32S,
}

JK_BMS_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_BLE_ID): cv.use_id(JkBmsBle),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2025, 7, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsBle),
            cv.Required(CONF_PROTOCOL_VERSION): cv.enum(
                PROTOCOL_VERSION_OPTIONS, upper=True
            ),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ERROR_OVERRIDES): ERROR_OVERRIDES_SCHEMA,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))

    errors_jk02 = apply_error_overrides(config.get(CONF_ERROR_OVERRIDES, {}))

    arr_name = f"{config[CONF_ID]}_ERRORS_JK02"
    entries = ", ".join(str(cg.safe_exp(label)) for label in errors_jk02)
    cg.add_global(
        cg.RawStatement(
            f"static constexpr const char *const {arr_name}[] = {{{entries}}};"
        )
    )
    cg.add(var.set_errors_jk02_table(cg.RawExpression(arr_name), len(errors_jk02)))
