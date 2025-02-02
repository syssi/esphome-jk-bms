#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jk_balancer {

static const char *const TAG = "jk_balancer.switch";

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBalancer Switch", this); }
void JkSwitch::write_state(bool state) {
  this->parent_->send(this->holding_register_, (uint16_t) state);
  this->publish_state(state);
}

}  // namespace jk_balancer
}  // namespace esphome
