import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv

from .. import CONF_JK_BMS_BLE_ID, JK_BMS_BLE_COMPONENT_SCHEMA, jk_bms_ble_ns
from ..const import (
    CONF_CAN_PROTOCOL,
    CONF_DRY1_TRIGGER,
    CONF_DRY2_TRIGGER,
    CONF_LCD_BUZZER_TRIGGER,
    CONF_LOAD_CONFIG_PRESET,
    CONF_MULTIPLEXED_PORT_MODE,
    CONF_UART1_PROTOCOL,
    CONF_UART2_PROTOCOL,
    CONF_UART3_PROTOCOL,
)

DEPENDENCIES = ["jk_bms_ble"]

CODEOWNERS = ["@syssi"]

UART_PROTOCOL_OPTIONS = [
    "4G-GPS Remote module Common protocol V4.2",
    "JK BMS RS485 Modbus V1.0",
    "NIU U SERIES",
    "China tower shared battery cabinet V1.1",
    "PACE_RS485_Modbus_V1.3",
    "PYLON_low_voltage_Protocol_RS485_V3.5",
    "Growatt_BMS_RS485_Protocol_1xSxxP_ESS_Rev2.01",
    "Voltronic_Inverter_and_BMS_485_communication_protocol_20200325",
    "China tower shared battery cabinet V2.0",
    "WOW_RS485_Modbus_V1.3",
    "JK BMS LCD Protocol V2.0",
    "UART1 User customization",
    "UART2 User customization",
    "(9600) JK BMS RS485 Modbus V1.0",
    "(9600) PYLON_low_voltage_Protocol_RS485_V3.5",
    "JK BMS PBxx SERIES LCD Protocol V1.0",
    "JK BMS LIN BUS V1.0",
    "RS485 Protocol 17",
    "RS485 Protocol 18",
    "RS485 Protocol 19",
    "RS485 Protocol 20",
]

CAN_PROTOCOL_OPTIONS = [
    "JK BMS CAN Protocol (250K) V2.0",
    "Deye Low-voltage hybrid inverter CAN communication protocol V1.0",
    "PYLON-Low-voltage-V1.2",
    "Growatt BMS CAN-Bus-protocol-low-voltage_Rev_05",
    "Victron_CANbus_BMS_protocol_20170717",
    "MEGAREVO_Hybird_BMSCAN_Protocol_V1.0",
    "JK BMS CAN Protocol (500K) V2.0",
    "INVT BMS CAN Bus protocol V1.02",
    "GoodWe LV BMS Protocol (EX/EM/S-BP/BP)",
    "FSS-ConnectingBat-TI-en-10 Version 1.0",
    "MUST PV1800F-CAN communication Protocol1.04.04",
    "LuxpowerTek Battery CAN protocol V01",
    "CAN BUS User customization 1",
    "CAN BUS User customization 2",
]

LCD_BUZZER_TRIGGER_OPTIONS = [
    "Off",
    "Low SoC",
    "Battery Over Voltage",
    "Battery Under Voltage",
    "Battery Cell Over Voltage",
    "Battery Cell Under Voltage",
    "Charge Over Current",
    "Discharge Over Current",
    "Battery Over Temperature",
    "MOSFET Over Temperature",
    "System Alarm",
    "Battery Low Temperature",
    "Remote Control",
    "Above SoC",
    "MOSFET Abnormal",
]

# JK02_32S: index 0 = CAN, index 1 = RS485 (register 0x2A, 4-byte write)
MULTIPLEXED_PORT_MODE_OPTIONS = [
    "CAN",
    "RS485",
]

# Maps config key → (register, options, table setter, constexpr array name, data_len).
# Entries sharing the same arr_name also share the same LookupTable in C++;
# emitted_arrays in to_code() ensures the constexpr array is defined only once.
_UART_ARR = "JK_BMS_BLE_UART_PROTOCOL_OPTS_"
_CAN_ARR = "JK_BMS_BLE_CAN_PROTOCOL_OPTS_"
_LCD_ARR = "JK_BMS_BLE_LCD_BUZZER_TRIGGER_OPTS_"
_MULTIPLEXED_PORT_MODE_ARR = "JK_BMS_BLE_MULTIPLEXED_PORT_MODE_OPTS_"

