#pragma once

#include "../jk_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include <vector>
#include <utility>

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

// Select where each option triggers a write to a distinct register with data_len=0.
class JkPresetSelect : public select::Select, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void add_option_register(const std::string &option, uint8_t holding_register) {
    this->option_registers_.emplace_back(option, holding_register);
  }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  std::vector<std::pair<std::string, uint8_t>> option_registers_;
};

}  // namespace esphome::jk_bms_ble
