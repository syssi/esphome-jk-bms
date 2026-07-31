import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

CODEOWNERS = ["@syssi"]
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

CONF_HELTEC_BALANCER_BLE_ID = "heltec_balancer_ble_id"
CONF_ERROR_OVERRIDES = "error_overrides"

# Aggregated error conditions, one bit each. The order must match collect_error_bits()
# in heltec_balancer_ble.cpp.
# fmt: off
DEFAULT_ERRORS = (
    "Battery detection failed",   # bit 0
    "Overvoltage",                # bit 1
    "Undervoltage",               # bit 2
    "Polarity error",             # bit 3
    "Excessive line resistance",  # bit 4
    "System overheating",         # bit 5
    "Charging fault",             # bit 6
    "Discharge fault",            # bit 7
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


heltec_balancer_ble_ns = cg.esphome_ns.namespace("heltec_balancer_ble")
HeltecBalancerBle = heltec_balancer_ble_ns.class_(
    "HeltecBalancerBle", ble_client.BLEClientNode, cg.PollingComponent
)

HELTEC_BALANCER_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HELTEC_BALANCER_BLE_ID): cv.use_id(HeltecBalancerBle),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2025, 11, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HeltecBalancerBle),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ERROR_OVERRIDES): error_overrides,
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
