#pragma once

#include <array>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include <esp_gattc_api.h>
namespace espbt = esphome::esp32_ble_tracker;
#endif

namespace esphome::jk_bms_ble {

struct LookupTable {
  const char *const *entries{nullptr};
  size_t count{0};
  const char *get(uint8_t index) const { return (entries != nullptr && index < count) ? entries[index] : nullptr; }
};

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

class JkBmsBle :
#ifdef USE_ESP32
    public esphome::ble_client::BLEClientNode,
#endif
    public PollingComponent {
 public:
#ifdef USE_ESP32
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
#endif
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_smart_sleep_voltage_number(number::Number *smart_sleep_voltage_number) {
    smart_sleep_voltage_number_ = smart_sleep_voltage_number;
  }
  void set_balance_trigger_voltage_number(number::Number *balance_trigger_voltage_number) {
    balance_trigger_voltage_number_ = balance_trigger_voltage_number;
  }
  void set_cell_soc100_voltage_number(number::Number *cell_soc100_voltage_number) {
    cell_soc100_voltage_number_ = cell_soc100_voltage_number;
  }
  void set_cell_soc0_voltage_number(number::Number *cell_soc0_voltage_number) {
    cell_soc0_voltage_number_ = cell_soc0_voltage_number;
  }
  void set_cell_request_charge_voltage_number(number::Number *cell_request_charge_voltage_number) {
    cell_request_charge_voltage_number_ = cell_request_charge_voltage_number;
  }
  void set_cell_request_float_voltage_number(number::Number *cell_request_float_voltage_number) {
    cell_request_float_voltage_number_ = cell_request_float_voltage_number;
  }
  void set_cell_request_charge_voltage_time_number(number::Number *cell_request_charge_voltage_time_number) {
    cell_request_charge_voltage_time_number_ = cell_request_charge_voltage_time_number;
  }
  void set_cell_request_float_voltage_time_number(number::Number *cell_request_float_voltage_time_number) {
    cell_request_float_voltage_time_number_ = cell_request_float_voltage_time_number;
  }
  void set_cell_count_number(number::Number *cell_count_number) { cell_count_number_ = cell_count_number; }
  void set_total_battery_capacity_number(number::Number *total_battery_capacity_number) {
    total_battery_capacity_number_ = total_battery_capacity_number;
  }
  void set_cell_voltage_overvoltage_protection_number(number::Number *cell_voltage_overvoltage_protection_number) {
    cell_voltage_overvoltage_protection_number_ = cell_voltage_overvoltage_protection_number;
  }
  void set_cell_voltage_overvoltage_recovery_number(number::Number *cell_voltage_overvoltage_recovery_number) {
    cell_voltage_overvoltage_recovery_number_ = cell_voltage_overvoltage_recovery_number;
  }
  void set_cell_voltage_undervoltage_protection_number(number::Number *cell_voltage_undervoltage_protection_number) {
    cell_voltage_undervoltage_protection_number_ = cell_voltage_undervoltage_protection_number;
  }
  void set_cell_voltage_undervoltage_recovery_number(number::Number *cell_voltage_undervoltage_recovery_number) {
    cell_voltage_undervoltage_recovery_number_ = cell_voltage_undervoltage_recovery_number;
  }
  void set_balancing_start_voltage_number(number::Number *balancing_start_voltage_number) {
    balancing_start_voltage_number_ = balancing_start_voltage_number;
  }
  void set_voltage_calibration_number(number::Number *voltage_calibration_number) {
    voltage_calibration_number_ = voltage_calibration_number;
  }
  void set_current_calibration_number(number::Number *current_calibration_number) {
    current_calibration_number_ = current_calibration_number;
  }
  void set_power_off_voltage_number(number::Number *power_off_voltage_number) {
    power_off_voltage_number_ = power_off_voltage_number;
  }
  void set_max_balance_current_number(number::Number *max_balance_current_number) {
    max_balance_current_number_ = max_balance_current_number;
  }
  void set_max_charge_current_number(number::Number *max_charge_current_number) {
    max_charge_current_number_ = max_charge_current_number;
  }
  void set_max_discharge_current_number(number::Number *max_discharge_current_number) {
    max_discharge_current_number_ = max_discharge_current_number;
  }
  void set_charge_overcurrent_protection_delay_number(number::Number *charge_overcurrent_protection_delay_number) {
    charge_overcurrent_protection_delay_number_ = charge_overcurrent_protection_delay_number;
  }
  void set_charge_overcurrent_protection_recovery_time_number(
      number::Number *charge_overcurrent_protection_recovery_time_number) {
    charge_overcurrent_protection_recovery_time_number_ = charge_overcurrent_protection_recovery_time_number;
  }
  void set_discharge_overcurrent_protection_delay_number(
      number::Number *discharge_overcurrent_protection_delay_number) {
    discharge_overcurrent_protection_delay_number_ = discharge_overcurrent_protection_delay_number;
  }
  void set_discharge_overcurrent_protection_recovery_time_number(
      number::Number *discharge_overcurrent_protection_recovery_time_number) {
    discharge_overcurrent_protection_recovery_time_number_ = discharge_overcurrent_protection_recovery_time_number;
  }
  void set_short_circuit_protection_delay_number(number::Number *short_circuit_protection_delay_number) {
    short_circuit_protection_delay_number_ = short_circuit_protection_delay_number;
  }
  void set_short_circuit_protection_recovery_time_number(
      number::Number *short_circuit_protection_recovery_time_number) {
    short_circuit_protection_recovery_time_number_ = short_circuit_protection_recovery_time_number;
  }
  void set_charge_overtemperature_protection_number(number::Number *charge_overtemperature_protection_number) {
    charge_overtemperature_protection_number_ = charge_overtemperature_protection_number;
  }
  void set_charge_overtemperature_protection_recovery_number(
      number::Number *charge_overtemperature_protection_recovery_number) {
    charge_overtemperature_protection_recovery_number_ = charge_overtemperature_protection_recovery_number;
  }
  void set_discharge_overtemperature_protection_number(number::Number *discharge_overtemperature_protection_number) {
    discharge_overtemperature_protection_number_ = discharge_overtemperature_protection_number;
  }
  void set_discharge_overtemperature_protection_recovery_number(
      number::Number *discharge_overtemperature_protection_recovery_number) {
    discharge_overtemperature_protection_recovery_number_ = discharge_overtemperature_protection_recovery_number;
  }
  void set_charge_undertemperature_protection_number(number::Number *charge_undertemperature_protection_number) {
    charge_undertemperature_protection_number_ = charge_undertemperature_protection_number;
  }
  void set_charge_undertemperature_protection_recovery_number(
      number::Number *charge_undertemperature_protection_recovery_number) {
    charge_undertemperature_protection_recovery_number_ = charge_undertemperature_protection_recovery_number;
  }
  void set_discharge_undertemperature_protection_number(number::Number *discharge_undertemperature_protection_number) {
    discharge_undertemperature_protection_number_ = discharge_undertemperature_protection_number;
  }
  void set_discharge_undertemperature_protection_recovery_number(
      number::Number *discharge_undertemperature_protection_recovery_number) {
    discharge_undertemperature_protection_recovery_number_ = discharge_undertemperature_protection_recovery_number;
  }
  void set_mosfet_overtemperature_protection_number(number::Number *mosfet_overtemperature_protection_number) {
    mosfet_overtemperature_protection_number_ = mosfet_overtemperature_protection_number;
  }
  void set_mosfet_overtemperature_protection_recovery_number(
      number::Number *mosfet_overtemperature_protection_recovery_number) {
    mosfet_overtemperature_protection_recovery_number_ = mosfet_overtemperature_protection_recovery_number;
  }
  void set_discharge_precharge_time_number(number::Number *discharge_precharge_time_number) {
    discharge_precharge_time_number_ = discharge_precharge_time_number;
  }
  void set_heating_start_temperature_number(number::Number *heating_start_temperature_number) {
    heating_start_temperature_number_ = heating_start_temperature_number;
  }
  void set_heating_stop_temperature_number(number::Number *heating_stop_temperature_number) {
    heating_stop_temperature_number_ = heating_stop_temperature_number;
  }
  void set_smart_sleep_delay_number(number::Number *smart_sleep_delay_number) {
    smart_sleep_delay_number_ = smart_sleep_delay_number;
  }
  void set_emergency_duration_number(number::Number *emergency_duration_number) {
    emergency_duration_number_ = emergency_duration_number;
  }
  void set_re_bulk_soc_number(number::Number *re_bulk_soc_number) { re_bulk_soc_number_ = re_bulk_soc_number; }

  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_balancer_status_bitmask_sensor(sensor::Sensor *balancer_status_sensor) {
    balancer_status_bitmask_sensor_ = balancer_status_sensor;
  }
  void set_precharging_binary_sensor(binary_sensor::BinarySensor *precharging_binary_sensor) {
    precharging_binary_sensor_ = precharging_binary_sensor;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }
  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status_binary_sensor) {
    online_status_binary_sensor_ = online_status_binary_sensor;
  }
  void set_heating_binary_sensor(binary_sensor::BinarySensor *heating_binary_sensor) {
    heating_binary_sensor_ = heating_binary_sensor;
  }
  void set_dry_contact_1_binary_sensor(binary_sensor::BinarySensor *dry_contact_1_binary_sensor) {
    dry_contact_1_binary_sensor_ = dry_contact_1_binary_sensor;
  }
  void set_dry_contact_2_binary_sensor(binary_sensor::BinarySensor *dry_contact_2_binary_sensor) {
    dry_contact_2_binary_sensor_ = dry_contact_2_binary_sensor;
  }

