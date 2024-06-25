#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace jk_rs485_bms {

class JkRS485Bms;
class JkRS485BmsSwitch : public switch_::Switch, public Component {
 public:
  explicit JkRS485BmsSwitch(bool initial_state) {
    this->write_state(initial_state);
  }

  // Constructor por defecto necesario para crear instancias sin argumentos
  JkRS485BmsSwitch() = default;

  void set_parent(JkRS485Bms *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }
  
  void write_state(bool state) override {
    this->state = state;
    this->publish_state(state);
  }

 protected:
  JkRS485Bms *parent_;
  uint8_t holding_register_;
};

}  // namespace jk_rs485_bms
}  // namespace esphome
