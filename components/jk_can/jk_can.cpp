#include "jk_can.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jk_can {

static const char *const TAG = "jk_can";

void JkCAN::loop() {
  const uint32_t now = millis();
  if (now - this->last_jk_can_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_jk_can_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_can_byte_(byte)) {
      this->last_jk_can_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

uint16_t chksum(const uint8_t data[], const uint16_t len) {
  uint16_t checksum = 0;
  for (uint16_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

bool JkCAN::parse_jk_can_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  // Byte 0: Start sequence (0x4E)
  if (at == 0)
    return true;
  uint8_t address = raw[0];

  // Byte 1: Start sequence (0x57)
  if (at == 1)
    return true;

  if (raw[0] != 0x4E || raw[1] != 0x57) {
    ESP_LOGW(TAG, "Invalid header");

    // return false to reset buffer
    return false;
  }

  // Byte 2: Size (low byte)
  if (at == 2)
    return true;

  // Byte 3: Size (high byte)
  if (at == 3)
    return true;
  uint16_t data_len = (uint16_t(raw[2]) << 8 | (uint16_t(raw[2 + 1]) << 0));

  // data_len: CRC_LO (over all bytes)
  if (at <= data_len)
    return true;

  uint8_t function = raw[8];

  // data_len+1: CRC_HI (over all bytes)
  uint16_t computed_crc = chksum(raw, data_len);
  uint16_t remote_crc = uint16_t(raw[data_len]) << 8 | (uint16_t(raw[data_len + 1]) << 0);
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "JkModbus CRC Check failed! %02X!=%02X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin() + 11, this->rx_buffer_.begin() + data_len - 3);

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

void JkCAN::dump_config() { ESP_LOGCONFIG(TAG, "JkCAN:"); }
float JkCAN::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

// The manufacturer states that no write operations are possible via the serial interface.
void JkModbus::send(uint8_t function, uint8_t address, uint8_t value) {
  uint8_t frame[22];
  frame[0] = 0x4E;      // start sequence
  frame[1] = 0x57;      // start sequence
  frame[2] = 0x00;      // data length lb
  frame[3] = 0x14;      // data length hb
  frame[4] = 0x00;      // bms terminal number
  frame[5] = 0x00;      // bms terminal number
  frame[6] = 0x00;      // bms terminal number
  frame[7] = 0x00;      // bms terminal number
  frame[8] = function;  // command word: 0x01 (activation), 0x02 (write), 0x03 (read), 0x05 (password), 0x06 (read all)
  frame[9] = 0x02;      // frame source: 0x00 (bms), 0x01 (bluetooth), 0x02 (gps), 0x03 (computer)
  frame[10] = 0x02;     // frame type: 0x00 (read data), 0x01 (reply frame), 0x02 (BMS active upload)
  frame[11] = address;  // register: 0x00 (read all registers), 0x8E...0xBF (holding registers)
  frame[12] = value;    // data
  frame[13] = 0x00;     // record number
  frame[14] = 0x00;     // record number
  frame[15] = 0x00;     // record number
  frame[16] = 0x00;     // record number
  frame[17] = 0x68;     // end sequence
  auto crc = chksum(frame, 17);
  frame[18] = 0x00;  // crc unused
  frame[19] = 0x00;  // crc unused
  frame[20] = crc >> 8;
  frame[21] = crc >> 0;

  this->write_array(frame, 22);
  this->flush();
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
  frame[17] = 0x00;  // crc unused
  frame[18] = 0x00;  // crc unused
  frame[19] = crc >> 8;
  frame[20] = crc >> 0;

  this->write_array(frame, 21);
  this->flush();
}

}  // namespace jk_can
}  // namespace esphome
