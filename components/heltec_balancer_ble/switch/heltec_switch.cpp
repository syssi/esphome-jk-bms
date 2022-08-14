#include "heltec_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble.switch";

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

void HeltecSwitch::dump_config() { LOG_SWITCH("", "HeltecBalancerBle Switch", this); }
void HeltecSwitch::write_state(bool state) {
  if (this->parent_->send_command(FUNCTION_WRITE, COMMAND_WRITE_REGISTER, this->holding_register_, (uint32_t) state)) {
    this->publish_state(state);
  }
}

}  // namespace heltec_balancer_ble
}  // namespace esphome
