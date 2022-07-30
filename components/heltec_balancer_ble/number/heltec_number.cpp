#include "heltec_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble.number";

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

void HeltecNumber::dump_config() { LOG_NUMBER("", "HeltecBalancerBle Number", this); }
void HeltecNumber::control(float value) {
  uint32_t payload =
      (this->holding_register_ == 0x01 || this->holding_register_ == 0x16) ? (uint32_t) value : ieee_float_(value);
  if (this->parent_->send_command(FUNCTION_WRITE, COMMAND_WRITE_REGISTER, this->holding_register_, payload)) {
    this->publish_state(state);
  }
}

}  // namespace heltec_balancer_ble
}  // namespace esphome
