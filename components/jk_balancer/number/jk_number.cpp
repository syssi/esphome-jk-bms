#include "jk_number.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::jk_balancer {

ESPHOME_LOG_TAG(TAG, "jk_balancer.number");

void JkNumber::dump_config() { LOG_NUMBER("", "JkBalancer Number", this); }
void JkNumber::control(float value) {
  this->parent_->send(this->holding_register_, (uint16_t) value);
  this->publish_state(value);
}

}  // namespace esphome::jk_balancer
