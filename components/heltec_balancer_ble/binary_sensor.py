import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_HELTEC_BALANCER_BLE_ID, HELTEC_BALANCER_BLE_COMPONENT_SCHEMA

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_ERROR_CHARGING = "error_charging"
CONF_ERROR_DISCHARGING = "error_discharging"
CONF_ERROR_SYSTEM_OVERHEATING = "error_system_overheating"
CONF_ONLINE_STATUS = "online_status"

BINARY_SENSOR_DEFS = {
    CONF_BALANCING: {"icon": "mdi:battery-heart-variant"},
    CONF_ERROR_CHARGING: {"icon": "mdi:alert-circle-outline"},
    CONF_ERROR_DISCHARGING: {"icon": "mdi:alert-circle-outline"},
    CONF_ERROR_SYSTEM_OVERHEATING: {"icon": "mdi:alert-circle-outline"},
    CONF_ONLINE_STATUS: {
        "device_class": DEVICE_CLASS_CONNECTIVITY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
