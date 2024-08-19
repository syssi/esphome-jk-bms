#include "jk_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_bms {

static const char *const TAG = "jk_bms";

static const uint8_t MAX_NO_RESPONSE_COUNT = 5;

static const uint8_t FUNCTION_READ_ALL = 0x06;
static const uint8_t FUNCTION_WRITE_REGISTER = 0x02;

static const uint8_t ERRORS_SIZE = 14;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Low capacity",                              // Byte 0.0, warning
    "Power tube overtemperature",                // Byte 0.1, alarm
    "Charging overvoltage",                      // Byte 0.2, alarm
    "Discharging undervoltage",                  // Byte 0.3, alarm
    "Battery over temperature",                  // Byte 0.4, alarm
    "Charging overcurrent",                      // Byte 0.5, alarm
    "Discharging overcurrent",                   // Byte 0.6, alarm
    "Cell pressure difference",                  // Byte 0.7, alarm
    "Overtemperature alarm in the battery box",  // Byte 1.0, alarm
    "Battery low temperature",                   // Byte 1.1, alarm
    "Cell overvoltage",                          // Byte 1.2, alarm
    "Cell undervoltage",                         // Byte 1.3, alarm
    "309_A protection",                          // Byte 1.4, alarm
    "309_A protection",                          // Byte 1.5, alarm
};

static const uint8_t OPERATION_MODES_SIZE = 4;
static const char *const OPERATION_MODES[OPERATION_MODES_SIZE] = {
    "Charging enabled",     // 0x00
    "Discharging enabled",  // 0x01
    "Balancer enabled",     // 0x02
    "Battery dropped",      // 0x03
};

static const uint8_t BATTERY_TYPES_SIZE = 3;
static const char *const BATTERY_TYPES[BATTERY_TYPES_SIZE] = {
    "Lithium Iron Phosphate",  // 0x00
    "Ternary Lithium",         // 0x01
    "Lithium Titanate",        // 0x02
};

void JkBms::on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  if (function == FUNCTION_READ_ALL) {
    this->on_status_data_(data);
    return;
  }

  if (function == FUNCTION_WRITE_REGISTER) {
    ESP_LOGI(TAG, "Register 0x%02X updated", data[0]);
    return;
  }

  ESP_LOGW(TAG, "Unhandled response (%zu bytes) received: %s", data.size(),
           format_hex_pretty(&data.front(), data.size()).c_str());
}

