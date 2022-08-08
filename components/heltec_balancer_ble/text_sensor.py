import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, ICON_TIMELAPSE

from . import CONF_HELTEC_BALANCER_BLE_ID, HeltecBalancerBle

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
CONF_OPERATION_STATUS = "operation_status"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"
CONF_BUZZER_MODE = "buzzer_mode"
CONF_BATTERY_TYPE = "battery_type"

ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_STATUS = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_OPERATION_STATUS,
    CONF_TOTAL_RUNTIME_FORMATTED,
    CONF_BUZZER_MODE,
    CONF_BATTERY_TYPE,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_HELTEC_BALANCER_BLE_ID): cv.use_id(HeltecBalancerBle),
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
        cv.Optional(CONF_BUZZER_MODE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_STATUS): cv.icon,
            }
        ),
        cv.Optional(CONF_BATTERY_TYPE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_STATUS): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
