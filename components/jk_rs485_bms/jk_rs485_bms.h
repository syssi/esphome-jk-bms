#pragma once

#include "switch/jk_switch.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "../jk_rs485_sniffer/jk_rs485_sniffer.h"
//#include "esphome/core/component.h"

namespace esphome {

namespace jk_rs485_sniffer {
class JkRS485Sniffer;  // Declaración anticipada
};


namespace jk_rs485_bms {

class JkRS485BmsSwitch;  // Declaración anticipada de JkRS485BmsSwitch

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

/*
El problema que estás experimentando se debe a que el compilador encuentra referencias a JkRS485BmsSwitch en el archivo jk_rs485_bms.h antes de conocer la 
definición de la clase JkRS485BmsSwitch. 

Esto ocurre porque jk_rs485_bms.h se incluye antes de que el compilador haya procesado la definición de JkRS485BmsSwitch.
1º jk_rs485_bms.h
2º jk_switch.h

*/
class JkRS485Bms : public PollingComponent, public jk_rs485_sniffer::JkRS485SnifferDevice {
 public:
  // Constructor por defecto necesario para crear instancias sin argumentos
  JkRS485Bms() = default;

  virtual ~JkRS485Bms() = default; // Destructor por defecto


  void JkRS485Bms_init(void);


  void set_sniffer_parent(jk_rs485_sniffer::JkRS485Sniffer *parent);
  
  jk_rs485_sniffer::JkRS485Sniffer* get_sniffer_parent(void); // Nuevo método para obtener el parent
  
  void set_address(uint8_t address) { address_ = address; }

  uint8_t get_address() const { return this->address_; }

  void trigger_bms2sniffer_event(std::string event, std::uint8_t frame_type);

  void trigger_bms2sniffer_switch16_event(std::uint8_t register_address);
  void trigger_bms2sniffer_switch64_event(std::uint8_t register_address, std::uint64_t value);

  void set_smart_sleep_time_sensor(sensor::Sensor *smart_sleep_time_sensor) {
    smart_sleep_time_sensor_ = smart_sleep_time_sensor;
  }
  void set_emergency_time_countdown_sensor(sensor::Sensor *emergency_time_countdown_sensor) {
    emergency_time_countdown_sensor_ = emergency_time_countdown_sensor;
  }
  void set_balancing_direction_sensor(sensor::Sensor *balancing_direction_sensor) {
    balancing_direction_sensor_ = balancing_direction_sensor;
  } 
  void set_status_balancing_binary_sensor(binary_sensor::BinarySensor *status_balancing_binary_sensor) {
    status_balancing_binary_sensor_ = status_balancing_binary_sensor;
  }
  void set_status_precharging_binary_sensor(binary_sensor::BinarySensor *status_precharging_binary_sensor) {
    status_precharging_binary_sensor_ = status_precharging_binary_sensor;
  }
  void set_status_charging_binary_sensor(binary_sensor::BinarySensor *status_charging_binary_sensor) {
    status_charging_binary_sensor_ = status_charging_binary_sensor;
  }
  void set_status_discharging_binary_sensor(binary_sensor::BinarySensor *status_discharging_binary_sensor) {
    status_discharging_binary_sensor_ = status_discharging_binary_sensor;
  }
  void set_status_online_binary_sensor(binary_sensor::BinarySensor *status_online_binary_sensor) {
    status_online_binary_sensor_ = status_online_binary_sensor;
  }
  void set_status_heating_binary_sensor(binary_sensor::BinarySensor *status_heating_binary_sensor) {
    status_heating_binary_sensor_ = status_heating_binary_sensor;
  }   
  void set_cell_count_settings_sensor(sensor::Sensor *cell_count_settings_sensor) { cell_count_settings_sensor_ = cell_count_settings_sensor; }

  void set_balancing_switch_binary_sensor(binary_sensor::BinarySensor *balancing_switch_binary_sensor) {
    balancing_switch_binary_sensor_ = balancing_switch_binary_sensor;
  }

  //void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
  //  charging_binary_sensor_ = charging_binary_sensor;
  //}
  void set_charging_switch_binary_sensor(binary_sensor::BinarySensor *charging_switch_binary_sensor) {
    charging_switch_binary_sensor_ = charging_switch_binary_sensor;
  }

  //void set_precharging_binary_sensor(binary_sensor::BinarySensor *precharging_binary_sensor) {
  //  precharging_binary_sensor_ = precharging_binary_sensor;
  //}
  void set_precharging_switch_binary_sensor(binary_sensor::BinarySensor *precharging_switch_binary_sensor) {
    precharging_switch_binary_sensor_ = precharging_switch_binary_sensor;
  }  
  
  //void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
  //  discharging_binary_sensor_ = discharging_binary_sensor;
  //}
  void set_discharging_switch_binary_sensor(binary_sensor::BinarySensor *discharging_switch_binary_sensor) {
    discharging_switch_binary_sensor_ = discharging_switch_binary_sensor;
  }

  void set_dedicated_charger_switch_binary_sensor(binary_sensor::BinarySensor *dedicated_charger_switch_binary_sensor) {
    dedicated_charger_switch_binary_sensor_ = dedicated_charger_switch_binary_sensor;
  }

  void set_cell_smart_sleep_voltage_sensor(sensor::Sensor *cell_smart_sleep_voltage_sensor) {
    cell_smart_sleep_voltage_sensor_ = cell_smart_sleep_voltage_sensor;
  }


  
  void set_cell_count_real_sensor(sensor::Sensor *cell_count_real_sensor) {
    cell_count_real_sensor_ = cell_count_real_sensor;
  }

