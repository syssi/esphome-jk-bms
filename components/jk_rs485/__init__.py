import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ADDRESS, CONF_ID

CODEOWNERS = ["@syssi"]
AUTO_LOAD = ["binary_sensor", "button", "number", "sensor", "switch", "text_sensor"]

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_PROTOCOL_VERSION = "protocol_version"
CONF_JK_RS485_ID = "jk_rs485_id"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_ADDRESS = "address"

jk_rs485_ns = cg.esphome_ns.namespace("jk_rs485")
JkRS485 = jk_rs485_ns.class_("JkRS485", cg.Component, uart.UARTDevice)
JkRS485Device = jk_rs485_ns.class_("JkRS485Device")

ProtocolVersion = jk_rs485_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "JK04": ProtocolVersion.PROTOCOL_VERSION_JK04,
    "JK02_24S": ProtocolVersion.PROTOCOL_VERSION_JK02_24S,
    "JK02_32S": ProtocolVersion.PROTOCOL_VERSION_JK02_32S,
}

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkRS485),
            cv.Optional(
                CONF_RX_TIMEOUT, default="50ms"
            ): cv.positive_time_period_milliseconds,
            cv.Required(CONF_PROTOCOL_VERSION): cv.enum(
                PROTOCOL_VERSION_OPTIONS, upper=True
            ),           
            cv.Required(CONF_ADDRESS): cv.All(cv.uint8_t, cv.Range(min=0,max=15)),        
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    cg.add_global(jk_rs485_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
#    cg.add(var.set.address(config[CONF_ADDRESS]))
#    cg.add(var.set.protocol_version(config[CONF_PROTOCOL_VERSION]))


def jk_rs485_device_schema(default_address):
    schema = {
        cv.GenerateID(CONF_JK_RS485_ID): cv.use_id(JkRS485),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t
    return cv.Schema(schema)


async def register_jk_rs485_device(var, config):
    parent = await cg.get_variable(config[CONF_JK_RS485_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))    
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))    
    cg.add(parent.register_device(var))
