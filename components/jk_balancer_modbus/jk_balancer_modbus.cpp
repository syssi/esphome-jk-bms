#include "jk_balancer_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_balancer_modbus {

static const char *const TAG = "jk_balancer_modbus";

static const uint8_t ADDRESS_READ_ALL = 0xFF;

void JkBalancerModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_jk_balancer_modbus_byte_ > this->rx_timeout_) {
    ESP_LOGVV(TAG, "Buffer cleared due to timeout: %s",
              format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    this->rx_buffer_.clear();
    this->last_jk_balancer_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_balancer_modbus_byte_(byte)) {
      this->last_jk_balancer_modbus_byte_ = now;
    } else {
      ESP_LOGVV(TAG, "Buffer cleared due to reset: %s",
                format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
      this->rx_buffer_.clear();
    }
  }
}

uint8_t chksum(const uint8_t data[], const uint8_t len) {
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

bool JkBalancerModbus::parse_jk_balancer_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  uint8_t frame_len = 74;

  // Byte 0: Start sequence (0xEB)
  if (at == 0) {
    // return false to reset buffer
    return raw[0] == 0xEB;
  }

  // Byte 1: Start sequence (0xEB 0x90)
  if (at == 1) {
    if (raw[0] != 0xEB || raw[1] != 0x90) {
      ESP_LOGW(TAG, "Invalid header: 0x%02X 0x%02X", raw[0], raw[1]);

      // return false to reset buffer
      return false;
    }

    return true;
  }

  // Byte 2: Device address
  if (at == 2)
    return true;

  uint8_t address = raw[2];

  // Byte 3: Function
  if (at == 3)
    return true;

  uint8_t function = raw[3];

  if (at < frame_len - 1)
    return true;

  // Checksum over all bytes
  uint8_t computed_crc = chksum(raw, frame_len - 1);
  uint8_t remote_crc = raw[frame_len - 1];
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "CRC check failed! 0x%02X != 0x%02X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.begin() + frame_len);

  bool found = false;
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      device->on_jk_balancer_modbus_data(function, data);
      found = true;
    }
  }
  if (!found) {
    ESP_LOGW(TAG, "Got JkBalancerModbus frame from unknown address 0x%02X!", address);
  }

  // return false to reset buffer
  return false;
}

void JkBalancerModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "JkBalancerModbus:");
  ESP_LOGCONFIG(TAG, "  RX timeout: %d ms", this->rx_timeout_);
}
float JkBalancerModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void JkBalancerModbus::send(uint8_t address, uint8_t function, uint16_t value) {
  uint8_t frame[7];
  frame[0] = 0x55;
  frame[1] = 0xAA;
  frame[2] = address;
  frame[3] = function;
  frame[4] = value >> 8;
  frame[5] = value >> 0;
  frame[6] = chksum(frame, 6);

  this->write_array(frame, 7);
  this->flush();
}

}  // namespace jk_balancer_modbus
}  // namespace esphome
