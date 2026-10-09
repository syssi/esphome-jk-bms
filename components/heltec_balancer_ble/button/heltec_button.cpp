#include "heltec_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::heltec_balancer_ble {

ESPHOME_LOG_TAG(TAG, "heltec_balancer_ble.button");

static const uint8_t FUNCTION_READ = 0x01;

void HeltecButton::dump_config() { LOG_BUTTON("", "HeltecBalancerBle Button", this); }
void HeltecButton::press_action() { this->parent_->send_command(FUNCTION_READ, this->holding_register_); }

}  // namespace esphome::heltec_balancer_ble
