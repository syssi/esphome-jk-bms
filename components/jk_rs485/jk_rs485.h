#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace jk_rs485 {

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

class JkRS485Device;

class JkRS485 : public uart::UARTDevice, public Component {
 public:
  JkRS485() = default;

  void loop() override;
  void dump_config() override;
//  void update() override;
//  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_last_detected_address(uint8_t address){
    last_detected_address_=address;
  }

  uint8_t get_last_detected_address(void){
    return(last_detected_address_);
  }

 
  void set_balancing_switch_binary_sensor(binary_sensor::BinarySensor *balancing_switch_binary_sensor) {
    balancing_switch_binary_sensor_ = balancing_switch_binary_sensor;
  }

  void set_charging_switch_binary_sensor(binary_sensor::BinarySensor *charging_switch_binary_sensor) {
    charging_switch_binary_sensor_ = charging_switch_binary_sensor;
  }

  void set_discharging_switch_binary_sensor(binary_sensor::BinarySensor *discharging_switch_binary_sensor) {
    discharging_switch_binary_sensor_ = discharging_switch_binary_sensor;
  }
  void set_dedicated_charger_switch_binary_sensor(binary_sensor::BinarySensor *dedicated_charger_switch_binary_sensor) {
    dedicated_charger_switch_binary_sensor_ = dedicated_charger_switch_binary_sensor;
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
  void set_power_tube_temperature_sensor(sensor::Sensor *power_tube_temperature_sensor) {
    power_tube_temperature_sensor_ = power_tube_temperature_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }
  void set_temperature_sensor_1_sensor(sensor::Sensor *temperature_sensor_1_sensor) {
    temperature_sensor_1_sensor_ = temperature_sensor_1_sensor;
  }
  void set_temperature_sensor_2_sensor(sensor::Sensor *temperature_sensor_2_sensor) {
    temperature_sensor_2_sensor_ = temperature_sensor_2_sensor;
  }

  void set_charging_power_sensor(sensor::Sensor *charging_power_sensor) {
    charging_power_sensor_ = charging_power_sensor;
  }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power_sensor) {
    discharging_power_sensor_ = discharging_power_sensor;
  }
  void set_capacity_remaining_sensor(sensor::Sensor *capacity_remaining_sensor) {
    capacity_remaining_sensor_ = capacity_remaining_sensor;
  }
  void set_capacity_remaining_derived_sensor(sensor::Sensor *capacity_remaining_derived_sensor) {
    capacity_remaining_derived_sensor_ = capacity_remaining_derived_sensor;
  }
  void set_temperature_sensors_sensor(sensor::Sensor *temperature_sensors_sensor) {
    temperature_sensors_sensor_ = temperature_sensors_sensor;
  }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }
  void set_total_charging_cycle_capacity_sensor(sensor::Sensor *total_charging_cycle_capacity_sensor) {
    total_charging_cycle_capacity_sensor_ = total_charging_cycle_capacity_sensor;
  }
  void set_battery_strings_sensor(sensor::Sensor *battery_strings_sensor) {
    battery_strings_sensor_ = battery_strings_sensor;
  }
  void set_errors_bitmask_sensor(sensor::Sensor *errors_bitmask_sensor) {
    errors_bitmask_sensor_ = errors_bitmask_sensor;
  }
  void set_operation_mode_bitmask_sensor(sensor::Sensor *operation_mode_bitmask_sensor) {
    operation_mode_bitmask_sensor_ = operation_mode_bitmask_sensor;
  }
  void set_total_voltage_overvoltage_protection_sensor(sensor::Sensor *total_voltage_overvoltage_protection_sensor) {
    total_voltage_overvoltage_protection_sensor_ = total_voltage_overvoltage_protection_sensor;
  }
  void set_total_voltage_undervoltage_protection_sensor(sensor::Sensor *total_voltage_undervoltage_protection_sensor) {
    total_voltage_undervoltage_protection_sensor_ = total_voltage_undervoltage_protection_sensor;
  }
  void set_cell_voltage_overvoltage_protection_sensor(sensor::Sensor *cell_voltage_overvoltage_protection_sensor) {
    cell_voltage_overvoltage_protection_sensor_ = cell_voltage_overvoltage_protection_sensor;
  }
  void set_cell_voltage_overvoltage_recovery_sensor(sensor::Sensor *cell_voltage_overvoltage_recovery_sensor) {
    cell_voltage_overvoltage_recovery_sensor_ = cell_voltage_overvoltage_recovery_sensor;
  }
  void set_cell_voltage_overvoltage_delay_sensor(sensor::Sensor *cell_voltage_overvoltage_delay_sensor) {
    cell_voltage_overvoltage_delay_sensor_ = cell_voltage_overvoltage_delay_sensor;
  }
  void set_cell_voltage_undervoltage_protection_sensor(sensor::Sensor *cell_voltage_undervoltage_protection_sensor) {
    cell_voltage_undervoltage_protection_sensor_ = cell_voltage_undervoltage_protection_sensor;
  }
  void set_cell_voltage_undervoltage_recovery_sensor(sensor::Sensor *cell_voltage_undervoltage_recovery_sensor) {
    cell_voltage_undervoltage_recovery_sensor_ = cell_voltage_undervoltage_recovery_sensor;
  }
  void set_cell_voltage_undervoltage_delay_sensor(sensor::Sensor *cell_voltage_undervoltage_delay_sensor) {
    cell_voltage_undervoltage_delay_sensor_ = cell_voltage_undervoltage_delay_sensor;
  }
  void set_cell_pressure_difference_protection_sensor(sensor::Sensor *cell_pressure_difference_protection_sensor) {
    cell_pressure_difference_protection_sensor_ = cell_pressure_difference_protection_sensor;
  }
  void set_discharging_overcurrent_protection_sensor(sensor::Sensor *discharging_overcurrent_protection_sensor) {
    discharging_overcurrent_protection_sensor_ = discharging_overcurrent_protection_sensor;
  }
  void set_discharging_overcurrent_delay_sensor(sensor::Sensor *discharging_overcurrent_delay_sensor) {
    discharging_overcurrent_delay_sensor_ = discharging_overcurrent_delay_sensor;
  }
  void set_charging_overcurrent_protection_sensor(sensor::Sensor *charging_overcurrent_protection_sensor) {
    charging_overcurrent_protection_sensor_ = charging_overcurrent_protection_sensor;
  }
  void set_charging_overcurrent_delay_sensor(sensor::Sensor *charging_overcurrent_delay_sensor) {
    charging_overcurrent_delay_sensor_ = charging_overcurrent_delay_sensor;
  }
  void set_balance_starting_voltage_sensor(sensor::Sensor *balance_starting_voltage_sensor) {
    balance_starting_voltage_sensor_ = balance_starting_voltage_sensor;
  }
  void set_balance_opening_pressure_difference_sensor(sensor::Sensor *balance_opening_pressure_difference_sensor) {
    balance_opening_pressure_difference_sensor_ = balance_opening_pressure_difference_sensor;
  }
  void set_power_tube_temperature_protection_sensor(sensor::Sensor *power_tube_temperature_protection_sensor) {
    power_tube_temperature_protection_sensor_ = power_tube_temperature_protection_sensor;
  }
  void set_power_tube_temperature_recovery_sensor(sensor::Sensor *power_tube_temperature_recovery_sensor) {
    power_tube_temperature_recovery_sensor_ = power_tube_temperature_recovery_sensor;
  }
  void set_temperature_sensor_temperature_protection_sensor(
      sensor::Sensor *temperature_sensor_temperature_protection_sensor) {
    temperature_sensor_temperature_protection_sensor_ = temperature_sensor_temperature_protection_sensor;
  }
  void set_temperature_sensor_temperature_recovery_sensor(
      sensor::Sensor *temperature_sensor_temperature_recovery_sensor) {
    temperature_sensor_temperature_recovery_sensor_ = temperature_sensor_temperature_recovery_sensor;
  }
  void set_temperature_sensor_temperature_difference_protection_sensor(
      sensor::Sensor *temperature_sensor_temperature_difference_protection_sensor) {
    temperature_sensor_temperature_difference_protection_sensor_ =
        temperature_sensor_temperature_difference_protection_sensor;
  }
  void set_charging_high_temperature_protection_sensor(sensor::Sensor *charging_high_temperature_protection_sensor) {
    charging_high_temperature_protection_sensor_ = charging_high_temperature_protection_sensor;
  }
  void set_discharging_high_temperature_protection_sensor(
      sensor::Sensor *discharging_high_temperature_protection_sensor) {
    discharging_high_temperature_protection_sensor_ = discharging_high_temperature_protection_sensor;
  }
  void set_charging_low_temperature_protection_sensor(sensor::Sensor *charging_low_temperature_protection_sensor) {
    charging_low_temperature_protection_sensor_ = charging_low_temperature_protection_sensor;
  }
  void set_charging_low_temperature_recovery_sensor(sensor::Sensor *charging_low_temperature_recovery_sensor) {
    charging_low_temperature_recovery_sensor_ = charging_low_temperature_recovery_sensor;
  }
  void set_discharging_low_temperature_protection_sensor(
      sensor::Sensor *discharging_low_temperature_protection_sensor) {
    discharging_low_temperature_protection_sensor_ = discharging_low_temperature_protection_sensor;
  }
  void set_discharging_low_temperature_recovery_sensor(sensor::Sensor *discharging_low_temperature_recovery_sensor) {
    discharging_low_temperature_recovery_sensor_ = discharging_low_temperature_recovery_sensor;
  }
  void set_total_battery_capacity_setting_sensor(sensor::Sensor *total_battery_capacity_setting_sensor) {
    total_battery_capacity_setting_sensor_ = total_battery_capacity_setting_sensor;
  }
  void set_current_calibration_sensor(sensor::Sensor *current_calibration_sensor) {
    current_calibration_sensor_ = current_calibration_sensor;
  }
  void set_device_address_sensor(sensor::Sensor *device_address_sensor) {
    device_address_sensor_ = device_address_sensor;
  }
  void set_sleep_wait_time_sensor(sensor::Sensor *sleep_wait_time_sensor) {
    sleep_wait_time_sensor_ = sleep_wait_time_sensor;
  }
  void set_alarm_low_volume_sensor(sensor::Sensor *alarm_low_volume_sensor) {
    alarm_low_volume_sensor_ = alarm_low_volume_sensor;
  }
  void set_manufacturing_date_sensor(sensor::Sensor *manufacturing_date_sensor) {
    manufacturing_date_sensor_ = manufacturing_date_sensor;
  }
  void set_total_runtime_sensor(sensor::Sensor *total_runtime_sensor) { total_runtime_sensor_ = total_runtime_sensor; }
  void set_start_current_calibration_sensor(sensor::Sensor *start_current_calibration_sensor) {
    start_current_calibration_sensor_ = start_current_calibration_sensor;
  }
  void set_actual_battery_capacity_sensor(sensor::Sensor *actual_battery_capacity_sensor) {
    actual_battery_capacity_sensor_ = actual_battery_capacity_sensor;
  }
  void set_protocol_version_sensor(sensor::Sensor *protocol_version_sensor) {
    protocol_version_sensor_ = protocol_version_sensor;
  }

  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }
  void set_discharging_switch(switch_::Switch *discharging_switch) { discharging_switch_ = discharging_switch; }
  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_operation_mode_text_sensor(text_sensor::TextSensor *operation_mode_text_sensor) {
    operation_mode_text_sensor_ = operation_mode_text_sensor;
  }
  void set_battery_type_text_sensor(text_sensor::TextSensor *battery_type_text_sensor) {
    battery_type_text_sensor_ = battery_type_text_sensor;
  }
  void set_password_text_sensor(text_sensor::TextSensor *password_text_sensor) {
    password_text_sensor_ = password_text_sensor;
  }
  void set_device_type_text_sensor(text_sensor::TextSensor *device_type_text_sensor) {
    device_type_text_sensor_ = device_type_text_sensor;
  }
  void set_software_version_text_sensor(text_sensor::TextSensor *software_version_text_sensor) {
    software_version_text_sensor_ = software_version_text_sensor;
  }
  void set_manufacturer_text_sensor(text_sensor::TextSensor *manufacturer_text_sensor) {
    manufacturer_text_sensor_ = manufacturer_text_sensor;
  }
 
  void register_device(JkRS485Device *device) { this->devices_.push_back(device); }

