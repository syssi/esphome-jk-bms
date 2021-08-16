#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jk_bms {

static const char *const TAG = "jk_bms.switch";

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBms Switch", this); }
void JkSwitch::write_state(bool state) {
  // @FIXME
  // this->parent_->write_register(this->holding_register_, (uint16_t) state);
}

}  // namespace jk_bms
}  // namespace esphome
