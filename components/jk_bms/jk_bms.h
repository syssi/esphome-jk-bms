#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/jk_modbus/jk_modbus.h"

namespace esphome {
namespace jk_bms {

class JkBms : public PollingComponent, public jk_modbus::JkModbusDevice {
 public:
  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_balancing_switch_binary_sensor(binary_sensor::BinarySensor *balancing_switch_binary_sensor) {
    balancing_switch_binary_sensor_ = balancing_switch_binary_sensor;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_charging_switch_binary_sensor(binary_sensor::BinarySensor *charging_switch_binary_sensor) {
    charging_switch_binary_sensor_ = charging_switch_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
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
  void set_temperature_sensor_1_sensor(sensor::Sensor *temperature_sensor_1_sensor) {
    temperature_sensor_1_sensor_ = temperature_sensor_1_sensor;
  }
  void set_temperature_sensor_2_sensor(sensor::Sensor *temperature_sensor_2_sensor) {
    temperature_sensor_2_sensor_ = temperature_sensor_2_sensor;
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
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
  }

  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }

  void dump_config() override;

  void on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override;

  void update() override;

 protected:
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
  sensor::Sensor *total_battery_capacity_setting_sensor_;
  sensor::Sensor *charging_sensor_;
  sensor::Sensor *discharging_sensor_;
  sensor::Sensor *current_calibration_sensor_;
  sensor::Sensor *device_address_sensor_;
  sensor::Sensor *sleep_wait_time_sensor_;
  sensor::Sensor *alarm_low_volume_sensor_;
  sensor::Sensor *password_sensor_;
  sensor::Sensor *manufacturing_date_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *start_current_calibration_sensor_;
  sensor::Sensor *actual_battery_capacity_sensor_;
  sensor::Sensor *protocol_version_sensor_;

  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *balancing_switch_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *charging_switch_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_switch_binary_sensor_;
  binary_sensor::BinarySensor *dedicated_charger_switch_binary_sensor_;

  text_sensor::TextSensor *errors_text_sensor_;
  text_sensor::TextSensor *operation_mode_text_sensor_;
  text_sensor::TextSensor *battery_type_text_sensor_;
  text_sensor::TextSensor *password_text_sensor_;
  text_sensor::TextSensor *device_type_text_sensor_;
  text_sensor::TextSensor *software_version_text_sensor_;
  text_sensor::TextSensor *manufacturer_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[24];

  bool enable_fake_traffic_;

  void on_status_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);

  std::string error_bits_to_string_(uint16_t bitmask);
  std::string mode_bits_to_string_(uint16_t bitmask);

  float get_temperature_(const uint16_t value) {
    if (value > 100)
      return (float) (100 - (int16_t) value);

    return (float) value;
  };

  float get_current_(const uint16_t value, const uint8_t protocol_version) {
    float current = 0.0f;
    if (protocol_version == 0x01) {
      if ((value & 0x8000) == 0x8000) {
        current = (float) (value & 0x7FFF);
      } else {
        current = (float) (value & 0x7FFF) * -1;
      }
    }

    return current;
  };

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
};

}  // namespace jk_bms
}  // namespace esphome
