"""Schema structure tests for jk_bms ESPHome component modules."""

from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).parent.parent))

import pytest  # noqa: E402
import voluptuous as vol  # noqa: E402

import components.heltec_balancer_ble as hub_heltec  # noqa: E402
from components.heltec_balancer_ble import (  # noqa: E402
    binary_sensor as heltec_binary_sensor,
    button as heltec_button,  # noqa: E402
    number as heltec_number,  # noqa: E402
    sensor as heltec_sensor,
    switch as heltec_switch,  # noqa: E402
    text_sensor as heltec_text_sensor,
)
import components.jk_balancer as hub_jk_balancer  # noqa: E402
from components.jk_balancer import (  # noqa: E402
    binary_sensor as jk_balancer_binary_sensor,
    sensor as jk_balancer_sensor,
    switch as jk_balancer_switch,  # noqa: E402
    text_sensor as jk_balancer_text_sensor,
)
import components.jk_bms as hub  # noqa: E402
from components.jk_bms import (  # noqa: E402
    binary_sensor,
    sensor,
    switch,  # noqa: E402
    text_sensor,
)
import components.jk_bms_ble as hub_ble  # noqa: E402
from components.jk_bms_ble import (  # noqa: E402
    binary_sensor as ble_binary_sensor,
    button as ble_button,  # noqa: E402
    number as ble_number,
    sensor as ble_sensor,
    switch as ble_switch,  # noqa: E402
    text_sensor as ble_text_sensor,
)
import components.jk_bms_ble.select as ble_select  # noqa: E402
import components.jk_bms_display as hub_display  # noqa: E402
from components.jk_bms_display import (  # noqa: E402
    binary_sensor as display_binary_sensor,
    sensor as display_sensor,
)


class TestHubConstants:
    def test_conf_ids_defined(self):
        assert hub.CONF_JK_BMS_ID == "jk_bms_id"
        assert hub_ble.CONF_JK_BMS_BLE_ID == "jk_bms_ble_id"
        assert hub_display.CONF_JK_BMS_DISPLAY_ID == "jk_bms_display_id"
        assert hub_jk_balancer.CONF_JK_BALANCER_ID == "jk_balancer_id"
        assert hub_heltec.CONF_HELTEC_BALANCER_BLE_ID == "heltec_balancer_ble_id"


class TestJkBmsSensorLists:
    def test_cells_count(self):
        assert len(sensor.CELLS) == 24

    def test_cells_naming(self):
        assert sensor.CELLS[0] == "cell_voltage_1"
        assert sensor.CELLS[23] == "cell_voltage_24"
        for i, key in enumerate(sensor.CELLS, 1):
            assert key == f"cell_voltage_{i}"

    def test_sensor_defs_completeness(self):
        assert "total_voltage" in sensor.SENSOR_DEFS
        assert sensor.CONF_MIN_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_MAX_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_DELTA_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 58


class TestJkBmsBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        from components.jk_bms.const import CONF_CHARGING, CONF_DISCHARGING

        assert CONF_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert CONF_DISCHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_BALANCING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_ONLINE_STATUS in binary_sensor.BINARY_SENSOR_DEFS
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 6


class TestJkBmsTextSensorConstants:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_ERRORS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_OPERATION_MODE in text_sensor.TEXT_SENSORS
        assert len(text_sensor.TEXT_SENSORS) == 8


class TestJkBmsSwitchConstants:
    def test_switches_dict(self):
        from components.jk_bms.const import CONF_CHARGING, CONF_DISCHARGING

        assert CONF_CHARGING in switch.SWITCHES
        assert CONF_DISCHARGING in switch.SWITCHES
        assert switch.CONF_BALANCER in switch.SWITCHES
        assert len(switch.SWITCHES) == 3


