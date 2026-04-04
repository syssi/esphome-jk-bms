import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA
from .const import CONF_BALANCING, CONF_CHARGING, CONF_DISCHARGING, CONF_HEATING

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_ONLINE_STATUS = "online_status"
CONF_DRY_CONTACT_1 = "dry_contact_1"
CONF_DRY_CONTACT_2 = "dry_contact_2"

ICON_CHARGING = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_BALANCING = "mdi:battery-heart-variant"
ICON_HEATING = "mdi:radiator"
ICON_DRY_CONTACT_1 = "mdi:alarm-bell"
ICON_DRY_CONTACT_2 = "mdi:alarm-bell"

BINARY_SENSOR_DEFS = {
    CONF_CHARGING: {"icon": ICON_CHARGING},
    CONF_DISCHARGING: {"icon": ICON_DISCHARGING},
    CONF_BALANCING: {"icon": ICON_BALANCING},
    CONF_HEATING: {"icon": ICON_HEATING},
    CONF_DRY_CONTACT_1: {"icon": ICON_DRY_CONTACT_1},
    CONF_DRY_CONTACT_2: {"icon": ICON_DRY_CONTACT_2},
    CONF_ONLINE_STATUS: {
        "device_class": DEVICE_CLASS_CONNECTIVITY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
