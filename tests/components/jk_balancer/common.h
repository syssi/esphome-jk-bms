#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/jk_balancer/jk_balancer.h"

namespace esphome::jk_balancer::testing {

class TestableJkBalancer : public JkBalancer {
 public:
  void update() override {}  // avoids calling send() with null parent_
};

// Traffic from tests/esp8266-fake-active-balancer.yaml
// 74-byte status frame for a 17-cell JK Active Balancer.
// Decoded values:
//   total_voltage: 5649 × 0.01 = 56.49 V
//   average_cell_voltage: 3323 × 0.001 = 3.323 V
//   cell_count_detected: 17       balance_status: 0 (idle → balancing=false)
//   errors_bitmask: 1 (bit 0 = "Wrong cell count")
//   balance_current: 0 × 0.001 = 0.000 A
//   balance_trigger_voltage: 11 mV    max_balance_current: 2000 mA
//   balance_switch: on                configured_cell_count: 17
//   cell 1: 3321 mV = 3.321 V (min)   cells 2-5, 7-17: 3323 mV = 3.323 V
//   cell 6: 3324 mV = 3.324 V (max)
//   min_voltage_cell: 1  max_voltage_cell: 6  delta: 0.003 V
//   battery_temperature: 163 × 0.1 = 16.3 °C
//   online_status: true (from reset_online_status_tracker_)
static const std::vector<uint8_t> STATUS_FRAME_17S = {
    0xEB, 0x90, 0x01, 0xFF, 0x16, 0x11, 0x0C, 0xFB, 0x11, 0x01, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0B,
    0x07, 0xD0, 0x01, 0x11, 0x0C, 0xF9, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFC, 0x0C, 0xFB, 0x0C,
    0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB, 0x0C, 0xFB,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA3, 0xCB,
};

}  // namespace esphome::jk_balancer::testing
