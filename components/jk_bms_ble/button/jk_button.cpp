#include "jk_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble.button";

void JkButton::dump_config() { LOG_BUTTON("", "JkBmsBle Button", this); }
void JkButton::press_action() { this->parent_->write_register(this->holding_register_, 0x00000000, 0x00); }

}  // namespace jk_bms_ble
}  // namespace esphome
