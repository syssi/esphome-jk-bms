#pragma once

#include "../jk_rs485_bms.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

//#include "esphome/core/log.h"


namespace esphome {
namespace jk_rs485_bms {

class JkRS485Bms;  // Declaración anticipada
class JkRS485BmsSwitch : public switch_::Switch, public Component {
 public:
  // Constructor por defecto necesario para crear instancias sin argumentos
  JkRS485BmsSwitch() = default;


  explicit JkRS485BmsSwitch(bool initial_state) {
    this->write_state(initial_state);
  }

  
  void set_parent(JkRS485Bms *parent) { this->parent_ = parent; };
  void set_register_address(uint8_t register_address) { this->register_address_ = register_address; };
  uint8_t get_register_address(void) { return(this->register_address_); };  
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

  //void trigger_switch2bms_event(const std::string &event, uint8_t frame_type);

  //void write_state(bool state) override {
  //  this->state = state;
  //  this->publish_state(state);
  //}

  void write_state(bool state) override;
  
 protected:

  JkRS485Bms *parent_;
  uint8_t register_address_;
};




}  // namespace jk_rs485_bms
}  // namespace esphome
