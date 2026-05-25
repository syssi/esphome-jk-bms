#include "jk_number.h"
#include "esphome/core/log.h"
#include <cstring>

namespace esphome::jk_bms_ble {

static const char *const TAG = "jk_bms_ble.number";

void JkNumber::dump_config() { LOG_NUMBER("", "JkBmsBle Number", this); }

void JkNumber::control(float value) {
  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK04 && this->jk04_holding_register_) {
    uint32_t payload = JkBmsBle::encode_jk04_payload(value, this->jk04_length_, this->factor_);
    if (this->parent_->write_register(this->jk04_holding_register_, payload, this->jk04_length_)) {
      this->publish_state(value);
    }
    return;
  }

  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02_24S && this->jk02_holding_register_) {
    uint32_t payload = (uint32_t) (int32_t) (value * this->factor_);
    if (this->parent_->write_register(this->jk02_holding_register_, payload, this->length_)) {
      this->publish_state(value);
    }
    return;
  }

  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK02_32S && this->jk02_32s_holding_register_) {
    uint32_t payload = (uint32_t) (int32_t) (value * this->factor_);
    if (this->parent_->write_register(this->jk02_32s_holding_register_, payload, this->length_)) {
      this->publish_state(value);
    }
    return;
  }

  ESP_LOGE(TAG, "This number entity isn't supported by the selected protocol version");
}

}  // namespace esphome::jk_bms_ble
