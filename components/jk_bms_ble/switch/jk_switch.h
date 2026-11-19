#pragma once

#include "../jk_bms_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::jk_bms_ble {

class JkBmsBle;
class JkSwitch : public switch_::Switch {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; };
  void set_jk04_holding_register(uint8_t jk04_holding_register) {
    this->jk04_holding_register_ = jk04_holding_register;
  };
  void set_jk02_holding_register(uint8_t jk02_holding_register) {
    this->jk02_holding_register_ = jk02_holding_register;
  };
  void set_jk02_32s_holding_register(uint8_t jk02_32s_holding_register) {
    this->jk02_32s_holding_register_ = jk02_32s_holding_register;
  };

 protected:
  void write_state(bool state) override;
  JkBmsBle *parent_;
  uint8_t jk04_holding_register_;
  uint8_t jk02_holding_register_;
  uint8_t jk02_32s_holding_register_;
};

}  // namespace esphome::jk_bms_ble
