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

  const char *rejected =
      text_reject_reason(value, this->min_write_length_, this->max_write_length_, this->name_charset_);
  if (rejected != nullptr) {
    ESP_LOGW(TAG, "'%s': rejected '%s' (%s, %u-%u characters)", this->get_name().c_str(), value.c_str(), rejected,
             this->min_write_length_, this->max_write_length_);
    // Republish the stored text, so the frontend doesn't keep showing the rejected one
    if (this->has_state())
      this->publish_state(this->state);
    return;
  }

  if (this->parent_->write_register_bytes(this->holding_register_, reinterpret_cast<const uint8_t *>(value.data()),
                                          value.size(), this->verify_)) {
    this->publish_state(value);
  }
}

}  // namespace esphome::jk_bms_ble
