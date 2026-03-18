#include "heltec_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble.select";

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

void HeltecSelect::dump_config() { LOG_SELECT("", "HeltecBalancerBle Select", this); }

void HeltecSelect::control(const std::string &value) {
  const auto &options = this->traits.get_options();
  auto it = std::find(options.begin(), options.end(), value);
  if (it == options.end()) {
    ESP_LOGW(TAG, "Unknown option: %s", value.c_str());
    return;
  }
  // Options are 1-indexed in the register (0 = Unknown, not selectable)
  uint32_t index = (uint32_t) (it - options.begin()) + 1;
  if (this->parent_->send_command(FUNCTION_WRITE, COMMAND_WRITE_REGISTER, this->holding_register_, index)) {
    this->publish_state(value);
  }
}

}  // namespace heltec_balancer_ble
}  // namespace esphome
