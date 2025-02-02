#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/jk_balancer_modbus/jk_balancer_modbus.h"

namespace esphome {
namespace jk_balancer {

class JkBalancer : public PollingComponent, public jk_balancer_modbus::JkBalancerModbusDevice {
 public:
  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status_binary_sensor) {
    online_status_binary_sensor_ = online_status_binary_sensor;
  }

  void set_cell_count_number(number::Number *cell_count_number) { cell_count_number_ = cell_count_number; }
  void set_balance_trigger_voltage_number(number::Number *balance_trigger_voltage_number) {
    balance_trigger_voltage_number_ = balance_trigger_voltage_number;
  }
  void set_max_balance_current_number(number::Number *max_balance_current_number) {
    max_balance_current_number_ = max_balance_current_number;
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
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_battery_temperature_sensor(sensor::Sensor *battery_temperature_sensor) {
    battery_temperature_sensor_ = battery_temperature_sensor;
  }
  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_cell_count_detected_sensor(sensor::Sensor *cell_count_detected_sensor) {
    cell_count_detected_sensor_ = cell_count_detected_sensor;
  }
  void set_errors_bitmask_sensor(sensor::Sensor *errors_bitmask_sensor) {
    errors_bitmask_sensor_ = errors_bitmask_sensor;
  }
  void set_balancer_status_sensor(sensor::Sensor *balancer_status_sensor) {
    balancer_status_sensor_ = balancer_status_sensor;
  }
  void set_balancing_current_sensor(sensor::Sensor *balancing_current_sensor) {
    balancing_current_sensor_ = balancing_current_sensor;
  }

  void set_balancer_switch(switch_::Switch *balancer_switch) { balancer_switch_ = balancer_switch; }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }

  void dump_config() override;

  void on_jk_balancer_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override;

  void update() override;

 protected:
  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *online_status_binary_sensor_;

  number::Number *cell_count_number_;
  number::Number *balance_trigger_voltage_number_;
  number::Number *max_balance_current_number_;

  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;
  sensor::Sensor *battery_temperature_sensor_;
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *cell_count_detected_sensor_;
  sensor::Sensor *errors_bitmask_sensor_;
  sensor::Sensor *balancer_status_sensor_;
  sensor::Sensor *balancing_current_sensor_;

  switch_::Switch *balancer_switch_;

  text_sensor::TextSensor *errors_text_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[24];

  uint8_t no_response_count_{0};

  void on_status_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(number::Number *number, float value);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();

  std::string error_bits_to_string_(uint8_t bitmask);

  bool check_bit_(uint16_t mask, uint16_t flag) { return (mask & flag) == flag; }
};

}  // namespace jk_balancer
}  // namespace esphome
