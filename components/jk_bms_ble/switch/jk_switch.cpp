#include "jk_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble.switch";

void JkSwitch::dump_config() { LOG_SWITCH("", "JkBmsBle Switch", this); }
void JkSwitch::write_state(bool state) {
  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK04 && this->jk04_holding_register_) {
    if (this->parent_->write_register(this->jk04_holding_register_, (state) ? 0x00000001 : 0x00000000, 0x01)) {
      this->publish_state(state);
    }
    return;
  }

  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02 && this->jk02_holding_register_) {
    if (this->parent_->write_register(this->jk02_holding_register_, (state) ? 0x00000001 : 0x00000000, 0x04)) {
      this->publish_state(state);
    }
    return;
  }

  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02_32S && this->jk02_32s_holding_register_) {
    if (this->parent_->write_register(this->jk02_32s_holding_register_, (state) ? 0x00000001 : 0x00000000, 0x04)) {
      this->publish_state(state);
    }
    return;
  }

  ESP_LOGE(TAG, "This switch isn't supported by the selected protocol version");
}

bool JkSwitch::assumed_state() { return (this->jk02_32s_holding_register_ == 0x6B); }

}  // namespace jk_bms_ble
}  // namespace esphome
