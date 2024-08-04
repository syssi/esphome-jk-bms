import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome import pins
from esphome.core import CORE

DEPENDENCIES = ["uart"]
MULTI_CONF = True
CONF_JK_RS485_SNIFFER_ID = "jk_rs485_sniffer_id"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_PROTOCOL_VERSION = "protocol_version"
CONF_TALK_PIN = "talk_pin"
CONF_BROADCAST_TO_ALL_BMS = "broadcast_to_all_bms"

jk_rs485_sniffer_ns = cg.esphome_ns.namespace("jk_rs485_sniffer")

try:
    JkRS485Sniffer
    JkRS485SnifferDevice
except NameError:
    JkRS485Sniffer = jk_rs485_sniffer_ns.class_("JkRS485Sniffer", cg.Component, uart.UARTDevice)
    JkRS485SnifferDevice = jk_rs485_sniffer_ns.class_("JkRS485SnifferDevice")


ProtocolVersion = jk_rs485_sniffer_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "JK04": ProtocolVersion.PROTOCOL_VERSION_JK04,
    "JK02_24S": ProtocolVersion.PROTOCOL_VERSION_JK02_24S,
    "JK02_32S": ProtocolVersion.PROTOCOL_VERSION_JK02_32S,
}

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkRS485Sniffer),
            cv.Required(CONF_PROTOCOL_VERSION): cv.enum(
                PROTOCOL_VERSION_OPTIONS, upper=True
            ),         
            cv.Required(CONF_TALK_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_RX_TIMEOUT, default="50ms"
            ): cv.positive_time_period_milliseconds,            
            cv.Required(CONF_BROADCAST_TO_ALL_BMS): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

JK_RS485_SNIFFER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_RS485_SNIFFER_ID): cv.use_id(JkRS485Sniffer),
    }
)


async def to_code(config):
    cg.add_global(jk_rs485_sniffer_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))

    talk_pin = await cg.gpio_pin_expression(config[CONF_TALK_PIN])
    cg.add(var.set_talk_pin(talk_pin))
    cg.add(var.set_broadcast_to_all_bms(config[CONF_BROADCAST_TO_ALL_BMS]))    

def jk_rs485_sniffer_device_schema():
    schema = {
        cv.GenerateID(CONF_JK_RS485_SNIFFER_ID): cv.use_id(JkRS485Sniffer),
    }
    return cv.Schema(schema)


async def register_jk_rs485_bms_device(var, config):
    parent = await cg.get_variable(config[CONF_JK_RS485_SNIFFER_ID])
    cg.add(var.set_parent(parent))
##    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(parent.register_device(var))