//  float get_setup_priority() const override;

  void send(uint8_t function, uint8_t address, uint8_t value);
  void write_register(uint8_t address, uint8_t value);
  //void read_registers();
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }


  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
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
  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status_binary_sensor) {
    online_status_binary_sensor_ = online_status_binary_sensor;
  }
  void set_heating_binary_sensor(binary_sensor::BinarySensor *heating_binary_sensor) {
    heating_binary_sensor_ = heating_binary_sensor;
  }
  
  void set_balancer_switch(switch_::Switch *balancer_switch) { balancer_switch_ = balancer_switch; }
  void set_emergency_switch(switch_::Switch *emergency_switch) { emergency_switch_ = emergency_switch; }
  void set_heating_switch(switch_::Switch *heating_switch) { heating_switch_ = heating_switch; }
  void set_disable_temperature_sensors_switch(switch_::Switch *disable_temperature_sensors_switch) {
    disable_temperature_sensors_switch_ = disable_temperature_sensors_switch;
  }
  void set_display_always_on_switch(switch_::Switch *display_always_on_switch) {
    display_always_on_switch_ = display_always_on_switch;
  }


  void set_protocol_version(ProtocolVersion protocol_version) { protocol_version_ = protocol_version; }
  ProtocolVersion get_protocol_version() { return protocol_version_; }

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[32];
  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[4];

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_24S};

  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *balancing_switch_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *charging_switch_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_switch_binary_sensor_;
  binary_sensor::BinarySensor *dedicated_charger_switch_binary_sensor_;
  binary_sensor::BinarySensor *online_status_binary_sensor_;
  binary_sensor::BinarySensor *heating_binary_sensor_;

  uint8_t last_detected_address_;
  number::Number *soc100_voltage_number_;
  number::Number *soc0_voltage_number_;
  number::Number *voltage_cell_request_charge_voltage_number_;
  number::Number *voltage_cell_request_float_voltage_number_;
  number::Number *charge_ocp_delay_number_;
  number::Number *charge_ocp_recovery_delay_number_;
  number::Number *discharge_ocp_delay_number_;
  number::Number *discharge_ocp_recovery_time_number_;
  number::Number *scp_recovery_time_number_;    
  number::Number *balance_trigger_voltage_number_;
  number::Number *cell_count_number_;
  number::Number *total_battery_capacity_number_;
  number::Number *cell_voltage_overvoltage_protection_number_;
  number::Number *cell_voltage_overvoltage_recovery_number_;
  number::Number *voltage_smart_sleep_number_;
  number::Number *cell_voltage_undervoltage_protection_number_;  
  number::Number *cell_voltage_undervoltage_recovery_number_;
  number::Number *balance_starting_voltage_number_;
  number::Number *voltage_calibration_number_;  // @FIXME: Identify value at the settings frame
  number::Number *current_calibration_number_;  // @FIXME: Identify value at the settings frame
  number::Number *power_off_voltage_number_;
  number::Number *max_balance_current_number_;
  number::Number *max_charge_current_number_;
  number::Number *max_discharge_current_number_;

  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *total_battery_capacity_setting_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *balancing_current_sensor_;
  sensor::Sensor *emergency_time_countdown_sensor_;
  sensor::Sensor *heating_current_sensor_;
  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;
  sensor::Sensor *power_tube_temperature_sensor_;
  sensor::Sensor *temperature_sensor_1_sensor_;
  sensor::Sensor *temperature_sensor_2_sensor_;
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *charging_power_sensor_;
  sensor::Sensor *discharging_power_sensor_;
  sensor::Sensor *capacity_remaining_sensor_;
  sensor::Sensor *capacity_remaining_derived_sensor_;
  sensor::Sensor *temperature_sensors_sensor_;
  sensor::Sensor *charging_cycles_sensor_;
  sensor::Sensor *total_charging_cycle_capacity_sensor_;
  sensor::Sensor *battery_strings_sensor_;
  sensor::Sensor *errors_bitmask_sensor_;
  sensor::Sensor *operation_mode_bitmask_sensor_;
  sensor::Sensor *total_voltage_overvoltage_protection_sensor_;
  sensor::Sensor *total_voltage_undervoltage_protection_sensor_;
  sensor::Sensor *cell_voltage_overvoltage_protection_sensor_;
  sensor::Sensor *cell_voltage_overvoltage_recovery_sensor_;
  sensor::Sensor *cell_voltage_overvoltage_delay_sensor_;
  sensor::Sensor *cell_voltage_undervoltage_protection_sensor_;
  sensor::Sensor *cell_voltage_undervoltage_recovery_sensor_;
  sensor::Sensor *cell_voltage_undervoltage_delay_sensor_;
  sensor::Sensor *cell_pressure_difference_protection_sensor_;
  sensor::Sensor *discharging_overcurrent_protection_sensor_;
  sensor::Sensor *discharging_overcurrent_delay_sensor_;
  sensor::Sensor *charging_overcurrent_protection_sensor_;
  sensor::Sensor *charging_overcurrent_delay_sensor_;
  sensor::Sensor *balance_starting_voltage_sensor_;
  sensor::Sensor *balance_opening_pressure_difference_sensor_;
  sensor::Sensor *power_tube_temperature_protection_sensor_;
  sensor::Sensor *power_tube_temperature_recovery_sensor_;
  sensor::Sensor *temperature_sensor_temperature_protection_sensor_;
  sensor::Sensor *temperature_sensor_temperature_recovery_sensor_;
  sensor::Sensor *temperature_sensor_temperature_difference_protection_sensor_;
  sensor::Sensor *charging_high_temperature_protection_sensor_;
  sensor::Sensor *discharging_high_temperature_protection_sensor_;
  sensor::Sensor *charging_low_temperature_protection_sensor_;
  sensor::Sensor *charging_low_temperature_recovery_sensor_;
  sensor::Sensor *discharging_low_temperature_protection_sensor_;
  sensor::Sensor *discharging_low_temperature_recovery_sensor_;

  sensor::Sensor *charging_sensor_;
  sensor::Sensor *discharging_sensor_;
  sensor::Sensor *current_calibration_sensor_;
  sensor::Sensor *device_address_sensor_;
  sensor::Sensor *sleep_wait_time_sensor_;
  sensor::Sensor *alarm_low_volume_sensor_;
  sensor::Sensor *password_sensor_;
  sensor::Sensor *manufacturing_date_sensor_;
  sensor::Sensor *start_current_calibration_sensor_;
  sensor::Sensor *actual_battery_capacity_sensor_;
  sensor::Sensor *protocol_version_sensor_;

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
  text_sensor::TextSensor *operation_mode_text_sensor_;
  text_sensor::TextSensor *battery_type_text_sensor_;
  text_sensor::TextSensor *password_text_sensor_;
  text_sensor::TextSensor *device_type_text_sensor_;
  text_sensor::TextSensor *software_version_text_sensor_;
  text_sensor::TextSensor *manufacturer_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;


  std::vector<uint8_t> frame_buffer_;
  bool status_notification_received_ = false;
  uint8_t no_response_count_{0};
  uint16_t char_handle_;
  uint16_t notify_handle_;
  uint32_t last_cell_info_{0};
  uint32_t throttle_;

  void authenticate_();
  bool parse_jk_rs485_byte_(uint8_t byte);
  void assemble(const uint8_t *data, uint16_t length);
  void decode_(const std::vector<uint8_t> &data, uint8_t frame_type);
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



  std::vector<uint8_t> rx_buffer_;
  uint16_t rx_timeout_{50};
  uint32_t last_jk_rs485_byte_{0};
  std::vector<JkRS485Device *> devices_;
};

class JkRS485Device {
 public:
  void set_parent(JkRS485 *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_rs485_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(int8_t function, uint8_t address, uint8_t value) { this->parent_->send(function, address, value); }
  void write_register(uint8_t address, uint8_t value) { this->parent_->write_register(address, value); }
  //void read_registers() { this->parent_->read_registers(); }

 protected:
  friend JkRS485;

  JkRS485 *parent_;
  uint8_t address_;
};

}  // namespace jk_rs485
}  // namespace esphome
