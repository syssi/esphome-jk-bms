#include "jk_balancer.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_balancer {

static const char *const TAG = "jk_balancer";

static const uint8_t MAX_NO_RESPONSE_COUNT = 5;

static const uint8_t FUNCTION_READ_ALL = 0xFF;
static const uint8_t FUNCTION_SET_CELL_COUNT = 0xF0;
static const uint8_t FUNCTION_SET_TRIGGER_VOLTAGE = 0xF2;
static const uint8_t FUNCTION_SET_MAX_BALANCE_CURRENT = 0xF4;

static const uint8_t ERRORS_SIZE = 3;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Wrong cell count",
    "Resistance too high",
    "Overvoltage",
};

void JkBalancer::on_jk_balancer_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  switch (function) {
    case FUNCTION_READ_ALL:
      this->on_status_data_(data);
      break;
    case FUNCTION_SET_CELL_COUNT:
    case FUNCTION_SET_TRIGGER_VOLTAGE:
    case FUNCTION_SET_MAX_BALANCE_CURRENT:
      ESP_LOGD(TAG, "Write register response received: %s ...", format_hex_pretty(data.data(), 6).c_str());  // NOLINT
      break;
    default:
      ESP_LOGW(TAG, "Unhandled response (%zu bytes) received: %s", data.size(),
               format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT
  }
}

void JkBalancer::on_status_data_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };
  // auto jk_get_32bit = [&](size_t i) -> uint32_t {
  //   return (uint32_t(jk_get_16bit(i + 0)) << 16) | (uint32_t(jk_get_16bit(i + 2)) << 0);
  // };

  ESP_LOGI(TAG, "Status frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());  // NOLINT

  // Status request (7 bytes)
  // -> 0x55 0xAA 0x01 0xFF 0x00 0x00 0xFF
  //
  // Status response (74 bytes)
  // <- 0xEB 0x90 0x01 0xFF 0x19 0xF7 0x0C 0xFC 0x14 0x00 0x13 0x00 0x01 0x00 0x03 0x00 0x00 0x00 0x0B 0x07
  //    0xD0 0x01 0x14 0x0C 0xFC 0x0C 0xFC 0x0C 0xFC 0x0C 0xFB 0x0C 0xFB 0x0C 0xFC 0x0C 0xFB 0x0C 0xFB 0x0C
  //    0xFC 0x0C 0xFC 0x0C 0xFC 0x0C 0xFC 0x0C 0xFC 0x0C 0xFC 0x0C 0xFB 0x0C 0xFC 0x0C 0xFB 0x0C 0xFC 0x0C
  //    0xFC 0x0C 0xF9 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xA3 0xEF
  //
  // Byte Len  Payload                Content                         Coeff.      Unit        Example value
  // 0     2   0xEB 0x90              Header
  // 2     1   0x01                   Device address                                          1
  // 3     1   0xFF                   Function                                                0xFF: Get status
  // 4     2   0x19 0xF7              Total battery voltage           0.01f         V         6647 * 0.01 = 66.47V
  this->publish_state_(this->total_voltage_sensor_, jk_get_16bit(4) * 0.01f);

  // 6     2   0x0C 0xFC              Average cell voltage            0.001f        V         3324 * 0.001 = 3.324V
  this->publish_state_(this->average_cell_voltage_sensor_, jk_get_16bit(6) * 0.001f);

  // 8     1   0x14                   Detected Cell count                                     20
  this->publish_state_(this->cell_count_detected_sensor_, (float) data[8]);

  // 9     1   0x00                   Highest cell                                            0
  // this->publish_state_(this->max_voltage_cell_sensor_, (float) data[9]);

  // 10    1   0x13                   Lowest cell                                             19
  // this->publish_state_(this->min_voltage_cell_sensor_, (float) data[10]);

  // 11    1   0x00                   Balance Status                                          0x00 = idle,
  //                                                                                          0x01 = charging,
  //                                                                                          0x02 = discharging
  this->publish_state_(this->balancer_status_sensor_, (data[11]));
  this->publish_state_(this->balancing_binary_sensor_, data[11] != 0x00);

  // 12    1   0x01                   Alarm Status
  uint8_t raw_errors_bitmask = data[12];
  this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));

  // 13    2   0x00 0x03              Maximum Voltage Difference      0.001f        V         3 * 0.001 = 0.003V
  // this->publish_state_(this->delta_cell_voltage_sensor_, jk_get_16bit(13) * 0.001f);

  // 15    2   0x00 0x00              Balance Current                 0.001f        A
  this->publish_state_(this->balancing_current_sensor_, jk_get_16bit(15) * 0.001f);

  // 17    2   0x00 0x0B              Balance Trigger Voltage         1.0f          mV        11 * 0.001 = 0.011V
  this->publish_state_(this->balance_trigger_voltage_number_, jk_get_16bit(17) * 1.0f);

  // 19    2   0x07 0xD0              Maximum Balance Current         1.0f          mA        2000 * 0.001 = 2A
  this->publish_state_(this->max_balance_current_number_, jk_get_16bit(19) * 1.0f);

  // 21    1   0x01                   Balance Switch                                          0x00 = off, 0x01 = on
  this->publish_state_(this->balancer_switch_, (bool) data[21]);

  // 22    1   0x14                   Configured Cell Count                                   20
  uint8_t cells = std::min((int) data[22], 24);
  this->publish_state_(this->cell_count_number_, (float) data[22]);

  // 23    2   0x0C 0xFC              Cell voltage 1                  0.001f        V         3324 * 0.001 = 3.324V
  // 25    2   0x0C 0xFC              Cell voltage 2                  0.001f        V         3324 * 0.001 = 3.324V
  // 27    2   0x0C 0xFC              Cell voltage 3                  0.001f        V         3324 * 0.001 = 3.324V
  // 29    2   0x0C 0xFB              ...
  // 31    2   0x0C 0xFB
  // 33    2   0x0C 0xFC
  // 35    2   0x0C 0xFB
  // 37    2   0x0C 0xFB
  // 39    2   0x0C 0xFC
  // 41    2   0x0C 0xFC
  // 43    2   0x0C 0xFC
  // 45    2   0x0C 0xFC
  // 47    2   0x0C 0xFC
  // 49    2   0x0C 0xFC
  // 51    2   0x0C 0xFB
  // 53    2   0x0C 0xFC
  // 55    2   0x0C 0xFB
  // 57    2   0x0C 0xFC
  // 59    2   0x0C 0xFC
  // 61    2   0x0C 0xF9
  // 63    2   0x00 0x00
  // 65    2   0x00 0x00
  // 67    2   0x00 0x00              ...
  // 69    2   0x00 0x00              Cell voltage 24
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = jk_get_16bit(i * 2 + 23) * 0.001f;
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

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);

  // 71    2   0x00 0xA3              Battery Temperature             0.01f        °C         163 * 0.01 = 16.3°C
  this->publish_state_(this->battery_temperature_sensor_, ((int16_t) jk_get_16bit(71)) * 0.1f);

  // 25    1   0xEF                   Checksum
}

