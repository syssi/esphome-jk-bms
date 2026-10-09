#include "heltec_number.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::heltec_balancer_ble {

ESPHOME_LOG_TAG(TAG, "heltec_balancer_ble.number");

static const uint8_t FUNCTION_WRITE = 0x00;

void HeltecNumber::dump_config() { LOG_NUMBER("", "HeltecBalancerBle Number", this); }
void HeltecNumber::control(float value) {
  uint32_t payload = this->integer_payload_ ? (uint32_t) value : ieee_float_(value);
  if (this->parent_->send_command(FUNCTION_WRITE, this->holding_command_, this->holding_register_, payload)) {
    this->publish_state(value);
  }
}

}  // namespace esphome::heltec_balancer_ble
