#pragma once

#include "../jk_bms.h"
#include "esphome/components/switch/switch.h"

namespace esphome::jk_bms {

class JkBms;
class JkSwitch : public switch_::Switch {
 public:
  void set_parent(JkBms *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void write_state(bool state) override;
  JkBms *parent_;
  uint8_t holding_register_;
};

}  // namespace esphome::jk_bms
