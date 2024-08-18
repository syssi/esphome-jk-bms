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
    ENTITY_CATEGORY_CONFIG,
    ICON_EMPTY,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_HOUR,
    UNIT_VOLT,
)

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi", "@txubelaxu"]

DEFAULT_STEP = 1

# JK02_24S
#
# 01 04 03000000  Smart sleep voltage            0.003 V (3)
# 02 04 b0040000  Cell UVP                         1.2 V (1200)
# 03 04 b0040000  Cell UVPR                        1.2 V (1200)
# 04 04 420e0000  Cell OVP                        3.65 V (3650)
# 05 04 b0040000  Cell OVPR                        1.2 V (1200)
# 06 04 03000000  Balance trigger voltage        0.003 V (3)
# 07 04 03000000  SOC 100% voltage               0.003 V (3)
# 08 04 03000000  SOC 0% voltage                 0.003 V (3)
# 09 04 03000000  Requested charge voltage       0.003 V (3)
# 0a 04 03000000  Requested float voltage        0.003 V (3)
# 0b 04 b0040000  Power Off value                  1.2 V (1200)
# 0c 04 e8030000  Max charge Current               1.0 A (1000)
# 0d 04 02000000  Charge OCP delay                   2 S (2)
# 0e 04 02000000  Charge OCPR Time                   2 S (2)
# 0f 04 e8030000  Continuous Discharge Current       1 A (100)
# 10 04 02000000  Discharge OCP delay                2 S (2)
# 11 04 02000000  Discharge OCPR Time                2 S (2)
# 12 04 02000000  SCPR Time                          2 S (2)
# 13 04 2c010000  Max Balance current              0.3 A (300)
# 14 04 2c010000  Charge OTP                       30 °C (300)
# 15 04 2c010000  Charge OTPR                      30 °C (300)
# 16 04 2c010000  Discharge OTP                    30 °C (300)
# 17 04 2c010000  Discharge OTPR                   30 °C (300)
# 18 04 00000000  Charge UTP                        0 °C (0)
# 19 04 00000000  Charge UTPR                       0 °C (0)
# 1a 04 de030000  MOS OTP                        99.0 °C (990)
# 1b 04 0c030000  MOS OTPR                       78.0 °C (780)
# 1c 04 10000000  Cell count                          16 (16)
# 1d 04 01000000  Charge Mosfet on               enabled (1)
# 1e 04 01000000  Discharge Mosfet on            enabled (1)
# 1f 04 01000000  Balancer on                    enabled (1)
# 20 04 f0ba0400  Battery capacity                310 Ah (310000)
# 21 04 42d10000  Calibrate voltage              53.57 V (53570)
# 22              Unknown or unused
# 23              Unknown or unused
# 24 04 64000000  Calibrate current              0.100 A (100)
# 25 04 0a000000  SCP Delay                         10 S (10)
# 26 04 b0040000  Start Balance voltage            1.2 V (1200)
# 27 04 00000000  Con. Wire Res. 01 to 0.00
# ...
# 3e 04 00000000  Con. Wire Res. 24 to 0.00

# 9f 04 54657374  Set User Private Data (4 bytes) to TEST
# 9f 0d 30313233343536373839303132 c6  Set User Private Data (4 bytes) to 0123456789012
#
# https://github.com/jblance/mpp-solar/issues/170#issuecomment-1050503970

