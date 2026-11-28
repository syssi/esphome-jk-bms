#pragma once

#include "../jk_bms_ble.h"
#include "../text_validation.h"
#include "esphome/core/component.h"
#include "esphome/components/text/text.h"

namespace esphome::jk_bms_ble {

class JkBmsBle;
// A text register: the BMS takes the ASCII text as payload, its length as length byte. The traits allow the 16
// characters the BMS stores, so any stored text can be shown; control() rejects what the BMS can't take.
class JkText : public text::Text, public Component {
 public:
  void set_parent(JkBmsBle *parent) { this->parent_ = parent; }
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; }
  void set_verify(uint8_t verify) { this->verify_ = verify; }
  void set_min_write_length(uint8_t min_write_length) { this->min_write_length_ = min_write_length; }
  void set_max_write_length(uint8_t max_write_length) { this->max_write_length_ = max_write_length; }
  void set_name_charset(bool name_charset) { this->name_charset_ = name_charset; }
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  JkBmsBle *parent_;
  uint8_t holding_register_;
  uint8_t verify_{VERIFY_NONE};
  uint8_t min_write_length_{1};
  uint8_t max_write_length_{13};
  bool name_charset_{false};
};

}  // namespace esphome::jk_bms_ble
