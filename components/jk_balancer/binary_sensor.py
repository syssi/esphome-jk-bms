import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_JK_BALANCER_ID, JK_BALANCER_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_balancer"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_ONLINE_STATUS = "online_status"

ICON_BALANCING = "mdi:battery-heart-variant"

BINARY_SENSORS = [
    CONF_BALANCING,
    CONF_ONLINE_STATUS,
]

CONFIG_SCHEMA = JK_BALANCER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            icon=ICON_BALANCING
        ),
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BALANCER_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
