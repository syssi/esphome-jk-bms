#include "jk_switch.h"

namespace esphome::jk_balancer {

void JkSwitch::write_state(bool state) {
  this->parent_->send(this->holding_register_, (uint16_t) state);
  this->publish_state(state);
}

}  // namespace esphome::jk_balancer
