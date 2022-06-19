#include "jk_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble.number";

void JkNumber::dump_config() { LOG_NUMBER("", "JkBmsBle Number", this); }
void JkNumber::control(float value) {
  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02) {
    uint32_t payload = (uint32_t)(value * this->factor_);
    if (this->parent_->write_register(this->jk02_holding_register_, payload, sizeof(payload))) {
      this->publish_state(state);
    }
    return;
  }

  ESP_LOGE(TAG, "This number entity isn't supported by the selected protocol version");
}

}  // namespace jk_bms_ble
}  // namespace esphome