  void set_cell_voltage_min_sensor(sensor::Sensor *cell_voltage_min_sensor) {
    cell_voltage_min_sensor_ = cell_voltage_min_sensor;
  }
  void set_cell_voltage_max_sensor(sensor::Sensor *cell_voltage_max_sensor) {
    cell_voltage_max_sensor_ = cell_voltage_max_sensor;
  }
  void set_cell_resistance_min_sensor(sensor::Sensor *cell_resistance_min_sensor) {
    cell_resistance_min_sensor_ = cell_resistance_min_sensor;
  }
  void set_cell_resistance_max_sensor(sensor::Sensor *cell_resistance_max_sensor) {
    cell_resistance_max_sensor_ = cell_resistance_max_sensor;
  }
  void set_cell_voltage_min_cell_number_sensor(sensor::Sensor *cell_voltage_min_cell_number_sensor) {
    cell_voltage_min_cell_number_sensor_ = cell_voltage_min_cell_number_sensor;
  }
  void set_cell_voltage_max_cell_number_sensor(sensor::Sensor *cell_voltage_max_cell_number_sensor) {
    cell_voltage_max_cell_number_sensor_ = cell_voltage_max_cell_number_sensor;
  }  
  void set_cell_resistance_min_cell_number_sensor(sensor::Sensor *cell_resistance_min_cell_number_sensor) {
    cell_resistance_min_cell_number_sensor_ = cell_resistance_min_cell_number_sensor;
  }
  void set_cell_resistance_max_cell_number_sensor(sensor::Sensor *cell_resistance_max_cell_number_sensor) {
    cell_resistance_max_cell_number_sensor_ = cell_resistance_max_cell_number_sensor;
  }
  void set_cell_delta_voltage_sensor(sensor::Sensor *cell_delta_voltage_sensor) {
    cell_delta_voltage_sensor_ = cell_delta_voltage_sensor;
  }
  void set_cell_average_voltage_sensor(sensor::Sensor *cell_average_voltage_sensor) {
    cell_average_voltage_sensor_ = cell_average_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_cell_resistance_sensor(uint8_t cell, sensor::Sensor *cell_resistance_sensor) {
    this->cells_[cell].cell_resistance_sensor_ = cell_resistance_sensor;
  }
//  void set_temperature_temperature_sensor(uint8_t i, sensor::Sensor *temperature_temperature_sensor) {
//    this->temperatures_[i].temperature_sensor_ = temperature_temperature_sensor;
//  }  
  void set_temperature_sensor(uint8_t i, sensor::Sensor *temperature_sensor) {
    this->temperatures_[i].temperature_sensor_ = temperature_sensor;
  }    
  void set_temperature_powertube_sensor(sensor::Sensor *temperature_powertube_sensor) {
    temperature_powertube_sensor_ = temperature_powertube_sensor;
  }
  void set_temperature_sensor_1_sensor(sensor::Sensor *temperature_sensor_1_sensor) {
    temperature_sensor_1_sensor_ = temperature_sensor_1_sensor;
  }
  void set_temperature_sensor_2_sensor(sensor::Sensor *temperature_sensor_2_sensor) {
    temperature_sensor_2_sensor_ = temperature_sensor_2_sensor;
  }
  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) { battery_voltage_sensor_ = battery_voltage_sensor; }
  void set_battery_current_sensor(sensor::Sensor *battery_current_sensor) { battery_current_sensor_ = battery_current_sensor; }
  void set_battery_power_sensor(sensor::Sensor *battery_power_sensor) { battery_power_sensor_ = battery_power_sensor; }
  void set_battery_power_charging_sensor(sensor::Sensor *battery_power_charging_sensor) {
    battery_power_charging_sensor_ = battery_power_charging_sensor;
  }
  void set_battery_power_discharging_sensor(sensor::Sensor *battery_power_discharging_sensor) {
    battery_power_discharging_sensor_ = battery_power_discharging_sensor;
  }
  void set_battery_capacity_remaining_sensor(sensor::Sensor *battery_capacity_remaining_sensor) {
    battery_capacity_remaining_sensor_ = battery_capacity_remaining_sensor;
  }
  void set_battery_capacity_remaining_derived_sensor(sensor::Sensor *battery_capacity_remaining_derived_sensor) {
    battery_capacity_remaining_derived_sensor_ = battery_capacity_remaining_derived_sensor;
  }
  void set_temperature_sensors_sensor(sensor::Sensor *temperature_sensors_sensor) {
    temperature_sensors_sensor_ = temperature_sensors_sensor;
  }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }
  void set_battery_capacity_total_charging_cycle_sensor(sensor::Sensor *battery_capacity_total_charging_cycle_sensor) {
    battery_capacity_total_charging_cycle_sensor_ = battery_capacity_total_charging_cycle_sensor;
  }
  void set_battery_soh_valuation_sensor(sensor::Sensor *battery_soh_valuation_sensor) {
    battery_soh_valuation_sensor_ = battery_soh_valuation_sensor;
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
  void set_cell_overvoltage_protection_sensor(sensor::Sensor *cell_overvoltage_protection_sensor) {
    cell_overvoltage_protection_sensor_ = cell_overvoltage_protection_sensor;
  }
  void set_cell_overvoltage_protection_recovery_sensor(sensor::Sensor *cell_overvoltage_protection_recovery_sensor) {
    cell_overvoltage_protection_recovery_sensor_ = cell_overvoltage_protection_recovery_sensor;
  }
  void set_cell_voltage_overvoltage_delay_sensor(sensor::Sensor *cell_voltage_overvoltage_delay_sensor) {
    cell_voltage_overvoltage_delay_sensor_ = cell_voltage_overvoltage_delay_sensor;
  }
  void set_cell_undervoltage_protection_sensor(sensor::Sensor *cell_undervoltage_protection_sensor) {
    cell_undervoltage_protection_sensor_ = cell_undervoltage_protection_sensor;
  }
  void set_cell_undervoltage_protection_recovery_sensor(sensor::Sensor *cell_undervoltage_protection_recovery_sensor) {
    cell_undervoltage_protection_recovery_sensor_ = cell_undervoltage_protection_recovery_sensor;
  }
  void set_cell_request_charge_voltage_sensor(sensor::Sensor *cell_request_charge_voltage_sensor) {
    cell_request_charge_voltage_sensor_ = cell_request_charge_voltage_sensor;
  }
  void set_cell_request_float_voltage_sensor(sensor::Sensor *cell_request_float_voltage_sensor) {
    cell_request_float_voltage_sensor_ = cell_request_float_voltage_sensor;
  }
  void set_cell_voltage_undervoltage_delay_sensor(sensor::Sensor *cell_voltage_undervoltage_delay_sensor) {
    cell_voltage_undervoltage_delay_sensor_ = cell_voltage_undervoltage_delay_sensor;
  }
  void set_cell_soc100_voltage_sensor(sensor::Sensor *cell_soc100_voltage_sensor) {
    cell_soc100_voltage_sensor_ = cell_soc100_voltage_sensor;
  }
  void set_cell_soc0_voltage_sensor(sensor::Sensor *cell_soc0_voltage_sensor) {
    cell_soc0_voltage_sensor_ = cell_soc0_voltage_sensor;
  }
  void set_balancing_trigger_voltage_sensor(sensor::Sensor *balancing_trigger_voltage_sensor) {
    balancing_trigger_voltage_sensor_ = balancing_trigger_voltage_sensor;
  }
  
  void set_cell_power_off_voltage_sensor(sensor::Sensor *cell_power_off_voltage_sensor) {
    cell_power_off_voltage_sensor_ = cell_power_off_voltage_sensor;
  }

  void set_cell_pressure_difference_protection_sensor(sensor::Sensor *cell_pressure_difference_protection_sensor) {
    cell_pressure_difference_protection_sensor_ = cell_pressure_difference_protection_sensor;
  }
  
  
  void set_charging_overcurrent_protection_delay_sensor(sensor::Sensor *charging_overcurrent_protection_delay_sensor) {
    charging_overcurrent_protection_delay_sensor_ = charging_overcurrent_protection_delay_sensor;
  }
  void set_charging_overcurrent_protection_recovery_delay_sensor(sensor::Sensor *charging_overcurrent_protection_recovery_delay_sensor) {
    charging_overcurrent_protection_recovery_delay_sensor_ = charging_overcurrent_protection_recovery_delay_sensor;
  }
  void set_discharging_overcurrent_protection_delay_sensor(sensor::Sensor *discharging_overcurrent_protection_delay_sensor) {
    discharging_overcurrent_protection_delay_sensor_ = discharging_overcurrent_protection_delay_sensor;
  }
  void set_discharging_overcurrent_protection_recovery_delay_sensor(sensor::Sensor *discharging_overcurrent_protection_recovery_delay_sensor) {
    discharging_overcurrent_protection_recovery_delay_sensor_ = discharging_overcurrent_protection_recovery_delay_sensor;
  }
  void set_short_circuit_protection_delay_sensor(sensor::Sensor *short_circuit_protection_delay_sensor) {
    short_circuit_protection_delay_sensor_ = short_circuit_protection_delay_sensor;
  }
  void set_short_circuit_protection_recovery_delay_sensor(sensor::Sensor *short_circuit_protection_recovery_delay_sensor) {
    short_circuit_protection_recovery_delay_sensor_ = short_circuit_protection_recovery_delay_sensor;
  }
  void set_charging_overtemperature_protection_sensor(sensor::Sensor *charging_overtemperature_protection_sensor) {
    charging_overtemperature_protection_sensor_ = charging_overtemperature_protection_sensor;
  }
  void set_charging_overtemperature_protection_recovery_sensor(sensor::Sensor *charging_overtemperature_protection_recovery_sensor) {
    charging_overtemperature_protection_recovery_sensor_ = charging_overtemperature_protection_recovery_sensor;
  }
  void set_discharging_overtemperature_protection_sensor(sensor::Sensor *discharging_overtemperature_protection_sensor) {
    discharging_overtemperature_protection_sensor_ = discharging_overtemperature_protection_sensor;
  }
  void set_discharging_overtemperature_protection_recovery_sensor(sensor::Sensor *discharging_overtemperature_protection_recovery_sensor) {
    discharging_overtemperature_protection_recovery_sensor_ = discharging_overtemperature_protection_recovery_sensor;
  }
  void set_charging_lowtemperature_protection_sensor(sensor::Sensor *charging_lowtemperature_protection_sensor) {
    charging_lowtemperature_protection_sensor_ = charging_lowtemperature_protection_sensor;
  }
  void set_charging_lowtemperature_protection_recovery_sensor(sensor::Sensor *charging_lowtemperature_protection_recovery_sensor) {
    charging_lowtemperature_protection_recovery_sensor_ = charging_lowtemperature_protection_recovery_sensor;
  }  
  void set_mos_overtemperature_protection_sensor(sensor::Sensor *mos_overtemperature_protection_sensor) {
    mos_overtemperature_protection_sensor_ = mos_overtemperature_protection_sensor;
  }
  void set_mos_overtemperature_protection_recovery_sensor(sensor::Sensor *mos_overtemperature_protection_recovery_sensor) {
    mos_overtemperature_protection_recovery_sensor_ = mos_overtemperature_protection_recovery_sensor;
  }

  void set_precharging_time_from_discharge_sensor(sensor::Sensor *precharging_time_from_discharge_sensor) {
    precharging_time_from_discharge_sensor_ = precharging_time_from_discharge_sensor;
  }


  void set_discharging_overcurrent_protection_release_time_sensor(sensor::Sensor *discharging_overcurrent_protection_release_time_sensor) {
    discharging_overcurrent_protection_release_time_sensor_ = discharging_overcurrent_protection_release_time_sensor;
  }
  void set_discharging_short_circuit_protection_release_time_sensor(sensor::Sensor *discharging_short_circuit_protection_release_time_sensor) {
    discharging_short_circuit_protection_release_time_sensor_ = discharging_short_circuit_protection_release_time_sensor;
  }
  void set_charging_overcurrent_protection_release_time_sensor(sensor::Sensor *charging_overcurrent_protection_release_time_sensor) {
    charging_overcurrent_protection_release_time_sensor_ = charging_overcurrent_protection_release_time_sensor;
  }
  void set_charging_short_circuit_protection_release_time_sensor(sensor::Sensor *charging_short_circuit_protection_release_time_sensor) {
    charging_short_circuit_protection_release_time_sensor_ = charging_short_circuit_protection_release_time_sensor;
  }
  void set_cell_undervoltage_protection_release_time_sensor(sensor::Sensor *cell_undervoltage_protection_release_time_sensor) {
    cell_undervoltage_protection_release_time_sensor_ = cell_undervoltage_protection_release_time_sensor;
  }
  void set_cell_overvoltage_protection_release_time_sensor(sensor::Sensor *cell_overvoltage_protection_release_time_sensor) {
    cell_overvoltage_protection_release_time_sensor_ = cell_overvoltage_protection_release_time_sensor;
  }



  void set_cell_balancing_starting_voltage_sensor(sensor::Sensor *cell_balancing_starting_voltage_sensor) {
    cell_balancing_starting_voltage_sensor_ = cell_balancing_starting_voltage_sensor;
  }
  void set_balancing_opening_pressure_difference_sensor(sensor::Sensor *balancing_opening_pressure_difference_sensor) {
    balancing_opening_pressure_difference_sensor_ = balancing_opening_pressure_difference_sensor;
  }
  void set_powertube_temperature_protection_sensor(sensor::Sensor *powertube_temperature_protection_sensor) {
    powertube_temperature_protection_sensor_ = powertube_temperature_protection_sensor;
  }
  void set_powertube_temperature_protection_recovery_sensor(sensor::Sensor *powertube_temperature_protection_recovery_sensor) {
    powertube_temperature_protection_recovery_sensor_ = powertube_temperature_protection_recovery_sensor;
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
  void set_battery_capacity_state_of_charge_sensor(sensor::Sensor *battery_capacity_state_of_charge_sensor) {
    battery_capacity_state_of_charge_sensor_ = battery_capacity_state_of_charge_sensor;
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
  void set_battery_capacity_total_setting_sensor(sensor::Sensor *battery_capacity_total_setting_sensor) {
    battery_capacity_total_setting_sensor_ = battery_capacity_total_setting_sensor;
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
  
  void set_battery_total_runtime_sensor(sensor::Sensor *battery_total_runtime_sensor) { 
    battery_total_runtime_sensor_ = battery_total_runtime_sensor; 
  }
  void set_start_current_calibration_sensor(sensor::Sensor *start_current_calibration_sensor) {
    start_current_calibration_sensor_ = start_current_calibration_sensor;
  }
  void set_actual_battery_capacity_sensor(sensor::Sensor *actual_battery_capacity_sensor) {
    actual_battery_capacity_sensor_ = actual_battery_capacity_sensor;
  }
  void set_protocol_version_sensor(sensor::Sensor *protocol_version_sensor) {
    protocol_version_sensor_ = protocol_version_sensor;
  }


  void set_max_balancing_current_sensor(sensor::Sensor *max_balancing_current_sensor) {
    max_balancing_current_sensor_ = max_balancing_current_sensor;
  }
  void set_max_charging_current_sensor(sensor::Sensor *max_charging_current_sensor) {
    max_charging_current_sensor_ = max_charging_current_sensor;
  } 
  void set_max_discharging_current_sensor(sensor::Sensor *max_discharging_current_sensor) {
    max_discharging_current_sensor_ = max_discharging_current_sensor;
  }   

  void set_battery_total_alarms_count_sensor(sensor::Sensor *battery_total_alarms_count_sensor){
    battery_total_alarms_count_sensor_ = battery_total_alarms_count_sensor;
  }
  void set_battery_total_alarms_active_sensor(sensor::Sensor *battery_total_alarms_active_sensor){
    battery_total_alarms_active_sensor_ = battery_total_alarms_active_sensor;
  }

  void set_uart1_protocol_number_sensor(sensor::Sensor *uart1_protocol_number_sensor) {
    uart1_protocol_number_sensor_ = uart1_protocol_number_sensor;
  }
  void set_uart2_protocol_number_sensor(sensor::Sensor *uart2_protocol_number_sensor) {
    uart2_protocol_number_sensor_ = uart2_protocol_number_sensor;
  }

  void set_cell_request_charge_voltage_time_sensor(sensor::Sensor *cell_request_charge_voltage_time_sensor) {
    cell_request_charge_voltage_time_sensor_ = cell_request_charge_voltage_time_sensor;
  }
  void set_cell_request_float_voltage_time_sensor(sensor::Sensor *cell_request_float_voltage_time_sensor) {
    cell_request_float_voltage_time_sensor_ = cell_request_float_voltage_time_sensor;
  }


  //void set_battery_total_alarms_count(uint8_t battery_total_alarms_count) { battery_total_alarms_count_ = battery_total_alarms_count; }
  //void set_battery_total_alarms_active(uint8_t battery_total_alarms_active) { battery_total_alarms_active_ = battery_total_alarms_active; }  

  // ALARMS
  void set_alarm_wireres_binary_sensor(binary_sensor::BinarySensor *alarm_wireres_binary_sensor) {
    alarm_wireres_binary_sensor_ = alarm_wireres_binary_sensor;
  }

  void set_alarm_mosotp_binary_sensor(binary_sensor::BinarySensor *alarm_mosotp_binary_sensor) {
    alarm_mosotp_binary_sensor_ = alarm_mosotp_binary_sensor;
  }

  void set_alarm_cellquantity_binary_sensor(binary_sensor::BinarySensor *alarm_cellquantity_binary_sensor) {
    alarm_cellquantity_binary_sensor_ = alarm_cellquantity_binary_sensor;
  }

  void set_alarm_cursensorerr_binary_sensor(binary_sensor::BinarySensor *alarm_cursensorerr_binary_sensor) {
    alarm_cursensorerr_binary_sensor_ = alarm_cursensorerr_binary_sensor;
  }

  void set_alarm_cellovp_binary_sensor(binary_sensor::BinarySensor *alarm_cellovp_binary_sensor) {
    alarm_cellovp_binary_sensor_ = alarm_cellovp_binary_sensor;

  }

  void set_alarm_batovp_binary_sensor(binary_sensor::BinarySensor *alarm_batovp_binary_sensor) {
    alarm_batovp_binary_sensor_ = alarm_batovp_binary_sensor;
  }

  void set_alarm_chocp_binary_sensor(binary_sensor::BinarySensor *alarm_chocp_binary_sensor) {
    alarm_chocp_binary_sensor_ = alarm_chocp_binary_sensor;
  }

  void set_alarm_chscp_binary_sensor(binary_sensor::BinarySensor *alarm_chscp_binary_sensor) {
    alarm_chscp_binary_sensor_ = alarm_chscp_binary_sensor;
  }

  void set_alarm_chotp_binary_sensor(binary_sensor::BinarySensor *alarm_chotp_binary_sensor) {
    alarm_chotp_binary_sensor_ = alarm_chotp_binary_sensor;
  }

  void set_alarm_chutp_binary_sensor(binary_sensor::BinarySensor *alarm_chutp_binary_sensor) {
    alarm_chutp_binary_sensor_ = alarm_chutp_binary_sensor;
    battery_total_alarms_count_=battery_total_alarms_count_+1;
    if (alarm_chutp_binary_sensor->state){
      battery_total_alarms_active_=battery_total_alarms_active_+1;
    }
  }

  void set_alarm_cpuauxcommuerr_binary_sensor(binary_sensor::BinarySensor *alarm_cpuauxcommuerr_binary_sensor) {
    alarm_cpuauxcommuerr_binary_sensor_ = alarm_cpuauxcommuerr_binary_sensor;
  }

  void set_alarm_celluvp_binary_sensor(binary_sensor::BinarySensor *alarm_celluvp_binary_sensor) {
    alarm_celluvp_binary_sensor_ = alarm_celluvp_binary_sensor;

  }

  void set_alarm_batuvp_binary_sensor(binary_sensor::BinarySensor *alarm_batuvp_binary_sensor) {
    alarm_batuvp_binary_sensor_ = alarm_batuvp_binary_sensor;
  }

  void set_alarm_dchocp_binary_sensor(binary_sensor::BinarySensor *alarm_dchocp_binary_sensor) {
    alarm_dchocp_binary_sensor_ = alarm_dchocp_binary_sensor;
  }

  void set_alarm_dchscp_binary_sensor(binary_sensor::BinarySensor *alarm_dchscp_binary_sensor) {
    alarm_dchscp_binary_sensor_ = alarm_dchscp_binary_sensor;
  }
  
  void set_alarm_dchotp_binary_sensor(binary_sensor::BinarySensor *alarm_dchotp_binary_sensor) {
    alarm_dchotp_binary_sensor_ = alarm_dchotp_binary_sensor;
  }

  void set_alarm_chargemos_binary_sensor(binary_sensor::BinarySensor *alarm_chargemos_binary_sensor) {
    alarm_chargemos_binary_sensor_ = alarm_chargemos_binary_sensor;
  }

  void set_alarm_dischargemos_binary_sensor(binary_sensor::BinarySensor *alarm_dischargemos_binary_sensor) {
    alarm_dischargemos_binary_sensor_ = alarm_dischargemos_binary_sensor;
  }

  void set_alarm_gpsdisconneted_binary_sensor(binary_sensor::BinarySensor *alarm_gpsdisconneted_binary_sensor) {
    alarm_gpsdisconneted_binary_sensor_ = alarm_gpsdisconneted_binary_sensor;
  }
  
  void set_alarm_modifypwdintime_binary_sensor(binary_sensor::BinarySensor *alarm_modifypwdintime_binary_sensor) {
    alarm_modifypwdintime_binary_sensor_ = alarm_modifypwdintime_binary_sensor;
  }
  
  void set_alarm_dischargeonfailed_binary_sensor(binary_sensor::BinarySensor *alarm_dischargeonfailed_binary_sensor) {
    alarm_dischargeonfailed_binary_sensor_ = alarm_dischargeonfailed_binary_sensor;
  }
  
  void set_alarm_batteryovertemp_binary_sensor(binary_sensor::BinarySensor *alarm_batteryovertemp_binary_sensor) {
    alarm_batteryovertemp_binary_sensor_ = alarm_batteryovertemp_binary_sensor;
  }

  void set_alarm_temperaturesensoranomaly_binary_sensor(binary_sensor::BinarySensor *alarm_temperaturesensoranomaly_binary_sensor) {
    alarm_temperaturesensoranomaly_binary_sensor_ = alarm_temperaturesensoranomaly_binary_sensor;
  }

  void set_alarm_plcmoduleanomaly_binary_sensor(binary_sensor::BinarySensor *alarm_plcmoduleanomaly_binary_sensor) {
    alarm_plcmoduleanomaly_binary_sensor_ = alarm_plcmoduleanomaly_binary_sensor;
  }

  void set_alarm_mostempsensorabsent_binary_sensor(binary_sensor::BinarySensor *alarm_mostempsensorabsent_binary_sensor) {
    alarm_mostempsensorabsent_binary_sensor_ = alarm_mostempsensorabsent_binary_sensor;
  }

  void set_alarm_battempsensor1absent_binary_sensor(binary_sensor::BinarySensor *alarm_battempsensor1absent_binary_sensor) {
    alarm_battempsensor1absent_binary_sensor_ = alarm_battempsensor1absent_binary_sensor;
  }

  void set_alarm_battempsensor2absent_binary_sensor(binary_sensor::BinarySensor *alarm_battempsensor2absent_binary_sensor) {
    alarm_battempsensor2absent_binary_sensor_ = alarm_battempsensor2absent_binary_sensor;
  }

  void set_alarm_battempsensor3absent_binary_sensor(binary_sensor::BinarySensor *alarm_battempsensor3absent_binary_sensor) {
    alarm_battempsensor3absent_binary_sensor_ = alarm_battempsensor3absent_binary_sensor;
  }

  void set_alarm_battempsensor4absent_binary_sensor(binary_sensor::BinarySensor *alarm_battempsensor4absent_binary_sensor) {
    alarm_battempsensor4absent_binary_sensor_ = alarm_battempsensor4absent_binary_sensor;
  }

  void set_alarm_battempsensor5absent_binary_sensor(binary_sensor::BinarySensor *alarm_battempsensor5absent_binary_sensor) {
    alarm_battempsensor5absent_binary_sensor_ = alarm_battempsensor5absent_binary_sensor;
  }



  void set_disable_pcl_module_switch(JkRS485BmsSwitch *disable_pcl_module_switch);

  void set_precharging_switch(JkRS485BmsSwitch *precharging_switch);
  void set_charging_switch(JkRS485BmsSwitch *charging_switch);
  void set_discharging_switch(JkRS485BmsSwitch *discharging_switch);
  void set_balancing_switch(JkRS485BmsSwitch *balancer_switch);
  void set_emergency_switch(JkRS485BmsSwitch *emergency_switch);
  void set_heating_switch(JkRS485BmsSwitch *heating_switch);
  void set_display_always_on_switch(JkRS485BmsSwitch *display_always_on_switch);
  void set_charging_float_mode_switch(JkRS485BmsSwitch *charging_float_mode_switch);
  void set_disable_temperature_sensors_switch(JkRS485BmsSwitch *disable_temperature_sensors_switch);
  void set_timed_stored_data_switch(JkRS485BmsSwitch *timed_stored_data_switch);
  void set_gps_heartbeat_switch(JkRS485BmsSwitch *gps_heartbeat_switch);
  void set_port_selection_switch(JkRS485BmsSwitch *port_selection_switch);
  void set_special_charger_switch(JkRS485BmsSwitch *special_charger_switch);
  void set_smart_sleep_on_switch(JkRS485BmsSwitch *smart_sleep_on_switch);




  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }

  void set_info_vendorid_text_sensor(text_sensor::TextSensor *info_vendorid_text_sensor) {
    info_vendorid_text_sensor_ = info_vendorid_text_sensor;
  }


  void set_info_hardware_version_text_sensor(text_sensor::TextSensor *info_hardware_version_text_sensor) {
      info_hardware_version_text_sensor_ = info_hardware_version_text_sensor;
  }

  void set_info_software_version_text_sensor(text_sensor::TextSensor *info_software_version_text_sensor) {
      info_software_version_text_sensor_ = info_software_version_text_sensor;
  }

  void set_info_device_name_text_sensor(text_sensor::TextSensor *info_device_name_text_sensor) {
      info_device_name_text_sensor_ = info_device_name_text_sensor;
  }

  void set_info_device_password_text_sensor(text_sensor::TextSensor *info_device_password_text_sensor) {
      info_device_password_text_sensor_ = info_device_password_text_sensor;
  }
  void set_info_device_serial_number_text_sensor(text_sensor::TextSensor *info_device_serial_number_text_sensor) {
      info_device_serial_number_text_sensor_ = info_device_serial_number_text_sensor;
  }
  void set_info_device_setup_passcode_text_sensor(text_sensor::TextSensor *info_device_setup_passcode_text_sensor) {
      info_device_setup_passcode_text_sensor_ = info_device_setup_passcode_text_sensor;
  }




  void set_network_nodes_available_text_sensor(text_sensor::TextSensor *network_nodes_available_text_sensor) { network_nodes_available_text_sensor_ = network_nodes_available_text_sensor; }

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
  void set_balancing_current_sensor(sensor::Sensor *balancing_current_sensor) {
    balancing_current_sensor_ = balancing_current_sensor;
  }  


  void dump_config() override;

  void on_jk_rs485_sniffer_data(const uint8_t &origin_address, const uint8_t &frame_type, const std::vector<uint8_t> &data,const std::string &nodes_available) override;

  void update() override;

  //bool write_register(uint8_t address, uint32_t value, uint8_t length);

 protected:
  jk_rs485_sniffer::JkRS485Sniffer *parent_;
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_32S};

  uint8_t address_;
  uint8_t battery_total_alarms_count_;
  uint8_t battery_total_alarms_active_;
  std::string nodes_available;
  //std::vector<JkRS485BmsSwitch *> switches_;    
  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
    sensor::Sensor *cell_resistance_sensor_{nullptr};
  } cells_[32];
  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[5];

  uint8_t no_response_count_{0};

  void on_status_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(JkRS485BmsSwitch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_alarm_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);  
  void publish_device_unavailable_();
  void reset_status_online_tracker_();
  void track_status_online_();


  bool status_notification_received_ = false;

  uint32_t last_cell_info_{0};
  uint32_t throttle_;

  void decode_jk02_cell_info_(const std::vector<uint8_t> &data);
  void decode_jk02_settings_(const std::vector<uint8_t> &data);
  void decode_jk04_cell_info_(const std::vector<uint8_t> &data);
  void decode_device_info_(const std::vector<uint8_t> &data);

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

  std::string format_total_runtime_(float value) {
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

  bool check_bit_of_byte_(uint8_t value, uint8_t position) { 
    //position: 76543210
    return ((value >> position) & 0x01); 
  }

  private:
    JkRS485BmsSwitch *precharging_switch_;
    JkRS485BmsSwitch *charging_switch_;
    JkRS485BmsSwitch *discharging_switch_;
    JkRS485BmsSwitch *balancer_switch_;
    JkRS485BmsSwitch *emergency_switch_;
    JkRS485BmsSwitch *heating_switch_;
    JkRS485BmsSwitch *charging_float_mode_switch_;
    JkRS485BmsSwitch *disable_temperature_sensors_switch_;
    JkRS485BmsSwitch *display_always_on_switch_;
    JkRS485BmsSwitch *smart_sleep_on_switch_;
    JkRS485BmsSwitch *timed_stored_data_switch_;
    JkRS485BmsSwitch *disable_pcl_module_switch_;
    JkRS485BmsSwitch *gps_heartbeat_switch_;
    JkRS485BmsSwitch *port_selection_switch_;
    JkRS485BmsSwitch *special_charger_switch_;  

    text_sensor::TextSensor *battery_type_text_sensor_;
    text_sensor::TextSensor *password_text_sensor_;
    text_sensor::TextSensor *info_device_serial_number_text_sensor_;
    text_sensor::TextSensor *device_type_text_sensor_;
    text_sensor::TextSensor *software_version_text_sensor_;
    text_sensor::TextSensor *manufacturer_text_sensor_;
    text_sensor::TextSensor *network_nodes_available_text_sensor_;
    text_sensor::TextSensor *errors_text_sensor_;
    text_sensor::TextSensor *operation_status_text_sensor_;
    text_sensor::TextSensor *total_runtime_formatted_text_sensor_;
    text_sensor::TextSensor *info_vendorid_text_sensor_;
    text_sensor::TextSensor *info_hardware_version_text_sensor_;
    text_sensor::TextSensor *info_software_version_text_sensor_;
    text_sensor::TextSensor *info_device_name_text_sensor_;
    text_sensor::TextSensor *info_device_password_text_sensor_;
    text_sensor::TextSensor *info_device_setup_passcode_text_sensor_;

    binary_sensor::BinarySensor *balancing_switch_binary_sensor_;
    binary_sensor::BinarySensor *precharging_switch_binary_sensor_;
    binary_sensor::BinarySensor *charging_switch_binary_sensor_;
    binary_sensor::BinarySensor *discharging_switch_binary_sensor_;
    binary_sensor::BinarySensor *dedicated_charger_switch_binary_sensor_;

    binary_sensor::BinarySensor *status_online_binary_sensor_;
    binary_sensor::BinarySensor *status_balancing_binary_sensor_;
    binary_sensor::BinarySensor *status_precharging_binary_sensor_;  
    binary_sensor::BinarySensor *status_charging_binary_sensor_;
    binary_sensor::BinarySensor *status_discharging_binary_sensor_;
    binary_sensor::BinarySensor *status_heating_binary_sensor_;

    binary_sensor::BinarySensor *alarm_wireres_binary_sensor_;
    binary_sensor::BinarySensor *alarm_mosotp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_cellquantity_binary_sensor_;
    binary_sensor::BinarySensor *alarm_cursensorerr_binary_sensor_;
    binary_sensor::BinarySensor *alarm_cellovp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_batovp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_chocp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_chscp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_chotp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_chutp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_cpuauxcommuerr_binary_sensor_;
    binary_sensor::BinarySensor *alarm_celluvp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_batuvp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_dchocp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_dchscp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_dchotp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_chargemos_binary_sensor_;
    binary_sensor::BinarySensor *alarm_dischargemos_binary_sensor_;
    binary_sensor::BinarySensor *alarm_gpsdisconneted_binary_sensor_;
    binary_sensor::BinarySensor *alarm_modifypwdintime_binary_sensor_;
    binary_sensor::BinarySensor *alarm_dischargeonfailed_binary_sensor_;
    binary_sensor::BinarySensor *alarm_batteryovertemp_binary_sensor_;
    binary_sensor::BinarySensor *alarm_temperaturesensoranomaly_binary_sensor_;
    binary_sensor::BinarySensor *alarm_plcmoduleanomaly_binary_sensor_;
    binary_sensor::BinarySensor *alarm_mostempsensorabsent_binary_sensor_;
    binary_sensor::BinarySensor *alarm_battempsensor1absent_binary_sensor_;
    binary_sensor::BinarySensor *alarm_battempsensor2absent_binary_sensor_;
    binary_sensor::BinarySensor *alarm_battempsensor3absent_binary_sensor_;
    binary_sensor::BinarySensor *alarm_battempsensor4absent_binary_sensor_;
    binary_sensor::BinarySensor *alarm_battempsensor5absent_binary_sensor_;

    sensor::Sensor *battery_total_alarms_count_sensor_;
    sensor::Sensor *battery_total_alarms_active_sensor_;
    sensor::Sensor *smart_sleep_time_sensor_;
    sensor::Sensor *emergency_time_countdown_sensor_;
    sensor::Sensor *cell_undervoltage_protection_sensor_;  
    sensor::Sensor *cell_undervoltage_protection_recovery_sensor_;
    sensor::Sensor *cell_overvoltage_protection_sensor_; 
    sensor::Sensor *cell_overvoltage_protection_recovery_sensor_; 
    sensor::Sensor *cell_request_charge_voltage_sensor_;
    sensor::Sensor *cell_request_float_voltage_sensor_; 

    sensor::Sensor *balancing_trigger_voltage_sensor_;  
    sensor::Sensor *cell_soc100_voltage_sensor_;
    sensor::Sensor *cell_soc0_voltage_sensor_;  
    sensor::Sensor *cell_power_off_voltage_sensor_;

    sensor::Sensor *balancing_direction_sensor_; 
    sensor::Sensor *cell_count_settings_sensor_;  
    sensor::Sensor *cell_smart_sleep_voltage_sensor_;
    sensor::Sensor *max_balancing_current_sensor_; 
    sensor::Sensor *max_charging_current_sensor_;
    sensor::Sensor *max_discharging_current_sensor_;
    sensor::Sensor *charging_overcurrent_protection_delay_sensor_;
    sensor::Sensor *charging_overcurrent_protection_recovery_delay_sensor_;  
    sensor::Sensor *discharging_overcurrent_protection_delay_sensor_;
    sensor::Sensor *discharging_overcurrent_protection_recovery_delay_sensor_;  
    sensor::Sensor *short_circuit_protection_delay_sensor_;
    sensor::Sensor *short_circuit_protection_recovery_delay_sensor_;
    sensor::Sensor *charging_overtemperature_protection_sensor_;
    sensor::Sensor *charging_overtemperature_protection_recovery_sensor_;
    sensor::Sensor *discharging_overtemperature_protection_sensor_;
    sensor::Sensor *discharging_overtemperature_protection_recovery_sensor_;
    sensor::Sensor *charging_lowtemperature_protection_sensor_;
    sensor::Sensor *charging_lowtemperature_protection_recovery_sensor_;
    sensor::Sensor *mos_overtemperature_protection_sensor_;
    sensor::Sensor *mos_overtemperature_protection_recovery_sensor_;  
    sensor::Sensor *scp_recovery_time_number_;    
    sensor::Sensor *total_battery_capacity_number_;  

    sensor::Sensor *discharging_overcurrent_protection_release_time_sensor_;
    sensor::Sensor *discharging_short_circuit_protection_release_time_sensor_;
    sensor::Sensor *charging_overcurrent_protection_release_time_sensor_;
    sensor::Sensor *charging_short_circuit_protection_release_time_sensor_;
    sensor::Sensor *cell_undervoltage_protection_release_time_sensor_;
    sensor::Sensor *cell_overvoltage_protection_release_time_sensor_;

    sensor::Sensor *cell_count_real_sensor_;
    sensor::Sensor *cell_voltage_min_sensor_;
    sensor::Sensor *cell_voltage_max_sensor_;
    sensor::Sensor *cell_resistance_min_sensor_;
    sensor::Sensor *cell_resistance_max_sensor_;  
    sensor::Sensor *cell_voltage_min_cell_number_sensor_;
    sensor::Sensor *cell_voltage_max_cell_number_sensor_;
    sensor::Sensor *cell_resistance_min_cell_number_sensor_;
    sensor::Sensor *cell_resistance_max_cell_number_sensor_;  
    sensor::Sensor *cell_delta_voltage_sensor_;
    sensor::Sensor *cell_average_voltage_sensor_;
    sensor::Sensor *temperature_powertube_sensor_;
    sensor::Sensor *temperature_sensor_1_sensor_;
    sensor::Sensor *temperature_sensor_2_sensor_;
    sensor::Sensor *battery_voltage_sensor_;
    sensor::Sensor *battery_current_sensor_;
    sensor::Sensor *battery_power_sensor_;
    sensor::Sensor *battery_power_charging_sensor_;
    sensor::Sensor *battery_power_discharging_sensor_;
    sensor::Sensor *battery_capacity_remaining_sensor_;
    sensor::Sensor *battery_capacity_remaining_derived_sensor_;
    sensor::Sensor *temperature_sensors_sensor_;
    sensor::Sensor *charging_cycles_sensor_;
    sensor::Sensor *battery_capacity_total_charging_cycle_sensor_;
    sensor::Sensor *battery_strings_sensor_;
    sensor::Sensor *errors_bitmask_sensor_;
    sensor::Sensor *operation_mode_bitmask_sensor_;
    sensor::Sensor *total_voltage_overvoltage_protection_sensor_;
    sensor::Sensor *total_voltage_undervoltage_protection_sensor_;
    sensor::Sensor *cell_voltage_overvoltage_delay_sensor_;
    sensor::Sensor *cell_voltage_undervoltage_delay_sensor_;
    sensor::Sensor *cell_pressure_difference_protection_sensor_;
    sensor::Sensor *discharging_overcurrent_protection_sensor_;
    sensor::Sensor *discharging_overcurrent_delay_sensor_;
    sensor::Sensor *charging_overcurrent_protection_sensor_;
    sensor::Sensor *charging_overcurrent_delay_sensor_;
    sensor::Sensor *cell_balancing_starting_voltage_sensor_;
    sensor::Sensor *balancing_opening_pressure_difference_sensor_;
    sensor::Sensor *powertube_temperature_protection_sensor_;
    sensor::Sensor *powertube_temperature_protection_recovery_sensor_;
    sensor::Sensor *temperature_sensor_temperature_protection_sensor_;
    sensor::Sensor *temperature_sensor_temperature_recovery_sensor_;
    sensor::Sensor *temperature_sensor_temperature_difference_protection_sensor_;
    sensor::Sensor *charging_high_temperature_protection_sensor_;
    sensor::Sensor *discharging_high_temperature_protection_sensor_;
    sensor::Sensor *charging_low_temperature_protection_sensor_;
    sensor::Sensor *charging_low_temperature_recovery_sensor_;
    sensor::Sensor *discharging_low_temperature_protection_sensor_;
    sensor::Sensor *discharging_low_temperature_recovery_sensor_;
    sensor::Sensor *battery_capacity_total_setting_sensor_;
    sensor::Sensor *battery_soh_valuation_sensor_;

    sensor::Sensor *charging_sensor_;
    sensor::Sensor *discharging_sensor_;
    sensor::Sensor *current_calibration_sensor_;
    sensor::Sensor *device_address_sensor_;
    sensor::Sensor *sleep_wait_time_sensor_;
    sensor::Sensor *alarm_low_volume_sensor_;
    sensor::Sensor *password_sensor_;
    sensor::Sensor *manufacturing_date_sensor_;
    sensor::Sensor *battery_total_runtime_sensor_;
    sensor::Sensor *start_current_calibration_sensor_;
    sensor::Sensor *actual_battery_capacity_sensor_;
    sensor::Sensor *protocol_version_sensor_;

    sensor::Sensor *battery_capacity_state_of_charge_sensor_;
    sensor::Sensor *heating_current_sensor_;
    sensor::Sensor *balancing_current_sensor_;
    sensor::Sensor *uart1_protocol_number_sensor_;
    sensor::Sensor *uart2_protocol_number_sensor_;  
    sensor::Sensor *cell_request_charge_voltage_time_sensor_;
    sensor::Sensor *cell_request_float_voltage_time_sensor_; 

   sensor::Sensor *precharging_time_from_discharge_sensor_;

};



}  // namespace jk_rs485_bms
}  // namespace esphome