void JkBms::on_status_data_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 0)) << 16) | (uint32_t(jk_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");

  // Status request
  // -> 0x4E 0x57 0x00 0x13 0x00 0x00 0x00 0x00 0x06 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x68 0x00 0x00 0x01 0x29
  //
  // Status response
  // <- 0x4E 0x57 0x01 0x1B 0x00 0x00 0x00 0x00 0x06 0x00 0x01: Header
  //
  // *Data*
  //
  // Address Content: Description      Decoded content                         Coeff./Unit
  // 0x79: Individual Cell voltage
  // 0x2A: Cell count               42 / 3 bytes = 14 cells
  // 0x01 0x0E 0xED: Cell 1         3821 * 0.001 = 3.821V                        0.001 V
  // 0x02 0x0E 0xFA: Cell 2         3834 * 0.001 = 3.834V                        0.001 V
  // 0x03 0x0E 0xF7: Cell 3         3831 * 0.001 = 3.831V                        0.001 V
  // 0x04 0x0E 0xEC: Cell 4         ...                                          0.001 V
  // 0x05 0x0E 0xF8: Cell 5         ...                                          0.001 V
  // 0x06 0x0E 0xFA: Cell 6         ...                                          0.001 V
  // 0x07 0x0E 0xF1: Cell 7         ...                                          0.001 V
  // 0x08 0x0E 0xF8: Cell 8         ...                                          0.001 V
  // 0x09 0x0E 0xE3: Cell 9         ...                                          0.001 V
  // 0x0A 0x0E 0xFA: Cell 10        ...                                          0.001 V
  // 0x0B 0x0E 0xF1: Cell 11        ...                                          0.001 V
  // 0x0C 0x0E 0xFB: Cell 12        ...                                          0.001 V
  // 0x0D 0x0E 0xFB: Cell 13        ...                                          0.001 V
  // 0x0E 0x0E 0xF2: Cell 14        3826 * 0.001 = 3.826V                        0.001 V
  uint8_t cells = data[1] / 3;

  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_get_16bit(i * 3 + 3) * 0.001f;
    average_cell_voltage = average_cell_voltage + cell_voltage;
    if (cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
  }
  average_cell_voltage = average_cell_voltage / cells;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  uint16_t offset = data[1] + 3;

  // 0x80 0x00 0x1D: Read power tube temperature                 29°C                      1.0 °C
  // --->  99 = 99°C, 100 = 100°C, 101 = -1°C, 140 = -40°C
  this->publish_state_(this->power_tube_temperature_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 0)));

  // 0x81 0x00 0x1E: Read the temperature in the battery box     30°C                      1.0 °C
  this->publish_state_(this->temperature_sensor_1_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 1)));

  // 0x82 0x00 0x1C: Read battery temperature                    28°C                      1.0 °C
  this->publish_state_(this->temperature_sensor_2_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 2)));

  // 0x83 0x14 0xEF: Total battery voltage                       5359 * 0.01 = 53.59V      0.01 V
  float total_voltage = (float) jk_get_16bit(offset + 3 * 3) * 0.01f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 0x84 0x80 0xD0: Current data                                32976                     0.01 A
  // this->publish_state_(this->current_sensor_, get_current_(jk_get_16bit(offset + 3 * 4), 0x01) * 0.01f);
  float current = get_current_(jk_get_16bit(offset + 3 * 4), data[offset + 84 + 3 * 45]) * 0.01f;
  this->publish_state_(this->current_sensor_, current);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 0x85 0x0F: Battery remaining capacity                       15 %
  uint8_t raw_battery_remaining_capacity = data[offset + 3 * 5];
  this->publish_state_(this->capacity_remaining_sensor_, (float) raw_battery_remaining_capacity);

  // 0x86 0x02: Number of battery temperature sensors             2                        1.0  count
  this->publish_state_(this->temperature_sensors_sensor_, (float) data[offset + 2 + 3 * 5]);

  // 0x87 0x00 0x04: Number of battery cycles                     4                        1.0  count
  this->publish_state_(this->charging_cycles_sensor_, (float) jk_get_16bit(offset + 4 + 3 * 5));

  // 0x89 0x00 0x00 0x00 0x00: Total battery cycle capacity
  this->publish_state_(this->total_charging_cycle_capacity_sensor_, (float) jk_get_32bit(offset + 4 + 3 * 6));

  // 0x8A 0x00 0x0E: Total number of battery strings             14                        1.0  count
  this->publish_state_(this->battery_strings_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 7));

  // 0x8B 0x00 0x00: Battery warning message                     0000 0000 0000 0000
  //
  // Bit 0    Low capacity                                1 (alarm), 0 (normal)    warning
  // Bit 1    Power tube overtemperature                  1 (alarm), 0 (normal)    alarm
  // Bit 2    Charging overvoltage                        1 (alarm), 0 (normal)    alarm
  // Bit 3    Discharging undervoltage                    1 (alarm), 0 (normal)    alarm
  // Bit 4    Battery over temperature                    1 (alarm), 0 (normal)    alarm
  // Bit 5    Charging overcurrent                        1 (alarm), 0 (normal)    alarm
  // Bit 6    Discharging overcurrent                     1 (alarm), 0 (normal)    alarm
  // Bit 7    Cell pressure difference                    1 (alarm), 0 (normal)    alarm
  // Bit 8    Overtemperature alarm in the battery box    1 (alarm), 0 (normal)    alarm
  // Bit 9    Battery low temperature                     1 (alarm), 0 (normal)    alarm
  // Bit 10   Cell overvoltage                            1 (alarm), 0 (normal)    alarm
  // Bit 11   Cell undervoltage                           1 (alarm), 0 (normal)    alarm
  // Bit 12   309_A protection                            1 (alarm), 0 (normal)    alarm
  // Bit 13   309_A protection                            1 (alarm), 0 (normal)    alarm
  // Bit 14   Reserved
  // Bit 15   Reserved
  //
  // Examples:
  // 0x0001 = 00000000 00000001: Low capacity alarm
  // 0x0002 = 00000000 00000010: MOS tube over-temperature alarm
  // 0x0003 = 00000000 00000011: Low capacity alarm AND power tube over-temperature alarm
  uint16_t raw_errors_bitmask = jk_get_16bit(offset + 6 + 3 * 8);
  this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));

  // 0x8C 0x00 0x07: Battery status information                  0000 0000 0000 0111
  // Bit 0: Charging enabled        1 (on), 0 (off)
  // Bit 1: Discharging enabled     1 (on), 0 (off)
  // Bit 2: Balancer enabled        1 (on), 0 (off)
  // Bit 3: Battery dropped(?)      1 (normal), 0 (offline)
  // Bit 4...15: Reserved

  // Example: 0000 0000 0000 0111 -> Charging + Discharging + Balancer enabled
  uint16_t raw_modes_bitmask = jk_get_16bit(offset + 6 + 3 * 9);
  this->publish_state_(this->operation_mode_bitmask_sensor_, (float) raw_modes_bitmask);
  this->publish_state_(this->operation_mode_text_sensor_, this->mode_bits_to_string_(raw_modes_bitmask));
  this->publish_state_(this->charging_binary_sensor_, check_bit_(raw_modes_bitmask, 1));
  this->publish_state_(this->discharging_binary_sensor_, check_bit_(raw_modes_bitmask, 2));
  this->publish_state_(this->balancing_binary_sensor_, check_bit_(raw_modes_bitmask, 4));

  // 0x8E 0x16 0x26: Total voltage overvoltage protection        5670 * 0.01 = 56.70V     0.01 V
  this->publish_state_(this->total_voltage_overvoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 10) * 0.01f);

  // 0x8F 0x10 0xAE: Total voltage undervoltage protection       4270 * 0.01 = 42.70V     0.01 V
  this->publish_state_(this->total_voltage_undervoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 11) * 0.01f);

  // 0x90 0x0F 0xD2: Cell overvoltage protection voltage         4050 * 0.001 = 4.050V     0.001 V
  this->publish_state_(this->cell_voltage_overvoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 12) * 0.001f);

  // 0x91 0x0F 0xA0: Cell overvoltage recovery voltage           4000 * 0.001 = 4.000V     0.001 V
  this->publish_state_(this->cell_voltage_overvoltage_recovery_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 13) * 0.001f);

  // 0x92 0x00 0x05: Cell overvoltage protection delay            5s                         1.0 s
  this->publish_state_(this->cell_voltage_overvoltage_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 14));

  // 0x93 0x0B 0xEA: Cell undervoltage protection voltage        3050 * 0.001 = 3.050V     0.001 V
  this->publish_state_(this->cell_voltage_undervoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 15) * 0.001f);

  // 0x94 0x0C 0x1C: Cell undervoltage recovery voltage          3100 * 0.001 = 3.100V     0.001 V
  this->publish_state_(this->cell_voltage_undervoltage_recovery_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 16) * 0.001f);

  // 0x95 0x00 0x05: Cell undervoltage protection delay           5s                         1.0 s
  this->publish_state_(this->cell_voltage_undervoltage_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 17));

  // 0x96 0x01 0x2C: Cell pressure difference protection value    300 * 0.001 = 0.300V     0.001 V     0.000-1.000V
  this->publish_state_(this->cell_pressure_difference_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 18) * 0.001f);

  // 0x97 0x00 0x07: Discharge overcurrent protection value       7A                         1.0 A
  this->publish_state_(this->discharging_overcurrent_protection_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 19));

  // 0x98 0x00 0x03: Discharge overcurrent delay                  3s                         1.0 s
  this->publish_state_(this->discharging_overcurrent_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 20));

  // 0x99 0x00 0x05: Charging overcurrent protection value        5A                         1.0 A
  this->publish_state_(this->charging_overcurrent_protection_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 21));

  // 0x9A 0x00 0x05: Charge overcurrent delay                     5s                         1.0 s
  this->publish_state_(this->charging_overcurrent_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 22));

  // 0x9B 0x0C 0xE4: Balanced starting voltage                   3300 * 0.001 = 3.300V     0.001 V
  this->publish_state_(this->balance_starting_voltage_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 23) * 0.001f);

  // 0x9C 0x00 0x08: Balanced opening pressure difference           8 * 0.001 = 0.008V     0.001 V     0.01-1V
  this->publish_state_(this->balance_opening_pressure_difference_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 24) * 0.001f);

  // 0x9D 0x01: Active balance switch                              1 (on)                     Bool     0 (off), 1 (on)
  this->publish_state_(this->balancing_switch_binary_sensor_, (bool) data[offset + 6 + 3 * 25]);
  this->publish_state_(this->balancer_switch_, (bool) data[offset + 6 + 3 * 25]);

  // 0x9E 0x00 0x5A: Power tube temperature protection value                90°C            1.0 °C     0-100°C
  this->publish_state_(this->power_tube_temperature_protection_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 25));

  // 0x9F 0x00 0x46: Power tube temperature recovery value                  70°C            1.0 °C     0-100°C
  this->publish_state_(this->power_tube_temperature_recovery_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 26));

  // 0xA0 0x00 0x64: Temperature protection value in the battery box       100°C            1.0 °C     40-100°C
  this->publish_state_(this->temperature_sensor_temperature_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 27));

  // 0xA1 0x00 0x64: Temperature recovery value in the battery box         100°C            1.0 °C     40-100°C
  this->publish_state_(this->temperature_sensor_temperature_recovery_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 28));

  // 0xA2 0x00 0x14: Battery temperature difference protection value        20°C            1.0 °C     5-10°C
  this->publish_state_(this->temperature_sensor_temperature_difference_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 29));

  // 0xA3 0x00 0x46: Battery charging high temperature protection value     70°C            1.0 °C     0-100°C
  this->publish_state_(this->charging_high_temperature_protection_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 30));

  // 0xA4 0x00 0x46: Battery discharge high temperature protection value    70°C            1.0 °C     0-100°C
  this->publish_state_(this->discharging_high_temperature_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 31));

  // 0xA5 0xFF 0xEC: Charging low temperature protection value             -20°C            1.0 °C     -45...25°C
  this->publish_state_(this->charging_low_temperature_protection_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 32));

  // 0xA6 0xFF 0xF6: Charging low temperature protection recovery value    -10°C            1.0 °C     -45...25°C
  this->publish_state_(this->charging_low_temperature_recovery_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 33));

  // 0xA7 0xFF 0xEC: Discharge low temperature protection value            -20°C            1.0 °C     -45...25°C
  this->publish_state_(this->discharging_low_temperature_protection_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 34));

  // 0xA8 0xFF 0xF6: Discharge low temperature protection recovery value   -10°C            1.0 °C     -45...25°C
  this->publish_state_(this->discharging_low_temperature_recovery_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 35));

  // 0xA9 0x0E: Battery string setting                                      14              1.0 count
  // this->publish_state_(this->battery_string_setting_sensor_, (float) data[offset + 8 + 3 * 36]);
  // 0xAA 0x00 0x00 0x02 0x30: Total battery capacity setting              560 Ah           1.0 Ah
  uint32_t raw_total_battery_capacity_setting = jk_get_32bit(offset + 10 + 3 * 36);
  this->publish_state_(this->total_battery_capacity_setting_sensor_, (float) raw_total_battery_capacity_setting);
  this->publish_state_(this->capacity_remaining_derived_sensor_,
                       (float) (raw_total_battery_capacity_setting * (raw_battery_remaining_capacity * 0.01f)));

  // 0xAB 0x01: Charging MOS tube switch                                     1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->charging_switch_binary_sensor_, (bool) data[offset + 15 + 3 * 36]);
  this->publish_state_(this->charging_switch_, (bool) data[offset + 15 + 3 * 36]);

  // 0xAC 0x01: Discharge MOS tube switch                                    1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->discharging_switch_binary_sensor_, (bool) data[offset + 17 + 3 * 36]);
  this->publish_state_(this->discharging_switch_, (bool) data[offset + 17 + 3 * 36]);

  // 0xAD 0x04 0x11: Current calibration                       1041mA * 0.001 = 1.041A     0.001 A     0.1-2.0A
  this->publish_state_(this->current_calibration_sensor_, (float) jk_get_16bit(offset + 19 + 3 * 36) * 0.001f);

  // 0xAE 0x01: Protection board address                                     1              1.0
  this->publish_state_(this->device_address_sensor_, (float) data[offset + 19 + 3 * 37]);

  // 0xAF 0x01: Battery Type                                                 1              1.0
  // ---> 0 (lithium iron phosphate), 1 (ternary), 2 (lithium titanate)
  uint8_t raw_battery_type = data[offset + 21 + 3 * 37];
  if (raw_battery_type < BATTERY_TYPES_SIZE) {
    this->publish_state_(this->battery_type_text_sensor_, BATTERY_TYPES[raw_battery_type]);
  } else {
    this->publish_state_(this->battery_type_text_sensor_, "Unknown");
  }

  // 0xB0 0x00 0x0A: Sleep waiting time                                      10s            1.0 s
  this->publish_state_(this->sleep_wait_time_sensor_, (float) jk_get_16bit(offset + 23 + 3 * 37));

  // 0xB1 0x14: Low volume alarm                                             20%            1.0 %      0-80%
  this->publish_state_(this->alarm_low_volume_sensor_, (float) data[offset + 23 + 3 * 38]);

  // 0xB2 0x31 0x32 0x33 0x34 0x35 0x36 0x00 0x00 0x00 0x00: Modify parameter password
  this->publish_state_(this->password_text_sensor_,
                       std::string(data.begin() + offset + 25 + 3 * 38, data.begin() + offset + 35 + 3 * 38));

  // 0xB3 0x00: Dedicated charger switch                                     1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->dedicated_charger_switch_binary_sensor_, (bool) data[offset + 36 + 3 * 38]);

  // 0xB4 0x49 0x6E 0x70 0x75 0x74 0x20 0x55 0x73: Device ID code
  this->publish_state_(this->device_type_text_sensor_,
                       std::string(data.begin() + offset + 38 + 3 * 38, data.begin() + offset + 46 + 3 * 38));

  // 0xB5 0x32 0x31 0x30 0x31: Date of manufacture
  // 0xB6 0x00 0x00 0xE2 0x00: System working hours
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_32bit(offset + 46 + 3 * 40) * 0.0166666666667);
  this->publish_state_(this->total_runtime_formatted_text_sensor_,
                       format_total_runtime_(jk_get_32bit(offset + 46 + 3 * 40) * 60));

  // 0xB7 0x48 0x36 0x2E 0x58 0x5F 0x5F 0x53
  //      0x36 0x2E 0x31 0x2E 0x33 0x53 0x5F 0x5F: Software version number
  this->publish_state_(this->software_version_text_sensor_,
                       std::string(data.begin() + offset + 51 + 3 * 40, data.begin() + offset + 51 + 3 * 45));

  // 0xB8 0x00: Whether to start current calibration
  // 0xB9 0x00 0x00 0x00 0x00: Actual battery capacity
  //   Firmware version >= 10.10 required
  //   See https://github.com/syssi/esphome-jk-bms/issues/212 for details
  this->publish_state_(this->actual_battery_capacity_sensor_, (float) jk_get_32bit(offset + 54 + 3 * 45));

  // 0xBA 0x42 0x54 0x33 0x30 0x37 0x32 0x30 0x32 0x30 0x31 0x32 0x30
  //      0x30 0x30 0x30 0x32 0x30 0x30 0x35 0x32 0x31 0x30 0x30 0x31: Manufacturer ID naming
  this->publish_state_(this->manufacturer_text_sensor_,
                       std::string(data.begin() + offset + 59 + 3 * 45, data.begin() + offset + 83 + 3 * 45));

  // 0xC0 0x01: Protocol version number
  this->publish_state_(this->protocol_version_sensor_, (float) data[offset + 84 + 3 * 45]);

  // 00 00 00 00 68 00 00 54 D1: End of frame
}

