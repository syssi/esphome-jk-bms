import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_CURRENT,
    CONF_POWER,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_COUNTER,
    ICON_EMPTY,
    ICON_TIMELAPSE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_WATT,
)

from . import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA
from .const import CONF_BALANCING

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_MIN_VOLTAGE_CELL = "min_voltage_cell"
CONF_MAX_VOLTAGE_CELL = "max_voltage_cell"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"

CONF_CELL_VOLTAGE_1 = "cell_voltage_1"
CONF_CELL_VOLTAGE_2 = "cell_voltage_2"
CONF_CELL_VOLTAGE_3 = "cell_voltage_3"
CONF_CELL_VOLTAGE_4 = "cell_voltage_4"
CONF_CELL_VOLTAGE_5 = "cell_voltage_5"
CONF_CELL_VOLTAGE_6 = "cell_voltage_6"
CONF_CELL_VOLTAGE_7 = "cell_voltage_7"
CONF_CELL_VOLTAGE_8 = "cell_voltage_8"
CONF_CELL_VOLTAGE_9 = "cell_voltage_9"
CONF_CELL_VOLTAGE_10 = "cell_voltage_10"
CONF_CELL_VOLTAGE_11 = "cell_voltage_11"
CONF_CELL_VOLTAGE_12 = "cell_voltage_12"
CONF_CELL_VOLTAGE_13 = "cell_voltage_13"
CONF_CELL_VOLTAGE_14 = "cell_voltage_14"
CONF_CELL_VOLTAGE_15 = "cell_voltage_15"
CONF_CELL_VOLTAGE_16 = "cell_voltage_16"
CONF_CELL_VOLTAGE_17 = "cell_voltage_17"
CONF_CELL_VOLTAGE_18 = "cell_voltage_18"
CONF_CELL_VOLTAGE_19 = "cell_voltage_19"
CONF_CELL_VOLTAGE_20 = "cell_voltage_20"
CONF_CELL_VOLTAGE_21 = "cell_voltage_21"
CONF_CELL_VOLTAGE_22 = "cell_voltage_22"
CONF_CELL_VOLTAGE_23 = "cell_voltage_23"
CONF_CELL_VOLTAGE_24 = "cell_voltage_24"

CONF_CELL_RESISTANCE_1 = "cell_resistance_1"
CONF_CELL_RESISTANCE_2 = "cell_resistance_2"
CONF_CELL_RESISTANCE_3 = "cell_resistance_3"
CONF_CELL_RESISTANCE_4 = "cell_resistance_4"
CONF_CELL_RESISTANCE_5 = "cell_resistance_5"
CONF_CELL_RESISTANCE_6 = "cell_resistance_6"
CONF_CELL_RESISTANCE_7 = "cell_resistance_7"
CONF_CELL_RESISTANCE_8 = "cell_resistance_8"
CONF_CELL_RESISTANCE_9 = "cell_resistance_9"
CONF_CELL_RESISTANCE_10 = "cell_resistance_10"
CONF_CELL_RESISTANCE_11 = "cell_resistance_11"
CONF_CELL_RESISTANCE_12 = "cell_resistance_12"
CONF_CELL_RESISTANCE_13 = "cell_resistance_13"
CONF_CELL_RESISTANCE_14 = "cell_resistance_14"
CONF_CELL_RESISTANCE_15 = "cell_resistance_15"
CONF_CELL_RESISTANCE_16 = "cell_resistance_16"
CONF_CELL_RESISTANCE_17 = "cell_resistance_17"
CONF_CELL_RESISTANCE_18 = "cell_resistance_18"
CONF_CELL_RESISTANCE_19 = "cell_resistance_19"
CONF_CELL_RESISTANCE_20 = "cell_resistance_20"
CONF_CELL_RESISTANCE_21 = "cell_resistance_21"
CONF_CELL_RESISTANCE_22 = "cell_resistance_22"
CONF_CELL_RESISTANCE_23 = "cell_resistance_23"
CONF_CELL_RESISTANCE_24 = "cell_resistance_24"

CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_CHARGING_POWER = "charging_power"
CONF_DISCHARGING_POWER = "discharging_power"
CONF_TEMPERATURE_SENSOR_1 = "temperature_sensor_1"
CONF_TEMPERATURE_SENSOR_2 = "temperature_sensor_2"
CONF_TEMPERATURE_SENSOR_3 = "temperature_sensor_3"
CONF_TEMPERATURE_SENSOR_4 = "temperature_sensor_4"
CONF_TEMPERATURE_SENSOR_5 = "temperature_sensor_5"
CONF_POWER_TUBE_TEMPERATURE = "power_tube_temperature"
CONF_STATE_OF_CHARGE = "state_of_charge"
CONF_CAPACITY_REMAINING = "capacity_remaining"
CONF_TOTAL_BATTERY_CAPACITY_SETTING = "total_battery_capacity_setting"
CONF_CHARGING_CYCLES = "charging_cycles"
CONF_TOTAL_CHARGING_CYCLE_CAPACITY = "total_charging_cycle_capacity"
CONF_TOTAL_RUNTIME = "total_runtime"
CONF_BALANCING_CURRENT = "balancing_current"
CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_EMERGENCY_TIME_COUNTDOWN = "emergency_time_countdown"
CONF_HEATING_CURRENT = "heating_current"


UNIT_AMPERE_HOURS = "Ah"
UNIT_OHM = "Ω"
UNIT_SECONDS = "s"

ICON_CURRENT_DC = "mdi:current-dc"
ICON_CAPACITY = "mdi:battery-medium"
ICON_MIN_VOLTAGE_CELL = "mdi:battery-minus-outline"
ICON_MAX_VOLTAGE_CELL = "mdi:battery-plus-outline"
ICON_CAPACITY_REMAINING = "mdi:battery-50"
ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"
ICON_CELL_RESISTANCE = "mdi:omega"
ICON_BALANCER = "mdi:seesaw"

