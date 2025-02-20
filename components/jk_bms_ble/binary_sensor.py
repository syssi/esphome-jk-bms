import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

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

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_BALANCING,
    CONF_ONLINE_STATUS,
    CONF_HEATING,
    CONF_DRY_CONTACT_1,
    CONF_DRY_CONTACT_2,
]

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon=ICON_CHARGING
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon=ICON_DISCHARGING
        ),
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            icon=ICON_BALANCING
        ),
        cv.Optional(CONF_HEATING): binary_sensor.binary_sensor_schema(
            icon=ICON_HEATING
        ),
        cv.Optional(CONF_DRY_CONTACT_1): binary_sensor.binary_sensor_schema(
            icon=ICON_DRY_CONTACT_1
        ),
        cv.Optional(CONF_DRY_CONTACT_2): binary_sensor.binary_sensor_schema(
            icon=ICON_DRY_CONTACT_2
        ),
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
