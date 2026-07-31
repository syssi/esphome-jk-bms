import logging

import esphome.codegen as cg
from esphome.components import jk_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

_LOGGER = logging.getLogger(__name__)

AUTO_LOAD = ["jk_modbus", "binary_sensor", "sensor", "switch", "text_sensor"]
CODEOWNERS = ["@syssi"]
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


CONF_JK_BMS_ID = "jk_bms_id"
CONF_ERROR_OVERRIDES = "error_overrides"

# fmt: off
DEFAULT_ERRORS = (
    "Low capacity",                 # bit 0, warning
    "MOSFET overtemperature",       # bit 1, alarm
    "Charge overvoltage",           # bit 2, alarm
    "Discharge undervoltage",       # bit 3, alarm
    "Battery overtemperature",      # bit 4, alarm
    "Charge overcurrent",           # bit 5, alarm
    "Discharge overcurrent",        # bit 6, alarm
    "Cell pressure difference",     # bit 7, alarm
    "Battery box overtemperature",  # bit 8, alarm
    "Battery undertemperature",     # bit 9, alarm
    "Cell overvoltage",             # bit 10, alarm
    "Cell undervoltage",            # bit 11, alarm
    "309_A protection",             # bit 12, alarm
    "309_A protection",             # bit 13, alarm
    "",                             # bit 14 (reserved)
    "",                             # bit 15 (reserved)
)
# fmt: on
MAX_ERROR_BIT = len(DEFAULT_ERRORS) - 1


def error_overrides(value):
    # A plain {cv.int_range(...): cv.string_strict} schema rejects an out of range
    # bit with voluptuous' "extra keys not allowed", which tells the user nothing.
    value = cv.Schema({cv.string: cv.string_strict})(value)

    overrides = {}
    for key, label in value.items():
        try:
            bit = cv.int_(key)
        except cv.Invalid:
            raise cv.Invalid(
                f"'{key}' is not a valid error bit, expected a number between 0 and {MAX_ERROR_BIT}",
                path=[key],
            ) from None
        if not 0 <= bit <= MAX_ERROR_BIT:
            raise cv.Invalid(
                f"Error bit {bit} is out of range, must be between 0 and {MAX_ERROR_BIT}",
                path=[key],
            )
        overrides[bit] = label

    return overrides


jk_bms_ns = cg.esphome_ns.namespace("jk_bms")
JkBms = jk_bms_ns.class_("JkBms", cg.PollingComponent, jk_modbus.JkModbusDevice)

JK_BMS_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_ID): cv.use_id(JkBms),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2025, 7, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBms),
            cv.Optional(CONF_ERROR_OVERRIDES): error_overrides,
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(jk_modbus.jk_modbus_device_schema(0x4E)),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await jk_modbus.register_jk_modbus_device(var, config)

    errors = list(DEFAULT_ERRORS)
    for bit, label in config.get(CONF_ERROR_OVERRIDES, {}).items():
        errors[bit] = label

    arr_name = f"{config[CONF_ID]}_ERRORS"
    entries = ", ".join(str(cg.safe_exp(label)) for label in errors)
    cg.add_global(
        cg.RawStatement(
            f"static constexpr const char *const {arr_name}[] = {{{entries}}};"
        )
    )
    cg.add(var.set_errors_table(cg.RawExpression(arr_name), len(errors)))