CELL_VOLTAGES = [
    CONF_CELL_VOLTAGE_1,
    CONF_CELL_VOLTAGE_2,
    CONF_CELL_VOLTAGE_3,
    CONF_CELL_VOLTAGE_4,
    CONF_CELL_VOLTAGE_5,
    CONF_CELL_VOLTAGE_6,
    CONF_CELL_VOLTAGE_7,
    CONF_CELL_VOLTAGE_8,
    CONF_CELL_VOLTAGE_9,
    CONF_CELL_VOLTAGE_10,
    CONF_CELL_VOLTAGE_11,
    CONF_CELL_VOLTAGE_12,
    CONF_CELL_VOLTAGE_13,
    CONF_CELL_VOLTAGE_14,
    CONF_CELL_VOLTAGE_15,
    CONF_CELL_VOLTAGE_16,
    CONF_CELL_VOLTAGE_17,
    CONF_CELL_VOLTAGE_18,
    CONF_CELL_VOLTAGE_19,
    CONF_CELL_VOLTAGE_20,
    CONF_CELL_VOLTAGE_21,
    CONF_CELL_VOLTAGE_22,
    CONF_CELL_VOLTAGE_23,
    CONF_CELL_VOLTAGE_24,
]

CELL_RESISTANCES = [
    CONF_CELL_RESISTANCE_1,
    CONF_CELL_RESISTANCE_2,
    CONF_CELL_RESISTANCE_3,
    CONF_CELL_RESISTANCE_4,
    CONF_CELL_RESISTANCE_5,
    CONF_CELL_RESISTANCE_6,
    CONF_CELL_RESISTANCE_7,
    CONF_CELL_RESISTANCE_8,
    CONF_CELL_RESISTANCE_9,
    CONF_CELL_RESISTANCE_10,
    CONF_CELL_RESISTANCE_11,
    CONF_CELL_RESISTANCE_12,
    CONF_CELL_RESISTANCE_13,
    CONF_CELL_RESISTANCE_14,
    CONF_CELL_RESISTANCE_15,
    CONF_CELL_RESISTANCE_16,
    CONF_CELL_RESISTANCE_17,
    CONF_CELL_RESISTANCE_18,
    CONF_CELL_RESISTANCE_19,
    CONF_CELL_RESISTANCE_20,
    CONF_CELL_RESISTANCE_21,
    CONF_CELL_RESISTANCE_22,
    CONF_CELL_RESISTANCE_23,
    CONF_CELL_RESISTANCE_24,
]

TEMPERATURES = [
    CONF_TEMPERATURE_SENSOR_1,
    CONF_TEMPERATURE_SENSOR_2,
    CONF_TEMPERATURE_SENSOR_3,
    CONF_TEMPERATURE_SENSOR_4,
    CONF_TEMPERATURE_SENSOR_5,
]

SENSORS = [
    CONF_BALANCING,
    CONF_MIN_CELL_VOLTAGE,
    CONF_MAX_CELL_VOLTAGE,
    CONF_MIN_VOLTAGE_CELL,
    CONF_MAX_VOLTAGE_CELL,
    CONF_DELTA_CELL_VOLTAGE,
    CONF_AVERAGE_CELL_VOLTAGE,
    CONF_TOTAL_VOLTAGE,
    CONF_CURRENT,
    CONF_POWER,
    CONF_CHARGING_POWER,
    CONF_DISCHARGING_POWER,
    CONF_POWER_TUBE_TEMPERATURE,
    CONF_STATE_OF_CHARGE,
    CONF_CAPACITY_REMAINING,
    CONF_TOTAL_BATTERY_CAPACITY_SETTING,
    CONF_CHARGING_CYCLES,
    CONF_TOTAL_CHARGING_CYCLE_CAPACITY,
    CONF_TOTAL_RUNTIME,
    CONF_BALANCING_CURRENT,
    CONF_ERRORS_BITMASK,
    CONF_EMERGENCY_TIME_COUNTDOWN,
    CONF_HEATING_CURRENT,
]

# pylint: disable=too-many-function-args
CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCING): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_BALANCER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MIN_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MIN_VOLTAGE_CELL): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_MIN_VOLTAGE_CELL,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_VOLTAGE_CELL): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_MAX_VOLTAGE_CELL,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DELTA_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AVERAGE_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_1): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_2): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_3): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_4): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_5): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_6): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_7): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_8): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_9): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_10): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_11): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_12): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_13): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_14): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_15): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_16): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_17): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_18): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_19): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_20): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_21): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_22): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_23): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_24): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_1): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_2): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_3): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_4): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_5): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_6): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_7): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_8): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_9): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_10): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_11): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_12): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_13): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_14): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_15): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_16): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_17): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_18): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_19): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_20): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_21): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_22): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_23): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_24): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_1): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_2): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_3): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_4): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_5): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER_TUBE_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CAPACITY_REMAINING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_CAPACITY_REMAINING,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_BATTERY_CAPACITY_SETTING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_CYCLES): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CHARGING_CYCLES,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_CHARGING_CYCLE_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_COUNTER,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_RUNTIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
        cv.Optional(CONF_BALANCING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ERRORS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_ERRORS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_EMERGENCY_TIME_COUNTDOWN): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
        ),
        cv.Optional(CONF_HEATING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for i, key in enumerate(CELL_VOLTAGES):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(hub.set_cell_voltage_sensor(i, sens))
    for i, key in enumerate(CELL_RESISTANCES):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(hub.set_cell_resistance_sensor(i, sens))
    for i, key in enumerate(TEMPERATURES):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(hub.set_temperature_sensor(i, sens))
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
