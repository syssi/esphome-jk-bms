#pragma once

#include "../heltec_balancer_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::heltec_balancer_ble {

class HeltecBalancerBle;
class HeltecSwitch : public switch_::Switch {
 public:
  void set_parent(HeltecBalancerBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void write_state(bool state) override;
  HeltecBalancerBle *parent_;
  uint8_t holding_register_;
};

}  // namespace esphome::heltec_balancer_ble
