import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns
from ..const import CONF_BALANCER, CONF_CHARGING, CONF_DISCHARGING, CONF_HEATING, CONF_CHARGING_FLOAT_MODE,CONF_TIMED_STORE_DATA,CONF_EMERGENCY,CONF_DISABLE_TEMPERATURE_SENSORS,CONF_DISPLAY_ALWAYS_ON,CONF_SMART_SLEEP_ON,CONF_TIMED_STORED_DATA,CONF_DISABLE_PCL_MODULE

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi","@txubelaxu"]

ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"
ICON_BALANCER = "mdi:seesaw"
ICON_EMERGENCY = "mdi:exit-run"
ICON_HEATING = "mdi:radiator"
ICON_DISABLE_TEMPERATURE_SENSORS = "mdi:thermometer-off"
ICON_SMART_SLEEP_ON = "mdi:sleep"
ICON_TIMED_STORED_DATA = "mdi:calendar-clock"
ICON_DISABLE_PCL_MODULE = "mdi:power-plug-off"
ICON_CHARGING_FLOAT_MODE = "mdi:battery-charging-80"
ICON_DISPLAY_ALWAYS_ON = "mdi:television"

SWITCHES = {
    # JK04, JK02, JK02_32S
    CONF_CHARGING: [0x00, 0x1D, 0x1D],
    CONF_DISCHARGING: [0x00, 0x1E, 0x1E],
    CONF_BALANCER: [0x6C, 0x1F, 0x1F],
    CONF_EMERGENCY: [0x00, 0x00, 0x6B],
    CONF_HEATING: [0x00, 0x27, 0x27],
    CONF_DISABLE_TEMPERATURE_SENSORS: [0x00, 0x00, 0x28],
    CONF_DISPLAY_ALWAYS_ON: [0x00, 0x00, 0x2B],
    CONF_SMART_SLEEP_ON: [0x00, 0x00, 0x2D],
    CONF_DISABLE_PCL_MODULE: [0x00, 0x00, 0x2E],
    CONF_TIMED_STORED_DATA: [0x00, 0x00, 0x2F],
    CONF_CHARGING_FLOAT_MODE: [0x00, 0x00, 0x30],
}

JkSwitch = jk_bms_ble_ns.class_("JkSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): switch.switch_schema(JkSwitch, icon=ICON_CHARGING).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISCHARGING): switch.switch_schema(JkSwitch, icon=ICON_DISCHARGING).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_BALANCER): switch.switch_schema(JkSwitch, icon=ICON_BALANCER).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_EMERGENCY): switch.switch_schema(JkSwitch, icon=ICON_EMERGENCY).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_HEATING): switch.switch_schema(JkSwitch, icon=ICON_HEATING).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISABLE_TEMPERATURE_SENSORS): switch.switch_schema(JkSwitch, icon=ICON_DISABLE_TEMPERATURE_SENSORS).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISPLAY_ALWAYS_ON): switch.switch_schema(JkSwitch, icon=ICON_DISPLAY_ALWAYS_ON).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_SMART_SLEEP_ON): switch.switch_schema(JkSwitch, icon=ICON_SMART_SLEEP_ON).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISABLE_PCL_MODULE): switch.switch_schema(JkSwitch, icon=ICON_DISABLE_PCL_MODULE).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_TIMED_STORED_DATA): switch.switch_schema(JkSwitch, icon=ICON_TIMED_STORED_DATA).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_CHARGING_FLOAT_MODE): switch.switch_schema(JkSwitch, icon=ICON_CHARGING_FLOAT_MODE).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_jk04_holding_register(address[0]))
            cg.add(var.set_jk02_holding_register(address[1]))
            cg.add(var.set_jk02_32s_holding_register(address[2]))
