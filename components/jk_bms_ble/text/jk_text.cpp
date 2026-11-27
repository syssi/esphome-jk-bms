#include "jk_text.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::jk_bms_ble {

ESPHOME_LOG_TAG(TAG, "jk_bms_ble.text");

void JkText::dump_config() { LOG_TEXT("", "JkBmsBle Text", this); }

void JkText::control(const std::string &value) {
  if (this->parent_->get_protocol_version() == PROTOCOL_VERSION_JK04) {
    ESP_LOGW(TAG, "Text registers are not supported by JK04 devices");
    return;
  }

  if (this->parent_->write_register_bytes(this->holding_register_, reinterpret_cast<const uint8_t *>(value.data()),
                                          value.size(), this->verify_)) {
    this->publish_state(value);
  }
}

}  // namespace esphome::jk_bms_ble
