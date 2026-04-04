import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

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

BINARY_SENSOR_DEFS = {
    CONF_SYSTEM_WARNING: {"entity_category": ENTITY_CATEGORY_DIAGNOSTIC},
    CONF_BALANCING_SWITCH: {"icon": ICON_BALANCING},
    CONF_CHARGING: {"icon": ICON_CHARGING},
    CONF_DISCHARGING: {"icon": ICON_DISCHARGING},
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_OVERCURRENT_PROTECTION: {"entity_category": ENTITY_CATEGORY_DIAGNOSTIC},
    CONF_MOSFET_OVERTEMPERATURE_PROTECTION: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_BATTERY_TEMPERATURE_PROTECTION: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_SHORT_CIRCUIT_PROTECTION: {"entity_category": ENTITY_CATEGORY_DIAGNOSTIC},
    CONF_COPROCESSOR_COMMUNICATION_ERROR: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_BALANCER_WIRE_RESISTANCE_TOO_HIGH: {
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC
    },
    CONF_CELL_COUNT_MISMATCH: {"entity_category": ENTITY_CATEGORY_DIAGNOSTIC},
}

CONFIG_SCHEMA = JK_BMS_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_DISPLAY_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
