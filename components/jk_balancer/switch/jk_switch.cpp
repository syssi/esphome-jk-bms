#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::jk_balancer {

ESPHOME_LOG_TAG(TAG, "jk_balancer.switch");

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBalancer Switch", this); }
void JkSwitch::write_state(bool state) {
  this->parent_->send(this->holding_register_, (uint16_t) state);
  this->publish_state(state);
}

}  // namespace esphome::jk_balancer
