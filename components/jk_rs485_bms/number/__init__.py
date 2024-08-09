import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MODE,
    CONF_STEP,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_DEVICE_CLASS,
    ENTITY_CATEGORY_CONFIG,
    ICON_EMPTY,
    UNIT_AMPERE,
    UNIT_EMPTY,
    UNIT_VOLT,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_COUNTER,
    ICON_TIMELAPSE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_WATT,
)

#from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns
from .. import CONF_JK_RS485_BMS_ID, JK_RS485_BMS_COMPONENT_SCHEMA, jk_rs485_bms_ns

#DEPENDENCIES = ["jk_bms_ble"]
DEPENDENCIES = ["jk_rs485_bms"]

CODEOWNERS = ["@syssi","@txubelaxu"]

DEFAULT_STEP = 1

UNIT_MICROSECONDS = "us"
UNIT_SECONDS = "s"
UNIT_HOURS = "h"
UNIT_OHM = "Ω"
UNIT_AMPERE_HOURS = "Ah"

CONF_CELL_SMART_SLEEP_VOLTAGE = "cell_smart_sleep_voltage"
CONF_CELL_UNDERVOLTAGE_PROTECTION = "cell_undervoltage_protection"
CONF_CELL_UNDERVOLTAGE_PROTECTION_RECOVERY = "cell_undervoltage_protection_recovery"
CONF_CELL_OVERVOLTAGE_PROTECTION = "cell_overvoltage_protection"
CONF_CELL_OVERVOLTAGE_PROTECTION_RECOVERY = "cell_overvoltage_protection_recovery"
CONF_CELL_BALANCING_TRIGGER_VOLTAGE = "cell_balancing_trigger_voltage"
CONF_CELL_SOC100_VOLTAGE = "cell_soc100_voltage"
CONF_CELL_SOC0_VOLTAGE = "cell_soc0_voltage"
CONF_CELL_REQUEST_CHARGE_VOLTAGE = "cell_request_charge_voltage"
CONF_CELL_REQUEST_FLOAT_VOLTAGE = "cell_request_float_voltage"
CONF_CELL_POWER_OFF_VOLTAGE = "cell_power_off_voltage"
CONF_CELL_BALANCING_STARTING_VOLTAGE = "cell_balancing_starting_voltage"

CONF_MAX_CHARGING_CURRENT ="max_charging_current"
CONF_CHARGING_OVERCURRENT_PROTECTION_DELAY = "charging_overcurrent_protection_delay"
CONF_CHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY = "charging_overcurrent_protection_recovery_delay"
CONF_MAX_DISCHARGING_CURRENT ="max_discharging_current"
CONF_DISCHARGING_OVERCURRENT_PROTECTION_DELAY = "discharging_overcurrent_protection_delay"
CONF_DISCHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY = "discharging_overcurrent_protection_recovery_delay"
CONF_SHORT_CIRCUIT_PROTECTION_DELAY ="short_circuit_protection_delay"
CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY = "short_circuit_protection_recovery_delay"
CONF_MAX_BALANCING_CURRENT = "max_balancing_current"

CONF_CHARGING_OVERTEMPERATURE_PROTECTION = "charging_overtemperature_protection"
CONF_CHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY = "charging_overtemperature_protection_recovery"
CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION = "discharging_overtemperature_protection"
CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY = "discharging_overtemperature_protection_recovery"
CONF_CHARGING_LOWTEMPERATURE_PROTECTION = "charging_lowtemperature_protection"
CONF_CHARGING_LOWTEMPERATURE_PROTECTION_RECOVERY = "charging_lowtemperature_protection_recovery"
CONF_MOS_OVERTEMPERATURE_PROTECTION = "mos_overtemperature_protection"
CONF_MOS_OVERTEMPERATURE_PROTECTION_RECOVERY = "mos_overtemperature_protection_recovery"

CONF_CELL_COUNT_SETTINGS = "cell_count_settings"
CONF_BATTERY_CAPACITY_TOTAL_SETTINGS = "battery_capacity_total_settings"
CONF_PRECHARGING_TIME_FROM_DISCHARGE = "precharging_time_from_discharge";

CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME = "cell_request_charge_voltage_time"
CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME = "cell_request_float_voltage_time"




CONF_VOLTAGE_CALIBRATION = "voltage_calibration"
CONF_CURRENT_CALIBRATION = "current_calibration"


ICON_BALANCING = "mdi:seesaw"
ICON_CURRENT_DC = "mdi:current-dc"
ICON_CELL_VOLTAGE_MIN_CELL_NUMBER = "mdi:battery-minus-outline"
ICON_CELL_VOLTAGE_MAX_CELL_NUMBER = "mdi:battery-plus-outline"

ICON_BATTERY_STRINGS = "mdi:car-battery"
ICON_BATTERY_CAPACITY_REMAINING = "mdi:battery-50"
ICON_BATTERY_CAPACITY_REMAINING_DERIVED = "mdi:battery-50"
ICON_ACTUAL_BATTERY_CAPACITY = "mdi:battery-50"
ICON_BATTERY_CAPACITY_TOTAL_SETTING = "mdi:battery-sync"

ICON_DEVICE_ADDRESS = "mdi:identifier"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"
ICON_OPERATION_MODE_BITMASK = "mdi:heart-pulse"
ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ALARM_LOW_VOLUME = "mdi:volume-high"

ICON_CELL_RESISTANCE = "mdi:omega"
ICON_BALANCING = "mdi:seesaw"
ICON_DIRECTION = "mdi:swap-horizontal-bold"
ICON_CLOCK ="mdi:clock-outline"

ICON_HIGH_TEMPERATURE = "mdi:weather-sunny"
ICON_LOW_TEMPERATURE = "mdi:snowflake"


# JK02_32S
#
# 02 04 ac0d0000  UVP                              3.5 V (3500)
# 03 04 100e0000  UVPR                             3.6 V (3600)
# 04 04 68100000  OVP                              4.2 V (4200)
# 05 04 a00f0000  OVPR                             4.0 V (4000)
# 06 04 64000000  Set balance trigger voltage    0.100 V (100)
# 0b 04 480d0000  Power Off Vol.                  3.40 V (3400)
# 0c 04 b80b0000  Continued Charge Current         3.0 A (3000)
# 0d 04 28000000  Charge OCP Delay                  40 S (40)
# 0e 04 46000000  Charge OCPR Time                  70 S (70)
# 0f 04 e8030000  Continued Discharge Current       10 A (1000)
# 10 04 2c010000  Discharge OCP Delay              300 S (300)
# 11 04 46000000  Discharge OCPR Time               70 S (70)
# 12 04 46000000  SCPR Time                         70 S (70)
# 13 04 2c010000  Max Balance Current              0.3 A (300)
# 14 04 94020000  Charge OTP                     66.0 °C (660)
# 15 04 62020000  Charge OTPR                    61.0 °C (610)
# 16 04 02030000  Discharge OTP                  77.0 °C (770)
# 17 04 8a020000  Discharge OTPR                 65.0 °C (650)
# 18 04 24ffffff  Charge UTP                    -22.0 °C (-220)
# 19 04 a6ffffff  Charge UTPR                    -9.0 °C (-90)
# 1a 04 de030000  MOS OTP                        99.0 °C (990)
# 1b 04 0c030000  MOS OTPR                       78.0 °C (780)
# 1c 04 06000000  Set cell count settings to                    6 (6)
# 20 04 b0360000  Set battery capacity             14 Ah (14)
# 21 04 d0070000  SCP Delay                      2000 uS (2000)
# 22 04 04100000  Start Balance voltage            4.1 V (4100)
# 64 04 983a0000  Set calibration voltage        15.00 V (15000)
# 67 04 c8000000  Set calibration current          2.0 A (200)
#
# https://github.com/syssi/esphome-jk-bms/issues/276#issuecomment-1468145528

UNIT_AMPERE_HOUR = "Ah"