void JkBms::update() {
  this->track_online_status_();
  this->read_registers();
}

void JkBms::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkBms::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void JkBms::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(min_cell_voltage_sensor_, NAN);
  this->publish_state_(max_cell_voltage_sensor_, NAN);
  this->publish_state_(min_voltage_cell_sensor_, NAN);
  this->publish_state_(max_voltage_cell_sensor_, NAN);
  this->publish_state_(delta_cell_voltage_sensor_, NAN);
  this->publish_state_(average_cell_voltage_sensor_, NAN);
  this->publish_state_(power_tube_temperature_sensor_, NAN);
  this->publish_state_(temperature_sensor_1_sensor_, NAN);
  this->publish_state_(temperature_sensor_2_sensor_, NAN);
  this->publish_state_(total_voltage_sensor_, NAN);
  this->publish_state_(current_sensor_, NAN);
  this->publish_state_(power_sensor_, NAN);
  this->publish_state_(charging_power_sensor_, NAN);
  this->publish_state_(discharging_power_sensor_, NAN);
  this->publish_state_(capacity_remaining_sensor_, NAN);
  this->publish_state_(capacity_remaining_derived_sensor_, NAN);
  this->publish_state_(temperature_sensors_sensor_, NAN);
  this->publish_state_(charging_cycles_sensor_, NAN);
  this->publish_state_(total_charging_cycle_capacity_sensor_, NAN);
  this->publish_state_(battery_strings_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(operation_mode_bitmask_sensor_, NAN);
  this->publish_state_(total_voltage_overvoltage_protection_sensor_, NAN);
  this->publish_state_(total_voltage_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_delay_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_delay_sensor_, NAN);
  this->publish_state_(cell_pressure_difference_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(charging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(charging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(balance_starting_voltage_sensor_, NAN);
  this->publish_state_(balance_opening_pressure_difference_sensor_, NAN);
  this->publish_state_(power_tube_temperature_protection_sensor_, NAN);
  this->publish_state_(power_tube_temperature_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_protection_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_difference_protection_sensor_, NAN);
  this->publish_state_(charging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(total_battery_capacity_setting_sensor_, NAN);
  this->publish_state_(charging_sensor_, NAN);
  this->publish_state_(discharging_sensor_, NAN);
  this->publish_state_(current_calibration_sensor_, NAN);
  this->publish_state_(device_address_sensor_, NAN);
  this->publish_state_(sleep_wait_time_sensor_, NAN);
  this->publish_state_(alarm_low_volume_sensor_, NAN);
  this->publish_state_(password_sensor_, NAN);
  this->publish_state_(manufacturing_date_sensor_, NAN);
  this->publish_state_(total_runtime_sensor_, NAN);
  this->publish_state_(start_current_calibration_sensor_, NAN);
  this->publish_state_(actual_battery_capacity_sensor_, NAN);
  this->publish_state_(protocol_version_sensor_, NAN);

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
  }
}

void JkBms::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkBms::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void JkBms::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

std::string JkBms::error_bits_to_string_(const uint16_t mask) {
  bool first = true;
  std::string errors_list = "";

  if (mask) {
    for (int i = 0; i < ERRORS_SIZE; i++) {
      if (mask & (1 << i)) {
        if (first) {
          first = false;
        } else {
          errors_list.append(";");
        }
        errors_list.append(ERRORS[i]);
      }
    }
  }

  return errors_list;
}

std::string JkBms::mode_bits_to_string_(const uint16_t mask) {
  bool first = true;
  std::string modes_list = "";

  if (mask) {
    for (int i = 0; i < OPERATION_MODES_SIZE; i++) {
      if (mask & (1 << i)) {
        if (first) {
          first = false;
        } else {
          modes_list.append(";");
        }
        modes_list.append(OPERATION_MODES[i]);
      }
    }
  }

  return modes_list;
}

void JkBms::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "JkBms:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Minimum Cell Voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Maximum Cell Voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Minimum Voltage Cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Maximum Voltage Cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta Cell Voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Average Cell Voltage", this->average_cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 17", this->cells_[16].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 18", this->cells_[17].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 19", this->cells_[18].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 20", this->cells_[19].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 21", this->cells_[20].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 22", this->cells_[21].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 23", this->cells_[22].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 24", this->cells_[23].cell_voltage_sensor_);
  LOG_SENSOR("", "Power Tube Temperature", this->power_tube_temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging Power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Capacity Remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Capacity Remaining Derived", this->capacity_remaining_derived_sensor_);
  LOG_SENSOR("", "Temperature Sensors", this->temperature_sensors_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Total Charging Cycle Capacity", this->total_charging_cycle_capacity_sensor_);
  LOG_SENSOR("", "Battery Strings", this->battery_strings_sensor_);
  LOG_SENSOR("", "Errors Bitmask", this->errors_bitmask_sensor_);
  LOG_SENSOR("", "Operation Mode Bitmask", this->operation_mode_bitmask_sensor_);
  LOG_SENSOR("", "Total Voltage Overvoltage Protection", this->total_voltage_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Total Voltage Undervoltage Protection", this->total_voltage_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Protection", this->cell_voltage_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Recovery", this->cell_voltage_overvoltage_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Delay", this->cell_voltage_overvoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Protection", this->cell_voltage_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Recovery", this->cell_voltage_undervoltage_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Delay", this->cell_voltage_undervoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Pressure Difference Protection", this->cell_pressure_difference_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Protection", this->discharging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Delay", this->discharging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Protection", this->charging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Delay", this->charging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Balance Starting Voltage", this->balance_starting_voltage_sensor_);
  LOG_SENSOR("", "Balance Opening Pressure Difference", this->balance_opening_pressure_difference_sensor_);
  LOG_SENSOR("", "Power Tube Temperature Protection", this->power_tube_temperature_protection_sensor_);
  LOG_SENSOR("", "Power Tube Temperature Recovery", this->power_tube_temperature_recovery_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Protection", this->temperature_sensor_temperature_protection_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Recovery", this->temperature_sensor_temperature_recovery_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Difference Protection",
             this->temperature_sensor_temperature_difference_protection_sensor_);
  LOG_SENSOR("", "Charging High Temperature Protection", this->charging_high_temperature_protection_sensor_);
  LOG_SENSOR("", "Discharging High Temperature Protection", this->discharging_high_temperature_protection_sensor_);
  LOG_SENSOR("", "Charging Low Temperature Protection", this->charging_low_temperature_protection_sensor_);
  LOG_SENSOR("", "Charging Low Temperature Recovery", this->charging_low_temperature_recovery_sensor_);
  LOG_SENSOR("", "Discharging Low Temperature Protection", this->discharging_low_temperature_protection_sensor_);
  LOG_SENSOR("", "Discharging Low Temperature Recovery", this->discharging_low_temperature_recovery_sensor_);
  LOG_SENSOR("", "Total Battery Capacity Setting", this->total_battery_capacity_setting_sensor_);
  LOG_SENSOR("", "Current Calibration", this->current_calibration_sensor_);
  LOG_SENSOR("", "Device Address", this->device_address_sensor_);
  LOG_TEXT_SENSOR("", "Battery Type", this->battery_type_text_sensor_);
  LOG_SENSOR("", "Sleep Wait Time", this->sleep_wait_time_sensor_);
  LOG_SENSOR("", "Alarm Low Volume", this->alarm_low_volume_sensor_);
  LOG_TEXT_SENSOR("", "Password", this->password_text_sensor_);
  LOG_TEXT_SENSOR("", "Device Type", this->device_type_text_sensor_);
  LOG_SENSOR("", "Manufacturing Date", this->manufacturing_date_sensor_);
  LOG_SENSOR("", "Total Runtime", this->total_runtime_sensor_);
  LOG_TEXT_SENSOR("", "Software Version", this->software_version_text_sensor_);
  LOG_SENSOR("", "Start Current Calibration", this->start_current_calibration_sensor_);
  LOG_TEXT_SENSOR("", "Manufacturer", this->manufacturer_text_sensor_);
  LOG_SENSOR("", "Protocol Version", this->protocol_version_sensor_);
  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Balancing Switch", this->balancing_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging Switch", this->charging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging Switch", this->discharging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Dedicated Charger Switch", this->dedicated_charger_switch_binary_sensor_);
  LOG_TEXT_SENSOR("", "Total Runtime Formatted", this->total_runtime_formatted_text_sensor_);
}

}  // namespace jk_bms
}  // namespace esphome
