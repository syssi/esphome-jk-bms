import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, CONF_PASSWORD, ICON_EMPTY, ICON_TIMELAPSE

from . import CONF_JK_BMS_ID, JkBms

DEPENDENCIES = ["jk_bms"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_TYPE = "battery_type"
CONF_ERRORS = "errors"
CONF_OPERATION_MODE = "operation_mode"
CONF_DEVICE_TYPE = "device_type"
CONF_SOFTWARE_VERSION = "software_version"
CONF_MANUFACTURER = "manufacturer"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"

ICON_BATTERY_TYPE = "mdi:car-battery"
ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_MODE = "mdi:heart-pulse"
ICON_PASSWORD = "mdi:lock-outline"

TEXT_SENSORS = [
    CONF_OPERATION_MODE,
    CONF_ERRORS,
    CONF_BATTERY_TYPE,
    CONF_PASSWORD,
    CONF_DEVICE_TYPE,
    CONF_SOFTWARE_VERSION,
    CONF_MANUFACTURER,
    CONF_TOTAL_RUNTIME_FORMATTED,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_ID): cv.use_id(JkBms),
        cv.Optional(CONF_OPERATION_MODE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_MODE): cv.icon,
            }
        ),
        cv.Optional(CONF_ERRORS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_ERRORS): cv.icon,
            }
        ),
        cv.Optional(CONF_BATTERY_TYPE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_BATTERY_TYPE): cv.icon,
            }
        ),
        cv.Optional(CONF_PASSWORD): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_PASSWORD): cv.icon,
            }
        ),
        cv.Optional(CONF_DEVICE_TYPE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_SOFTWARE_VERSION): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
            }
        ),
        cv.Optional(CONF_MANUFACTURER): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
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
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
