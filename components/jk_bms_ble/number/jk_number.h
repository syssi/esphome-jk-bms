#pragma once

#include "../jk_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace jk_bms_ble {

class JkBmsBle;

class JkNumber : public number::Number, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; };
  void set_jk02_holding_register(uint8_t jk02_holding_register) {
    this->jk02_holding_register_ = jk02_holding_register;
  };
  void set_jk04_holding_register(uint8_t jk04_holding_register) {
    this->jk04_holding_register_ = jk04_holding_register;
  };
  void set_factor(float factor) { this->factor_ = factor; };
  void dump_config() override;

 protected:
  void control(float value) override;

  JkBmsBle *parent_;
  uint8_t jk02_holding_register_;
  uint8_t jk04_holding_register_;
  float factor_{1000.0f};
};

}  // namespace jk_bms_ble
}  // namespace esphome
