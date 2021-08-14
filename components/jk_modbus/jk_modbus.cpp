#include "jk_modbus.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jk_modbus {

static const char *const TAG = "jk_modbus";

void JkModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}
void JkModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_jk_modbus_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_jk_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_modbus_byte_(byte)) {
      this->last_jk_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

uint16_t chksum(const uint8_t data[], const uint8_t len) {
  uint8_t i;
  uint16_t checksum = 0;
  for (i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

bool JkModbus::parse_jk_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  // Byte 0: jk_modbus address (match all)
  if (at == 0)
    return true;
  uint8_t address = raw[0];

  // Byte 1: Function (msb indicates error)
  if (at == 1)
    return (byte & 0x80) != 0x80;
  uint8_t function = raw[1];

  // Byte 2: Size (with jk_modbus rtu function code 4/3)
  // See also https://en.wikipedia.org/wiki/Modbus
  if (at == 2)
    return true;

  uint8_t data_len = raw[2];
  // Byte 3..3+data_len-1: Data
  if (at < 3 + data_len)
    return true;

  // Byte 3+data_len: CRC_LO (over all bytes)
  if (at == 3 + data_len)
    return true;
  // Byte 3+len+1: CRC_HI (over all bytes)
  uint16_t computed_crc = chksum(raw, 3 + data_len);
  uint16_t remote_crc = uint16_t(raw[3 + data_len]) << 8 | (uint16_t(raw[3 + data_len + 1]) << 0);
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "JkModbus CRC Check failed! %02X!=%02X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin() + 3, this->rx_buffer_.begin() + 3 + data_len);

  bool found = false;
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      device->on_jk_modbus_data(function, data);
      found = true;
    }
  }
  if (!found) {
    ESP_LOGW(TAG, "Got JkModbus frame from unknown address 0x%02X!", address);
  }

  // return false to reset buffer
  return false;
}

void JkModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "JkModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
}
float JkModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}
void JkModbus::send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t register_count) {
  uint8_t frame[8];
  frame[0] = address;
  frame[1] = function;
  frame[2] = start_address >> 8;
  frame[3] = start_address >> 0;
  frame[4] = register_count >> 8;
  frame[5] = register_count >> 0;
  auto crc = chksum(frame, 6);
  frame[6] = crc >> 8;
  frame[7] = crc >> 0;

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(frame, 8);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

void JkModbus::read_registers(uint8_t function, uint8_t address) {
  uint8_t frame[21];
  frame[0] = 0x4E;      // start sequence
  frame[1] = 0x57;      // start sequence
  frame[2] = 0x00;      // data length lb
  frame[3] = 0x13;      // data length hb
  frame[4] = 0x00;      // bms terminal number
  frame[5] = 0x00;      // bms terminal number
  frame[6] = 0x00;      // bms terminal number
  frame[7] = 0x00;      // bms terminal number
  frame[8] = function;  // command word: 0x01 (activation), 0x02 (write), 0x03 (read), 0x05 (password), 0x06 (read all)
  frame[9] = 0x03;      // frame source: 0x00 (bms), 0x01 (bluetooth), 0x02 (gps), 0x03 (computer)
  frame[10] = 0x00;     // frame type: 0x00 (read data), 0x01 (reply frame), 0x02 (BMS active upload)
  frame[11] = address;  // register: 0x00 (read all registers), 0x8E...0xBF (holding registers)
  frame[12] = 0x00;     // record number
  frame[13] = 0x00;     // record number
  frame[14] = 0x00;     // record number
  frame[15] = 0x00;     // record number
  frame[16] = 0x68;     // end sequence
  auto crc = chksum(frame, 17);
  frame[17] = 0x00;     // crc unused
  frame[18] = 0x00;     // crc unused
  frame[19] = crc >> 8;
  frame[20] = crc >> 0;

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(frame, 21);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

}  // namespace jk_modbus
}  // namespace esphome
