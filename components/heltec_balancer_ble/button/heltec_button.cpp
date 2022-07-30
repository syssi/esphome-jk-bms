#include "heltec_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble.button";

static const uint8_t FUNCTION_READ = 0x01;

void HeltecButton::dump_config() { LOG_BUTTON("", "HeltecBalancerBle Button", this); }
void HeltecButton::press_action() { this->parent_->send_command(FUNCTION_READ, this->holding_register_); }

}  // namespace heltec_balancer_ble
}  // namespace esphome
