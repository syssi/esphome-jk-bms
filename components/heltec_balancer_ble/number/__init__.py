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

from .. import (
    CONF_HELTEC_BALANCER_BLE_ID,
    HELTEC_BALANCER_BLE_COMPONENT_SCHEMA,
    heltec_balancer_ble_ns,
)

DEPENDENCIES = ["heltec_balancer_ble"]

CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

CONF_CELL_COUNT = "cell_count"
CONF_BALANCE_TRIGGER_VOLTAGE = "balance_trigger_voltage"
CONF_MAX_BALANCE_CURRENT = "max_balance_current"
CONF_BALANCE_SLEEP_VOLTAGE = "balance_sleep_voltage"
CONF_BALANCE_START_VOLTAGE = "balance_start_voltage"
CONF_NOMINAL_BATTERY_CAPACITY = "nominal_battery_capacity"
# CONF_BUZZER_MODE = "buzzer_mode"
# CONF_BATTERY_TYPE = "battery_type"

UNIT_AMPERE_HOUR = "Ah"

# Config settings
#
# single_num     Cell count [1,24]: Set 1                              aa55110005 01 1400 01000000 000000000000ffff
# triger_mpa     Balancing Trigger Delta [0.001d, 1.0d]: Set 1.0f      aa55110005 02 1400 0000803f 00000000000042ff
# max_cur        Max balancing current [1.0f, 4.0f]: Set 1.0f          aa55110005 03 1400 0000803f 00000000000043ff
# auto_close     Balancing stop voltage [1.0f, 4.5f]: Set 1.0f         aa55110005 04 1400 0000803f 00000000000044ff
# auto_open      Balancing start voltage [1.0f, 4.5f]: Set 1.0f        aa55110005 17 1400 0000803f 00000000000057ff
# volume         Capacity [1.0f, 2000.0f]: Set 1                       aa55110005 16 1400 01000000 000000000000e8ff
# alarm_mode     Buzzer mode {1, 2, 3, 4}: Set 1                       aa55110005 14 1400 01000000 000000000000eaff
# bat_mode       Battery type {1, 2, 3}: Set 2                         aa55110005 15 1400 02000000 000000000000e8ff
#                Change device name: Set "test"                        aa55110005 13 1400 74657374000000000000 faff
#
# Factory defaults
#
# standardVol2   ReferenceVoltage [0.001f, 5.0f]: Set 1.0f             aa55110005 05 1400 0000803f 00000000000045ff
# battery_vol    BatteryVoltage [0.001f, 5.0f]: Set 1.0f               aa55110005 06 1400 0000803f 00000000000046ff
# standardCur2   Balancing Current Default? [0.001f, 5.0f]: Set 1.0f   aa55110005 07 1400 0000803f 00000000000047ff
# superBat2      Mean SuperCap Voltage [0.001f, 5.0f]: Set 1.0f        aa55110005 0e 1400 0000803f 0000000000004eff
# triger_mpa     StartVol(V) [0.001f, 5.0f]: Set 1.0f                  aa55110005 08 1400 0000803f 00000000000048ff
# open_num       Boot count []: Set 1.0f                               aa55110005 09 1400 0000803f 00000000000049ff
# batStatu       RefBat Vol [0.001f, 5.0f]: Set 1.0f                   aa55110005 0f 1400 0000803f 0000000000004fff
# battery_max    BatMax [0.001f, 5.0f]: Set 1.0f                       aa55110005 0b 1400 0000803f 0000000000004bff
# battery_min    BatMin [0.001f, 5.0f]: Set 1.0f                       aa55110005 0c 1400 0000803f 0000000000004cff
# ntc_max        NtcMax [-19.9f, 120.0f]: Set 1.0f                     aa55110005 11 1400 0000803f 00000000000051ff
# ntc_min        NtcMin [-19.9f, 120.0f]: Set 1.0f                     aa55110005 12 1400 0000803f 00000000000052ff
# total_time     Working time []: Set 1                                aa55110005 0a 1400 01000000 000000000000f4ff
# cycle          Production date: Set 20220802                         aa55110005 10 1400 3230323230383032 0000e7ff

NUMBERS = {
    CONF_CELL_COUNT: 0x01,
    CONF_BALANCE_TRIGGER_VOLTAGE: 0x02,
    CONF_MAX_BALANCE_CURRENT: 0x03,
    CONF_BALANCE_SLEEP_VOLTAGE: 0x04,
    CONF_BALANCE_START_VOLTAGE: 0x17,
    CONF_NOMINAL_BATTERY_CAPACITY: 0x16,
    # CONF_BUZZER_MODE: 0x14,
    # CONF_BATTERY_TYPE: 0x15,
}

HeltecNumber = heltec_balancer_ble_ns.class_(
    "HeltecNumber", number.Number, cg.Component
)

HELTEC_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HeltecNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): number.icon,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES, upper=True),
        cv.Optional(
            CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
        ): cv.entity_category,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = HELTEC_BALANCER_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CELL_COUNT): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=8): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=24): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_EMPTY
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_BALANCE_TRIGGER_VOLTAGE): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=0.001): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_MAX_BALANCE_CURRENT): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.0): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_BALANCE_SLEEP_VOLTAGE): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.5): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_BALANCE_START_VOLTAGE): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1.0): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=4.5): cv.float_,
                cv.Optional(CONF_STEP, default=0.001): cv.float_,
            }
        ),
        cv.Optional(CONF_NOMINAL_BATTERY_CAPACITY): HELTEC_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=2000): cv.float_,
                cv.Optional(CONF_STEP, default=1.0): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE_HOUR
                ): cv.string_strict,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_HELTEC_BALANCER_BLE_ID])
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
            cg.add(var.set_holding_register(address))
