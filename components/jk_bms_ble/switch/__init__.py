import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns
from ..const import (
    CONF_BALANCER,
    CONF_CHARGING,
    CONF_CHARGING_FLOAT_MODE,
    CONF_DISABLE_PCL_MODULE,
    CONF_DISABLE_TEMPERATURE_SENSORS,
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_2,
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_3,
    CONF_DISCHARGING,
    CONF_DISPLAY_ALWAYS_ON,
    CONF_DRY_CONTACT_ALARM_INTERMITTENT,
    CONF_EMERGENCY,
    CONF_EMERGENCY_BUTTON_TRIGGER,
    CONF_GPS_LOCKED_CHARGING,
    CONF_GPS_LOCKED_DISCHARGING,
    CONF_HEATING,
    CONF_SMART_SLEEP,
    CONF_TIMED_STORED_DATA,
)

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"
ICON_BALANCER = "mdi:seesaw"
ICON_EMERGENCY = "mdi:exit-run"
ICON_HEATING = "mdi:radiator"
ICON_DISABLE_TEMPERATURE_SENSORS = "mdi:thermometer-off"
ICON_SMART_SLEEP = "mdi:sleep"
ICON_TIMED_STORED_DATA = "mdi:calendar-clock"
ICON_DISABLE_PCL_MODULE = "mdi:power-plug-off"
ICON_CHARGING_FLOAT_MODE = "mdi:battery-charging-80"
ICON_DISPLAY_ALWAYS_ON = "mdi:television"
ICON_EMERGENCY_BUTTON_TRIGGER = "mdi:button-pointer"
ICON_DRY_CONTACT_ALARM_INTERMITTENT = "mdi:leak"
ICON_DISCHARGE_OVERCURRENT_PROTECTION_2 = "mdi:current-dc"
ICON_DISCHARGE_OVERCURRENT_PROTECTION_3 = "mdi:current-dc"
ICON_GPS_LOCKED_CHARGING = "mdi:battery-lock"
ICON_GPS_LOCKED_DISCHARGING = "mdi:battery-lock-open"

SWITCHES = {
    # JK04, JK02_24S, JK02_32S
    CONF_CHARGING: [0x00, 0x1D, 0x1D],
    CONF_DISCHARGING: [0x00, 0x1E, 0x1E],
    CONF_BALANCER: [0x6C, 0x1F, 0x1F],
    CONF_EMERGENCY: [0x00, 0x00, 0x6B],
    CONF_HEATING: [0x00, 0x00, 0x27],
    CONF_DISABLE_TEMPERATURE_SENSORS: [0x00, 0x00, 0x28],
    CONF_DISPLAY_ALWAYS_ON: [0x00, 0x00, 0x2B],
    CONF_SMART_SLEEP: [0x00, 0x00, 0x2D],
    CONF_DISABLE_PCL_MODULE: [0x00, 0x00, 0x2E],
    CONF_TIMED_STORED_DATA: [0x00, 0x00, 0x2F],
    CONF_CHARGING_FLOAT_MODE: [0x00, 0x00, 0x30],
    CONF_EMERGENCY_BUTTON_TRIGGER: [0x00, 0x00, 0x31],
    CONF_DRY_CONTACT_ALARM_INTERMITTENT: [0x00, 0x00, 0x32],
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_2: [0x00, 0x00, 0x33],
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_3: [0x00, 0x00, 0x34],
    CONF_GPS_LOCKED_CHARGING: [0x00, 0x00, 0x35],
    CONF_GPS_LOCKED_DISCHARGING: [0x00, 0x00, 0x36],
}

JkSwitch = jk_bms_ble_ns.class_("JkSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_CHARGING,
        ),
        cv.Optional(CONF_DISCHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISCHARGING,
        ),
        cv.Optional(CONF_BALANCER): switch.switch_schema(
            JkSwitch,
            icon=ICON_BALANCER,
        ),
        cv.Optional(CONF_EMERGENCY): switch.switch_schema(
            JkSwitch,
            icon=ICON_EMERGENCY,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_HEATING): switch.switch_schema(
            JkSwitch,
            icon=ICON_HEATING,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISABLE_TEMPERATURE_SENSORS): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISABLE_TEMPERATURE_SENSORS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISPLAY_ALWAYS_ON): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISPLAY_ALWAYS_ON,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_SMART_SLEEP): switch.switch_schema(
            JkSwitch,
            icon=ICON_SMART_SLEEP,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISABLE_PCL_MODULE): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISABLE_PCL_MODULE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TIMED_STORED_DATA): switch.switch_schema(
            JkSwitch,
            icon=ICON_TIMED_STORED_DATA,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_FLOAT_MODE): switch.switch_schema(
            JkSwitch,
            icon=ICON_CHARGING_FLOAT_MODE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_EMERGENCY_BUTTON_TRIGGER): switch.switch_schema(
            JkSwitch,
            icon=ICON_EMERGENCY_BUTTON_TRIGGER,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DRY_CONTACT_ALARM_INTERMITTENT): switch.switch_schema(
            JkSwitch,
            icon=ICON_DRY_CONTACT_ALARM_INTERMITTENT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGE_OVERCURRENT_PROTECTION_2): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISCHARGE_OVERCURRENT_PROTECTION_2,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGE_OVERCURRENT_PROTECTION_3): switch.switch_schema(
            JkSwitch,
            icon=ICON_DISCHARGE_OVERCURRENT_PROTECTION_3,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_GPS_LOCKED_CHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_GPS_LOCKED_CHARGING,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_GPS_LOCKED_DISCHARGING): switch.switch_schema(
            JkSwitch,
            icon=ICON_GPS_LOCKED_DISCHARGING,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_jk04_holding_register(address[0]))
            cg.add(var.set_jk02_holding_register(address[1]))
            cg.add(var.set_jk02_32s_holding_register(address[2]))
