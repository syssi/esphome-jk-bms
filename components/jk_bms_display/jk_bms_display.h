#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_bms_display {

class JkBmsDisplay : public uart::UARTDevice, public Component {
 public:
  void set_system_warning_binary_sensor(binary_sensor::BinarySensor *system_warning_binary_sensor) {
    system_warning_binary_sensor_ = system_warning_binary_sensor;
  }
  void set_balancing_switch_binary_sensor(binary_sensor::BinarySensor *balancing_switch_binary_sensor) {
    balancing_switch_binary_sensor_ = balancing_switch_binary_sensor;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }
  void set_cell_voltage_undervoltage_protection_binary_sensor(
      binary_sensor::BinarySensor *cell_voltage_undervoltage_protection_binary_sensor) {
    cell_voltage_undervoltage_protection_binary_sensor_ = cell_voltage_undervoltage_protection_binary_sensor;
  }
  void set_cell_voltage_overvoltage_protection_binary_sensor(
      binary_sensor::BinarySensor *cell_voltage_overvoltage_protection_binary_sensor) {
    cell_voltage_overvoltage_protection_binary_sensor_ = cell_voltage_overvoltage_protection_binary_sensor;
  }
  void set_overcurrent_protection_binary_sensor(binary_sensor::BinarySensor *overcurrent_protection_binary_sensor) {
    overcurrent_protection_binary_sensor_ = overcurrent_protection_binary_sensor;
  }
  void set_mosfet_overtemperature_protection_binary_sensor(
      binary_sensor::BinarySensor *mosfet_overtemperature_protection_binary_sensor) {
    mosfet_overtemperature_protection_binary_sensor_ = mosfet_overtemperature_protection_binary_sensor;
  }
  void set_battery_temperature_protection_binary_sensor(
      binary_sensor::BinarySensor *battery_temperature_protection_binary_sensor) {
    battery_temperature_protection_binary_sensor_ = battery_temperature_protection_binary_sensor;
  }
  void set_short_circuit_protection_binary_sensor(binary_sensor::BinarySensor *short_circuit_protection_binary_sensor) {
    short_circuit_protection_binary_sensor_ = short_circuit_protection_binary_sensor;
  }
  void set_coprocessor_communication_error_binary_sensor(
      binary_sensor::BinarySensor *coprocessor_communication_error_binary_sensor) {
    coprocessor_communication_error_binary_sensor_ = coprocessor_communication_error_binary_sensor;
  }
  void set_balancer_wire_resistance_too_high_binary_sensor(
      binary_sensor::BinarySensor *balancer_wire_resistance_too_high_binary_sensor) {
    balancer_wire_resistance_too_high_binary_sensor_ = balancer_wire_resistance_too_high_binary_sensor;
  }
  void set_cell_count_mismatch_binary_sensor(binary_sensor::BinarySensor *cell_count_mismatch_binary_sensor) {
    cell_count_mismatch_binary_sensor_ = cell_count_mismatch_binary_sensor;
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
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_mosfet_temperature_sensor(sensor::Sensor *mosfet_temperature_sensor) {
    mosfet_temperature_sensor_ = mosfet_temperature_sensor;
  }
  void set_battery_temperature_sensor(sensor::Sensor *battery_temperature_sensor) {
    battery_temperature_sensor_ = battery_temperature_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }

  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  binary_sensor::BinarySensor *system_warning_binary_sensor_;
  binary_sensor::BinarySensor *balancing_switch_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *cell_voltage_undervoltage_protection_binary_sensor_;
  binary_sensor::BinarySensor *cell_voltage_overvoltage_protection_binary_sensor_;
  binary_sensor::BinarySensor *overcurrent_protection_binary_sensor_;
  binary_sensor::BinarySensor *mosfet_overtemperature_protection_binary_sensor_;
  binary_sensor::BinarySensor *battery_temperature_protection_binary_sensor_;
  binary_sensor::BinarySensor *short_circuit_protection_binary_sensor_;
  binary_sensor::BinarySensor *coprocessor_communication_error_binary_sensor_;
  binary_sensor::BinarySensor *balancer_wire_resistance_too_high_binary_sensor_;
  binary_sensor::BinarySensor *cell_count_mismatch_binary_sensor_;

  sensor::Sensor *error_bitmask_sensor_;
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *charging_power_sensor_;
  sensor::Sensor *discharging_power_sensor_;
  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *mosfet_temperature_sensor_;
  sensor::Sensor *battery_temperature_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[24];

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};

  void on_jk_bms_display_data_(const std::vector<uint8_t> &data);
  void on_jk_bms_display_status_data_(const std::vector<uint8_t> &data);
  void on_jk_bms_display_raw_data_(const std::vector<uint8_t> &data);
  bool parse_jk_bms_display_byte_(uint8_t byte);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);

  std::string device_type_to_string_(const uint8_t &device_type);
  std::string error_bits_to_string_(const uint8_t &mask);
};

}  // namespace jk_bms_display
}  // namespace esphome
