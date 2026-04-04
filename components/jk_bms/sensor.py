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
    ENTITY_CATEGORY_DIAGNOSTIC,
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

from . import CONF_JK_BMS_ID, JK_BMS_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_bms"]

CODEOWNERS = ["@syssi"]

CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_MIN_VOLTAGE_CELL = "min_voltage_cell"
CONF_MAX_VOLTAGE_CELL = "max_voltage_cell"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"

CONF_POWER_TUBE_TEMPERATURE = "power_tube_temperature"
CONF_TEMPERATURE_SENSOR_1 = "temperature_sensor_1"
CONF_TEMPERATURE_SENSOR_2 = "temperature_sensor_2"
CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_CHARGING_POWER = "charging_power"
CONF_DISCHARGING_POWER = "discharging_power"
CONF_CAPACITY_REMAINING = "capacity_remaining"
CONF_CAPACITY_REMAINING_DERIVED = "capacity_remaining_derived"
CONF_TEMPERATURE_SENSORS = "temperature_sensors"
CONF_CHARGING_CYCLES = "charging_cycles"
CONF_TOTAL_CHARGING_CYCLE_CAPACITY = "total_charging_cycle_capacity"
CONF_BATTERY_STRINGS = "battery_strings"

CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_OPERATION_MODE_BITMASK = "operation_mode_bitmask"

CONF_TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION = "total_voltage_overvoltage_protection"
CONF_TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "total_voltage_undervoltage_protection"

CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION = "cell_voltage_overvoltage_protection"
CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY = "cell_voltage_overvoltage_recovery"
CONF_CELL_VOLTAGE_OVERVOLTAGE_DELAY = "cell_voltage_overvoltage_delay"

CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "cell_voltage_undervoltage_protection"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY = "cell_voltage_undervoltage_recovery"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_DELAY = "cell_voltage_undervoltage_delay"

CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION = "cell_pressure_difference_protection"

CONF_DISCHARGING_OVERCURRENT_PROTECTION = "discharging_overcurrent_protection"
CONF_DISCHARGING_OVERCURRENT_DELAY = "discharging_overcurrent_delay"

CONF_CHARGING_OVERCURRENT_PROTECTION = "charging_overcurrent_protection"
CONF_CHARGING_OVERCURRENT_DELAY = "charging_overcurrent_delay"

CONF_BALANCE_STARTING_VOLTAGE = "balance_starting_voltage"
CONF_BALANCE_OPENING_PRESSURE_DIFFERENCE = "balance_opening_pressure_difference"

CONF_POWER_TUBE_TEMPERATURE_PROTECTION = "power_tube_temperature_protection"
CONF_POWER_TUBE_TEMPERATURE_RECOVERY = "power_tube_temperature_recovery"

CONF_TEMPERATURE_SENSOR_TEMPERATURE_PROTECTION = (
    "temperature_sensor_temperature_protection"
)
CONF_TEMPERATURE_SENSOR_TEMPERATURE_RECOVERY = "temperature_sensor_temperature_recovery"
CONF_TEMPERATURE_SENSOR_TEMPERATURE_DIFFERENCE_PROTECTION = (
    "temperature_sensor_temperature_difference_protection"
)

CONF_CHARGING_HIGH_TEMPERATURE_PROTECTION = "charging_high_temperature_protection"
CONF_DISCHARGING_HIGH_TEMPERATURE_PROTECTION = "discharging_high_temperature_protection"

CONF_CHARGING_LOW_TEMPERATURE_PROTECTION = "charging_low_temperature_protection"
CONF_CHARGING_LOW_TEMPERATURE_RECOVERY = "charging_low_temperature_recovery"
CONF_DISCHARGING_LOW_TEMPERATURE_PROTECTION = "discharging_low_temperature_protection"
CONF_DISCHARGING_LOW_TEMPERATURE_RECOVERY = "discharging_low_temperature_recovery"

# r/w
# CONF_BATTERY_STRINGS = "battery_strings"
CONF_TOTAL_BATTERY_CAPACITY_SETTING = "total_battery_capacity_setting"

