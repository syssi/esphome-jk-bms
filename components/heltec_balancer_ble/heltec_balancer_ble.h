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
namespace heltec_balancer_ble {

namespace espbt = esphome::esp32_ble_tracker;

class HeltecBalancerBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_cell_count_number(number::Number *cell_count_number) { cell_count_number_ = cell_count_number; }
  void set_balance_trigger_voltage_number(number::Number *balance_trigger_voltage_number) {
    balance_trigger_voltage_number_ = balance_trigger_voltage_number;
  }
  void set_max_balance_current_number(number::Number *max_balance_current_number) {
    max_balance_current_number_ = max_balance_current_number;
  }
  void set_balance_sleep_voltage_number(number::Number *balance_sleep_voltage_number) {
    balance_sleep_voltage_number_ = balance_sleep_voltage_number;
  }
  void set_balance_start_voltage_number(number::Number *balance_start_voltage_number) {
    balance_start_voltage_number_ = balance_start_voltage_number;
  }
  void set_nominal_battery_capacity_number(number::Number *nominal_battery_capacity_number) {
    nominal_battery_capacity_number_ = nominal_battery_capacity_number;
  }

  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_error_charging_binary_sensor(binary_sensor::BinarySensor *error_charging_binary_sensor) {
    error_charging_binary_sensor_ = error_charging_binary_sensor;
  }
  void set_error_discharging_binary_sensor(binary_sensor::BinarySensor *error_discharging_binary_sensor) {
    error_discharging_binary_sensor_ = error_discharging_binary_sensor;
  }
  void set_error_system_overheating_binary_sensor(binary_sensor::BinarySensor *error_system_overheating_binary_sensor) {
    error_system_overheating_binary_sensor_ = error_system_overheating_binary_sensor;
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
  void set_temperature_sensor_1_sensor(sensor::Sensor *temperature_sensor_1_sensor) {
    temperature_sensor_1_sensor_ = temperature_sensor_1_sensor;
  }
  void set_temperature_sensor_2_sensor(sensor::Sensor *temperature_sensor_2_sensor) {
    temperature_sensor_2_sensor_ = temperature_sensor_2_sensor;
  }
  void set_total_runtime_sensor(sensor::Sensor *total_runtime_sensor) { total_runtime_sensor_ = total_runtime_sensor; }
  void set_balancing_current_sensor(sensor::Sensor *balancing_current_sensor) {
    balancing_current_sensor_ = balancing_current_sensor;
  }
  void set_errors_bitmask_sensor(sensor::Sensor *errors_bitmask_sensor) {
    errors_bitmask_sensor_ = errors_bitmask_sensor;
  }
  void set_cell_detection_failed_bitmask_sensor(sensor::Sensor *cell_detection_failed_bitmask_sensor) {
    cell_detection_failed_bitmask_sensor_ = cell_detection_failed_bitmask_sensor;
  }
  void set_cell_overvoltage_bitmask_sensor(sensor::Sensor *cell_overvoltage_bitmask_sensor) {
    cell_overvoltage_bitmask_sensor_ = cell_overvoltage_bitmask_sensor;
  }
  void set_cell_undervoltage_bitmask_sensor(sensor::Sensor *cell_undervoltage_bitmask_sensor) {
    cell_undervoltage_bitmask_sensor_ = cell_undervoltage_bitmask_sensor;
  }
  void set_cell_polarity_error_bitmask_sensor(sensor::Sensor *cell_polarity_error_bitmask_sensor) {
    cell_polarity_error_bitmask_sensor_ = cell_polarity_error_bitmask_sensor;
  }
  void set_cell_excessive_line_resistance_bitmask_sensor(
      sensor::Sensor *cell_excessive_line_resistance_bitmask_sensor) {
    cell_excessive_line_resistance_bitmask_sensor_ = cell_excessive_line_resistance_bitmask_sensor;
  }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
  }
  void set_buzzer_mode_text_sensor(text_sensor::TextSensor *buzzer_mode_text_sensor) {
    buzzer_mode_text_sensor_ = buzzer_mode_text_sensor;
  }
  void set_battery_type_text_sensor(text_sensor::TextSensor *battery_type_text_sensor) {
    battery_type_text_sensor_ = battery_type_text_sensor;
  }

  void set_balancer_switch(switch_::Switch *balancer_switch) { balancer_switch_ = balancer_switch; }

  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }
  bool send_command(uint8_t function, uint8_t command, uint8_t register_address = 0x00, uint32_t value = 0x00000000);

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[24];

 protected:
  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *error_charging_binary_sensor_;
  binary_sensor::BinarySensor *error_discharging_binary_sensor_;
  binary_sensor::BinarySensor *error_system_overheating_binary_sensor_;

  number::Number *cell_count_number_;
  number::Number *balance_trigger_voltage_number_;
  number::Number *max_balance_current_number_;
  number::Number *balance_sleep_voltage_number_;
  number::Number *balance_start_voltage_number_;
  number::Number *nominal_battery_capacity_number_;

  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *temperature_sensor_1_sensor_;
  sensor::Sensor *temperature_sensor_2_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *balancing_current_sensor_;
  sensor::Sensor *errors_bitmask_sensor_;
  sensor::Sensor *cell_detection_failed_bitmask_sensor_;
  sensor::Sensor *cell_overvoltage_bitmask_sensor_;
  sensor::Sensor *cell_undervoltage_bitmask_sensor_;
  sensor::Sensor *cell_polarity_error_bitmask_sensor_;
  sensor::Sensor *cell_excessive_line_resistance_bitmask_sensor_;

  switch_::Switch *balancer_switch_;

  text_sensor::TextSensor *errors_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;
  text_sensor::TextSensor *buzzer_mode_text_sensor_;
  text_sensor::TextSensor *battery_type_text_sensor_;

  std::vector<uint8_t> frame_buffer_;
  bool status_notification_received_ = false;
  bool enable_fake_traffic_;
  uint16_t char_handle_;
  uint32_t last_cell_info_{0};
  uint16_t throttle_;

  void assemble_(const uint8_t *data, uint16_t length);
  void decode_(const std::vector<uint8_t> &data);
  void decode_device_info_(const std::vector<uint8_t> &data);
  void decode_cell_info_(const std::vector<uint8_t> &data);
  void decode_settings_(const std::vector<uint8_t> &data);
  void decode_factory_defaults_(const std::vector<uint8_t> &data);
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

}  // namespace heltec_balancer_ble
}  // namespace esphome

#endif
