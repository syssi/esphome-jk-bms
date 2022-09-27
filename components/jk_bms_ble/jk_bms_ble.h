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
  PROTOCOL_VERSION_JK02,
  PROTOCOL_VERSION_JK04,
};

class JkBmsBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_balance_trigger_voltage_number(number::Number *balance_trigger_voltage_number) {
    balance_trigger_voltage_number_ = balance_trigger_voltage_number;
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

  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }

  void set_throttle(uint16_t throttle) { this->throttle_ = throttle; }
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
  void set_temperature_sensor_1_sensor(sensor::Sensor *temperature_sensor_1_sensor) {
    temperature_sensor_1_sensor_ = temperature_sensor_1_sensor;
  }
  void set_temperature_sensor_2_sensor(sensor::Sensor *temperature_sensor_2_sensor) {
    temperature_sensor_2_sensor_ = temperature_sensor_2_sensor;
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

  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }
  void set_protocol_version(ProtocolVersion protocol_version) { protocol_version_ = protocol_version; }
  bool get_protocol_version() { return protocol_version_; }
  bool write_register(uint8_t address, uint32_t value, uint8_t length);

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[32];

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02};

  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;

  number::Number *balance_trigger_voltage_number_;
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
  sensor::Sensor *temperature_sensor_1_sensor_;
  sensor::Sensor *temperature_sensor_2_sensor_;
  sensor::Sensor *power_tube_temperature_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *capacity_remaining_sensor_;
  sensor::Sensor *total_battery_capacity_setting_sensor_;
  sensor::Sensor *charging_cycles_sensor_;
  sensor::Sensor *total_charging_cycle_capacity_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *balancing_current_sensor_;
  sensor::Sensor *errors_bitmask_sensor_;

  switch_::Switch *charging_switch_;
  switch_::Switch *discharging_switch_;
  switch_::Switch *balancer_switch_;

  text_sensor::TextSensor *errors_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;

  std::vector<uint8_t> frame_buffer_;
  bool status_notification_received_ = false;
  bool enable_fake_traffic_;
  uint16_t char_handle_;
  uint16_t notify_handle_;
  uint32_t last_cell_info_{0};
  uint16_t throttle_;

  void assemble_(const uint8_t *data, uint16_t length);
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
};

}  // namespace jk_bms_ble
}  // namespace esphome

#endif