  void set_throttle(uint32_t throttle) { this->throttle_ = throttle; }
  void set_min_cell_voltage_sensor(sensor::Sensor *min_cell_voltage_sensor) {
    min_cell_voltage_sensor_ = min_cell_voltage_sensor;
  }
  void set_max_cell_voltage_sensor(sensor::Sensor *max_cell_voltage_sensor) {
    max_cell_voltage_sensor_ = max_cell_voltage_sensor;
  }
  void set_min_voltage_cell_sensor(sensor::Sensor *min_voltage_cell_sensor) {
    min_voltage_cell_sensor_ = min_voltage_cell_sensor;
  }
  void set_max_voltage_cell_sensor(sensor::Sensor *max_voltage_cell_sensor) {
    max_voltage_cell_sensor_ = max_voltage_cell_sensor;
  }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_cell_resistance_sensor(uint8_t cell, sensor::Sensor *cell_resistance_sensor) {
    this->cells_[cell].cell_resistance_sensor_ = cell_resistance_sensor;
  }
  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_charging_power_sensor(sensor::Sensor *charging_power_sensor) {
    charging_power_sensor_ = charging_power_sensor;
  }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power_sensor) {
    discharging_power_sensor_ = discharging_power_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }
  void set_mosfet_temperature_sensor(sensor::Sensor *mosfet_temperature_sensor) {
    mosfet_temperature_sensor_ = mosfet_temperature_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_state_of_health_sensor(sensor::Sensor *state_of_health_sensor) {
    state_of_health_sensor_ = state_of_health_sensor;
  }
  void set_capacity_remaining_sensor(sensor::Sensor *capacity_remaining_sensor) {
    capacity_remaining_sensor_ = capacity_remaining_sensor;
  }
  void set_full_charge_capacity_sensor(sensor::Sensor *full_charge_capacity_sensor) {
    full_charge_capacity_sensor_ = full_charge_capacity_sensor;
  }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }
  void set_total_charging_cycle_capacity_sensor(sensor::Sensor *total_charging_cycle_capacity_sensor) {
    total_charging_cycle_capacity_sensor_ = total_charging_cycle_capacity_sensor;
  }
  void set_total_runtime_sensor(sensor::Sensor *total_runtime_sensor) { total_runtime_sensor_ = total_runtime_sensor; }
  void set_balancing_current_sensor(sensor::Sensor *balancing_current_sensor) {
    balancing_current_sensor_ = balancing_current_sensor;
  }
  void set_emergency_time_countdown_sensor(sensor::Sensor *emergency_time_countdown_sensor) {
    emergency_time_countdown_sensor_ = emergency_time_countdown_sensor;
  }
  void set_smart_sleep_countdown_sensor(sensor::Sensor *smart_sleep_countdown_sensor) {
    smart_sleep_countdown_sensor_ = smart_sleep_countdown_sensor;
  }
  void set_heating_current_sensor(sensor::Sensor *heating_current_sensor) {
    heating_current_sensor_ = heating_current_sensor;
  }
  void set_charge_status_id_sensor(sensor::Sensor *charge_status_id_sensor) {
    charge_status_id_sensor_ = charge_status_id_sensor;
  }
  void set_charge_status_time_elapsed_sensor(sensor::Sensor *charge_status_time_elapsed_sensor) {
    charge_status_time_elapsed_sensor_ = charge_status_time_elapsed_sensor;
  }
  void set_detail_log_entry_count_sensor(sensor::Sensor *detail_log_entry_count_sensor) {
    detail_log_entry_count_sensor_ = detail_log_entry_count_sensor;
  }
  void set_battery_type_id_sensor(sensor::Sensor *battery_type_id_sensor) {
    battery_type_id_sensor_ = battery_type_id_sensor;
  }
  void set_uart1_protocols_enabled_bitmask_sensor(sensor::Sensor *uart1_protocols_enabled_bitmask_sensor) {
    uart1_protocols_enabled_bitmask_sensor_ = uart1_protocols_enabled_bitmask_sensor;
  }
  void set_uart2_protocols_enabled_bitmask_sensor(sensor::Sensor *uart2_protocols_enabled_bitmask_sensor) {
    uart2_protocols_enabled_bitmask_sensor_ = uart2_protocols_enabled_bitmask_sensor;
  }
  void set_uart3_protocols_enabled_bitmask_sensor(sensor::Sensor *uart3_protocols_enabled_bitmask_sensor) {
    uart3_protocols_enabled_bitmask_sensor_ = uart3_protocols_enabled_bitmask_sensor;
  }
  void set_can_protocols_enabled_bitmask_sensor(sensor::Sensor *can_protocols_enabled_bitmask_sensor) {
    can_protocols_enabled_bitmask_sensor_ = can_protocols_enabled_bitmask_sensor;
  }

  void set_errors_bitmask_hex_text_sensor(text_sensor::TextSensor *errors_bitmask_hex_text_sensor) {
    errors_bitmask_hex_text_sensor_ = errors_bitmask_hex_text_sensor;
  }
  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_balancer_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    balancer_status_text_sensor_ = operation_status_text_sensor;
  }
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
  }
  void set_charge_status_text_sensor(text_sensor::TextSensor *charge_status_text_sensor) {
    charge_status_text_sensor_ = charge_status_text_sensor;
  }
  void set_software_version_text_sensor(text_sensor::TextSensor *software_version_text_sensor) {
    software_version_text_sensor_ = software_version_text_sensor;
  }
  void set_hardware_version_text_sensor(text_sensor::TextSensor *hardware_version_text_sensor) {
    hardware_version_text_sensor_ = hardware_version_text_sensor;
  }
  void set_battery_type_text_sensor(text_sensor::TextSensor *battery_type_text_sensor) {
    battery_type_text_sensor_ = battery_type_text_sensor;
  }

  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }
  void set_discharging_switch(switch_::Switch *discharging_switch) { discharging_switch_ = discharging_switch; }
  void set_balancer_switch(switch_::Switch *balancer_switch) { balancer_switch_ = balancer_switch; }
  void set_emergency_switch(switch_::Switch *emergency_switch) { emergency_switch_ = emergency_switch; }
  void set_heating_switch(switch_::Switch *heating_switch) { heating_switch_ = heating_switch; }
  void set_disable_temperature_sensors_switch(switch_::Switch *disable_temperature_sensors_switch) {
    disable_temperature_sensors_switch_ = disable_temperature_sensors_switch;
  }
  void set_display_always_on_switch(switch_::Switch *display_always_on_switch) {
    display_always_on_switch_ = display_always_on_switch;
  }
  void set_smart_sleep_switch(switch_::Switch *smart_sleep_switch) { smart_sleep_switch_ = smart_sleep_switch; }
  void set_timed_stored_data_switch(switch_::Switch *timed_stored_data_switch) {
    timed_stored_data_switch_ = timed_stored_data_switch;
  }
  void set_disable_pcl_module_switch(switch_::Switch *disable_pcl_module_switch) {
    disable_pcl_module_switch_ = disable_pcl_module_switch;
  }
  void set_charging_float_mode_switch(switch_::Switch *charging_float_mode_switch) {
    charging_float_mode_switch_ = charging_float_mode_switch;
  }
  void set_emergency_button_trigger_switch(switch_::Switch *emergency_button_trigger_switch) {
    emergency_button_trigger_switch_ = emergency_button_trigger_switch;
  }
  void set_dry_contact_alarm_intermittent_switch(switch_::Switch *dry_contact_alarm_intermittent_switch) {
    dry_contact_alarm_intermittent_switch_ = dry_contact_alarm_intermittent_switch;
  }
  void set_discharge_overcurrent_protection_2_switch(switch_::Switch *discharge_overcurrent_protection_2_switch) {
    discharge_overcurrent_protection_2_switch_ = discharge_overcurrent_protection_2_switch;
  }
  void set_discharge_overcurrent_protection_3_switch(switch_::Switch *discharge_overcurrent_protection_3_switch) {
    discharge_overcurrent_protection_3_switch_ = discharge_overcurrent_protection_3_switch;
  }
  void set_gps_locked_charging_switch(switch_::Switch *gps_locked_charging_switch) {
    gps_locked_charging_switch_ = gps_locked_charging_switch;
  }
  void set_gps_locked_discharging_switch(switch_::Switch *gps_locked_discharging_switch) {
    gps_locked_discharging_switch_ = gps_locked_discharging_switch;
  }
  void set_uart1_protocol_select(select::Select *uart1_protocol_select) {
    uart1_protocol_select_ = uart1_protocol_select;
  }
  void set_uart2_protocol_select(select::Select *uart2_protocol_select) {
    uart2_protocol_select_ = uart2_protocol_select;
  }
  void set_can_protocol_select(select::Select *can_protocol_select) { can_protocol_select_ = can_protocol_select; }
  void set_lcd_buzzer_trigger_select(select::Select *lcd_buzzer_trigger_select) {
    lcd_buzzer_trigger_select_ = lcd_buzzer_trigger_select;
  }
  void set_uart3_protocol_select(select::Select *uart3_protocol_select) {
    uart3_protocol_select_ = uart3_protocol_select;
  }
  void set_dry1_trigger_select(select::Select *dry1_trigger_select) { dry1_trigger_select_ = dry1_trigger_select; }
  void set_dry2_trigger_select(select::Select *dry2_trigger_select) { dry2_trigger_select_ = dry2_trigger_select; }
  void set_multiplexed_port_mode_select(select::Select *multiplexed_port_mode_select) {
    multiplexed_port_mode_select_ = multiplexed_port_mode_select;
  }
  void set_uart_protocol_table(const char *const *entries, size_t count) { uart_protocol_table_ = {entries, count}; }
  void set_can_protocol_table(const char *const *entries, size_t count) { can_protocol_table_ = {entries, count}; }
  void set_lcd_buzzer_trigger_table(const char *const *entries, size_t count) {
    lcd_buzzer_trigger_table_ = {entries, count};
  }
  void set_multiplexed_port_mode_table(const char *const *entries, size_t count) {
    multiplexed_port_mode_table_ = {entries, count};
  }

  void assemble(const uint8_t *data, uint16_t length);
  void set_protocol_version(ProtocolVersion protocol_version) { protocol_version_ = protocol_version; }
  ProtocolVersion get_protocol_version() { return protocol_version_; }
  static std::array<uint8_t, 20> build_frame(uint8_t address, uint32_t value, uint8_t length);
  static uint32_t encode_jk04_payload(float value, uint8_t len, float factor);