# JK02_32S
#
# 01 04 03000000  Smart sleep voltage            0.003 V (3)
# 02 04 ac0d0000  UVP                              3.5 V (3500)
# 03 04 100e0000  UVPR                             3.6 V (3600)
# 04 04 68100000  OVP                              4.2 V (4200)
# 05 04 a00f0000  OVPR                             4.0 V (4000)
# 06 04 64000000  Balance trigger voltage        0.100 V (100)
# 07 04 03000000  SOC 100% voltage               0.003 V (3)
# 08 04 03000000  SOC 0% voltage                 0.003 V (3)
# 09 04 7f0d0000  Requested charge voltage       3.455 V (3455)
# 0a 04 490d0000  Requested float voltage        3.401 V (3401)
# 0b 04 480d0000  Power Off Voltage               3.40 V (3400)
# 0c 04 b80b0000  Continuous Charge Current        3.0 A (3000)
# 0d 04 28000000  Charge OCP Delay                  40 S (40)
# 0e 04 46000000  Charge OCPR Time                  70 S (70)
# 0f 04 e8030000  Continuous Discharge Current      10 A (1000)
# 10 04 2c010000  Discharge OCP Delay              300 S (300)
# 11 04 46000000  Discharge OCPR Time               70 S (70)
# 12 04 46000000  SCP Recovery Time                 70 S (70)
# 13 04 2c010000  Max Balance Current              0.3 A (300)
# 14 04 94020000  Charge OTP                     66.0 °C (660)
# 15 04 62020000  Charge OTPR                    61.0 °C (610)
# 16 04 02030000  Discharge OTP                  77.0 °C (770)
# 17 04 8a020000  Discharge OTPR                 65.0 °C (650)
# 18 04 24ffffff  Charge UTP                    -22.0 °C (-220)
# 19 04 a6ffffff  Charge UTPR                    -9.0 °C (-90)
# 1a 04 de030000  MOS OTP                        99.0 °C (990)
# 1b 04 0c030000  MOS OTPR                       78.0 °C (780)
# 1c 04 06000000  Cell count                           6 (6)
# 1d 04 01000000  Charge Mosfet on               enabled (1)
# 1e 04 01000000  Discharge Mosfet on            enabled (1)
# 1f 04 01000000  Balancer on                    enabled (1)
# 20 04 b0360000  Battery capacity                 14 Ah (14)
# 21 04 d0070000  SCP Delay                      2000 uS (2000)
# 22 04 04100000  Start Balance voltage            4.1 V (4100)
# ...
# 27 04 01000000  Heating on                     enabled (1)
# 28 04 01000000  Disable temperature sensors    enabled (1)
# 2b 04 01000000  Display always on              enabled (1)
# 2d 04 01000000  Smart sleep on                 enabled (1)
# 2e 04 01000000  Disable PCL module             enabled (1)
# 2f 04 01000000  Timed stored data              enabled (1)
# 30 04 01000000  Charging float mode            enabled (1)
# ...
# 39 01 17000000  Time Smart Sleep                  23 h (23)
# ...
# 64 04 983a0000  Calibration voltage            15.00 V (15000)
# 67 04 c8000000  Calibration current              2.0 A (200)
# 6b 04 01000000  Emergency Mode                 enabled (1)
# ...
# b3 01 14        Requested charge voltage time    2.0 h (20)
# b4 01 5a        Requested float voltage time     9.0 h (90)

# https://github.com/syssi/esphome-jk-bms/issues/276#issuecomment-1468145528

CONF_SMART_SLEEP_VOLTAGE = "smart_sleep_voltage"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "cell_voltage_undervoltage_protection"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY = "cell_voltage_undervoltage_recovery"
CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION = "cell_voltage_overvoltage_protection"
CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY = "cell_voltage_overvoltage_recovery"
CONF_BALANCE_TRIGGER_VOLTAGE = "balance_trigger_voltage"
CONF_CELL_SOC100_VOLTAGE = "cell_soc100_voltage"
CONF_CELL_SOC0_VOLTAGE = "cell_soc0_voltage"
CONF_CELL_REQUEST_CHARGE_VOLTAGE = "cell_request_charge_voltage"
CONF_CELL_REQUEST_FLOAT_VOLTAGE = "cell_request_float_voltage"
CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME = "cell_request_charge_voltage_time"
CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME = "cell_request_float_voltage_time"

CONF_CELL_COUNT = "cell_count"
CONF_TOTAL_BATTERY_CAPACITY = "total_battery_capacity"

CONF_BALANCE_STARTING_VOLTAGE = "balance_starting_voltage"
CONF_VOLTAGE_CALIBRATION = "voltage_calibration"
CONF_CURRENT_CALIBRATION = "current_calibration"
CONF_POWER_OFF_VOLTAGE = "power_off_voltage"
CONF_MAX_BALANCE_CURRENT = "max_balance_current"
CONF_MAX_CHARGE_CURRENT = "max_charge_current"
CONF_MAX_DISCHARGE_CURRENT = "max_discharge_current"

