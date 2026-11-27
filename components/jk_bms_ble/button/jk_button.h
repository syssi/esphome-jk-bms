#pragma once

#include "../jk_bms_ble.h"
#include "esphome/components/button/button.h"

namespace esphome::jk_bms_ble {

class JkBmsBle;
class JkButton : public button::Button {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void set_verify(uint8_t verify) { this->verify_ = verify; }

 protected:
  void press_action() override;
  JkBmsBle *parent_;
  uint8_t holding_register_;
  uint8_t verify_{VERIFY_NONE};
};

class JkSyncTimeButton : public button::Button {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; };

 protected:
  void press_action() override;
  JkBmsBle *parent_;
};

}  // namespace esphome::jk_bms_ble
