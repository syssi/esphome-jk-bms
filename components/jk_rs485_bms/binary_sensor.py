import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_JK_RS485_BMS_ID, JK_RS485_BMS_COMPONENT_SCHEMA
from .const import (
    CONF_ONLINE_STATUS,CONF_PRECHARGING_STATUS,CONF_CHARGING_STATUS, CONF_DISCHARGING_STATUS, CONF_BALANCING_STATUS, CONF_HEATING_STATUS,
    CONF_ALARM_WIRERES, CONF_ALARM_MOSOTP, CONF_ALARM_CELLQUANTITY, CONF_ALARM_CURSENSORERR, CONF_ALARM_CELLOVP, CONF_ALARM_BATOVP, 
    CONF_ALARM_CHOCP, CONF_ALARM_CHSCP, CONF_ALARM_CHOTP, CONF_ALARM_CHUTP, CONF_ALARM_CPUAUXCOMMUERR, CONF_ALARM_CELLUVP, CONF_ALARM_BATUVP, 
    CONF_ALARM_DCHOCP, CONF_ALARM_DCHSCP, CONF_ALARM_DCHOTP, CONF_ALARM_CHARGEMOS, CONF_ALARM_DISCHARGEMOS, CONF_ALARM_GPSDISCONNETED, 
    CONF_ALARM_MODIFYPWDINTIME, CONF_ALARM_DISCHARGEONFAILED, CONF_ALARM_BATTERYOVERTEMP, CONF_ALARM_TEMPERATURESENSORANOMALY, CONF_ALARM_PLCMODULEANOMALY,
    CONF_ALARM_MOSTEMPSENSORABSENT, CONF_ALARM_BATTEMPSENSOR1ABSENT, CONF_ALARM_BATTEMPSENSOR2ABSENT, CONF_ALARM_BATTEMPSENSOR3ABSENT, 
    CONF_ALARM_BATTEMPSENSOR4ABSENT, CONF_ALARM_BATTEMPSENSOR5ABSENT,
    CONF_SMART_SLEEP_TIME
)
DEPENDENCIES = ["jk_rs485_bms"]

CODEOWNERS = ["@syssi","@txubelaxu"]

CONF_CHARGING_SWITCH = (
    "charging_switch"  # @DEPRECATED and superseded by switch.charging
)
CONF_DISCHARGING_SWITCH = (
    "discharging_switch"  # @DEPRECATED and superseded by switch.discharging
)
#CONF_PRECHARGING = "precharging"
CONF_PRECHARGING_SWITCH = "precharging_switch"
#CONF_BALANCING = "balancing"
CONF_BALANCING_SWITCH = "balancing_switch"
CONF_DEDICATED_CHARGER_SWITCH = "dedicated_charger_switch"
CONF_ONLINE_STATUS = "online_status"
CONF_DISPLAY_ALWAYS_ON_SWITCHSTATUS = "online_status"

ICON_PRECHARGING = "mdi:battery-charging"
ICON_PRECHARGING_SWITCH = "mdi:battery-charging"
ICON_CHARGING_SWITCH = "mdi:battery-charging"
ICON_DISCHARGING_SWITCH = "mdi:power-plug"
ICON_BALANCING = "mdi:battery-heart-variant"
ICON_BALANCING_SWITCH = "mdi:battery-heart-variant"
ICON_DEDICATED_CHARGER_SWITCH = "mdi:battery-charging"
ICON_CHARGING = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_BALANCING = "mdi:battery-heart-variant"
ICON_HEATING = "mdi:radiator"
ICON_ALARM = "mdi:alarm-light"
ICON_TEMP_ALARM = "mdi:thermometer-alert"


BINARY_SENSORS = [
    CONF_PRECHARGING_STATUS,    
    CONF_CHARGING_STATUS,
    CONF_DISCHARGING_STATUS,
    CONF_BALANCING_STATUS,
    CONF_ONLINE_STATUS,
    CONF_HEATING_STATUS,

    CONF_ALARM_WIRERES,
    CONF_ALARM_MOSOTP,
    CONF_ALARM_CELLQUANTITY,
    CONF_ALARM_CURSENSORERR,
    CONF_ALARM_CELLOVP,
    CONF_ALARM_BATOVP,
    CONF_ALARM_CHOCP,
    CONF_ALARM_CHSCP,

    CONF_ALARM_CHOTP,
    CONF_ALARM_CHUTP,
    CONF_ALARM_CPUAUXCOMMUERR,
    CONF_ALARM_CELLUVP,
    CONF_ALARM_BATUVP,
    CONF_ALARM_DCHOCP,
    CONF_ALARM_DCHSCP,
    CONF_ALARM_DCHOTP,

    CONF_ALARM_CHARGEMOS,
    CONF_ALARM_DISCHARGEMOS,
    CONF_ALARM_GPSDISCONNETED,
    CONF_ALARM_MODIFYPWDINTIME,
    CONF_ALARM_DISCHARGEONFAILED,
    CONF_ALARM_BATTERYOVERTEMP,
    CONF_ALARM_TEMPERATURESENSORANOMALY,
    CONF_ALARM_PLCMODULEANOMALY,

    CONF_ALARM_MOSTEMPSENSORABSENT, 
    CONF_ALARM_BATTEMPSENSOR1ABSENT, 
    CONF_ALARM_BATTEMPSENSOR2ABSENT, 
    CONF_ALARM_BATTEMPSENSOR3ABSENT, 
    CONF_ALARM_BATTEMPSENSOR4ABSENT, 
    CONF_ALARM_BATTEMPSENSOR5ABSENT,
    
]

CONFIG_SCHEMA = JK_RS485_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_PRECHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon=ICON_CHARGING
        ),        
        cv.Optional(CONF_CHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon=ICON_CHARGING
        ),
        cv.Optional(CONF_DISCHARGING_STATUS): binary_sensor.binary_sensor_schema(
            icon=ICON_DISCHARGING
        ),
        cv.Optional(CONF_BALANCING_STATUS): binary_sensor.binary_sensor_schema(
            icon=ICON_BALANCING
        ),
        cv.Optional(CONF_HEATING_STATUS): binary_sensor.binary_sensor_schema(
            icon=ICON_HEATING
        ),
        cv.Optional(CONF_ALARM_WIRERES): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_MOSOTP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CELLQUANTITY): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CURSENSORERR): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CELLOVP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_BATOVP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CHOCP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CHSCP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CHOTP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CHUTP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CPUAUXCOMMUERR): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CELLUVP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_BATUVP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_DCHOCP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_DCHSCP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_DCHOTP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_CHARGEMOS): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_DISCHARGEMOS): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_GPSDISCONNETED): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_MODIFYPWDINTIME): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_DISCHARGEONFAILED): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTERYOVERTEMP): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_TEMPERATURESENSORANOMALY): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_PLCMODULEANOMALY): binary_sensor.binary_sensor_schema(
            icon=ICON_ALARM
        ),
        cv.Optional(CONF_ALARM_MOSTEMPSENSORABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTEMPSENSOR1ABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTEMPSENSOR2ABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTEMPSENSOR3ABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTEMPSENSOR4ABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),
        cv.Optional(CONF_ALARM_BATTEMPSENSOR5ABSENT): binary_sensor.binary_sensor_schema(
            icon=ICON_TEMP_ALARM
        ),


        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_RS485_BMS_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
