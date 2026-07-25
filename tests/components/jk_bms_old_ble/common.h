#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/jk_bms_old_ble/jk_bms_old_ble.h"

namespace esphome::jk_bms_old_ble::testing {

// Exposes protected decoder methods for direct testing.
class TestableJkBmsOldBle : public JkBmsOldBle {
 public:
  void update() override {}
  using JkBmsOldBle::decode_;
  using JkBmsOldBle::decode_basic_info_;
};

}  // namespace esphome::jk_bms_old_ble::testing

// Traffic from esp32-ble-old-example-faker.yaml
#include "frames.h"