#ifdef USE_ESP32
  bool write_register(uint8_t address, uint32_t value, uint8_t length);
#endif

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[32];
  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[5];

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_24S};

  binary_sensor::BinarySensor *balancing_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *precharging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *discharging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *online_status_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *heating_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *dry_contact_1_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *dry_contact_2_binary_sensor_{nullptr};

  number::Number *smart_sleep_voltage_number_{nullptr};
  number::Number *balance_trigger_voltage_number_{nullptr};
  number::Number *cell_soc100_voltage_number_{nullptr};
  number::Number *cell_soc0_voltage_number_{nullptr};
  number::Number *cell_request_charge_voltage_number_{nullptr};
  number::Number *cell_request_float_voltage_number_{nullptr};
  number::Number *cell_request_charge_voltage_time_number_{nullptr};
  number::Number *cell_request_float_voltage_time_number_{nullptr};
  number::Number *cell_count_number_{nullptr};
  number::Number *total_battery_capacity_number_{nullptr};
  number::Number *cell_voltage_overvoltage_protection_number_{nullptr};
  number::Number *cell_voltage_overvoltage_recovery_number_{nullptr};
  number::Number *cell_voltage_undervoltage_protection_number_{nullptr};
  number::Number *cell_voltage_undervoltage_recovery_number_{nullptr};
  number::Number *balancing_start_voltage_number_{nullptr};
  number::Number *voltage_calibration_number_{nullptr};  // @FIXME: Identify value at the settings frame
  number::Number *current_calibration_number_{nullptr};  // @FIXME: Identify value at the settings frame
  number::Number *power_off_voltage_number_{nullptr};
  number::Number *max_balance_current_number_{nullptr};
  number::Number *max_charge_current_number_{nullptr};
  number::Number *max_discharge_current_number_{nullptr};
  number::Number *charge_overcurrent_protection_delay_number_{nullptr};
  number::Number *charge_overcurrent_protection_recovery_time_number_{nullptr};
  number::Number *discharge_overcurrent_protection_delay_number_{nullptr};
  number::Number *discharge_overcurrent_protection_recovery_time_number_{nullptr};
  number::Number *short_circuit_protection_delay_number_{nullptr};
  number::Number *short_circuit_protection_recovery_time_number_{nullptr};
  number::Number *charge_overtemperature_protection_number_{nullptr};
  number::Number *charge_overtemperature_protection_recovery_number_{nullptr};
  number::Number *discharge_overtemperature_protection_number_{nullptr};
  number::Number *discharge_overtemperature_protection_recovery_number_{nullptr};
  number::Number *charge_undertemperature_protection_number_{nullptr};
  number::Number *charge_undertemperature_protection_recovery_number_{nullptr};
  number::Number *discharge_undertemperature_protection_number_{nullptr};
  number::Number *discharge_undertemperature_protection_recovery_number_{nullptr};
  number::Number *mosfet_overtemperature_protection_number_{nullptr};
  number::Number *mosfet_overtemperature_protection_recovery_number_{nullptr};
  number::Number *discharge_precharge_time_number_{nullptr};
  number::Number *heating_start_temperature_number_{nullptr};
  number::Number *heating_stop_temperature_number_{nullptr};
  number::Number *smart_sleep_delay_number_{nullptr};
  number::Number *emergency_duration_number_{nullptr};
  number::Number *re_bulk_soc_number_{nullptr};

  sensor::Sensor *balancer_status_bitmask_sensor_{nullptr};
  sensor::Sensor *min_cell_voltage_sensor_{nullptr};
  sensor::Sensor *max_cell_voltage_sensor_{nullptr};
  sensor::Sensor *min_voltage_cell_sensor_{nullptr};
  sensor::Sensor *max_voltage_cell_sensor_{nullptr};
  sensor::Sensor *delta_cell_voltage_sensor_{nullptr};
  sensor::Sensor *average_cell_voltage_sensor_{nullptr};
  sensor::Sensor *total_voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *discharging_power_sensor_{nullptr};
  sensor::Sensor *mosfet_temperature_sensor_{nullptr};
  sensor::Sensor *state_of_charge_sensor_{nullptr};
  sensor::Sensor *state_of_health_sensor_{nullptr};
  sensor::Sensor *capacity_remaining_sensor_{nullptr};
  sensor::Sensor *full_charge_capacity_sensor_{nullptr};
  sensor::Sensor *charging_cycles_sensor_{nullptr};
  sensor::Sensor *total_charging_cycle_capacity_sensor_{nullptr};
  sensor::Sensor *total_runtime_sensor_{nullptr};
  sensor::Sensor *balancing_current_sensor_{nullptr};
  sensor::Sensor *emergency_time_countdown_sensor_{nullptr};
  sensor::Sensor *smart_sleep_countdown_sensor_{nullptr};
  sensor::Sensor *heating_current_sensor_{nullptr};
  sensor::Sensor *charge_status_id_sensor_{nullptr};
  sensor::Sensor *charge_status_time_elapsed_sensor_{nullptr};
  sensor::Sensor *detail_log_entry_count_sensor_{nullptr};
  sensor::Sensor *battery_type_id_sensor_{nullptr};
  sensor::Sensor *uart1_protocols_enabled_bitmask_sensor_{nullptr};
  sensor::Sensor *uart2_protocols_enabled_bitmask_sensor_{nullptr};
  sensor::Sensor *uart3_protocols_enabled_bitmask_sensor_{nullptr};
  sensor::Sensor *can_protocols_enabled_bitmask_sensor_{nullptr};

  switch_::Switch *charging_switch_{nullptr};
  switch_::Switch *discharging_switch_{nullptr};
  switch_::Switch *balancer_switch_{nullptr};
  switch_::Switch *emergency_switch_{nullptr};
  switch_::Switch *heating_switch_{nullptr};
  switch_::Switch *disable_temperature_sensors_switch_{nullptr};
  switch_::Switch *display_always_on_switch_{nullptr};
  switch_::Switch *smart_sleep_switch_{nullptr};
  switch_::Switch *timed_stored_data_switch_{nullptr};
  switch_::Switch *disable_pcl_module_switch_{nullptr};
  switch_::Switch *charging_float_mode_switch_{nullptr};
  switch_::Switch *emergency_button_trigger_switch_{nullptr};
  switch_::Switch *dry_contact_alarm_intermittent_switch_{nullptr};
  switch_::Switch *discharge_overcurrent_protection_2_switch_{nullptr};
  switch_::Switch *discharge_overcurrent_protection_3_switch_{nullptr};
  switch_::Switch *gps_locked_charging_switch_{nullptr};
  switch_::Switch *gps_locked_discharging_switch_{nullptr};

  select::Select *uart1_protocol_select_{nullptr};
  select::Select *uart2_protocol_select_{nullptr};
  select::Select *uart3_protocol_select_{nullptr};
  select::Select *can_protocol_select_{nullptr};
  select::Select *lcd_buzzer_trigger_select_{nullptr};
  select::Select *dry1_trigger_select_{nullptr};
  select::Select *dry2_trigger_select_{nullptr};
  select::Select *multiplexed_port_mode_select_{nullptr};

  LookupTable uart_protocol_table_;
  LookupTable can_protocol_table_;
  LookupTable lcd_buzzer_trigger_table_;
  LookupTable multiplexed_port_mode_table_;

  text_sensor::TextSensor *errors_bitmask_hex_text_sensor_{nullptr};
  text_sensor::TextSensor *errors_text_sensor_{nullptr};
  text_sensor::TextSensor *balancer_status_text_sensor_{nullptr};
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_{nullptr};
  text_sensor::TextSensor *charge_status_text_sensor_{nullptr};
  text_sensor::TextSensor *software_version_text_sensor_{nullptr};
  text_sensor::TextSensor *hardware_version_text_sensor_{nullptr};
  text_sensor::TextSensor *battery_type_text_sensor_{nullptr};

  std::vector<uint8_t> frame_buffer_;
  bool status_notification_received_ = false;
  uint8_t no_response_count_{0};
