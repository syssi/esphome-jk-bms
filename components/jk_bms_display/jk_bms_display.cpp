#include "jk_bms_display.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_bms_display {

static const char *const TAG = "jk_bms_display";

static const uint8_t SOF_BYTE1 = 0xA5;
static const uint8_t SOF_BYTE2 = 0x5A;

void JkBmsDisplay::loop() {
  const uint32_t now = millis();

  if (now - this->last_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_bms_display_byte_(byte)) {
      this->last_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

bool JkBmsDisplay::parse_jk_bms_display_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  if (at == 0)
    return true;

  if (at == 1) {
    if (raw[0] != SOF_BYTE1 || raw[1] != SOF_BYTE2) {
      ESP_LOGVV(TAG, "Invalid header: 0x%02X 0x%02X", raw[0], raw[1]);

      // return false to reset buffer
      return false;
    }

    return true;
  }

  if (at == 2)
    return true;

  uint8_t data_len = raw[2];
  uint8_t frame_len = data_len + 3;

  if (at < frame_len - 1)
    return true;

  std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.begin() + frame_len);

  this->on_jk_bms_display_data_(data);

  // return false to reset buffer
  return false;
}

void JkBmsDisplay::on_jk_bms_display_data_(const std::vector<uint8_t> &data) {
  if (data.size() < 6) {
    ESP_LOGW(TAG, "Invalid frame size!");
    return;
  }

  uint8_t data_len = data[2];
  uint8_t frame_type = data[3];

  if (frame_type == 0x82 && data_len == 93 && data[4] == 0x10 && data[5] == 0x00) {
    this->on_jk_bms_display_status_data_(data);
    return;
  }

  // 0xA5 0x5A 0x05 0x82 0x20 0x53 0x07 0xE0
  if (frame_type == 0x82 && data_len > 4) {
    this->on_jk_bms_display_raw_data_(data);
    return;
  }

  // 0xA5 0x5A 0x03 0x80 0x01 0x00
  if (frame_type == 0x80 && data_len == 3) {
    ESP_LOGD(TAG, "Frame 0x80: 0x%02X 0x%02X", data[4], data[5]);
    return;
  }

  ESP_LOGW(TAG, "Unhandled response received (frame_type: 0x%02X): %s", frame_type,
           format_hex_pretty(&data.front(), data.size()).c_str());
}

