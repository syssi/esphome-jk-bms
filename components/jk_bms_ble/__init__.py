import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_JK_BMS_BLE_ID = "jk_bms_ble_id"
CONF_ENABLE_FAKE_TRAFFIC = "enable_fake_traffic"

jk_bms_ble_ns = cg.esphome_ns.namespace("jk_bms_ble")
JkBmsBle = jk_bms_ble_ns.class_(
    "JkBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsBle),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_ENABLE_FAKE_TRAFFIC, default=False): cv.boolean,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_enable_fake_traffic(config[CONF_ENABLE_FAKE_TRAFFIC]))
    cg.add(var.set_throttle(config[CONF_THROTTLE]))