CONF_CURRENT_CALIBRATION = "current_calibration"
CONF_DEVICE_ADDRESS = "device_address"
CONF_SLEEP_WAIT_TIME = "sleep_wait_time"
CONF_ALARM_LOW_VOLUME = "alarm_low_volume"
CONF_MANUFACTURING_DATE = "manufacturing_date"
CONF_TOTAL_RUNTIME = "total_runtime"
CONF_START_CURRENT_CALIBRATION = "start_current_calibration"
CONF_ACTUAL_BATTERY_CAPACITY = "actual_battery_capacity"
CONF_PROTOCOL_VERSION = "protocol_version"

ICON_CURRENT_DC = "mdi:current-dc"
ICON_MIN_VOLTAGE_CELL = "mdi:battery-minus-outline"
ICON_MAX_VOLTAGE_CELL = "mdi:battery-plus-outline"

ICON_BATTERY_STRINGS = "mdi:car-battery"
ICON_CAPACITY_REMAINING_DERIVED = "mdi:battery-50"
ICON_ACTUAL_BATTERY_CAPACITY = "mdi:battery-50"
ICON_TOTAL_BATTERY_CAPACITY_SETTING = "mdi:battery-sync"

ICON_DEVICE_ADDRESS = "mdi:identifier"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"
ICON_OPERATION_MODE_BITMASK = "mdi:heart-pulse"
ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ALARM_LOW_VOLUME = "mdi:volume-high"

UNIT_SECONDS = "s"
UNIT_HOURS = "h"
UNIT_AMPERE_HOURS = "Ah"

CELLS = [f"cell_voltage_{i}" for i in range(1, 25)]

_CELL_VOLTAGE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_VOLT,
    icon=ICON_EMPTY,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_VOLTAGE,
    state_class=STATE_CLASS_MEASUREMENT,
)

SENSOR_DEFS = {
    CONF_MIN_CELL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MAX_CELL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MIN_VOLTAGE_CELL: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_MIN_VOLTAGE_CELL,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MAX_VOLTAGE_CELL: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_MAX_VOLTAGE_CELL,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DELTA_CELL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AVERAGE_CELL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_POWER_TUBE_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_1: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_2: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TOTAL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CAPACITY_REMAINING: {
        "unit_of_measurement": UNIT_PERCENT,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_BATTERY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CAPACITY_REMAINING_DERIVED: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_CAPACITY_REMAINING_DERIVED,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSORS: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_CYCLES: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_CHARGING_CYCLES,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_TOTAL_CHARGING_CYCLE_CAPACITY: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_COUNTER,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_BATTERY_STRINGS: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_BATTERY_STRINGS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_ERRORS_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_ERRORS_BITMASK,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_OPERATION_MODE_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_OPERATION_MODE_BITMASK,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_OVERVOLTAGE_DELAY: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_DELAY: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_OVERCURRENT_PROTECTION: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_OVERCURRENT_DELAY: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_OVERCURRENT_PROTECTION: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_OVERCURRENT_DELAY: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BALANCE_STARTING_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BALANCE_OPENING_PRESSURE_DIFFERENCE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_POWER_TUBE_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_POWER_TUBE_TEMPERATURE_RECOVERY: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_RECOVERY: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_DIFFERENCE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_HIGH_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_HIGH_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_LOW_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGING_LOW_TEMPERATURE_RECOVERY: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_LOW_TEMPERATURE_PROTECTION: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DISCHARGING_LOW_TEMPERATURE_RECOVERY: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TOTAL_BATTERY_CAPACITY_SETTING: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_TOTAL_BATTERY_CAPACITY_SETTING,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CURRENT_CALIBRATION: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DEVICE_ADDRESS: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_DEVICE_ADDRESS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_SLEEP_WAIT_TIME: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_ALARM_LOW_VOLUME: {
        "unit_of_measurement": UNIT_PERCENT,
        "icon": ICON_ALARM_LOW_VOLUME,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MANUFACTURING_DATE: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TOTAL_RUNTIME: {
        "unit_of_measurement": UNIT_HOURS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_START_CURRENT_CALIBRATION: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_ACTUAL_BATTERY_CAPACITY: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_ACTUAL_BATTERY_CAPACITY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PROTOCOL_VERSION: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
}

CONFIG_SCHEMA = JK_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
).extend({cv.Optional(key): _CELL_VOLTAGE_SCHEMA for key in CELLS})


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_ID])
    for i, key in enumerate(CELLS):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(hub.set_cell_voltage_sensor(i, sens))
    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
