#pragma once

#include "../jk_rs485_bms.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace jk_rs485_bms {

class JkRS485Bms;

class JkRS485BmsNumber : public number::Number, public Component {
 public:
  // Constructor por defecto necesario para crear instancias sin argumentos
  JkRS485BmsNumber() = default;

  explicit JkRS485BmsNumber(bool initial_state);
  virtual ~JkRS485BmsNumber() = default; // Destructor por defecto


  void set_parent(JkRS485Bms *parent) { this->parent_ = parent; };
  void set_register_address(uint16_t register_address) { this->register_address_ = register_address; };
  void set_factor(float factor) { this->factor_ = factor; };
  void set_type(uint8_t type) { this->type_ = type; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }


 protected:
  void control(float value) override;

  JkRS485Bms *parent_;
  uint16_t register_address_;
  float factor_{1000.0f};
  uint8_t type_;
};

}  // namespace jk_bms_ble
}  // namespace esphome
