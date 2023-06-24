#pragma once

#include "../jk_bms.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace jk_bms {

class JkBms;
class JkSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(JkBms *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  JkBms *parent_;
  uint8_t holding_register_;
};

}  // namespace jk_bms
}  // namespace esphome
