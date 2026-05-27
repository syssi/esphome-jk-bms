#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/jk_bms_ble/jk_bms_ble.h"

namespace esphome::jk_bms_ble::testing {

struct TestNumber : number::Number {
  void control(float) override {}
};

struct TestSwitch : switch_::Switch {
  void write_state(bool) override {}
};

// Exposes protected decoder methods for direct testing.
class TestableJkBmsBle : public JkBmsBle {
 public:
  void update() override {}
  using JkBmsBle::decode_;
  using JkBmsBle::decode_device_info_;
  using JkBmsBle::decode_jk02_cell_info_;
  using JkBmsBle::decode_jk04_cell_info_;
  using JkBmsBle::decode_jk02_settings_;
  using JkBmsBle::decode_jk04_settings_;
  using JkBmsBle::decode_logbook_;
};

}  // namespace esphome::jk_bms_ble::testing

// Traffic from esp32-ble-example-faker.yaml
#include "frames_jk02_24s_v10.h"
