#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace jk_bms_ble {

namespace espbt = esphome::esp32_ble_tracker;

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

class JkBmsBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
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
  void set_balance_starting_voltage_number(number::Number *balance_starting_voltage_number) {
    balance_starting_voltage_number_ = balance_starting_voltage_number;
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
  void set_power_tube_overtemperature_protection_number(number::Number *power_tube_overtemperature_protection_number) {
    power_tube_overtemperature_protection_number_ = power_tube_overtemperature_protection_number;
  }
  void set_power_tube_overtemperature_protection_recovery_number(
      number::Number *power_tube_overtemperature_protection_recovery_number) {
    power_tube_overtemperature_protection_recovery_number_ = power_tube_overtemperature_protection_recovery_number;
  }

  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_balancing_sensor(sensor::Sensor *balancing_sensor) { balancing_sensor_ = balancing_sensor; }
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
  void set_power_tube_temperature_sensor(sensor::Sensor *power_tube_temperature_sensor) {
    power_tube_temperature_sensor_ = power_tube_temperature_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_capacity_remaining_sensor(sensor::Sensor *capacity_remaining_sensor) {
    capacity_remaining_sensor_ = capacity_remaining_sensor;
  }
  void set_total_battery_capacity_setting_sensor(sensor::Sensor *total_battery_capacity_setting_sensor) {
    total_battery_capacity_setting_sensor_ = total_battery_capacity_setting_sensor;
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
  void set_errors_bitmask_sensor(sensor::Sensor *errors_bitmask_sensor) {
    errors_bitmask_sensor_ = errors_bitmask_sensor;
  }
  void set_emergency_time_countdown_sensor(sensor::Sensor *emergency_time_countdown_sensor) {
    emergency_time_countdown_sensor_ = emergency_time_countdown_sensor;
  }
  void set_heating_current_sensor(sensor::Sensor *heating_current_sensor) {
    heating_current_sensor_ = heating_current_sensor;
  }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
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

  void assemble(const uint8_t *data, uint16_t length);
  void set_protocol_version(ProtocolVersion protocol_version) { protocol_version_ = protocol_version; }
  ProtocolVersion get_protocol_version() { return protocol_version_; }
  bool write_register(uint8_t address, uint32_t value, uint8_t length);

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[32];
  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[5];

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_24S};

  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *precharging_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *online_status_binary_sensor_;
  binary_sensor::BinarySensor *heating_binary_sensor_;

  number::Number *smart_sleep_voltage_number_;
  number::Number *balance_trigger_voltage_number_;
  number::Number *cell_soc100_voltage_number_;
  number::Number *cell_soc0_voltage_number_;
  number::Number *cell_request_charge_voltage_number_;
  number::Number *cell_request_float_voltage_number_;
  number::Number *cell_request_charge_voltage_time_number_;
  number::Number *cell_request_float_voltage_time_number_;
  number::Number *cell_count_number_;
  number::Number *total_battery_capacity_number_;
  number::Number *cell_voltage_overvoltage_protection_number_;
  number::Number *cell_voltage_overvoltage_recovery_number_;
  number::Number *cell_voltage_undervoltage_protection_number_;
  number::Number *cell_voltage_undervoltage_recovery_number_;
  number::Number *balance_starting_voltage_number_;
  number::Number *voltage_calibration_number_;  // @FIXME: Identify value at the settings frame
  number::Number *current_calibration_number_;  // @FIXME: Identify value at the settings frame
  number::Number *power_off_voltage_number_;
  number::Number *max_balance_current_number_;
  number::Number *max_charge_current_number_;
  number::Number *max_discharge_current_number_;
  number::Number *charge_overcurrent_protection_delay_number_;
  number::Number *charge_overcurrent_protection_recovery_time_number_;
  number::Number *discharge_overcurrent_protection_delay_number_;
  number::Number *discharge_overcurrent_protection_recovery_time_number_;
  number::Number *short_circuit_protection_delay_number_;
  number::Number *short_circuit_protection_recovery_time_number_;
  number::Number *charge_overtemperature_protection_number_;
  number::Number *charge_overtemperature_protection_recovery_number_;
  number::Number *discharge_overtemperature_protection_number_;
  number::Number *discharge_overtemperature_protection_recovery_number_;
  number::Number *charge_undertemperature_protection_number_;
  number::Number *charge_undertemperature_protection_recovery_number_;
  number::Number *power_tube_overtemperature_protection_number_;
  number::Number *power_tube_overtemperature_protection_recovery_number_;

  sensor::Sensor *balancing_sensor_;
  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *charging_power_sensor_;
  sensor::Sensor *discharging_power_sensor_;
  sensor::Sensor *power_tube_temperature_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *capacity_remaining_sensor_;
  sensor::Sensor *total_battery_capacity_setting_sensor_;
  sensor::Sensor *charging_cycles_sensor_;
  sensor::Sensor *total_charging_cycle_capacity_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *balancing_current_sensor_;
  sensor::Sensor *errors_bitmask_sensor_;
  sensor::Sensor *emergency_time_countdown_sensor_;
  sensor::Sensor *heating_current_sensor_;

  switch_::Switch *charging_switch_;
  switch_::Switch *discharging_switch_;
  switch_::Switch *balancer_switch_;
  switch_::Switch *emergency_switch_;
  switch_::Switch *heating_switch_;
  switch_::Switch *disable_temperature_sensors_switch_;
  switch_::Switch *display_always_on_switch_;
  switch_::Switch *smart_sleep_switch_;
  switch_::Switch *timed_stored_data_switch_;
  switch_::Switch *disable_pcl_module_switch_;
  switch_::Switch *charging_float_mode_switch_;

  text_sensor::TextSensor *errors_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;

  std::vector<uint8_t> frame_buffer_;
  bool status_notification_received_ = false;
  uint8_t no_response_count_{0};
  uint16_t char_handle_;
  uint16_t notify_handle_;
  uint32_t last_cell_info_{0};
  uint32_t throttle_;

  void decode_(const std::vector<uint8_t> &data);
  void decode_device_info_(const std::vector<uint8_t> &data);
  void decode_jk02_cell_info_(const std::vector<uint8_t> &data);
  void decode_jk04_cell_info_(const std::vector<uint8_t> &data);
  void decode_jk02_settings_(const std::vector<uint8_t> &data);
  void decode_jk04_settings_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(number::Number *number, float value);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();
  std::string error_bits_to_string_(uint16_t bitmask);

  std::string format_total_runtime_(const uint32_t value) {
    int seconds = (int) value;
    int years = seconds / (24 * 3600 * 365);
    seconds = seconds % (24 * 3600 * 365);
    int days = seconds / (24 * 3600);
    seconds = seconds % (24 * 3600);
    int hours = seconds / 3600;
    return (years ? to_string(years) + "y " : "") + (days ? to_string(days) + "d " : "") +
           (hours ? to_string(hours) + "h" : "");
  }

  float ieee_float_(uint32_t f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }

  bool check_bit_(uint8_t mask, uint8_t flag) { return (mask & flag) == flag; }
};

}  // namespace jk_bms_ble
}  // namespace esphome

#endif
