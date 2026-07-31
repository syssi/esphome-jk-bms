import esphome.codegen as cg
from esphome.components import jk_balancer_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = [
    "jk_balancer_modbus",
    "binary_sensor",
    "number",
    "sensor",
    "switch",
    "text_sensor",
]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_JK_BALANCER_ID = "jk_balancer_id"
CONF_ERROR_OVERRIDES = "error_overrides"

# fmt: off
DEFAULT_ERRORS = (
    "Wrong cell count",     # bit 0
    "Resistance too high",  # bit 1
    "Overvoltage",          # bit 2
    "",                     # bit 3 (reserved)
    "",                     # bit 4 (reserved)
    "",                     # bit 5 (reserved)
    "",                     # bit 6 (reserved)
    "",                     # bit 7 (reserved)
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


jk_balancer_ns = cg.esphome_ns.namespace("jk_balancer")
JkBalancer = jk_balancer_ns.class_(
    "JkBalancer", cg.PollingComponent, jk_balancer_modbus.JkBalancerModbusDevice
)

JK_BALANCER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BALANCER_ID): cv.use_id(JkBalancer),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2024, 12, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBalancer),
            cv.Optional(CONF_ERROR_OVERRIDES): error_overrides,
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(jk_balancer_modbus.jk_balancer_modbus_device_schema(0x01)),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await jk_balancer_modbus.register_jk_balancer_modbus_device(var, config)

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
