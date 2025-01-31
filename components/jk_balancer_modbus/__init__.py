import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_ID

DEPENDENCIES = ["uart"]
MULTI_CONF = True

jk_balancer_modbus_ns = cg.esphome_ns.namespace("jk_balancer_modbus")
JkBalancerModbus = jk_balancer_modbus_ns.class_(
    "JkBalancerModbus", cg.Component, uart.UARTDevice
)
JkBalancerModbusDevice = jk_balancer_modbus_ns.class_("JkBalancerModbusDevice")

CONF_JK_BALANCER_MODBUS_ID = "jk_balancer_modbus_id"
CONF_RX_TIMEOUT = "rx_timeout"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBalancerModbus),
            cv.Optional(
                CONF_RX_TIMEOUT, default="100ms"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    cg.add_global(jk_balancer_modbus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))


def jk_balancer_modbus_device_schema(default_address):
    schema = {
        cv.GenerateID(CONF_JK_BALANCER_MODBUS_ID): cv.use_id(JkBalancerModbus),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t
    return cv.Schema(schema)


async def register_jk_balancer_modbus_device(var, config):
    parent = await cg.get_variable(config[CONF_JK_BALANCER_MODBUS_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))
