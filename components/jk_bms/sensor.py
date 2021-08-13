import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_EMPTY,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
)
from . import JkBms, CONF_JK_BMS_ID

DEPENDENCIES = ["jk_bms"]

CODEOWNERS = ["@syssi"]

CONF_FIRMWARE_VERSION = "firmware_version"

SENSORS = [
    CONF_FIRMWARE_VERSION,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_ID): cv.use_id(JkBms),
        cv.Optional(CONF_FIRMWARE_VERSION): sensor.sensor_schema(
            UNIT_EMPTY, ICON_EMPTY, 0, DEVICE_CLASS_EMPTY, STATE_CLASS_MEASUREMENT
        ),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_JK_BMS_ID])
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = yield sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
