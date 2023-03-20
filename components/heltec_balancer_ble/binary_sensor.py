import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_HELTEC_BALANCER_BLE_ID, HeltecBalancerBle

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_ERROR_CHARGING = "error_charging"
CONF_ERROR_DISCHARGING = "error_discharging"
CONF_ERROR_SYSTEM_OVERHEATING = "error_system_overheating"
CONF_ONLINE_STATUS = "online_status"

BINARY_SENSORS = [
    CONF_BALANCING,
    CONF_ERROR_CHARGING,
    CONF_ERROR_DISCHARGING,
    CONF_ERROR_SYSTEM_OVERHEATING,
    CONF_ONLINE_STATUS,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HELTEC_BALANCER_BLE_ID): cv.use_id(HeltecBalancerBle),
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-heart-variant"
        ),
        cv.Optional(CONF_ERROR_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle-outline"
        ),
        cv.Optional(CONF_ERROR_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle-outline"
        ),
        cv.Optional(CONF_ERROR_SYSTEM_OVERHEATING): binary_sensor.binary_sensor_schema(
            icon="mdi:alert-circle-outline"
        ),
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
