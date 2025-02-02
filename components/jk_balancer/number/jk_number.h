#pragma once

#include "../jk_balancer.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace jk_balancer {

class JkBalancer;

class JkNumber : public number::Number, public Component {
 public:
  void set_parent(JkBalancer *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;

 protected:
  void control(float value) override;

  JkBalancer *parent_;
  uint8_t holding_register_;
};

}  // namespace jk_balancer
}  // namespace esphome
