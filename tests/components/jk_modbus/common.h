#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "esphome/components/jk_modbus/jk_modbus.h"

namespace esphome::jk_modbus::testing {

struct Capture {
  uint8_t function;
  std::vector<uint8_t> data;
};

class MockDevice : public JkModbusDevice {
 public:
  void on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override {
    captured = {function, data};
  }
  std::optional<Capture> captured;
};

class TestableJkModbus : public JkModbus {
 public:
  bool parse_byte(uint8_t byte) { return parse_jk_modbus_byte_(byte); }
};

// Build a valid jk_modbus response frame.
//
// Frame structure (data_len = 19, total = 21 bytes):
//   [0..1]   header: 0x4E 0x57
//   [2..3]   data_len = 19 (big-endian: 0x00 0x13)
//   [4..7]   terminal number (zero)
//   [8]      function
//   [9..10]  source + type
//   [11..15] dispatched payload (data_len-3=16, exclusive end → 5 bytes)
//   [16..18] padding (zero, within CRC coverage)
//   [19..20] CRC (sum of bytes 0..18, big-endian)
//
// Device dispatch address = raw[0] = 0x4E.
static inline std::vector<uint8_t> make_frame(uint8_t function, const std::vector<uint8_t> &payload = {}) {
  static const uint8_t DATA_LEN = 19;
  std::vector<uint8_t> f(DATA_LEN + 2, 0x00);  // 21 bytes
  f[0] = 0x4E;
  f[1] = 0x57;
  f[2] = 0x00;
  f[3] = DATA_LEN;
  f[8] = function;
  // payload goes into indices 11..DATA_LEN-4 = 11..15 (5 slots)
  for (size_t i = 0; i < payload.size() && (11 + i) < static_cast<size_t>(DATA_LEN - 3); i++)
    f[11 + i] = payload[i];
  // CRC over bytes 0..DATA_LEN-1
  uint16_t crc = 0;
  for (size_t i = 0; i < DATA_LEN; i++)
    crc += f[i];
  f[DATA_LEN] = crc >> 8;
  f[DATA_LEN + 1] = crc & 0xFF;
  return f;
}

}  // namespace esphome::jk_modbus::testing
