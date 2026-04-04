"""Schema structure tests for jk_bms ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

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
    sensor as ble_sensor,
    switch as ble_switch,  # noqa: E402
    text_sensor as ble_text_sensor,
)
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
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 8


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
        assert len(ble_sensor.SENSOR_DEFS) == 26


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
        assert len(ble_binary_sensor.BINARY_SENSOR_DEFS) == 7


class TestJkBmsBleSwitchConstants:
    def test_switches_dict(self):
        assert len(ble_switch.SWITCHES) == 11


class TestJkBmsBleButtonConstants:
    def test_buttons_dict(self):
        assert ble_button.CONF_RETRIEVE_SETTINGS in ble_button.BUTTONS
        assert ble_button.CONF_RETRIEVE_DEVICE_INFO in ble_button.BUTTONS
        assert len(ble_button.BUTTONS) == 2

    def test_button_addresses_are_unique(self):
        addresses = list(ble_button.BUTTONS.values())
        assert len(addresses) == len(set(addresses))


class TestJkBleTextSensorConstants:
    def test_text_sensors_list(self):
        assert ble_text_sensor.CONF_ERRORS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_OPERATION_STATUS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_CHARGE_STATUS in ble_text_sensor.TEXT_SENSORS
        assert len(ble_text_sensor.TEXT_SENSORS) == 6


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
        assert len(jk_balancer_sensor.SENSOR_DEFS) == 14

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


class TestHeltecBalancerBleSensorLists:
    def test_sensor_defs_completeness(self):
        assert len(heltec_sensor.SENSOR_DEFS) == 18

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
        assert len(heltec_text_sensor.TEXT_SENSORS) == 5

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

    def test_number_addresses_are_unique(self):
        addresses = list(heltec_number.NUMBERS.values())
        assert len(addresses) == len(set(addresses))

    def test_switches_dict(self):
        assert heltec_switch.CONF_BALANCER in heltec_switch.SWITCHES
        assert len(heltec_switch.SWITCHES) == 1
