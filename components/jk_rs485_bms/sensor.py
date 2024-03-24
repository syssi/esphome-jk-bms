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

CONF_CELL_SMART_SLEEP_VOLTAGE = "cell_smart_sleep_voltage"
CONF_CELL_VOLTAGE_MIN = "cell_voltage_min"
CONF_CELL_VOLTAGE_MAX = "cell_voltage_max"
CONF_CELL_VOLTAGE_MIN_CELL_NUMBER = "cell_voltage_min_cell_number"
CONF_CELL_VOLTAGE_MAX_CELL_NUMBER = "cell_voltage_max_cell_number"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"
CONF_CELL_COUNT = "cell_count"
CONF_CELL_REQUEST_CHARGE_VOLTAGE = "cell_request_charge_voltage"
CONF_CELL_REQUEST_FLOAT_VOLTAGE = "cell_request_float_voltage"
CONF_CELL_SOC100_VOLTAGE = "cell_soc100_voltage"
CONF_CELL_SOC0_VOLTAGE = "cell_soc0_voltage"
CONF_CELL_POWER_OFF_VOLTAGE = "cell_power_off_voltage"

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
CONF_POWER_TUBE_TEMPERATURE = "power_tube_temperature"
CONF_TEMPERATURE_SENSORS = "temperature_sensors"

CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_CHARGING_POWER = "charging_power"
CONF_DISCHARGING_POWER = "discharging_power"
CONF_CAPACITY_REMAINING = "capacity_remaining"
CONF_CAPACITY_REMAINING_DERIVED = "capacity_remaining_derived"
CONF_CHARGING_CYCLES = "charging_cycles"
CONF_TOTAL_CHARGING_CYCLE_CAPACITY = "total_charging_cycle_capacity"
CONF_BATTERY_STRINGS = "battery_strings"
CONF_STATE_OF_CHARGE = "state_of_charge"

CONF_ERRORS_BITMASK = "errors_bitmask"
CONF_OPERATION_MODE_BITMASK = "operation_mode_bitmask"

CONF_TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION = "total_voltage_overvoltage_protection"
CONF_TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "total_voltage_undervoltage_protection"

CONF_CELL_OVERVOLTAGE_PROTECTION = "cell_overvoltage_protection"
CONF_CELL_OVERVOLTAGE_RECOVERY = "cell_overvoltage_protection_recovery"
CONF_CELL_VOLTAGE_OVERVOLTAGE_DELAY = "cell_overvoltage_delay"

CONF_CELL_UNDERVOLTAGE_PROTECTION = "cell_undervoltage_protection"
CONF_CELL_UNDERVOLTAGE_RECOVERY = "cell_undervoltage_protection_recovery"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_DELAY = "cell_undervoltage_delay"

CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION = "cell_pressure_difference_protection"

CONF_BALANCING_TRIGGER_VOLTAGE = "balancing_trigger_voltage"
CONF_BALANCING_STARTING_VOLTAGE = "balancing_starting_voltage"
CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE = "balancing_opening_pressure_difference"

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

CONF_BALANCING_DIRECTION = "balancing_direction"
CONF_CURRENT_CALIBRATION = "current_calibration"
CONF_DEVICE_ADDRESS = "device_address"
CONF_SLEEP_WAIT_TIME = "sleep_wait_time"
CONF_ALARM_LOW_VOLUME = "alarm_low_volume"
CONF_MANUFACTURING_DATE = "manufacturing_date"
CONF_TOTAL_RUNTIME = "total_runtime"
CONF_BALANCING_CURRENT = "balancing_current"
CONF_MAX_BALANCING_CURRENT = "max_balancing_current"
CONF_MAX_CHARGING_CURRENT ="max_charging_current"
CONF_MAX_DISCHARGING_CURRENT ="max_discharging_current"
CONF_START_CURRENT_CALIBRATION = "start_current_calibration"
CONF_ACTUAL_BATTERY_CAPACITY = "actual_battery_capacity"
CONF_PROTOCOL_VERSION = "protocol_version"

