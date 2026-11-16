#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/heltec_balancer_ble/heltec_balancer_ble.h"

namespace esphome::heltec_balancer_ble::testing {

struct TestNumber : number::Number {
  void control(float) override {}
};

struct TestSwitch : switch_::Switch {
  void write_state(bool) override {}
};

struct TestSelect : select::Select {
  explicit TestSelect(std::initializer_list<const char *> options) { this->traits.set_options(options); }
  void control(size_t) override {}
};

class TestableHeltecBalancerBle : public HeltecBalancerBle {
 public:
  TestableHeltecBalancerBle() { set_throttle(0); }
  void update() override {}
  using HeltecBalancerBle::build_command_frame_;
  using HeltecBalancerBle::decode_;
  using HeltecBalancerBle::decode_cell_info_;
  using HeltecBalancerBle::decode_cell_info_v2_;
  using HeltecBalancerBle::decode_device_info_;
  using HeltecBalancerBle::decode_settings_;
  using HeltecBalancerBle::decode_settings_v2_;
  using HeltecBalancerBle::publish_device_unavailable_;
};

}  // namespace esphome::heltec_balancer_ble::testing
