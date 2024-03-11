#pragma once

#include "../jk_rs485_bms.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace jk_rs485_bms {

class JkRS485Bms;
class JkRS485BmsSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(JkRS485Bms *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  JkRS485Bms *parent_;
  uint8_t holding_register_;
};

}  // namespace jk_rs485_bms
}  // namespace esphome
