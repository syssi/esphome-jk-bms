import esphome.codegen as cg
from esphome.components import jk_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["jk_modbus", "binary_sensor", "sensor", "switch", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_JK_BMS_ID = "jk_bms_id"

jk_bms_ns = cg.esphome_ns.namespace("jk_bms")
JkBms = jk_bms_ns.class_("JkBms", cg.PollingComponent, jk_modbus.JkModbusDevice)

JK_BMS_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_ID): cv.use_id(JkBms),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBms),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(jk_modbus.jk_modbus_device_schema(0x4E))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await jk_modbus.register_jk_modbus_device(var, config)