class TestJkBmsErrorOverrides:
    def test_default_errors_cover_all_16_bits(self):
        assert len(hub.DEFAULT_ERRORS) == 16

    def test_default_errors_are_immutable(self):
        # to_code() patches a copy per hub; a mutable default would let one hub's
        # error_overrides leak into the next one.
        assert isinstance(hub.DEFAULT_ERRORS, tuple)

    def test_default_errors_labels(self):
        assert hub.DEFAULT_ERRORS[0] == "Low capacity"
        assert hub.DEFAULT_ERRORS[10] == "Cell overvoltage"
        assert hub.DEFAULT_ERRORS[13] == "309_A protection"

    def test_reserved_bits_are_empty(self):
        for bit in (14, 15):
            assert hub.DEFAULT_ERRORS[bit] == ""

    @staticmethod
    def _validate(overrides):
        config = hub.CONFIG_SCHEMA({"error_overrides": overrides})
        return config["error_overrides"]

    def test_schema_accepts_bit_index_as_string(self):
        # ESPHome's YAML loader turns every mapping key into a string.
        assert self._validate({"0": "Low SoC"}) == {0: "Low SoC"}

    def test_schema_accepts_boundary_bits(self):
        assert self._validate({"0": "a", "15": "b"}) == {0: "a", 15: "b"}

    def test_schema_accepts_empty_label(self):
        assert self._validate({"0": ""}) == {0: ""}

    def test_schema_rejects_out_of_range_bit(self):
        with pytest.raises(vol.Invalid, match="Error bit 16 is out of range"):
            self._validate({"16": "Nope"})
        with pytest.raises(vol.Invalid, match="Error bit -1 is out of range"):
            self._validate({"-1": "Nope"})

    @pytest.mark.parametrize("alias", ["01", "0x1", " 1", "+1"])
    def test_schema_rejects_non_decimal_bit_spelling(self, alias):
        with pytest.raises(
            vol.Invalid, match=f"Write error bit '{re.escape(alias)}' as 1"
        ):
            self._validate({"1": "a", alias: "b"})

    def test_schema_rejects_aliased_suppressed_bit(self):
        with pytest.raises(vol.Invalid, match="Write error bit '0x1' as 1"):
            self._validate({"1": "", "0x1": ""})

    def test_schema_rejects_non_numeric_bit(self):
        with pytest.raises(vol.Invalid, match="'abc' is not a valid error bit"):
            self._validate({"abc": "Nope"})

    def test_schema_rejects_non_string_label(self):
        with pytest.raises(vol.Invalid, match="Must be string"):
            self._validate({"0": 42})

    def test_error_overrides_is_optional(self):
        assert hub.CONF_ERROR_OVERRIDES not in hub.CONFIG_SCHEMA({})


class TestJkBmsBleSensorLists:
    def test_cells_count(self):
        assert len(ble_sensor.CELL_VOLTAGES) == 32
        assert len(ble_sensor.CELL_RESISTANCES) == 32

    def test_temperatures_count(self):
        assert len(ble_sensor.TEMPERATURES) == 5

    def test_temperatures_naming(self):
        assert ble_sensor.TEMPERATURES[0] == "temperature_sensor_1"
        assert ble_sensor.TEMPERATURES[4] == "temperature_sensor_5"
        for i, key in enumerate(ble_sensor.TEMPERATURES, 1):
            assert key == f"temperature_sensor_{i}"

    def test_sensor_defs_completeness(self):
        assert "total_voltage" in ble_sensor.SENSOR_DEFS
        assert "full_charge_capacity" in ble_sensor.SENSOR_DEFS
        assert "balancer_status_bitmask" in ble_sensor.SENSOR_DEFS
        assert "detail_log_entry_count" in ble_sensor.SENSOR_DEFS
        assert "battery_type_id" in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_POWER_ON_COUNT in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_DEVICE_ADDRESS in ble_sensor.SENSOR_DEFS
        for key in (
            "discharge_overcurrent_protection_recovery_countdown",
            "discharge_short_circuit_protection_recovery_countdown",
            "charge_overcurrent_protection_recovery_countdown",
            "charge_short_circuit_protection_recovery_countdown",
            "cell_undervoltage_protection_recovery_countdown",
            "cell_overvoltage_protection_recovery_countdown",
        ):
            assert key in ble_sensor.SENSOR_DEFS
        assert len(ble_sensor.SENSOR_DEFS) == 40


