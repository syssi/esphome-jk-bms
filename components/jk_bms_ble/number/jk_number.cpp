#include "jk_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble.number";

void JkNumber::dump_config() { LOG_NUMBER("", "JkBmsBle Number", this); }
void JkNumber::control(float value) {
  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK04) {
    ESP_LOGE(TAG, "This number entity isn't supported by the selected protocol version");
    return;
  }

  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02_32S) {
    ESP_LOGW(TAG, "Please be careful! The registers of your BMS model are untested/unverified."
                  "See https://github.com/syssi/esphome-jk-bms/issues/276");
  }

  // JK02 & JK02_32S
  uint32_t payload = (uint32_t)(value * this->factor_);
  if (this->parent_->write_register(this->jk02_holding_register_, payload, sizeof(payload))) {
    this->publish_state(state);
  }
}

}  // namespace jk_bms_ble
}  // namespace esphome
