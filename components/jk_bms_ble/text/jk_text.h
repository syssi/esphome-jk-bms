#pragma once

#include "../jk_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/text/text.h"

namespace esphome::jk_bms_ble {

class JkBmsBle;
// A text register: the BMS takes the ASCII text as payload, its length as length byte
class JkText : public text::Text, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; }
  void set_verify(uint8_t verify) { this->verify_ = verify; }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  uint8_t holding_register_;
  uint8_t verify_{VERIFY_NONE};
};

}  // namespace esphome::jk_bms_ble
