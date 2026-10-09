#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::jk_bms {

ESPHOME_LOG_TAG(TAG, "jk_bms.switch");

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBms Switch", this); }
void JkSwitch::write_state(bool state) {
  this->parent_->write_register(this->holding_register_, (uint8_t) state);
  this->publish_state(state);
}

}  // namespace esphome::jk_bms
