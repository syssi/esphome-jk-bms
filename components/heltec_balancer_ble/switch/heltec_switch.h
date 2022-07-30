#pragma once

#include "../heltec_balancer_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace heltec_balancer_ble {

class HeltecBalancerBle;
class HeltecSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(HeltecBalancerBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  HeltecBalancerBle *parent_;
  uint8_t holding_register_;
};

}  // namespace heltec_balancer_ble
}  // namespace esphome
