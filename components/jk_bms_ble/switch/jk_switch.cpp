#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble.switch";

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBmsBle Switch", this); }
void JkSwitch::write_state(bool state) {
  if (this->parent_->write_register(this->holding_register_, (state) ? 0x01000000 : 0x00000000)) {
    this->publish_state(state);
  }
}

}  // namespace jk_bms_ble
}  // namespace esphome
