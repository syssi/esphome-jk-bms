#include "jk_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jk_balancer {

static const char *const TAG = "jk_balancer.number";

void JkNumber::dump_config() { LOG_NUMBER("", "JkBalancer Number", this); }
void JkNumber::control(float value) {
  this->parent_->send(this->holding_register_, (uint16_t) value);
  this->publish_state(value);
}

}  // namespace jk_balancer
}  // namespace esphome
