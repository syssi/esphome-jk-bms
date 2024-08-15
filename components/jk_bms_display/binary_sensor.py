import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_JK_BMS_DISPLAY_ID, JK_BMS_DISPLAY_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_bms_display"]

CODEOWNERS = ["@syssi"]

CONF_SYSTEM_WARNING = "system_warning"
CONF_BALANCING_SWITCH = "balancing_switch"
CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "cell_voltage_undervoltage_protection"
CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION = "cell_voltage_overvoltage_protection"
CONF_OVERCURRENT_PROTECTION = "overcurrent_protection"
CONF_MOSFET_OVERTEMPERATURE_PROTECTION = "mosfet_overtemperature_protection"
CONF_BATTERY_TEMPERATURE_PROTECTION = "battery_temperature_protection"
CONF_SHORT_CIRCUIT_PROTECTION = "short_circuit_protection"
CONF_COPROCESSOR_COMMUNICATION_ERROR = "coprocessor_communication_error"
CONF_BALANCER_WIRE_RESISTANCE_TOO_HIGH = "balancer_wire_resistance_too_high"
CONF_CELL_COUNT_MISMATCH = "cell_count_mismatch"

ICON_CHARGING = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_BALANCING = "mdi:battery-heart-variant"

BINARY_SENSORS = [
    CONF_SYSTEM_WARNING,
    CONF_BALANCING_SWITCH,
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION,
    CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION,
    CONF_OVERCURRENT_PROTECTION,
    CONF_MOSFET_OVERTEMPERATURE_PROTECTION,
    CONF_BATTERY_TEMPERATURE_PROTECTION,
    CONF_SHORT_CIRCUIT_PROTECTION,
    CONF_COPROCESSOR_COMMUNICATION_ERROR,
    CONF_BALANCER_WIRE_RESISTANCE_TOO_HIGH,
    CONF_CELL_COUNT_MISMATCH,
]

CONFIG_SCHEMA = JK_BMS_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SYSTEM_WARNING): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_BALANCING_SWITCH): binary_sensor.binary_sensor_schema(
            icon=ICON_BALANCING
        ),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon=ICON_CHARGING
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon=ICON_DISCHARGING
        ),
        cv.Optional(
            CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_OVERCURRENT_PROTECTION): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_MOSFET_OVERTEMPERATURE_PROTECTION
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_BATTERY_TEMPERATURE_PROTECTION
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_SHORT_CIRCUIT_PROTECTION
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_COPROCESSOR_COMMUNICATION_ERROR
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(
            CONF_BALANCER_WIRE_RESISTANCE_TOO_HIGH
        ): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_CELL_COUNT_MISMATCH): binary_sensor.binary_sensor_schema(),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_DISPLAY_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
