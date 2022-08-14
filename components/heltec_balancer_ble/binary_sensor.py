import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_HELTEC_BALANCER_BLE_ID, HeltecBalancerBle

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_ERROR_CHARGING = "error_charging"
CONF_ERROR_DISCHARGING = "error_discharging"
CONF_ERROR_SYSTEM_OVERHEATING = "error_system_overheating"

BINARY_SENSORS = [
    CONF_BALANCING,
    CONF_ERROR_CHARGING,
    CONF_ERROR_DISCHARGING,
    CONF_ERROR_SYSTEM_OVERHEATING,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HELTEC_BALANCER_BLE_ID): cv.use_id(HeltecBalancerBle),
        cv.Optional(CONF_BALANCING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default="mdi:battery-heart-variant"): cv.icon,
            }
        ),
        cv.Optional(CONF_ERROR_CHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default="mdi:alert-circle-outline"): cv.icon,
            }
        ),
        cv.Optional(CONF_ERROR_DISCHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default="mdi:alert-circle-outline"): cv.icon,
            }
        ),
        cv.Optional(
            CONF_ERROR_SYSTEM_OVERHEATING
        ): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default="mdi:alert-circle-outline"): cv.icon,
            }
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
