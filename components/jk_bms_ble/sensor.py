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

from . import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA
from .const import CONF_BALANCING

CODEOWNERS = ["@syssi", "@txubelaxu"]

CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_MIN_VOLTAGE_CELL = "min_voltage_cell"
CONF_MAX_VOLTAGE_CELL = "max_voltage_cell"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"

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
CONF_STATE_OF_HEALTH = "state_of_health"
CONF_CAPACITY_REMAINING = "capacity_remaining"
CONF_TOTAL_BATTERY_CAPACITY_SETTING = "total_battery_capacity_setting"
CONF_CHARGING_CYCLES = "charging_cycles"
CONF_TOTAL_CHARGING_CYCLE_CAPACITY = "total_charging_cycle_capacity"
CONF_TOTAL_RUNTIME = "total_runtime"
CONF_BALANCING_CURRENT = "balancing_current"
CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_EMERGENCY_TIME_COUNTDOWN = "emergency_time_countdown"
CONF_HEATING_CURRENT = "heating_current"
CONF_CHARGE_STATUS_ID = "charge_status_id"
CONF_CHARGE_STATUS_TIME_ELAPSED = "charge_status_time_elapsed"

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
ICON_CHARGE_STATUS_ID = "mdi:battery-clock"
ICON_CHARGE_STATUS_TIME_ELAPSED = "mdi:timer-outline"

CELL_VOLTAGES = [f"cell_voltage_{i}" for i in range(1, 33)]
CELL_RESISTANCES = [f"cell_resistance_{i}" for i in range(1, 33)]
TEMPERATURES = [f"temperature_sensor_{i}" for i in range(1, 6)]

_CELL_VOLTAGE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_VOLT,
    icon=ICON_EMPTY,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_VOLTAGE,
    state_class=STATE_CLASS_MEASUREMENT,
)

_CELL_RESISTANCE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_OHM,
    icon=ICON_CELL_RESISTANCE,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_EMPTY,
    state_class=STATE_CLASS_MEASUREMENT,
)

_TEMPERATURE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_CELSIUS,
    icon=ICON_EMPTY,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_TEMPERATURE,
    state_class=STATE_CLASS_MEASUREMENT,
)

SENSOR_DEFS = {
    CONF_BALANCING: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_BALANCER,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
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
    CONF_TOTAL_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 3,
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
    CONF_POWER_TUBE_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_STATE_OF_CHARGE: {
        "unit_of_measurement": UNIT_PERCENT,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_BATTERY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_STATE_OF_HEALTH: {
        "unit_of_measurement": UNIT_PERCENT,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CAPACITY_REMAINING: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_CAPACITY_REMAINING,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TOTAL_BATTERY_CAPACITY_SETTING: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
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
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_TOTAL_RUNTIME: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_BALANCING_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_ERRORS_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_ERRORS_BITMASK,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_EMERGENCY_TIME_COUNTDOWN: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_TIMELAPSE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
    },
    CONF_HEATING_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CHARGE_STATUS_ID: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_CHARGE_STATUS_ID,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
    },
    CONF_CHARGE_STATUS_TIME_ELAPSED: {
        "unit_of_measurement": UNIT_SECONDS,
        "icon": ICON_CHARGE_STATUS_TIME_ELAPSED,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
    },
}

CONFIG_SCHEMA = (
    JK_BMS_BLE_COMPONENT_SCHEMA.extend(
        {
            cv.Optional(key): sensor.sensor_schema(**kwargs)
            for key, kwargs in SENSOR_DEFS.items()
        }
    )
    .extend({cv.Optional(key): _CELL_VOLTAGE_SCHEMA for key in CELL_VOLTAGES})
    .extend({cv.Optional(key): _CELL_RESISTANCE_SCHEMA for key in CELL_RESISTANCES})
    .extend({cv.Optional(key): _TEMPERATURE_SCHEMA for key in TEMPERATURES})
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
    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
