from esphome import pins
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_FLOW_CONTROL_PIN, CONF_ID
from esphome.cpp_helpers import gpio_pin_expression

DEPENDENCIES = ["uart"]
MULTI_CONF = True

jk_modbus_ns = cg.esphome_ns.namespace("jk_modbus")
JkModbus = jk_modbus_ns.class_("JkModbus", cg.Component, uart.UARTDevice)
JkModbusDevice = jk_modbus_ns.class_("JkModbusDevice")

CONF_JK_MODBUS_ID = "jk_modbus_id"
CONF_RX_TIMEOUT = "rx_timeout"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkModbus),
            cv.Optional(
                CONF_RX_TIMEOUT, default="50ms"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    cg.add_global(jk_modbus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))


def jk_modbus_device_schema(default_address):
    schema = {
        cv.GenerateID(CONF_JK_MODBUS_ID): cv.use_id(JkModbus),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t
    return cv.Schema(schema)


async def register_jk_modbus_device(var, config):
    parent = await cg.get_variable(config[CONF_JK_MODBUS_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))
