import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

CODEOWNERS = ["@syssi", "@txubelaxu"]

AUTO_LOAD = ["binary_sensor", "button", "number", "sensor", "switch", "text_sensor"]
MULTI_CONF = True

CONF_JK_BMS_BLE_ID = "jk_bms_ble_id"
CONF_PROTOCOL_VERSION = "protocol_version"

jk_bms_ble_ns = cg.esphome_ns.namespace("jk_bms_ble")
JkBmsBle = jk_bms_ble_ns.class_(
    "JkBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

ProtocolVersion = jk_bms_ble_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "JK04": ProtocolVersion.PROTOCOL_VERSION_JK04,
    "JK02_24S": ProtocolVersion.PROTOCOL_VERSION_JK02_24S,
    "JK02_32S": ProtocolVersion.PROTOCOL_VERSION_JK02_32S,
}

JK_BMS_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_BLE_ID): cv.use_id(JkBmsBle),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsBle),
            cv.Required(CONF_PROTOCOL_VERSION): cv.enum(
                PROTOCOL_VERSION_OPTIONS, upper=True
            ),
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
    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
