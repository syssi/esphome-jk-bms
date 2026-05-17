#include "heltec_number.h"
#include "esphome/core/log.h"

namespace esphome::heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble.number";

static const uint8_t FUNCTION_WRITE = 0x00;

void HeltecNumber::dump_config() { LOG_NUMBER("", "HeltecBalancerBle Number", this); }
void HeltecNumber::control(float value) {
  uint32_t payload = this->integer_payload_ ? (uint32_t) value : ieee_float_(value);
  if (this->parent_->send_command(FUNCTION_WRITE, this->holding_command_, this->holding_register_, payload)) {
    this->publish_state(value);
  }
}

}  // namespace esphome::heltec_balancer_ble