void JkBmsDisplay::on_jk_bms_display_status_data_(const std::vector<uint8_t> &data) {
  auto jk_bms_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Status frame (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  uint8_t offset = 6;

  // Offset  Content                                 Type     Unit    Remarks
  //  0      Battery voltage                        UINT16    10mV
  float total_voltage = jk_bms_get_16bit(0 + offset) * 0.01f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  //  2      Battery current                         INT16    0.1A
  float current = ((int16_t) jk_bms_get_16bit(2 + offset)) * 0.1f;
  this->publish_state_(this->current_sensor_, current);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  //  4      Reserved                               UINT16    -
  //  6      State of charge                        UINT16    %
  this->publish_state_(this->state_of_charge_sensor_, jk_bms_get_16bit(6 + offset));

  //  8      Maximum pressure difference            UINT16    mV
  // this->publish_state_(this->delta_cell_voltage_sensor_, jk_bms_get_16bit(8 + offset) * 0.001f);

  // 10      MOSFET temperature                      INT16    ℃
  this->publish_state_(this->mosfet_temperature_sensor_, ((int16_t) jk_bms_get_16bit(10 + offset)) * 1.0f);

  // 12      Battery temperature                     INT16    ℃
  this->publish_state_(this->battery_temperature_sensor_, ((int16_t) jk_bms_get_16bit(12 + offset)) * 1.0f);

  // 14      System warning                         UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->system_warning_binary_sensor_, jk_bms_get_16bit(14 + offset) == 0x0001);

  // 16      Cell average voltage                   UINT16    mV
  // this->publish_state_(this->average_cell_voltage_sensor_, jk_bms_get_16bit(16 + offset) * 0.001f);

  // 18      Balanced switch state                  UINT16    -       0: close, 1: open
  this->publish_state_(this->balancing_switch_binary_sensor_, jk_bms_get_16bit(18 + offset) == 0x0001);

  // 20      Charging MOS status                    UINT16    -       0: close, 1: open
  this->publish_state_(this->charging_binary_sensor_, jk_bms_get_16bit(20 + offset) == 0x0001);

  // 22      Discharging MOS status                 UINT16    -       0: close, 1: open
  this->publish_state_(this->discharging_binary_sensor_, jk_bms_get_16bit(22 + offset) == 0x0001);

  // 24      Cell voltages[24]                   24*UINT16    mV
  uint8_t cells = 24;
  uint8_t cells_enabled = 0;
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_bms_get_16bit(i * 2 + 24 + offset) * 0.001f;
    if (cell_voltage > 0) {
      average_cell_voltage = average_cell_voltage + cell_voltage;
      cells_enabled++;
    }
    if (cell_voltage > 0 && cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    if (cell_voltage > 0) {
      this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
    }
  }
  average_cell_voltage = average_cell_voltage / cells_enabled;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  // 72      Cell voltage undervoltage alarm        UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->cell_voltage_undervoltage_protection_binary_sensor_,
                       jk_bms_get_16bit(72 + offset) == 0x0001);

  // 74      Cell voltage overvoltage alarm         UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->cell_voltage_overvoltage_protection_binary_sensor_,
                       jk_bms_get_16bit(74 + offset) == 0x0001);

  // 76      Overcurrent alarm                      UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->overcurrent_protection_binary_sensor_, jk_bms_get_16bit(76 + offset) == 0x0001);

  // 78      MOS over temperature alarm             UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->mosfet_overtemperature_protection_binary_sensor_, jk_bms_get_16bit(78 + offset) == 0x0001);

  // 80      Battery over temperature alarm         UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->battery_temperature_protection_binary_sensor_, jk_bms_get_16bit(80 + offset) == 0x0001);

  // 82      Short circuit alarm                    UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->short_circuit_protection_binary_sensor_, jk_bms_get_16bit(82 + offset) == 0x0001);

  // 84      Co-handling communication exceptions   UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->coprocessor_communication_error_binary_sensor_, jk_bms_get_16bit(84 + offset) == 0x0001);

  // 86      Balance wire resistance is too large   UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->balancer_wire_resistance_too_high_binary_sensor_, jk_bms_get_16bit(86 + offset) == 0x0001);

  // 88      The number of strings does not match   UINT16    -       0: no alarm, 1: alarm
  this->publish_state_(this->cell_count_mismatch_binary_sensor_, jk_bms_get_16bit(88 + offset) == 0x0001);
}

void JkBmsDisplay::on_jk_bms_display_raw_data_(const std::vector<uint8_t> &data) {
  auto jk_bms_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGD(TAG, "Raw data (%d bytes) received", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  uint16_t address = jk_bms_get_16bit(4);
  uint16_t value = 0;
  uint8_t data_len = data[2] - 3;

  for (uint8_t i = 0; i < data_len; i = i + 2) {
    value = jk_bms_get_16bit(6 + i);
    ESP_LOGD(TAG, "Register 0x%04X (%d): %d (%.3f)", address + i, address + i, value, value * 0.001f);
  }
}

void JkBmsDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "JkBmsDisplay:");
  LOG_BINARY_SENSOR("", "System warning", this->system_warning_binary_sensor_);
  LOG_BINARY_SENSOR("", "Balancing switch", this->balancing_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Cell voltage undervoltage protection",
                    this->cell_voltage_undervoltage_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Cell voltage overvoltage protection",
                    this->cell_voltage_overvoltage_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Overcurrent protection", this->overcurrent_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Mosfet overtemperature protection", this->mosfet_overtemperature_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Battery temperature protection", this->battery_temperature_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Short circuit protection", this->short_circuit_protection_binary_sensor_);
  LOG_BINARY_SENSOR("", "Coprocessor communication error", this->coprocessor_communication_error_binary_sensor_);
  LOG_BINARY_SENSOR("", "Balancer wire resistance too high", this->balancer_wire_resistance_too_high_binary_sensor_);
  LOG_BINARY_SENSOR("", "Cell count mismatch", this->cell_count_mismatch_binary_sensor_);

  LOG_SENSOR("", "Total voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Delta cell voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Mosfet temperature", this->mosfet_temperature_sensor_);
  LOG_SENSOR("", "Battery temperature", this->battery_temperature_sensor_);
  LOG_SENSOR("", "Average cell voltage", this->average_cell_voltage_sensor_);
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

  this->check_uart_settings(2400);
}

float JkBmsDisplay::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void JkBmsDisplay::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBmsDisplay::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

}  // namespace jk_bms_display
}  // namespace esphome
