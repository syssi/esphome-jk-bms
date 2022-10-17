#pragma once

#include "../heltec_balancer_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace heltec_balancer_ble {

class HeltecBalancerBle;

class HeltecNumber : public number::Number, public Component {
 public:
  void set_parent(HeltecBalancerBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;

 protected:
  void control(float value) override;

  HeltecBalancerBle *parent_;
  uint8_t holding_register_;

  uint32_t ieee_float_(float f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    uint32_t ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }
};

}  // namespace heltec_balancer_ble
}  // namespace esphome
