#include "heltec_button.h"

namespace esphome::heltec_balancer_ble {

static const uint8_t FUNCTION_READ = 0x01;

void HeltecButton::press_action() { this->parent_->send_command(FUNCTION_READ, this->holding_register_); }

}  // namespace esphome::heltec_balancer_ble
