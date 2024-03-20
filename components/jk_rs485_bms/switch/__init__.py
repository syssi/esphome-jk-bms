import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_JK_RS485_BMS_ID, JK_RS485_BMS_COMPONENT_SCHEMA, jk_rs485_bms_ns
from ..const import CONF_BALANCER, CONF_PRECHARGING, CONF_CHARGING, CONF_DISCHARGING, CONF_DISPLAY_ALWAYS_ON, CONF_EMERGENCY, CONF_HEATING, CONF_CHARGING_FLOAT_MODE, CONF_SMART_SLEEP_ON,CONF_DISABLE_PCL_MODULE,CONF_DISABLE_TEMPERATURE_SENSORS, CONF_TIMED_STORED_DATA    

DEPENDENCIES = ["jk_rs485_bms"]

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
    CONF_PRECHARGING: 0xAB,
    CONF_CHARGING: 0xAB,
    CONF_DISCHARGING: 0xAC,
    # The BMS (v11) doesn't accept updates of register 0x9D at the moment
    CONF_BALANCER: 0x9D,
    CONF_DISPLAY_ALWAYS_ON: 0x00,
    CONF_EMERGENCY: 0x00,
    CONF_HEATING: 0x00,    
    CONF_CHARGING_FLOAT_MODE: 0x00,
    CONF_DISABLE_TEMPERATURE_SENSORS: 0x28,
    CONF_SMART_SLEEP_ON: 0x2D,
    CONF_DISABLE_PCL_MODULE: 0x2E,
    CONF_TIMED_STORED_DATA: 0x2F,
}

JkRS485BmsSwitch = jk_rs485_bms_ns.class_("JkRS485BmsSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = JK_RS485_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_PRECHARGING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_CHARGING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_CHARGING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_CHARGING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_DISCHARGING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_DISCHARGING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
         cv.Optional(CONF_BALANCER): switch.SWITCH_SCHEMA.extend(
             {
                 cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                 cv.Optional(CONF_ICON, default=ICON_BALANCER): cv.icon,
             }
         ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_EMERGENCY): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_EMERGENCY): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),  
        cv.Optional(CONF_HEATING): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_HEATING): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),   
        cv.Optional(CONF_DISABLE_TEMPERATURE_SENSORS): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(
                    CONF_ICON, default=ICON_DISABLE_TEMPERATURE_SENSORS
                ): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),                    
         cv.Optional(CONF_DISPLAY_ALWAYS_ON): switch.SWITCH_SCHEMA.extend(
             {
                 cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                 cv.Optional(CONF_ICON, default=ICON_DISPLAY_ALWAYS_ON): cv.icon,
             }
         ).extend(cv.COMPONENT_SCHEMA),    
        cv.Optional(CONF_SMART_SLEEP_ON): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_SMART_SLEEP_ON): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),    
        cv.Optional(CONF_TIMED_STORED_DATA): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_TIMED_STORED_DATA): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),             
        cv.Optional(CONF_CHARGING_FLOAT_MODE): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_CHARGING_FLOAT_MODE): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),  
        cv.Optional(CONF_DISABLE_PCL_MODULE): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(JkRS485BmsSwitch),
                cv.Optional(CONF_ICON, default=ICON_DISABLE_PCL_MODULE): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),                    
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_RS485_BMS_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            #cg.add(var.set_holding_register(address))