SELECTS = {
    CONF_UART1_PROTOCOL: (
        0xA5,
        UART_PROTOCOL_OPTIONS,
        "set_uart_protocol_table",
        _UART_ARR,
        0x02,
    ),
    CONF_UART2_PROTOCOL: (
        0xA8,
        UART_PROTOCOL_OPTIONS,
        "set_uart_protocol_table",
        _UART_ARR,
        0x02,
    ),
    CONF_UART3_PROTOCOL: (
        0xB6,
        UART_PROTOCOL_OPTIONS,
        "set_uart_protocol_table",
        _UART_ARR,
        0x02,
    ),
    CONF_CAN_PROTOCOL: (
        0xA6,
        CAN_PROTOCOL_OPTIONS,
        "set_can_protocol_table",
        _CAN_ARR,
        0x02,
    ),
    CONF_LCD_BUZZER_TRIGGER: (
        0xA9,
        LCD_BUZZER_TRIGGER_OPTIONS,
        "set_lcd_buzzer_trigger_table",
        _LCD_ARR,
        0x02,
    ),
    CONF_DRY1_TRIGGER: (
        0xAA,
        LCD_BUZZER_TRIGGER_OPTIONS,
        "set_lcd_buzzer_trigger_table",
        _LCD_ARR,
        0x02,
    ),
    CONF_DRY2_TRIGGER: (
        0xAB,
        LCD_BUZZER_TRIGGER_OPTIONS,
        "set_lcd_buzzer_trigger_table",
        _LCD_ARR,
        0x02,
    ),
    CONF_MULTIPLEXED_PORT_MODE: (
        0x2A,
        MULTIPLEXED_PORT_MODE_OPTIONS,
        "set_multiplexed_port_mode_table",
        _MULTIPLEXED_PORT_MODE_ARR,
        0x04,
    ),
}

# JK02_32S: each chemistry loads via a dedicated register with data_len=0
LOAD_CONFIG_PRESET_OPTIONS = {
    "Li-Ion": 0x68,
    "LiFePO4": 0x69,
    "LTO": 0x6A,
}

JkSelect = jk_bms_ble_ns.class_("JkSelect", select.Select, cg.Component)
JkPresetSelect = jk_bms_ble_ns.class_("JkPresetSelect", select.Select, cg.Component)

CONFIG_SCHEMA = JK_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_UART1_PROTOCOL): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_UART2_PROTOCOL): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_UART3_PROTOCOL): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_CAN_PROTOCOL): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_LCD_BUZZER_TRIGGER): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_DRY1_TRIGGER): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_DRY2_TRIGGER): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_MULTIPLEXED_PORT_MODE): select.select_schema(JkSelect).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_LOAD_CONFIG_PRESET): select.select_schema(
            JkPresetSelect
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_JK_BMS_BLE_ID])
    emitted_arrays = set()
    for key, (address, options, table_setter, arr_name, data_len) in SELECTS.items():
        if key not in config:
            continue
        conf = config[key]
        var = await select.new_select(conf, options=options)
        await cg.register_component(var, conf)
        cg.add(getattr(hub, f"set_{key}_select")(var))
        cg.add(var.set_parent(hub))
        cg.add(var.set_holding_register(address))
        cg.add(var.set_data_len(data_len))

        # Emit a static constexpr lookup table in flash and hand a pointer to the hub.
        # This keeps the option strings as the single source of truth in Python while
        # preserving flash (.rodata) placement on embedded targets.
        # emitted_arrays guards against double-definition when two selects share a table
        # (e.g. uart1_protocol and uart2_protocol both use UART_PROTOCOL_OPTIONS).
        if arr_name not in emitted_arrays:
            emitted_arrays.add(arr_name)
            entries = ", ".join(f'"{o}"' for o in options)
            cg.add_global(
                cg.RawStatement(
                    f"static constexpr const char *const {arr_name}[] = {{{entries}}};"
                )
            )
        cg.add(getattr(hub, table_setter)(cg.RawExpression(arr_name), len(options)))

    if CONF_LOAD_CONFIG_PRESET in config:
        conf = config[CONF_LOAD_CONFIG_PRESET]
        var = await select.new_select(
            conf, options=[""] + list(LOAD_CONFIG_PRESET_OPTIONS)
        )
        await cg.register_component(var, conf)
        cg.add(var.set_parent(hub))
        for opt, reg in LOAD_CONFIG_PRESET_OPTIONS.items():
            cg.add(var.add_option_register(opt, reg))
