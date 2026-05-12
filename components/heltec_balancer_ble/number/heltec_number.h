#pragma once

#include "../heltec_balancer_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome::heltec_balancer_ble {

class HeltecBalancerBle;

class HeltecNumber : public number::Number, public Component {
 public:
  void set_parent(HeltecBalancerBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void set_holding_command(uint8_t holding_command) { this->holding_command_ = holding_command; }
  void set_integer_payload(bool integer_payload) { this->integer_payload_ = integer_payload; }
  void dump_config() override;

 protected:
  void control(float value) override;

  HeltecBalancerBle *parent_;
  uint8_t holding_register_;
  uint8_t holding_command_{0x05};
  bool integer_payload_{false};

  uint32_t ieee_float_(float f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    uint32_t ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }
};

}  // namespace esphome::heltec_balancer_ble
