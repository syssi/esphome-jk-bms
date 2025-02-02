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

jk_balancer_ns = cg.esphome_ns.namespace("jk_balancer")
JkBalancer = jk_balancer_ns.class_(
    "JkBalancer", cg.PollingComponent, jk_balancer_modbus.JkBalancerModbusDevice
)

JK_BALANCER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BALANCER_ID): cv.use_id(JkBalancer),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBalancer),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(jk_balancer_modbus.jk_balancer_modbus_device_schema(0x01))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await jk_balancer_modbus.register_jk_balancer_modbus_device(var, config)
