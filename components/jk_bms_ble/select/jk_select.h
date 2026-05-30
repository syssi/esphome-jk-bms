#pragma once

#include "../jk_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"

namespace esphome::jk_bms_ble {

class JkBmsBle;

class JkSelect : public select::Select, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  uint8_t holding_register_;
};

}  // namespace esphome::jk_bms_ble
