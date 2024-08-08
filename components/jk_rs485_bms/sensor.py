import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
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

from .const import (
    CONF_SMART_SLEEP_TIME
)

from . import CONF_JK_RS485_BMS_ID, JK_RS485_BMS_COMPONENT_SCHEMA

DEPENDENCIES = ["jk_rs485_bms"]

CODEOWNERS = ["@syssi","txubelaxu"]

CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME = "cell_request_charge_voltage_time"
CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME = "cell_request_float_voltage_time"




CONF_CELL_COUNT_REAL = "cell_count_real"
CONF_CELL_VOLTAGE_MIN = "cell_voltage_min"
CONF_CELL_VOLTAGE_MAX = "cell_voltage_max"
CONF_CELL_VOLTAGE_MIN_CELL_NUMBER = "cell_voltage_min_cell_number"
CONF_CELL_VOLTAGE_MAX_CELL_NUMBER = "cell_voltage_max_cell_number"
CONF_CELL_DELTA_VOLTAGE = "cell_delta_voltage"
CONF_CELL_AVERAGE_VOLTAGE = "cell_average_voltage"

CONF_CELL_RESISTANCE_MIN = "cell_resistance_min"
CONF_CELL_RESISTANCE_MAX = "cell_resistance_max"
CONF_CELL_RESISTANCE_MIN_CELL_NUMBER = "cell_resistance_min_cell_number"
CONF_CELL_RESISTANCE_MAX_CELL_NUMBER = "cell_resistance_max_cell_number"

CONF_CELL_VOLTAGE_01 = "cell_voltage_01"
CONF_CELL_VOLTAGE_02 = "cell_voltage_02"
CONF_CELL_VOLTAGE_03 = "cell_voltage_03"
CONF_CELL_VOLTAGE_04 = "cell_voltage_04"
CONF_CELL_VOLTAGE_05 = "cell_voltage_05"
CONF_CELL_VOLTAGE_06 = "cell_voltage_06"
CONF_CELL_VOLTAGE_07 = "cell_voltage_07"
CONF_CELL_VOLTAGE_08 = "cell_voltage_08"
CONF_CELL_VOLTAGE_09 = "cell_voltage_09"
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

CONF_CELL_RESISTANCE_01 = "cell_resistance_01"
CONF_CELL_RESISTANCE_02 = "cell_resistance_02"
CONF_CELL_RESISTANCE_03 = "cell_resistance_03"
CONF_CELL_RESISTANCE_04 = "cell_resistance_04"
CONF_CELL_RESISTANCE_05 = "cell_resistance_05"
CONF_CELL_RESISTANCE_06 = "cell_resistance_06"
CONF_CELL_RESISTANCE_07 = "cell_resistance_07"
CONF_CELL_RESISTANCE_08 = "cell_resistance_08"
CONF_CELL_RESISTANCE_09 = "cell_resistance_09"
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


CONF_TEMPERATURE_SENSOR_1 = "temperature_sensor_1"
CONF_TEMPERATURE_SENSOR_2 = "temperature_sensor_2"
CONF_TEMPERATURE_SENSOR_3 = "temperature_sensor_3"
CONF_TEMPERATURE_SENSOR_4 = "temperature_sensor_4"
CONF_TEMPERATURE_SENSOR_5 = "temperature_sensor_5"
CONF_TEMPERATURE_POWERTUBE = "temperature_powertube"
CONF_TEMPERATURE_SENSORS = "temperature_sensors"

CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_POWER = "battery_power"
CONF_BATTERY_POWER_CHARGING = "battery_power_charging"
CONF_BATTERY_POWER_DISCHARGING = "battery_power_discharging"
CONF_BATTERY_CAPACITY_REMAINING = "battery_capacity_remaining"
CONF_BATTERY_CAPACITY_REMAINING_DERIVED = "battery_capacity_remaining_derived"
CONF_CHARGING_CYCLES = "charging_cycles"
CONF_BATTERY_CAPACITY_TOTAL_CHARGING_CYCLE = "battery_capacity_total_charging_cycle"
CONF_BATTERY_STRINGS = "battery_strings"
CONF_BATTERY_CAPACITY_STATE_OF_CHARGE = "battery_capacity_state_of_charge"
CONF_BATTERY_SOH_VALUATION= "battery_soh_valuation"

CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_OPERATION_MODE_BITMASK = "operation_mode_bitmask"



CONF_DISCHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME = "discharging_overcurrent_protection_release_time";
CONF_DISCHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME = "discharging_short_circuit_protection_release_time";
CONF_CHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME = "charging_overcurrent_protection_release_time";
CONF_CHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME = "charging_short_circuit_protection_release_time";
CONF_CELL_UNDERVOLTAGE_PROTECTION_RELEASE_TIME = "cell_undervoltage_protection_release_time";
CONF_CELL_OVERVOLTAGE_PROTECTION_RELEASE_TIME = "cell_overvoltage_protection_release_time";



CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION = "cell_pressure_difference_protection"



CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE = "balancing_opening_pressure_difference"

CONF_POWERTUBE_TEMPERATURE_PROTECTION = "powertube_temperature_protection"
CONF_POWERTUBE_TEMPERATURE_PROTECTION_RECOVERY = "powertube_temperature_protection_recovery"

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


CONF_BALANCING_DIRECTION = "balancing_direction"
CONF_CURRENT_CALIBRATION = "current_calibration"
CONF_DEVICE_ADDRESS = "device_address"
CONF_SLEEP_WAIT_TIME = "sleep_wait_time"
CONF_ALARM_LOW_VOLUME = "alarm_low_volume"
CONF_MANUFACTURING_DATE = "manufacturing_date"
CONF_TOTAL_RUNTIME = "battery_total_runtime"
CONF_BALANCING_CURRENT = "balancing_current"

CONF_MAX_DISCHARGING_CURRENT ="max_discharging_current"
CONF_START_CURRENT_CALIBRATION = "start_current_calibration"
CONF_ACTUAL_BATTERY_CAPACITY = "actual_battery_capacity"
CONF_PROTOCOL_VERSION = "protocol_version"


CONF_DISCHARGING_OVERCURRENT_PROTECTION_DELAY = "discharging_overcurrent_protection_delay"
CONF_DISCHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY = "discharging_overcurrent_protection_recovery_delay"

CONF_BATTERY_TOTAL_ALARMS_COUNT = "battery_total_alarms_count"
CONF_BATTERY_TOTAL_ALARMS_ACTIVE = "battery_total_alarms_active"

CONF_EMERGENCY_TIME_COUNTDOWN = "emergency_time_countdown"

CONF_UART1_PROTOCOL_NUMBER ="uart1_protocol_number"
CONF_UART2_PROTOCOL_NUMBER ="uart2_protocol_number"

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

UNIT_MICROSECONDS = "us"
UNIT_SECONDS = "s"
UNIT_HOURS = "h"
UNIT_OHM = "Ω"
UNIT_AMPERE_HOURS = "Ah"