CONF_CHARGE_OVERCURRENT_PROTECTION_DELAY = "charge_overcurrent_protection_delay"
CONF_CHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME = (
    "charge_overcurrent_protection_recovery_time"
)
CONF_DISCHARGE_OVERCURRENT_PROTECTION_DELAY = "discharge_overcurrent_protection_delay"
CONF_DISCHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME = (
    "discharge_overcurrent_protection_recovery_time"
)
CONF_SHORT_CIRCUIT_PROTECTION_DELAY = "short_circuit_protection_delay"
CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_TIME = "short_circuit_protection_recovery_time"
CONF_CHARGE_OVERTEMPERATURE_PROTECTION = "charge_overtemperature_protection"
CONF_CHARGE_OVERTMPERATURE_PROTECTION_RECOVERY = (
    "charge_overtemperature_protection_recovery"
)
CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION = "discharge_overtemperature_protection"
CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION_RECOVERY = (
    "discharge_overtemperature_protection_recovery"
)
CONF_CHARGE_UNDERTEMPERATURE_PROTECTION = "charge_undertemperature_protection"
CONF_CHARGE_UNDERTMPERATURE_PROTECTION_RECOVERY = (
    "charge_undertemperature_protection_recovery"
)
CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION = "power_tube_overtemperature_protection"
CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION_RECOVERY = (
    "power_tube_overtemperature_protection_recovery"
)

UNIT_AMPERE_HOUR = "Ah"
UNIT_SECONDS = "s"
UNIT_MICROSECONDS = "μs"

NUMBERS = {
    # JK04, JK02_24S, JK02_32S, factor, length
    CONF_SMART_SLEEP_VOLTAGE: [0x00, 0x01, 0x01, 1000.0, 1],
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION: [0x00, 0x02, 0x02, 1000.0, 4],
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY: [0x00, 0x03, 0x03, 1000.0, 4],
    CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION: [0x00, 0x04, 0x04, 1000.0, 4],
    CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY: [0x00, 0x05, 0x05, 1000.0, 4],
    CONF_BALANCE_TRIGGER_VOLTAGE: [0x00, 0x06, 0x06, 1000.0, 4],
    CONF_CELL_SOC100_VOLTAGE: [0x00, 0x07, 0x07, 1000.0, 4],
    CONF_CELL_SOC0_VOLTAGE: [0x00, 0x08, 0x08, 1000.0, 4],
    CONF_CELL_REQUEST_CHARGE_VOLTAGE: [0x00, 0x09, 0x09, 1000.0, 4],
    CONF_CELL_REQUEST_FLOAT_VOLTAGE: [0x00, 0x0A, 0x0A, 1000.0, 4],
    CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME: [0x00, 0x00, 0xB3, 10.0, 1],
    CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME: [0x00, 0x00, 0xB4, 10.0, 1],
    CONF_CELL_COUNT: [0x00, 0x1C, 0x1C, 1.0, 4],
    CONF_TOTAL_BATTERY_CAPACITY: [0x00, 0x20, 0x20, 1000.0, 4],
    CONF_BALANCE_STARTING_VOLTAGE: [0x00, 0x26, 0x22, 1000.0, 4],
    CONF_VOLTAGE_CALIBRATION: [0x00, 0x21, 0x64, 1000.0, 4],
    CONF_CURRENT_CALIBRATION: [0x00, 0x24, 0x67, 1000.0, 4],
    CONF_POWER_OFF_VOLTAGE: [0x00, 0x0B, 0x0B, 1000.0, 4],
    CONF_MAX_BALANCE_CURRENT: [0x00, 0x13, 0x13, 1000.0, 4],
    CONF_MAX_CHARGE_CURRENT: [0x00, 0x0C, 0x0C, 1000.0, 4],
    CONF_MAX_DISCHARGE_CURRENT: [0x00, 0x0F, 0x0F, 1000.0, 4],
    CONF_CHARGE_OVERCURRENT_PROTECTION_DELAY: [0x00, 0x0D, 0x0D, 1.0, 4],
    CONF_CHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME: [
        0x00,
        0x0E,
        0x0E,
        1.0,
        4,
    ],
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_DELAY: [
        0x00,
        0x10,
        0x10,
        1.0,
        4,
    ],
    CONF_DISCHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME: [
        0x00,
        0x11,
        0x11,
        1.0,
        4,
    ],
    CONF_SHORT_CIRCUIT_PROTECTION_DELAY: [0x00, 0x25, 0x21, 1.0, 4],
    CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_TIME: [
        0x00,
        0x12,
        0x12,
        1.0,
        4,
    ],
    CONF_CHARGE_OVERTEMPERATURE_PROTECTION: [0x00, 0x14, 0x14, 10.0, 4],
    CONF_CHARGE_OVERTMPERATURE_PROTECTION_RECOVERY: [
        0x00,
        0x15,
        0x15,
        10.0,
        4,
    ],
    CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION: [
        0x00,
        0x16,
        0x16,
        10.0,
        4,
    ],
    CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION_RECOVERY: [
        0x00,
        0x17,
        0x17,
        10.0,
        4,
    ],
    CONF_CHARGE_UNDERTEMPERATURE_PROTECTION: [
        0x00,
        0x18,
        0x18,
        10.0,
        4,
    ],
    CONF_CHARGE_UNDERTMPERATURE_PROTECTION_RECOVERY: [
        0x00,
        0x19,
        0x19,
        10.0,
        4,
    ],
    CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION: [
        0x00,
        0x1A,
        0x1A,
        10.0,
        4,
    ],
    CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION_RECOVERY: [
        0x00,
        0x1B,
        0x1B,
        10.0,
        4,
    ],
}

