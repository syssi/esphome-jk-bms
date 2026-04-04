import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_CURRENT,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_EMPTY,
    ICON_TIMELAPSE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_VOLT,
)

from . import CONF_HELTEC_BALANCER_BLE_ID, HELTEC_BALANCER_BLE_COMPONENT_SCHEMA

CODEOWNERS = ["@syssi"]

CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_MIN_VOLTAGE_CELL = "min_voltage_cell"
CONF_MAX_VOLTAGE_CELL = "max_voltage_cell"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"

CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_TEMPERATURE_SENSOR_1 = "temperature_sensor_1"
CONF_TEMPERATURE_SENSOR_2 = "temperature_sensor_2"
CONF_TOTAL_RUNTIME = "total_runtime"
CONF_BALANCING_CURRENT = "balancing_current"
CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_CELL_DETECTION_FAILED_BITMASK = "cell_detection_failed_bitmask"
CONF_CELL_OVERVOLTAGE_BITMASK = "cell_overvoltage_bitmask"
CONF_CELL_UNDERVOLTAGE_BITMASK = "cell_undervoltage_bitmask"
CONF_CELL_POLARITY_ERROR_BITMASK = "cell_polarity_error_bitmask"
CONF_CELL_EXCESSIVE_LINE_RESISTANCE_BITMASK = "cell_excessive_line_resistance_bitmask"

UNIT_AMPERE_HOURS = "Ah"
UNIT_OHM = "Ω"
UNIT_SECONDS = "s"

ICON_CURRENT_DC = "mdi:current-dc"
ICON_CELL_RESISTANCE = "mdi:omega"

CELL_VOLTAGES = [f"cell_voltage_{i}" for i in range(1, 25)]
CELL_RESISTANCES = [f"cell_resistance_{i}" for i in range(1, 25)]

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
        "icon": "mdi:battery-minus-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_MAX_VOLTAGE_CELL: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:battery-plus-outline",
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
    CONF_TEMPERATURE_SENSOR_1: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_SENSOR_2: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
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
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CELL_DETECTION_FAILED_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CELL_OVERVOLTAGE_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CELL_UNDERVOLTAGE_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CELL_POLARITY_ERROR_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CELL_EXCESSIVE_LINE_RESISTANCE_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": "mdi:alert-circle-outline",
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = (
    HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
        {
            cv.Optional(key): sensor.sensor_schema(**kwargs)
            for key, kwargs in SENSOR_DEFS.items()
        }
    )
    .extend({cv.Optional(key): _CELL_VOLTAGE_SCHEMA for key in CELL_VOLTAGES})
    .extend({cv.Optional(key): _CELL_RESISTANCE_SCHEMA for key in CELL_RESISTANCES})
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
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
    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
