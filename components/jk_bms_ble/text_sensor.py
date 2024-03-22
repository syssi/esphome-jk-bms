import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, ICON_TIMELAPSE

from . import CONF_JK_BMS_BLE_ID, JkBmsBle

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
CONF_OPERATION_STATUS = "operation_status"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"
CONF_INFO_VENDORID = "info_vendorid"
CONF_INFO_HARDWARE_VERSION = "info_hardware_version";
CONF_INFO_SOFTWARE_VERSION = "info_software_version";
CONF_INFO_DEVICE_NAME = "info_device_name";
CONF_INFO_DEVICE_PASSWORD = "info_device_password";



ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_STATUS = "mdi:heart-pulse"
ICON_INFORMATION = "mdi:information"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_OPERATION_STATUS,
    CONF_TOTAL_RUNTIME_FORMATTED,
    CONF_INFO_VENDORID,
    CONF_INFO_HARDWARE_VERSION,
    CONF_INFO_SOFTWARE_VERSION,
    CONF_INFO_DEVICE_NAME,
    CONF_INFO_DEVICE_PASSWORD,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_BLE_ID): cv.use_id(JkBmsBle),
        cv.Optional(CONF_ERRORS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_ERRORS): cv.icon,
            }
        ),
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_STATUS): cv.icon,
            }
        ),
        cv.Optional(
            CONF_TOTAL_RUNTIME_FORMATTED
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_TIMELAPSE): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_VENDORID
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_HARDWARE_VERSION
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_SOFTWARE_VERSION
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_DEVICE_NAME
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),
        cv.Optional(
            CONF_INFO_DEVICE_PASSWORD
        ): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_INFORMATION): cv.icon,
            }
        ),


        
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
