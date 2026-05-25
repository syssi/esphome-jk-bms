import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC, ICON_EMPTY, ICON_TIMELAPSE

from . import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, deprecated_renames

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_ERRORS = "errors"
CONF_ERRORS_BITMASK_HEX = "errors_bitmask_hex"
CONF_OPERATION_STATUS = "operation_status"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"
CONF_CHARGING_STATUS = "charging_status"
CONF_SOFTWARE_VERSION = "software_version"
CONF_HARDWARE_VERSION = "hardware_version"
CONF_BATTERY_TYPE = "battery_type"

ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_STATUS = "mdi:heart-pulse"
ICON_CHARGING_STATUS = "mdi:battery-clock"
ICON_BATTERY_TYPE = "mdi:battery-heart-variant"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_ERRORS_BITMASK_HEX,
    CONF_OPERATION_STATUS,
    CONF_TOTAL_RUNTIME_FORMATTED,
    CONF_CHARGING_STATUS,
    CONF_SOFTWARE_VERSION,
    CONF_HARDWARE_VERSION,
    CONF_BATTERY_TYPE,
]

_RENAMED_TEXT_SENSORS = {
    "charge_status": "charging_status",
}

CONFIG_SCHEMA = cv.All(
    deprecated_renames(_RENAMED_TEXT_SENSORS),
    JK_BMS_BLE_COMPONENT_SCHEMA.extend(
        {
            cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
                icon=ICON_ERRORS,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_ERRORS_BITMASK_HEX): text_sensor.text_sensor_schema(
                text_sensor.TextSensor, icon=ICON_ERRORS
            ),
            cv.Optional(CONF_OPERATION_STATUS): text_sensor.text_sensor_schema(
                icon=ICON_OPERATION_STATUS
            ),
            cv.Optional(CONF_TOTAL_RUNTIME_FORMATTED): text_sensor.text_sensor_schema(
                icon=ICON_TIMELAPSE
            ),
            cv.Optional(CONF_CHARGING_STATUS): text_sensor.text_sensor_schema(
                icon=ICON_CHARGING_STATUS
            ),
            cv.Optional(CONF_SOFTWARE_VERSION): text_sensor.text_sensor_schema(
                icon=ICON_EMPTY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(
                icon=ICON_EMPTY,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_BATTERY_TYPE): text_sensor.text_sensor_schema(
                icon=ICON_BATTERY_TYPE,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
        }
    ),
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
