#include "jk_switch.h"

namespace esphome::jk_bms {

void JkSwitch::write_state(bool state) {
  this->parent_->write_register(this->holding_register_, (uint8_t) state);
  this->publish_state(state);
}

}  // namespace esphome::jk_bms