CONF_CHARGING_OVERCURRENT_PROTECION_DELAY = "charging_overcurrent_protection_delay"
CONF_CHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY = "charging_overcurrent_protection_recovery_delay"
CONF_DISCHARGING_OVERCURRENT_PROTECION_DELAY = "discharging_overcurrent_protection_delay"
CONF_DISCHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY = "discharging_overcurrent_protection_recovery_delay"
CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY ="short_circuit_protection_recovery_delay"
CONF_CHARGING_OVERTEMPERATURE_PROTECION = "charging_overtemperature_protection"
CONF_CHARGING_OVERTEMPERATURE_PROTECION_RECOVERY = "charging_overtemperature_protection_recovery"
CONF_DISCHARGING_OVERTEMPERATURE_PROTECION = "discharging_overtemperature_protection"
CONF_DISCHARGING_OVERTEMPERATURE_PROTECION_RECOVERY = "discharging_overtemperature_protection_recovery"

ICON_CURRENT_DC = "mdi:current-dc"
ICON_CELL_VOLTAGE_MIN_CELL_NUMBER = "mdi:battery-minus-outline"
ICON_CELL_VOLTAGE_MAX_CELL_NUMBER = "mdi:battery-plus-outline"

ICON_BATTERY_STRINGS = "mdi:car-battery"
ICON_CAPACITY_REMAINING = "mdi:battery-50"
ICON_CAPACITY_REMAINING_DERIVED = "mdi:battery-50"
ICON_ACTUAL_BATTERY_CAPACITY = "mdi:battery-50"
ICON_TOTAL_BATTERY_CAPACITY_SETTING = "mdi:battery-sync"

ICON_DEVICE_ADDRESS = "mdi:identifier"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"
ICON_OPERATION_MODE_BITMASK = "mdi:heart-pulse"
ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ALARM_LOW_VOLUME = "mdi:volume-high"

ICON_CELL_RESISTANCE = "mdi:omega"
ICON_BALANCING = "mdi:seesaw"
ICON_DIRECTION = "mdi:swap-horizontal-bold"
ICON_CLOCK ="mdi:clock-outline"

ICON_HIGH_TEMPERATURE = "mdi:thermometer-alert"


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
]