class TestJkBmsBleErrorOverrides:
    def test_default_errors_cover_all_32_bits(self):
        assert len(hub_ble.DEFAULT_ERRORS_JK02) == 32

    def test_default_errors_are_immutable(self):
        # to_code() patches a copy per hub; a mutable default would let one hub's
        # error_overrides leak into the next one.
        assert isinstance(hub_ble.DEFAULT_ERRORS_JK02, tuple)

    def test_default_errors_labels(self):
        assert hub_ble.DEFAULT_ERRORS_JK02[0] == "Wire resistance"
        assert hub_ble.DEFAULT_ERRORS_JK02[4] == "Battery is fully charged"
        assert hub_ble.DEFAULT_ERRORS_JK02[28] == "GPS remote lock"

    def test_unused_bits_are_empty(self):
        for bit in (3, 29, 30, 31):
            assert hub_ble.DEFAULT_ERRORS_JK02[bit] == ""

    @staticmethod
    def _validate(overrides):
        config = hub_ble.CONFIG_SCHEMA(
            {
                "protocol_version": "JK02_32S",
                "ble_client_id": "client0",
                "error_overrides": overrides,
            }
        )
        return config["error_overrides"]

    def test_schema_accepts_bit_index_as_string(self):
        # ESPHome's YAML loader turns every mapping key into a string.
        assert self._validate({"4": "Cell overvoltage"}) == {4: "Cell overvoltage"}

    def test_schema_accepts_boundary_bits(self):
        assert self._validate({"0": "a", "31": "b"}) == {0: "a", 31: "b"}

    def test_schema_accepts_empty_label(self):
        assert self._validate({"4": ""}) == {4: ""}

    def test_schema_rejects_quoted_hex_bit_index(self):
        # Unquoted 0x1F arrives as "31" from the YAML loader; only the quoted
        # spelling reaches the validator and would alias bit 31.
        with pytest.raises(vol.Invalid, match="Write error bit '0x1F' as 31"):
            self._validate({"0x1F": "Reserved"})

    def test_schema_rejects_out_of_range_bit(self):
        with pytest.raises(vol.Invalid, match="Error bit 32 is out of range"):
            self._validate({"32": "Nope"})
        with pytest.raises(vol.Invalid, match="Error bit -1 is out of range"):
            self._validate({"-1": "Nope"})

    @pytest.mark.parametrize("alias", ["01", "0x1", " 1", "+1"])
    def test_schema_rejects_non_decimal_bit_spelling(self, alias):
        with pytest.raises(
            vol.Invalid, match=f"Write error bit '{re.escape(alias)}' as 1"
        ):
            self._validate({"1": "a", alias: "b"})

    def test_schema_rejects_aliased_suppressed_bit(self):
        with pytest.raises(vol.Invalid, match="Write error bit '0x1' as 1"):
            self._validate({"1": "", "0x1": ""})

    def test_schema_rejects_non_numeric_bit(self):
        with pytest.raises(vol.Invalid, match="'abc' is not a valid error bit"):
            self._validate({"abc": "Nope"})

    def test_schema_rejects_non_string_label(self):
        with pytest.raises(vol.Invalid, match="Must be string"):
            self._validate({"4": 42})

    def test_error_overrides_is_optional(self):
        config = hub_ble.CONFIG_SCHEMA(
            {"protocol_version": "JK02_32S", "ble_client_id": "client0"}
        )

        assert hub_ble.CONF_ERROR_OVERRIDES not in config


class TestJkBmsBleBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        from components.jk_bms_ble.const import (
            CONF_BALANCING,
            CONF_CHARGING,
            CONF_DISCHARGING,
            CONF_HEATING,
        )

        assert CONF_CHARGING in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert CONF_DISCHARGING in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert CONF_BALANCING in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert CONF_HEATING in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert (
            ble_binary_sensor.CONF_ONLINE_STATUS in ble_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert (
            ble_binary_sensor.CONF_PRECHARGING in ble_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert len(ble_binary_sensor.BINARY_SENSOR_DEFS) == 8


class TestJkBmsBleSelectConstants:
    def test_selects_count(self):
        assert len(ble_select.SELECTS) == 8

    def test_selects_keys(self):
        assert "uart1_protocol" in ble_select.SELECTS
        assert "uart2_protocol" in ble_select.SELECTS
        assert "uart3_protocol" in ble_select.SELECTS
        assert "can_protocol" in ble_select.SELECTS
        assert "lcd_buzzer_trigger" in ble_select.SELECTS
        assert "dry1_trigger" in ble_select.SELECTS
        assert "dry2_trigger" in ble_select.SELECTS
        assert "multiplexed_port_mode" in ble_select.SELECTS

    def test_selects_registers_are_unique(self):
        registers = [v[2] for v in ble_select.SELECTS.values()]
        assert len(registers) == len(set(registers))

    def test_selects_registers(self):
        assert ble_select.SELECTS["uart1_protocol"][2] == 0xA5
        assert ble_select.SELECTS["uart2_protocol"][2] == 0xA8
        assert ble_select.SELECTS["uart3_protocol"][2] == 0xB6
        assert ble_select.SELECTS["can_protocol"][2] == 0xA6
        assert ble_select.SELECTS["lcd_buzzer_trigger"][2] == 0xA9
        assert ble_select.SELECTS["dry1_trigger"][2] == 0xAA
        assert ble_select.SELECTS["dry2_trigger"][2] == 0xAB
        assert ble_select.SELECTS["multiplexed_port_mode"][2] == 0x2A

    def test_multiplexed_port_mode_data_len(self):
        assert ble_select.SELECTS["multiplexed_port_mode"][6] == 0x04

    def test_multiplexed_port_mode_options(self):
        assert ble_select.MULTIPLEXED_PORT_MODE_OPTIONS == ["CAN", "RS485"]

    def test_protocol_options_cover_indices_0_to_20(self):
        # The BMS reports protocol numbers 0-20. An index without an option leaves
        # the select without a state and can't be selected.
        assert len(ble_select.UART_PROTOCOL_OPTIONS) == 21
        assert len(ble_select.CAN_PROTOCOL_OPTIONS) == 21

    def test_protocol_options_are_unique(self):
        for options in (
            ble_select.UART_PROTOCOL_OPTIONS,
            ble_select.CAN_PROTOCOL_OPTIONS,
        ):
            assert len(options) == len(set(options))

    def test_can_protocol_options_keep_existing_names(self):
        # Option strings are the select state, so existing entries must not change.
        options = ble_select.CAN_PROTOCOL_OPTIONS
        assert options[0] == "JK BMS CAN Protocol (250K) V2.0"
        assert options[3] == "Growatt BMS CAN-Bus-protocol-low-voltage_Rev_05"
        assert options[12] == "CAN BUS User customization 1"
        assert options[13] == "CAN BUS User customization 2"

    def test_can_protocol_options_14_to_20(self):
        options = ble_select.CAN_PROTOCOL_OPTIONS
        assert options[14] == "JINLONG Low-voltage inverter CAN Bus Protocol V1.0"
        assert options[15] == "NMEA 2000 V1.300"
        assert options[16:] == [f"CAN BUS Protocol {i}" for i in range(16, 21)]

    def test_uart_protocol_option_17(self):
        assert (
            ble_select.UART_PROTOCOL_OPTIONS[17]
            == "JK Active Balancer LCD Protocol V1.0"
        )


class TestJkBmsBleLoadConfigPresetConstants:
    def test_load_config_preset_options(self):
        assert len(ble_select.LOAD_CONFIG_PRESET_OPTIONS) == 3
        assert "Li-Ion" in ble_select.LOAD_CONFIG_PRESET_OPTIONS
        assert "LiFePO4" in ble_select.LOAD_CONFIG_PRESET_OPTIONS
        assert "LTO" in ble_select.LOAD_CONFIG_PRESET_OPTIONS

    def test_load_config_preset_registers(self):
        assert ble_select.LOAD_CONFIG_PRESET_OPTIONS["Li-Ion"] == (0x00, 0x00, 0x68)
        assert ble_select.LOAD_CONFIG_PRESET_OPTIONS["LiFePO4"] == (0x00, 0x00, 0x69)
        assert ble_select.LOAD_CONFIG_PRESET_OPTIONS["LTO"] == (0x00, 0x00, 0x6A)

    def test_load_config_preset_registers_are_unique(self):
        registers = list(ble_select.LOAD_CONFIG_PRESET_OPTIONS.values())
        assert len(registers) == len(set(registers))


class TestJkBmsBleSwitchConstants:
    def test_switches_dict(self):
        assert len(ble_switch.SWITCHES) == 17


class TestJkBmsBleNumberConstants:
    def test_soc_soh_calibration_registers(self):
        assert ble_number.NUMBERS[ble_number.CONF_SOC_CALIBRATION][:-1] == [
            0x00,
            0x00,
            0x6E,
            1.0,
            1,
            0,
        ]
        assert ble_number.NUMBERS[ble_number.CONF_SOH_CALIBRATION][:-1] == [
            0x00,
            0x00,
            0x6F,
            1.0,
            1,
            0,
        ]


class TestJkBmsBleButtonConstants:
    def test_buttons_dict(self):
        assert ble_button.CONF_RETRIEVE_SETTINGS in ble_button.BUTTONS
        assert ble_button.CONF_RETRIEVE_DEVICE_INFO in ble_button.BUTTONS
        assert ble_button.CONF_RETRIEVE_LOGBOOK in ble_button.BUTTONS
        assert ble_button.CONF_SHUTDOWN in ble_button.BUTTONS
        assert ble_button.CONF_RESTART in ble_button.BUTTONS
        assert ble_button.CONF_FACTORY_RESET in ble_button.BUTTONS
        assert ble_button.CONF_HEATING_TEST in ble_button.BUTTONS
        assert ble_button.CONF_SYNC_TIME in ble_button.BUTTONS
        assert len(ble_button.BUTTONS) == 8

    def test_button_addresses_are_unique(self):
        addresses = [entry[2] for entry in ble_button.BUTTONS.values()]
        assert len(addresses) == len(set(addresses))

    def test_button_addresses(self):
        assert ble_button.BUTTONS[ble_button.CONF_SHUTDOWN][2] == 0x65
        assert ble_button.BUTTONS[ble_button.CONF_RESTART][2] == 0x66
        assert ble_button.BUTTONS[ble_button.CONF_FACTORY_RESET][2] == 0x9D
        assert ble_button.BUTTONS[ble_button.CONF_HEATING_TEST][2] == 0x6D


class TestJkBleTextSensorConstants:
    def test_text_sensors_list(self):
        assert ble_text_sensor.CONF_ERRORS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_BALANCER_STATUS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_CHARGE_STATUS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_BATTERY_TYPE in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_DEVICE_MODEL in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_DEVICE_NAME in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_MANUFACTURING_DATE in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_SERIAL_NUMBER in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_DEVICE_TIME in ble_text_sensor.TEXT_SENSORS
        assert (
            ble_text_sensor.CONF_SUPPORTED_COMMANDS_BITMASK_HEX
            in ble_text_sensor.TEXT_SENSORS
        )
        assert (
            ble_text_sensor.CONF_WIRE_RESISTANCE_WARNING_BITMASK_HEX
            in ble_text_sensor.TEXT_SENSORS
        )
        assert len(ble_text_sensor.TEXT_SENSORS) == 15


class TestJkBmsDisplaySensorLists:
    def test_cells_count(self):
        assert len(display_sensor.CELLS) == 24

    def test_sensor_defs_completeness(self):
        assert len(display_sensor.SENSOR_DEFS) == 14

    def test_binary_sensor_defs_dict(self):
        assert len(display_binary_sensor.BINARY_SENSOR_DEFS) == 13


class TestJkBalancerSensorLists:
    def test_cells_count(self):
        assert len(jk_balancer_sensor.CELLS) == 24

    def test_sensor_defs_completeness(self):
        assert len(jk_balancer_sensor.SENSOR_DEFS) == 12

    def test_binary_sensor_defs_dict(self):
        assert len(jk_balancer_binary_sensor.BINARY_SENSOR_DEFS) == 2

    def test_text_sensors_list(self):
        assert (
            jk_balancer_text_sensor.CONF_ERRORS in jk_balancer_text_sensor.TEXT_SENSORS
        )
        assert len(jk_balancer_text_sensor.TEXT_SENSORS) == 1

    def test_switches_dict(self):
        assert jk_balancer_switch.CONF_BALANCER in jk_balancer_switch.SWITCHES
        assert len(jk_balancer_switch.SWITCHES) == 1


class TestJkBalancerErrorOverrides:
    def test_default_errors_cover_all_8_bits(self):
        assert len(hub_jk_balancer.DEFAULT_ERRORS) == 8

    def test_default_errors_are_immutable(self):
        # to_code() patches a copy per hub; a mutable default would let one hub's
        # error_overrides leak into the next one.
        assert isinstance(hub_jk_balancer.DEFAULT_ERRORS, tuple)

    def test_default_errors_labels(self):
        assert hub_jk_balancer.DEFAULT_ERRORS[0] == "Wrong cell count"
        assert hub_jk_balancer.DEFAULT_ERRORS[1] == "Resistance too high"
        assert hub_jk_balancer.DEFAULT_ERRORS[2] == "Overvoltage"

    def test_reserved_bits_are_empty(self):
        for bit in range(3, 8):
            assert hub_jk_balancer.DEFAULT_ERRORS[bit] == ""

    @staticmethod
    def _validate(overrides):
        config = hub_jk_balancer.CONFIG_SCHEMA({"error_overrides": overrides})
        return config["error_overrides"]

    def test_schema_accepts_bit_index_as_string(self):
        # ESPHome's YAML loader turns every mapping key into a string.
        assert self._validate({"0": "Cell count mismatch"}) == {
            0: "Cell count mismatch"
        }

    def test_schema_accepts_boundary_bits(self):
        assert self._validate({"0": "a", "7": "b"}) == {0: "a", 7: "b"}

    def test_schema_accepts_empty_label(self):
        assert self._validate({"0": ""}) == {0: ""}

    def test_schema_rejects_out_of_range_bit(self):
        with pytest.raises(vol.Invalid, match="Error bit 8 is out of range"):
            self._validate({"8": "Nope"})
        with pytest.raises(vol.Invalid, match="Error bit -1 is out of range"):
            self._validate({"-1": "Nope"})

    @pytest.mark.parametrize("alias", ["01", "0x1", " 1", "+1"])
    def test_schema_rejects_non_decimal_bit_spelling(self, alias):
        with pytest.raises(
            vol.Invalid, match=f"Write error bit '{re.escape(alias)}' as 1"
        ):
            self._validate({"1": "a", alias: "b"})

    def test_schema_rejects_aliased_suppressed_bit(self):
        with pytest.raises(vol.Invalid, match="Write error bit '0x1' as 1"):
            self._validate({"1": "", "0x1": ""})

    def test_schema_rejects_non_numeric_bit(self):
        with pytest.raises(vol.Invalid, match="'abc' is not a valid error bit"):
            self._validate({"abc": "Nope"})

    def test_schema_rejects_non_string_label(self):
        with pytest.raises(vol.Invalid, match="Must be string"):
            self._validate({"0": 42})

    def test_error_overrides_is_optional(self):
        assert (
            hub_jk_balancer.CONF_ERROR_OVERRIDES
            not in hub_jk_balancer.CONFIG_SCHEMA({})
        )


class TestHeltecBalancerBleSensorLists:
    def test_sensor_defs_completeness(self):
        assert heltec_sensor.CONF_TOTAL_RUNTIME in heltec_sensor.SENSOR_DEFS
        assert heltec_sensor.CONF_POWER_ON_COUNT in heltec_sensor.SENSOR_DEFS
        assert len(heltec_sensor.SENSOR_DEFS) == 23

    def test_binary_sensor_defs_dict(self):
        assert (
            heltec_binary_sensor.CONF_BALANCING
            in heltec_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert (
            heltec_binary_sensor.CONF_ONLINE_STATUS
            in heltec_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert len(heltec_binary_sensor.BINARY_SENSOR_DEFS) == 5

    def test_text_sensors_list(self):
        assert heltec_text_sensor.CONF_ERRORS in heltec_text_sensor.TEXT_SENSORS
        assert heltec_text_sensor.CONF_BATTERY_TYPE in heltec_text_sensor.TEXT_SENSORS
        assert heltec_text_sensor.CONF_DEVICE_MODEL in heltec_text_sensor.TEXT_SENSORS
        assert (
            heltec_text_sensor.CONF_HARDWARE_VERSION in heltec_text_sensor.TEXT_SENSORS
        )
        assert (
            heltec_text_sensor.CONF_SOFTWARE_VERSION in heltec_text_sensor.TEXT_SENSORS
        )
        assert (
            heltec_text_sensor.CONF_PROTOCOL_VERSION in heltec_text_sensor.TEXT_SENSORS
        )
        assert (
            heltec_text_sensor.CONF_MANUFACTURING_DATE
            in heltec_text_sensor.TEXT_SENSORS
        )
        assert len(heltec_text_sensor.TEXT_SENSORS) == 10

    def test_buttons_dict(self):
        assert heltec_button.CONF_RETRIEVE_SETTINGS in heltec_button.BUTTONS
        assert heltec_button.CONF_RETRIEVE_DEVICE_INFO in heltec_button.BUTTONS
        assert len(heltec_button.BUTTONS) == 3

    def test_button_addresses_are_unique(self):
        addresses = list(heltec_button.BUTTONS.values())
        assert len(addresses) == len(set(addresses))

    def test_numbers_dict(self):
        assert heltec_number.CONF_CELL_COUNT in heltec_number.NUMBERS
        assert heltec_number.CONF_BALANCE_TRIGGER_VOLTAGE in heltec_number.NUMBERS
        assert len(heltec_number.NUMBERS) == 7

    def test_v2_numbers_dict(self):
        assert (
            heltec_number.CONF_CELL_OVERVOLTAGE_PROTECTION in heltec_number.V2_NUMBERS
        )
        assert (
            heltec_number.CONF_DISCHARGING_UNDERTEMPERATURE_RECOVERY
            in heltec_number.V2_NUMBERS
        )
        assert len(heltec_number.V2_NUMBERS) == 27

    def test_number_addresses_are_unique(self):
        addresses = list(heltec_number.NUMBERS.values())
        assert len(addresses) == len(set(addresses))

    def test_switches_dict(self):
        assert heltec_switch.CONF_BALANCER in heltec_switch.SWITCHES
        assert len(heltec_switch.SWITCHES) == 1
