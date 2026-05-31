#pragma once

#include "../jk_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include <array>
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

class JkPresetSelect : public select::Select, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void add_option_frame(const std::string &option, const std::array<uint8_t, 20> &frame) {
    this->frames_.emplace_back(option, frame);
  }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  std::vector<std::pair<std::string, std::array<uint8_t, 20>>> frames_;
};

}  // namespace esphome::jk_bms_ble
