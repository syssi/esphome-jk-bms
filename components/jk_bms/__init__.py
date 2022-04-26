import esphome.codegen as cg
from esphome.components import jk_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["jk_modbus", "binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_JK_BMS_ID = "jk_bms_id"
CONF_ENABLE_FAKE_TRAFFIC = "enable_fake_traffic"

jk_bms_ns = cg.esphome_ns.namespace("jk_bms")
JkBms = jk_bms_ns.class_("JkBms", cg.PollingComponent, jk_modbus.JkModbusDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBms),
            cv.Optional(CONF_ENABLE_FAKE_TRAFFIC, default=False): cv.boolean,
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(jk_modbus.jk_modbus_device_schema(0x4E))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await jk_modbus.register_jk_modbus_device(var, config)

    cg.add(var.set_enable_fake_traffic(config[CONF_ENABLE_FAKE_TRAFFIC]))
