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
    UNIT_EMPTY,
    UNIT_VOLT,
)

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

# 06 04 03000000  Set balance trig voltage to 0.003
# 1c 04 10000000  Set cell count to 16
# 20 04 f0ba0400  Set battery cap to 310
# 04 04 420e0000  Set Cell OVP to 3.65
# 05 04 b0040000  Set Cell OVPR to 1.2
# 02 04 b0040000  Set Cell UVP to 1.2
# 03 04 b0040000  Set Cell UVPR to 1.2
# 26 04 b0040000  Set Start Balance voltage to 1.2
# 21 04 42d10000  Set cal voltage to 53.57
# 24 04 64000000  Set cal current to 0.100
# 0b 04 b0040000  Set Power Off value to 1.2
# 13 04 2c010000  Set max Balance current to 0.3
# 0c 04 e8030000  Set Max charge Current to 1.0
# 0f 04 e8030000  Set Max discharge Current to 1.0

CONF_BALANCE_TRIGGER_VOLTAGE = "balance_trigger_voltage"
CONF_CELL_COUNT = "cell_count"
CONF_TOTAL_BATTERY_CAPACITY = "total_battery_capacity"
CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION = "cell_voltage_overvoltage_protection"
CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY = "cell_voltage_overvoltage_recovery"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION = "cell_voltage_undervoltage_protection"
CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY = "cell_voltage_undervoltage_recovery"
CONF_BALANCE_STARTING_VOLTAGE = "balance_starting_voltage"
CONF_VOLTAGE_CALIBRATION = "voltage_calibration"
CONF_CURRENT_CALIBRATION = "current_calibration"
CONF_POWER_OFF_VOLTAGE = "power_off_voltage"
CONF_MAX_BALANCE_CURRENT = "max_balance_current"
CONF_MAX_CHARGE_CURRENT = "max_charge_current"
CONF_MAX_DISCHARGE_CURRENT = "max_discharge_current"

# 0d 04 02000000  Set Charge OCP delay to 2
# 0e 04 02000000  Set Charge OCPR Time to 2
# 10 04 02000000  Set Discharge OCP delay to 2
# 11 04 02000000  Set Discharge OCPR Time to 2
# 25 04 0a000000  Set to SCP Delay to 10.000
# 12 04 02000000  Set SCPR Time to 2
# 14 04 2c010000  Set Charge OTP to 30
# 15 04 2c010000  Set Charge OTPR to 30
# 16 04 2c010000  Set Discharge OTP to 30
# 17 04 2c010000  Set Discharge OTPR to 30
# 18 04 00000000  Set Charge UTP to 0
# 19 04 00000000  Set Charge UTPR to 0

# 9f 04 54657374  Set User Private Data (4 bytes) to TEST
# 9f 0d 30313233343536373839303132 c6  Set User Private Data (4 bytes) to 0123456789012
# 27 04 00000000  Set Con. Wire Res.01 to 0.00
# ...
# 3e 04 00000000  Set Con. Wire Res.24 to 0.00
#
# https://github.com/jblance/mpp-solar/issues/170#issuecomment-1050503970

UNIT_AMPERE_HOUR = "Ah"

NUMBERS = {
    # JK02 register, JK04 register, factor
    CONF_BALANCE_TRIGGER_VOLTAGE: [0x06, 0x00, 1000.0],
    CONF_CELL_COUNT: [0x1C, 0x00, 1.0],
    CONF_TOTAL_BATTERY_CAPACITY: [0x20, 0x00, 1000.0],
    CONF_CELL_VOLTAGE_OVERVOLTAGE_PROTECTION: [0x04, 0x00, 1000.0],
    CONF_CELL_VOLTAGE_OVERVOLTAGE_RECOVERY: [0x05, 0x00, 1000.0],
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION: [0x02, 0x00, 1000.0],
    CONF_CELL_VOLTAGE_UNDERVOLTAGE_RECOVERY: [0x03, 0x00, 1000.0],
    CONF_BALANCE_STARTING_VOLTAGE: [0x26, 0x00, 1000.0],
    CONF_VOLTAGE_CALIBRATION: [0x21, 0x00, 1000.0],
    CONF_CURRENT_CALIBRATION: [0x24, 0x00, 1000.0],
    CONF_POWER_OFF_VOLTAGE: [0x0B, 0x00, 1000.0],
    CONF_MAX_BALANCE_CURRENT: [0x13, 0x00, 1000.0],
    CONF_MAX_CHARGE_CURRENT: [0x0C, 0x00, 1000.0],
    CONF_MAX_DISCHARGE_CURRENT: [0x0F, 0x00, 1000.0],
}

JkNumber = jk_bms_ble_ns.class_("JkNumber", number.Number, cg.Component)

JK_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(JkNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): number.icon,
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
        cv.Optional(CONF_BALANCE_TRIGGER_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.003): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_CELL_COUNT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=8): cv.float_,
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
        cv.Optional(CONF_CELL_VOLTAGE_UNDERVOLTAGE_PROTECTION): JK_NUMBER_SCHEMA.extend(
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
        cv.Optional(CONF_BALANCE_STARTING_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.20): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.25): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_VOLTAGE_CALIBRATION): JK_NUMBER_SCHEMA.extend(
            {
                # @FIXME The exact limits are unknown
                cv.Optional(CONF_MIN_VALUE, default=10.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=100.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_CURRENT_CALIBRATION): JK_NUMBER_SCHEMA.extend(
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
        cv.Optional(CONF_POWER_OFF_VOLTAGE): JK_NUMBER_SCHEMA.extend(
            {
                # @FIXME The upper limit is unknown
                cv.Optional(CONF_MIN_VALUE, default=1.20): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.25): cv.float_,
                cv.Optional(CONF_STEP, default=0.01): cv.float_,
            }
        ),
        cv.Optional(CONF_MAX_BALANCE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.3): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=5.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_MAX_CHARGE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_MAX_DISCHARGE_CURRENT): JK_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=200.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
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
            cg.add(var.set_jk02_holding_register(address[0]))
            cg.add(var.set_jk04_holding_register(address[1]))
            cg.add(var.set_factor(address[2]))
