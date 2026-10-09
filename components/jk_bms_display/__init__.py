import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]
DOMAIN = "jk_bms_display"

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["binary_sensor", "sensor"]
MULTI_CONF = True

CONF_JK_BMS_DISPLAY_ID = "jk_bms_display_id"

jk_bms_display_ns = cg.esphome_ns.namespace("jk_bms_display")
JkBmsDisplay = jk_bms_display_ns.class_("JkBmsDisplay", cg.Component, uart.UARTDevice)

CONF_JK_BMS_DISPLAY_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_DISPLAY_ID): cv.use_id(JkBmsDisplay),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2024, 12, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsDisplay),
        }
    ).extend(uart.UART_DEVICE_SCHEMA),
)

JK_BMS_DISPLAY_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_DISPLAY_ID): cv.use_id(JkBmsDisplay),
    }
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    DOMAIN,
    baud_rate=2400,
    data_bits=8,
    parity="NONE",
    stop_bits=1,
    require_rx=True,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
