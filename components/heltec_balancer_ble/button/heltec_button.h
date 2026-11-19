#pragma once

#include "../heltec_balancer_ble.h"
#include "esphome/components/button/button.h"

namespace esphome::heltec_balancer_ble {

class HeltecBalancerBle;
class HeltecButton : public button::Button {
 public:
  void set_parent(HeltecBalancerBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void press_action() override;
  HeltecBalancerBle *parent_;
  uint8_t holding_register_;
};

}  // namespace esphome::heltec_balancer_ble
