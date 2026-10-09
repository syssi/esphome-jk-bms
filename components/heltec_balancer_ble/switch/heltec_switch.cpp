#include "heltec_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::heltec_balancer_ble {

ESPHOME_LOG_TAG(TAG, "heltec_balancer_ble.switch");

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

void HeltecSwitch::dump_config() { LOG_SWITCH("", "HeltecBalancerBle Switch", this); }
void HeltecSwitch::write_state(bool state) {
  if (this->parent_->send_command(FUNCTION_WRITE, COMMAND_WRITE_REGISTER, this->holding_register_, (uint32_t) state)) {
    this->publish_state(state);
  }
}

}  // namespace esphome::heltec_balancer_ble