SENSORS = [
    CONF_BALANCING_DIRECTION,
    CONF_CELL_SMART_SLEEP_VOLTAGE,
    CONF_CELL_VOLTAGE_MIN,
    CONF_CELL_VOLTAGE_MAX,
    CONF_CELL_VOLTAGE_MIN_CELL_NUMBER,
    CONF_CELL_VOLTAGE_MAX_CELL_NUMBER,
    CONF_DELTA_CELL_VOLTAGE,
    CONF_AVERAGE_CELL_VOLTAGE,
    CONF_CELL_COUNT,
    CONF_CELL_REQUEST_CHARGE_VOLTAGE,
    CONF_CELL_REQUEST_FLOAT_VOLTAGE,    
    CONF_CELL_SOC100_VOLTAGE,
    CONF_CELL_SOC0_VOLTAGE,
    CONF_CELL_POWER_OFF_VOLTAGE,
    CONF_POWER_TUBE_TEMPERATURE,
    CONF_TOTAL_VOLTAGE,
    CONF_CURRENT,
    CONF_POWER,
    CONF_CHARGING_POWER,
    CONF_DISCHARGING_POWER,
    CONF_CAPACITY_REMAINING,
    CONF_CAPACITY_REMAINING_DERIVED,
    CONF_TEMPERATURE_SENSORS,
    CONF_CHARGING_CYCLES,
    CONF_TOTAL_CHARGING_CYCLE_CAPACITY,
    CONF_BATTERY_STRINGS,
    CONF_ERRORS_BITMASK,
    CONF_OPERATION_MODE_BITMASK,
    CONF_TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION,
    CONF_TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION,
    CONF_CELL_OVERVOLTAGE_PROTECTION,
    CONF_CELL_OVERVOLTAGE_RECOVERY,
    CONF_CELL_VOLTAGE_OVERVOLTAGE_DELAY,
    CONF_CELL_UNDERVOLTAGE_PROTECTION,
    CONF_CELL_UNDERVOLTAGE_RECOVERY,
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_DELAY,
    CONF_CELL_PRESSURE_DIFFERENCE_PROTECTION,
    CONF_CHARGING_OVERCURRENT_PROTECION_DELAY,
    CONF_CHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY,
    CONF_DISCHARGING_OVERCURRENT_PROTECION_DELAY,
    CONF_DISCHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY,
    CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY,
    CONF_CHARGING_OVERTEMPERATURE_PROTECION,
    CONF_CHARGING_OVERTEMPERATURE_PROTECION_RECOVERY,
    CONF_DISCHARGING_OVERTEMPERATURE_PROTECION,
    CONF_DISCHARGING_OVERTEMPERATURE_PROTECION_RECOVERY,      
    CONF_BALANCING_TRIGGER_VOLTAGE,
    CONF_BALANCING_STARTING_VOLTAGE,
    CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE,
    CONF_POWER_TUBE_TEMPERATURE_PROTECTION,
    CONF_POWER_TUBE_TEMPERATURE_RECOVERY,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_PROTECTION,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_RECOVERY,
    CONF_TEMPERATURE_SENSOR_TEMPERATURE_DIFFERENCE_PROTECTION,
    CONF_CHARGING_HIGH_TEMPERATURE_PROTECTION,
    CONF_DISCHARGING_HIGH_TEMPERATURE_PROTECTION,
    CONF_CHARGING_LOW_TEMPERATURE_PROTECTION,
    CONF_CHARGING_LOW_TEMPERATURE_RECOVERY,
    CONF_DISCHARGING_LOW_TEMPERATURE_PROTECTION,
    CONF_DISCHARGING_LOW_TEMPERATURE_RECOVERY,
    CONF_TOTAL_BATTERY_CAPACITY_SETTING,
    CONF_CURRENT_CALIBRATION,
    CONF_DEVICE_ADDRESS,
    CONF_SLEEP_WAIT_TIME,
    CONF_ALARM_LOW_VOLUME,
    CONF_MANUFACTURING_DATE,
    CONF_TOTAL_RUNTIME,
    CONF_BALANCING_CURRENT,  
    CONF_MAX_BALANCING_CURRENT,
    CONF_MAX_CHARGING_CURRENT,
    CONF_MAX_DISCHARGING_CURRENT,
    CONF_START_CURRENT_CALIBRATION,
    CONF_ACTUAL_BATTERY_CAPACITY,
    CONF_PROTOCOL_VERSION,
    CONF_STATE_OF_CHARGE,
    CONF_SMART_SLEEP_TIME,
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
        cv.Optional(CONF_CELL_SMART_SLEEP_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
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
        cv.Optional(CONF_DELTA_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AVERAGE_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_COUNT): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),   
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),         
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),   
        cv.Optional(CONF_CELL_SOC100_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CELL_SOC0_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),     
        cv.Optional(CONF_CELL_POWER_OFF_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
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
        cv.Optional(CONF_POWER_TUBE_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_TOTAL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CAPACITY_REMAINING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_CAPACITY_REMAINING,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CAPACITY_REMAINING_DERIVED): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_CAPACITY_REMAINING_DERIVED,
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
        cv.Optional(CONF_TOTAL_CHARGING_CYCLE_CAPACITY): sensor.sensor_schema(
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
        cv.Optional(CONF_TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_OVERVOLTAGE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_OVERVOLTAGE_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_OVERVOLTAGE_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_UNDERVOLTAGE_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
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
        cv.Optional(CONF_CHARGING_OVERCURRENT_PROTECION_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECION_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_PROTECION_RECOVERY_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECONDS,
            icon=ICON_TIMELAPSE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_PROTECION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_HIGH_TEMPERATURE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_PROTECION_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_HIGH_TEMPERATURE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_PROTECION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_HIGH_TEMPERATURE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_PROTECION_RECOVERY): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_HIGH_TEMPERATURE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                      
        cv.Optional(CONF_BALANCING_TRIGGER_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),        
        cv.Optional(CONF_BALANCING_STARTING_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BALANCING_OPENING_PRESSURE_DIFFERENCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER_TUBE_TEMPERATURE_PROTECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER_TUBE_TEMPERATURE_RECOVERY): sensor.sensor_schema(
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
        cv.Optional(CONF_TOTAL_BATTERY_CAPACITY_SETTING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_TOTAL_BATTERY_CAPACITY_SETTING,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
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
        cv.Optional(CONF_MAX_BALANCING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),          
        cv.Optional(CONF_MAX_CHARGING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),    
        cv.Optional(CONF_MAX_DISCHARGING_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),                  
        cv.Optional(CONF_START_CURRENT_CALIBRATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
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