JkNumber = jk_bms_ble_ns.class_("JkNumber", number.Number, cg.Component)

JK_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(JkNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES, upper=True),
        cv.Optional(
            CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
        ): cv.entity_category,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SMART_SLEEP_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3.650): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.350): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_BALANCE_TRIGGER_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_SOC100_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3.650): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_SOC0_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3.650): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3.650): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=3.650): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_REQUEST_CHARGE_VOLTAGE_TIME): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=25.5): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_HOUR
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_CELL_REQUEST_FLOAT_VOLTAGE_TIME): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=25.5): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_HOUR
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_CELL_COUNT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=24): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_TOTAL_BATTERY_CAPACITY): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=5): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=2000): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE_HOUR
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_BALANCE_STARTING_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.20): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.25): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_VOLTAGE_CALIBRATION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_CURRENT_CALIBRATION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1000.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_POWER_OFF_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.20): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.35): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_MAX_BALANCE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.3): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=10.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_MAX_CHARGE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600.1): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_MAX_DISCHARGE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600.1): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_CHARGE_OVERCURRENT_PROTECTION_DELAY): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_CHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_DISCHARGE_OVERCURRENT_PROTECTION_DELAY
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_DISCHARGE_OVERCURRENT_PROTECTION_RECOVERY_TIME
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_DELAY): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=10000000): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_MICROSECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_SHORT_CIRCUIT_PROTECTION_RECOVERY_TIME
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=2): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=600): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECONDS
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_CHARGE_OVERTEMPERATURE_PROTECTION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=80): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_CHARGE_OVERTMPERATURE_PROTECTION_RECOVERY
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=80): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=80): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_DISCHARGE_OVERTEMPERATURE_PROTECTION_RECOVERY
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=80): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_CHARGE_UNDERTEMPERATURE_PROTECTION): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=-30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=20): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_CHARGE_UNDERTMPERATURE_PROTECTION_RECOVERY
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=-30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=20): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
        cv.Optional(
            CONF_POWER_TUBE_OVERTEMPERATURE_PROTECTION_RECOVERY
        ): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=30): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CELSIUS
                ): cv.string_strict,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    for key, address in NUMBERS.items():
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
            cg.add(var.set_jk04_holding_register(address[0]))
            cg.add(var.set_jk02_holding_register(address[1]))
            cg.add(var.set_jk02_32s_holding_register(address[2]))
            cg.add(var.set_factor(address[3]))
            cg.add(var.set_length(address[4]))
