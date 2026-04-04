import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_JK_BALANCER_ID, JK_BALANCER_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_balancer"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_ONLINE_STATUS = "online_status"

ICON_BALANCING = "mdi:battery-heart-variant"

BINARY_SENSOR_DEFS = {
    CONF_BALANCING: {"icon": ICON_BALANCING},
    CONF_ONLINE_STATUS: {
        "device_class": DEVICE_CLASS_CONNECTIVITY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = JK_BALANCER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BALANCER_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