#ifdef USE_ESP32
  uint16_t char_handle_{0};
  uint16_t notify_handle_{0};
#endif
  uint32_t last_cell_info_{0};
  uint32_t throttle_{0};

  void decode_(const std::vector<uint8_t> &data);
  void decode_logbook_(const std::vector<uint8_t> &data);
  void decode_device_info_(const std::vector<uint8_t> &data);
  void decode_jk02_cell_info_(const std::vector<uint8_t> &data);
  void decode_jk04_cell_info_(const std::vector<uint8_t> &data);
  void decode_jk02_settings_(const std::vector<uint8_t> &data);
  void decode_jk04_settings_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(number::Number *number, float value);
  void publish_state_(select::Select *obj, const std::string &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();
  std::string to_hex_string_(uint32_t mask);
  std::string error_bits_to_string_(uint32_t bitmask, const char *const *errors, uint8_t errors_size);
  std::string charge_status_id_to_string_(uint8_t status);
  std::string battery_type_id_to_string_(uint8_t code);

  std::string format_total_runtime_(const uint32_t value) {
    int seconds = (int) value;
    int years = seconds / (24 * 3600 * 365);
    seconds = seconds % (24 * 3600 * 365);
    int days = seconds / (24 * 3600);
    seconds = seconds % (24 * 3600);
    int hours = seconds / 3600;

    char buf[16];
    int len = 0;
    if (years)
      len += snprintf(buf + len, sizeof(buf) - len, "%dy ", years);
    if (days)
      len += snprintf(buf + len, sizeof(buf) - len, "%dd ", days);
    if (hours)
      len += snprintf(buf + len, sizeof(buf) - len, "%dh", hours);

    return std::string(buf, len);
  }

  float ieee_float_(uint32_t f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }

  bool check_bit_(uint8_t mask, uint8_t flag) { return (mask & flag) == flag; }
};

}  // namespace esphome::jk_bms_ble
