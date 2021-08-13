#include "jk_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_bms {

static const char *const TAG = "jk_bms";

static const uint8_t READ_HOLDING_REGISTERS = 0x03;
static const uint8_t READ_INPUT_REGISTERS = 0x04;
static const uint8_t WRITE_SINGLE_REGISTER = 0x06;

void JkBms::on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) {
  if (function == READ_INPUT_REGISTERS && data.size() == 58) {
    this->on_status_data_(data);
    return;
  }

  ESP_LOGW(TAG, "Invalid size (%zu) for JK BMS frame!", data.size());
}

void JkBms::on_status_data_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 0)) << 16) | (uint32_t(jk_get_16bit(i + 2)) << 0);
  };
}

void JkBms::update() { this->send(READ_INPUT_REGISTERS, 0x1000, 29); }

void JkBms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkBms::dump_config() {
  ESP_LOGCONFIG(TAG, "JkBms:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Firmware Version", this->firmware_version_sensor_);
}

}  // namespace jk_bms
}  // namespace esphome
