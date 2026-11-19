#include "heltec_switch.h"

namespace esphome::heltec_balancer_ble {

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

void HeltecSwitch::write_state(bool state) {
  if (this->parent_->send_command(FUNCTION_WRITE, COMMAND_WRITE_REGISTER, this->holding_register_, (uint32_t) state)) {
    this->publish_state(state);
  }
}

}  // namespace esphome::heltec_balancer_ble
