import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

CODEOWNERS = ["@syssi"]
DEPENDENCIES = ["ble_client"]
AUTO_LOAD = [
    "binary_sensor",
    "sensor",
]
MULTI_CONF = True

CONF_JK_BMS_OLD_BLE_ID = "jk_bms_old_ble_id"

jk_bms_old_ble_ns = cg.esphome_ns.namespace("jk_bms_old_ble")
JkBmsOldBle = jk_bms_old_ble_ns.class_(
    "JkBmsOldBle", ble_client.BLEClientNode, cg.PollingComponent
)

JK_BMS_OLD_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_OLD_BLE_ID): cv.use_id(JkBmsOldBle),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2025, 7, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsOldBle),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