NUMBERS = {
    # JK04, JK02, JK02_32S, factor                          address  3th   L        E^  type (0:uint|1:int)
    CONF_CELL_SMART_SLEEP_VOLTAGE:                          [0x0000, 0x10,   0x04,  3,  0],      
    CONF_CELL_UNDERVOLTAGE_PROTECTION:                      [0x0004, 0x10,   0x04,  3,  0],
    CONF_CELL_UNDERVOLTAGE_PROTECTION_RECOVERY:             [0x0008, 0x10,   0x04,  3,  0],
    CONF_CELL_OVERVOLTAGE_PROTECTION:                       [0x000C, 0x10,   0x04,  3,  0],
    CONF_CELL_OVERVOLTAGE_PROTECTION_RECOVERY:              [0x0010, 0x10,   0x04,  3,  0],
    CONF_CELL_BALANCING_TRIGGER_VOLTAGE:                    [0x0014, 0x10,   0x04,  3,  0],
    CONF_CELL_SOC100_VOLTAGE:                               [0x0018, 0x10,   0x04,  3,  0],
    CONF_CELL_SOC0_VOLTAGE:                                 [0x001C, 0x10,   0x04,  3,  0],
    CONF_CELL_REQUEST_CHARGE_VOLTAGE:                       [0x0020, 0x10,   0x04,  3,  0],
    CONF_CELL_REQUEST_FLOAT_VOLTAGE:                        [0x0024, 0x10,   0x04,  3,  0],
    CONF_CELL_POWER_OFF_VOLTAGE:                            [0x0028, 0x10,   0x04,  3,  0],
    CONF_CELL_BALANCING_STARTING_VOLTAGE:                   [0x0084, 0x10,   0x04,  3,  0],      #02.10.10.84.00.02.04.00.00.0D.70.3D.CC.
    #
    CONF_MAX_CHARGING_CURRENT:                              [0x002C, 0x10,   0x04,  3,  0],       
    CONF_CHARGING_OVERCURRENT_PROTECTION_DELAY:             [0x0030, 0x10,   0x04,  0,  0],
    CONF_CHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY:    [0x0034, 0x10,   0x04,  0,  0],
    CONF_MAX_DISCHARGING_CURRENT:                           [0x0038, 0x10,   0x04,  3,  0],
    CONF_DISCHARGING_OVERCURRENT_PROTECTION_DELAY:          [0x003C, 0x10,   0x04,  0,  0],
    CONF_DISCHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY: [0x0040, 0x10,   0x04,  0,  0],
    CONF_SHORT_CIRCUIT_PROTECTION_DELAY:                    [0x0080, 0x10,   0x04,  6,  0],
    CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY:           [0x0044, 0x10,   0x04,  0,  0],
    CONF_MAX_BALANCING_CURRENT:                             [0x0048, 0x10,   0x04,  3,  0],
    #
    CONF_CHARGING_OVERTEMPERATURE_PROTECTION:               [0x004C, 0x10,   0x04,  1,  1],
    CONF_CHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY:      [0x0050, 0x10,   0x04,  1,  1],
    CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION:            [0x0054, 0x10,   0x04,  1,  1],
    CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY:   [0x0058, 0x10,   0x04,  1,  1],
    CONF_CHARGING_LOWTEMPERATURE_PROTECTION:                [0x005C, 0x10,   0x04,  1,  1],
    CONF_CHARGING_LOWTEMPERATURE_PROTECTION_RECOVERY:       [0x0060, 0x10,   0x04,  1,  1],
    CONF_MOS_OVERTEMPERATURE_PROTECTION:                    [0x0064, 0x10,   0x04,  1,  1],
    CONF_MOS_OVERTEMPERATURE_PROTECTION_RECOVERY:           [0x0068, 0x10,   0x04,  1,  1],
    #
    CONF_CELL_COUNT_SETTINGS:                               [0x006C, 0x10,   0x04,  0,  0],
    CONF_BATTERY_CAPACITY_TOTAL_SETTINGS:                   [0x007C, 0x10,   0x04,  3,  0],
    CONF_PRECHARGING_TIME_FROM_DISCHARGE:                   [0x010C, 0x10,   0x04,  3,  0],   ############### TO VERIFY !!!

    CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME:                  [0x0104, 0x15,   0x02,  1,  0],
    CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME:                   [0x0104, 0x15,   0x02,  1,  0],


    #0x0104
    #02.10.15.04.00.01.02.00.01.36.25

    #RCV TIME 
    #0,1-→ 02.10.15.04.00.01.02.00.01.36.25.                               02.90.03.FC.01 (16)
    #1,0-→ 02.10.15.04.00.01.02.00.0A.77.E2.                               02.90.03.FC.01 (16)

        #  02.10.15.04.00.01.02.00.00.F7.E5
        #  02.10.15.04.00.01.02.00.01.36.25
}

