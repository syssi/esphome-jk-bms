#include "jk_button.h"

namespace esphome::jk_bms_ble {

void JkButton::press_action() {
  // JK04, JK02_24S, JK02_32S
  this->parent_->write_register(this->holding_register_, 0x00000000, 0x00);
}

}  // namespace esphome::jk_bms_ble
