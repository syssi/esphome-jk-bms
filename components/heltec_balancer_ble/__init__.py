import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

CODEOWNERS = ["@syssi"]

AUTO_LOAD = ["binary_sensor", "button", "number", "sensor", "switch", "text_sensor"]
MULTI_CONF = True

CONF_HELTEC_BALANCER_BLE_ID = "heltec_balancer_ble_id"

heltec_balancer_ble_ns = cg.esphome_ns.namespace("heltec_balancer_ble")
HeltecBalancerBle = heltec_balancer_ble_ns.class_(
    "HeltecBalancerBle", ble_client.BLEClientNode, cg.PollingComponent
)

HELTEC_BALANCER_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HELTEC_BALANCER_BLE_ID): cv.use_id(HeltecBalancerBle),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HeltecBalancerBle),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