void JkBalancer::update() {
  this->track_online_status_();
  this->send(FUNCTION_READ_ALL, 0x0000);
}

void JkBalancer::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkBalancer::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void JkBalancer::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(min_cell_voltage_sensor_, NAN);
  this->publish_state_(max_cell_voltage_sensor_, NAN);
  this->publish_state_(min_voltage_cell_sensor_, NAN);
  this->publish_state_(max_voltage_cell_sensor_, NAN);
  this->publish_state_(delta_cell_voltage_sensor_, NAN);
  this->publish_state_(average_cell_voltage_sensor_, NAN);
  this->publish_state_(battery_temperature_sensor_, NAN);
  this->publish_state_(total_voltage_sensor_, NAN);
  this->publish_state_(cell_count_detected_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(balancer_status_sensor_, NAN);
  this->publish_state_(balancing_current_sensor_, NAN);

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
  }
}

void JkBalancer::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBalancer::publish_state_(number::Number *number, float value) {
  if (number == nullptr)
    return;

  number->publish_state(value);
}

void JkBalancer::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkBalancer::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void JkBalancer::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

std::string JkBalancer::error_bits_to_string_(const uint8_t mask) {
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

void JkBalancer::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "JkBalancer:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Online Status", this->online_status_binary_sensor_);
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
  LOG_SENSOR("", "Battery Temperature", this->battery_temperature_sensor_);
  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Cell Count Detected", this->cell_count_detected_sensor_);
  LOG_SENSOR("", "Errors Bitmask", this->errors_bitmask_sensor_);
  LOG_SENSOR("", "Balancer Status", this->balancer_status_sensor_);
  LOG_SENSOR("", "Balancing Current", this->balancing_current_sensor_);
  LOG_TEXT_SENSOR("", "Errors", this->errors_text_sensor_);
}

}  // namespace jk_balancer
}  // namespace esphome
