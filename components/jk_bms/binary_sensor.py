import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_JK_BMS_ID, JkBms

DEPENDENCIES = ["jk_bms"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_CHARGING_SWITCH = "charging_switch"
CONF_DISCHARGING = "discharging"
CONF_DISCHARGING_SWITCH = "discharging_switch"
CONF_BALANCING = "balancing"
CONF_BALANCING_SWITCH = "balancing_switch"
CONF_DEDICATED_CHARGER_SWITCH = "dedicated_charger_switch"

ICON_CHARGING = "mdi:battery-charging"
ICON_CHARGING_SWITCH = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_DISCHARGING_SWITCH = "mdi:power-plug"
ICON_BALANCING = "mdi:battery-heart-variant"
ICON_BALANCING_SWITCH = "mdi:battery-heart-variant"
ICON_DEDICATED_CHARGER_SWITCH = "mdi:battery-charging"

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_CHARGING_SWITCH,
    CONF_DISCHARGING,
    CONF_DISCHARGING_SWITCH,
    CONF_BALANCING,
    CONF_BALANCING_SWITCH,
    CONF_DEDICATED_CHARGER_SWITCH,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_JK_BMS_ID): cv.use_id(JkBms),
        cv.Optional(CONF_CHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_CHARGING): cv.icon,
            }
        ),
        cv.Optional(CONF_CHARGING_SWITCH): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_CHARGING_SWITCH): cv.icon,
            }
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_DISCHARGING): cv.icon,
            }
        ),
        cv.Optional(CONF_DISCHARGING_SWITCH): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_DISCHARGING_SWITCH): cv.icon,
            }
        ),
        cv.Optional(CONF_BALANCING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_BALANCING): cv.icon,
            }
        ),
        cv.Optional(CONF_BALANCING_SWITCH): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_BALANCING_SWITCH): cv.icon,
            }
        ),
        cv.Optional(
            CONF_DEDICATED_CHARGER_SWITCH
        ): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_DEDICATED_CHARGER_SWITCH): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