CELL_VOLTAGES = [
    CONF_CELL_VOLTAGE_01,
    CONF_CELL_VOLTAGE_02,
    CONF_CELL_VOLTAGE_03,
    CONF_CELL_VOLTAGE_04,
    CONF_CELL_VOLTAGE_05,
    CONF_CELL_VOLTAGE_06,
    CONF_CELL_VOLTAGE_07,
    CONF_CELL_VOLTAGE_08,
    CONF_CELL_VOLTAGE_09,
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
    CONF_CELL_RESISTANCE_01,
    CONF_CELL_RESISTANCE_02,
    CONF_CELL_RESISTANCE_03,
    CONF_CELL_RESISTANCE_04,
    CONF_CELL_RESISTANCE_05,
    CONF_CELL_RESISTANCE_06,
    CONF_CELL_RESISTANCE_07,
    CONF_CELL_RESISTANCE_08,
    CONF_CELL_RESISTANCE_09,
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
    CONF_BALANCING_DIRECTION,
    CONF_CELL_VOLTAGE_MIN,
    CONF_CELL_VOLTAGE_MAX,
    CONF_CELL_VOLTAGE_MIN_CELL_NUMBER,
    CONF_CELL_VOLTAGE_MAX_CELL_NUMBER,
    CONF_CELL_DELTA_VOLTAGE,
    CONF_CELL_AVERAGE_VOLTAGE,
    CONF_CELL_RESISTANCE_MIN,
    CONF_CELL_RESISTANCE_MAX,
    CONF_CELL_RESISTANCE_MIN_CELL_NUMBER,
    CONF_CELL_RESISTANCE_MAX_CELL_NUMBER,    
    CONF_CELL_COUNT_REAL,    
    CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME,
    CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME,    
    CONF_TEMPERATURE_POWERTUBE,
    CONF_BATTERY_VOLTAGE,
    CONF_BATTERY_CURRENT,
    CONF_BATTERY_POWER,
    CONF_BATTERY_POWER_CHARGING,
    CONF_BATTERY_POWER_DISCHARGING,
    CONF_BATTERY_CAPACITY_REMAINING,
    CONF_BATTERY_CAPACITY_REMAINING_DERIVED,
    CONF_TEMPERATURE_SENSORS,
    CONF_CHARGING_CYCLES,
    CONF_BATTERY_CAPACITY_TOTAL_CHARGING_CYCLE,
    CONF_BATTERY_STRINGS,
    CONF_ERRORS_BITMASK,
    CONF_OPERATION_MODE_BITMASK,
    CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION,

    CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE,
    CONF_POWERTUBE_TEMPERATURE_PROTECTION,
    CONF_POWERTUBE_TEMPERATURE_PROTECTION_RECOVERY,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_PROTECTION,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_RECOVERY,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_DIFFERENCE_PROTECTION,
    CONF_CHARGING_HIGH_TEMPERATURE_PROTECTION,
    CONF_DISCHARGING_HIGH_TEMPERATURE_PROTECTION,
    CONF_CHARGING_LOW_TEMPERATURE_PROTECTION,
    CONF_CHARGING_LOW_TEMPERATURE_RECOVERY,
    CONF_DISCHARGING_LOW_TEMPERATURE_PROTECTION,
    CONF_DISCHARGING_LOW_TEMPERATURE_RECOVERY,
    CONF_CURRENT_CALIBRATION,
    CONF_DEVICE_ADDRESS,
    CONF_SLEEP_WAIT_TIME,
    CONF_ALARM_LOW_VOLUME,
    CONF_MANUFACTURING_DATE,
    CONF_TOTAL_RUNTIME,
    CONF_BALANCING_CURRENT,  
    CONF_MAX_DISCHARGING_CURRENT,
    CONF_START_CURRENT_CALIBRATION,
    CONF_ACTUAL_BATTERY_CAPACITY,
    CONF_PROTOCOL_VERSION,
    CONF_BATTERY_CAPACITY_STATE_OF_CHARGE,
    CONF_BATTERY_SOH_VALUATION,
    CONF_SMART_SLEEP_TIME,
    CONF_BATTERY_TOTAL_ALARMS_COUNT,
    CONF_BATTERY_TOTAL_ALARMS_ACTIVE,
    CONF_EMERGENCY_TIME_COUNTDOWN,
    CONF_UART1_PROTOCOL_NUMBER,
    CONF_UART2_PROTOCOL_NUMBER,
    CONF_DISCHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME,
    CONF_DISCHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME,
    CONF_CHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME,
    CONF_CHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME,
    CONF_CELL_UNDERVOLTAGE_PROTECTION_RELEASE_TIME,
    CONF_CELL_OVERVOLTAGE_PROTECTION_RELEASE_TIME,
]

