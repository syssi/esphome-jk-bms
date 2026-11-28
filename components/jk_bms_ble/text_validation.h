#pragma once

#include <algorithm>
#include <cstddef>
#include <string>

namespace esphome::jk_bms_ble {

// The characters the BMS passes on to its Bluetooth module as name
inline bool is_name_character(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
}

// Why the BMS can't take this text, or nullptr if it can. Free of ESPHome dependencies, so it is unit-testable.
inline const char *text_reject_reason(const std::string &value, size_t min_write_length, size_t max_write_length,
                                      bool name_charset) {
  if (value.size() < min_write_length)
    return "too short";
  if (value.size() > max_write_length)
    return "too long";
  if (name_charset && !std::all_of(value.begin(), value.end(), is_name_character))
    return "only A-Z, a-z, 0-9, _ and - allowed";
  return nullptr;
}

}  // namespace esphome::jk_bms_ble
