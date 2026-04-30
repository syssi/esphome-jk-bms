#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "esphome/components/jk_balancer_modbus/jk_balancer_modbus.h"

namespace esphome::jk_balancer_modbus::testing {

struct Capture {
  uint8_t function;
  std::vector<uint8_t> data;
};

class MockDevice : public JkBalancerModbusDevice {
 public:
  void on_jk_balancer_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override {
    captured = {function, data};
  }
  std::optional<Capture> captured;
};

class TestableJkBalancerModbus : public JkBalancerModbus {
 public:
  bool parse_byte(uint8_t byte) { return parse_jk_balancer_modbus_byte_(byte); }
};

// Build a valid 74-byte frame.
// Frame layout: [0xEB, 0x90, address, function, payload(69 bytes), checksum]
static inline std::vector<uint8_t> make_frame(uint8_t address, uint8_t function,
                                              const std::vector<uint8_t> &payload = {}) {
  std::vector<uint8_t> f(74, 0x00);
  f[0] = 0xEB;
  f[1] = 0x90;
  f[2] = address;
  f[3] = function;
  for (size_t i = 0; i < payload.size() && i < 69; i++)
    f[4 + i] = payload[i];
  uint8_t sum = 0;
  for (size_t i = 0; i < 73; i++)
    sum += f[i];
  f[73] = sum;
  return f;
}

}  // namespace esphome::jk_balancer_modbus::testing
