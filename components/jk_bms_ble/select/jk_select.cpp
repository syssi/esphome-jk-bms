#include "jk_select.h"
#include "esphome/core/log.h"

namespace esphome::jk_bms_ble {

static const char *const TAG = "jk_bms_ble.select";

void JkSelect::dump_config() { LOG_SELECT("", "JkBmsBle Select", this); }

void JkSelect::control(const std::string &value) {
  const auto &options = this->traits.get_options();
  const auto *it = std::find(options.begin(), options.end(), value);
  if (it == options.end()) {
    ESP_LOGW(TAG, "Unknown option: %s", value.c_str());
    return;
  }
  uint32_t index = (uint32_t) (it - options.begin());
  if (this->parent_->write_register(this->holding_register_, index, this->data_len_)) {
    this->publish_state(value);
  }
}

void JkPresetSelect::dump_config() { LOG_SELECT("", "JkBmsBle Preset Select", this); }

void JkPresetSelect::control(const std::string &value) {
  for (const auto &[opt, reg] : this->option_registers_) {
    if (opt == value) {
      this->parent_->write_register(reg, 0x00000000, 0x00);
      return;
    }
  }
  ESP_LOGW(TAG, "Unknown preset: %s", value.c_str());
}

}  // namespace esphome::jk_bms_ble