# pylint: disable=too-many-function-args
CONFIG_SCHEMA = JK_RS485_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCING_DIRECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,  
            icon=ICON_DIRECTION,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                
        cv.Optional(CONF_CELL_COUNT_REAL): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_CELL_VOLTAGE_MIN): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_MAX): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_MIN_CELL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CELL_VOLTAGE_MIN_CELL_NUMBER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_MAX_CELL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CELL_VOLTAGE_MAX_CELL_NUMBER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_DELTA_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_AVERAGE_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_MIN): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_MAX): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_MIN_CELL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_MAX_CELL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_HOURS,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),   
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_HOURS,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                   
        cv.Optional(CONF_CELL_VOLTAGE_01): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_02): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_03): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_04): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_05): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_06): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_07): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_08): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_09): sensor.sensor_schema(
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
        cv.Optional(CONF_CELL_RESISTANCE_01): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_02): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_03): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_04): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_05): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_06): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_07): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_08): sensor.sensor_schema(
            unit_of_measurement=UNIT_OHM,
            icon=ICON_CELL_RESISTANCE,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_RESISTANCE_09): sensor.sensor_schema(
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
        cv.Optional(CONF_TEMPERATURE_POWERTUBE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CAPACITY_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),  
        cv.Optional(CONF_BATTERY_SOH_VALUATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_POWER_CHARGING): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_POWER_DISCHARGING): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CAPACITY_REMAINING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_BATTERY_CAPACITY_REMAINING,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CAPACITY_REMAINING_DERIVED): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_BATTERY_CAPACITY_REMAINING_DERIVED,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSORS): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
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
        cv.Optional(CONF_BATTERY_CAPACITY_TOTAL_CHARGING_CYCLE): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_COUNTER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_STRINGS): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_BATTERY_STRINGS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ERRORS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_ERRORS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_OPERATION_MODE_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_OPERATION_MODE_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECTION_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECTION_RECOVERY_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),

        cv.Optional(CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWERTUBE_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWERTUBE_TEMPERATURE_PROTECTION_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(
            CONF_TEMPERATURE_SENSOR_TEMPERATURE_PROTECTION
        ): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_TEMPERATURE_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(
            CONF_TEMPERATURE_SENSOR_TEMPERATURE_DIFFERENCE_PROTECTION
        ): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_HIGH_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_HIGH_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_LOW_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_LOW_TEMPERATURE_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_LOW_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_LOW_TEMPERATURE_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT_CALIBRATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DEVICE_ADDRESS): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_DEVICE_ADDRESS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SLEEP_WAIT_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ALARM_LOW_VOLUME): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_ALARM_LOW_VOLUME,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MANUFACTURING_DATE): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
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
        cv.Optional(CONF_MAX_DISCHARGING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                  
        cv.Optional(CONF_START_CURRENT_CALIBRATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ACTUAL_BATTERY_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_ACTUAL_BATTERY_CAPACITY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PROTOCOL_VERSION): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SMART_SLEEP_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_HOURS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_BATTERY_TOTAL_ALARMS_COUNT): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_ALARM_LOW_VOLUME,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),         
        cv.Optional(CONF_BATTERY_TOTAL_ALARMS_ACTIVE): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_ALARM_LOW_VOLUME,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),  
        cv.Optional(CONF_EMERGENCY_TIME_COUNTDOWN): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_UART1_PROTOCOL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_UART2_PROTOCOL_NUMBER): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),  
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),   
        cv.Optional(CONF_DISCHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CHARGING_OVERCURRENT_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CHARGING_SHORT_CIRCUIT_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CELL_UNDERVOLTAGE_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CELL_OVERVOLTAGE_PROTECTION_RELEASE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_CLOCK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
    }
)



async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_RS485_BMS_ID])
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