JkRS485BmsNumber = jk_rs485_bms_ns.class_("JkRS485BmsNumber", number.Number, cg.Component)

JK_RS485_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(JkRS485BmsNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES, upper=True),
        cv.Optional(
            CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
        ): cv.entity_category,
        cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_EMPTY): cv.string_strict,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = JK_RS485_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CELL_SMART_SLEEP_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,      
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,   
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),        
        cv.Optional(CONF_CELL_OVERVOLTAGE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),
        cv.Optional(CONF_CELL_OVERVOLTAGE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),    
        cv.Optional(CONF_CELL_BALANCING_TRIGGER_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0.001): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ), 
        cv.Optional(CONF_CELL_SOC100_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ), 
        cv.Optional(CONF_CELL_SOC0_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ), 
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ), 
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),
        cv.Optional(CONF_CELL_POWER_OFF_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),  
        cv.Optional(CONF_CELL_BALANCING_STARTING_VOLTAGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_VOLTAGE):cv.string_strict,                  
            }
        ),         





        cv.Optional(CONF_MAX_CHARGING_CURRENT): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),  
        cv.Optional(CONF_CHARGING_OVERCURRENT_PROTECTION_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),  
        cv.Optional(CONF_CHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ), 
        cv.Optional(CONF_MAX_DISCHARGING_CURRENT): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CURRENT_DC): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),  
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECTION_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),  
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ), 

        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_MICROSECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),         
        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ), 
        cv.Optional(CONF_MAX_BALANCING_CURRENT): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CURRENT_DC): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=2): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),         



        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ), 

        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),
        cv.Optional(CONF_CHARGING_LOWTEMPERATURE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_LOW_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=-100): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ), 

        cv.Optional(CONF_CHARGING_LOWTEMPERATURE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_LOW_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=-100): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),
        cv.Optional(CONF_MOS_OVERTEMPERATURE_PROTECTION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),
        cv.Optional(CONF_MOS_OVERTEMPERATURE_PROTECTION_RECOVERY): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_HIGH_TEMPERATURE): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),        

        cv.Optional(CONF_CELL_COUNT_SETTINGS): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=3): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=24): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_BATTERY_CAPACITY_TOTAL_SETTINGS): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE_HOURS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_BATTERY_CAPACITY_TOTAL_SETTING): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=500): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),       
        cv.Optional(CONF_PRECHARGING_TIME_FROM_DISCHARGE): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_CLOCK): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3600): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ), 
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_HOURS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=255): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ), 
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME): JK_RS485_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_HOURS): cv.string_strict,
                cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,             
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=255): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
            }
        ),         







        cv.Optional(CONF_VOLTAGE_CALIBRATION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                # @FIXME The exact limits are unknown
                cv.Optional(CONF_MIN_VALUE, default=10.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_CURRENT_CALIBRATION): JK_RS485_NUMBER_SCHEMA.extend(
            {
                # @FIXME Exact limits are unknown
                cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
    }
)


async def to_code(config):
    #hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    hub = await cg.get_variable(config[CONF_JK_RS485_BMS_ID])
    for key, param_config in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await number.register_number(
                var,
                conf,
                min_value=conf[CONF_MIN_VALUE],
                max_value=conf[CONF_MAX_VALUE],
                step=conf[CONF_STEP],
            )
            cg.add(getattr(hub, f"set_{key}_number")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_register_address(param_config[0]))
            cg.add(var.set_third_element_of_frame(param_config[1]))
            cg.add(var.set_data_length(param_config[2]))
            cg.add(var.set_factor(param_config[3]))
            cg.add(var.set_type(param_config[4]))

            #[0x0000, 0x10,   0x04,  3,  0],

        

