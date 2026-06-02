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
  void set_data_len(uint8_t data_len) { this->data_len_ = data_len; }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  uint8_t holding_register_;
  uint8_t data_len_{0x02};
};

// Select where each option triggers a write to a distinct register with data_len=0.
// State is pinned to "" on setup and never updated: the BMS provides no read-back
// confirmation, so the empty placeholder stays selected across page reloads.
class JkPresetSelect : public select::Select, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void add_option_register(const std::string &option, uint8_t holding_register) {
    this->option_registers_.emplace_back(option, holding_register);
  }
  void setup() override { this->publish_state(""); }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  std::vector<std::pair<std::string, uint8_t>> option_registers_;
};

}  // namespace esphome::jk_bms_ble
