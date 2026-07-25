#include "jk_bms_old_ble.h"
#include <array>
#include "esphome/core/log.h"
#include "esphome/core/version.h"

#if ESPHOME_VERSION_CODE >= VERSION_CODE(2025, 12, 0)
#define ADDR_STR(x) x
#else
#define ADDR_STR(x) (x).c_str()
#endif

namespace esphome::jk_bms_old_ble {

static constexpr const char *const TAG = "jk_bms_old_ble";

static const uint8_t MAX_NO_RESPONSE_COUNT = 10;

static const uint16_t JK_BMS_OLD_SERVICE_UUID = 0xFFE0;
static const uint16_t JK_BMS_OLD_NOTIFY_CHARACTERISTIC_UUID = 0xFFE1;
static const uint16_t JK_BMS_OLD_WRITE_CHARACTERISTIC_UUID = 0xFFE2;

static const uint8_t START_OF_FRAME = 0xDD;
static const uint8_t END_OF_FRAME = 0x77;
static const uint8_t OPCODE_READ = 0xA5;
static const uint8_t REGISTER_BASIC_INFO = 0x03;

static const uint16_t MIN_RESPONSE_SIZE = 7;    // Empty payload response
static const uint16_t MAX_RESPONSE_SIZE = 128;  // Generous upper bound, actual frames are <40 bytes

// Legacy "protocol v4" request to read the basic info register (firmware <6.0, f.e. via the JBD-style
// BLE-UART bridge on C8:47:8C:XX:XX:XX modules). See https://github.com/syssi/esphome-jk-bms/issues/13
// and https://github.com/syssi/esphome-jk-bms/pull/14 for the original capture this component is based on.
static constexpr std::array<uint8_t, 7> REQUEST_BASIC_INFO = {0xDD, OPCODE_READ, REGISTER_BASIC_INFO, 0x00,
                                                              0xFF, 0xFD,        END_OF_FRAME};

// Checksum = two's complement of the sum of every byte from the register/status byte up to (and
// including) the last payload byte, i.e. data[2 .. 3 + data[3]]. Verified against real captures for
// both the request (register+length, no payload) and the response (status+length+payload) direction.
static uint16_t checksum(const uint8_t *data) {
  uint16_t sum = 0;
  for (uint16_t i = 2; i < 4u + data[3]; i++) {
    sum += data[i];
  }
  return (uint16_t) (0 - sum);
}

void JkBmsOldBle::dump_config() {
  ESP_LOGCONFIG(TAG, "JkBmsOldBle");

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Online Status", this->online_status_binary_sensor_);

  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging Power", this->discharging_power_sensor_);
  LOG_SENSOR("", "State Of Charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Capacity Remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Full Charge Capacity", this->full_charge_capacity_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Cell Count", this->cell_count_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
}

#ifdef USE_ESP32
void JkBmsOldBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                      esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;
      this->status_notification_received_ = false;

      if (this->notify_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(this->parent()->get_gattc_if(),
                                                          this->parent()->get_remote_bda(), this->notify_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->notify_handle_ = 0;
      this->write_handle_ = 0;

      this->frame_buffer_.clear();

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      // The legacy protocol runs over the same BLE-UART bridge module as the JBD/Xiaoxiang "Smart BMS"
      // clones: a single service (0xFFE0) with a write-only characteristic (0xFFE2) and a
      // notify/write characteristic (0xFFE1).
      auto *notify_chr =
          this->parent_->get_characteristic(JK_BMS_OLD_SERVICE_UUID, JK_BMS_OLD_NOTIFY_CHARACTERISTIC_UUID);
      if (notify_chr == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an JK BMS (legacy protocol)..?",
                 ADDR_STR(this->parent_->address_str()));
        break;
      }
      this->notify_handle_ = notify_chr->handle;

      auto *write_chr =
          this->parent_->get_characteristic(JK_BMS_OLD_SERVICE_UUID, JK_BMS_OLD_WRITE_CHARACTERISTIC_UUID);
      this->write_handle_ = (write_chr != nullptr) ? write_chr->handle : this->notify_handle_;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      this->notify_handle_);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;
      this->status_notification_received_ = false;
      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      if (param->notify.handle != this->notify_handle_)
        break;

      ESP_LOGVV(TAG, "Notification received: %s",
                format_hex_pretty(param->notify.value, param->notify.value_len).c_str());  // NOLINT

      this->assemble(param->notify.value, param->notify.value_len);

      break;
    }
    default:
      break;
  }
}

void JkBmsOldBle::update() {
  this->track_online_status_();
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", ADDR_STR(this->parent_->address_str()));
    return;
  }

  this->request_basic_info_();
}

bool JkBmsOldBle::request_basic_info_() {
  ESP_LOGD(TAG, "Request basic info: %s",
           format_hex_pretty(REQUEST_BASIC_INFO.data(), REQUEST_BASIC_INFO.size()).c_str());  // NOLINT
  std::array<uint8_t, 7> frame = REQUEST_BASIC_INFO;  // esp_ble_gattc_write_char() wants a non-const pointer
  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->write_handle_,
                               frame.size(), frame.data(), ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", ADDR_STR(this->parent_->address_str()), status);
  }

  return (status == 0);
}

#else

void JkBmsOldBle::update() {}

#endif  // USE_ESP32

void JkBmsOldBle::assemble(const uint8_t *data, uint16_t length) {
  if (this->frame_buffer_.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Frame dropped because of invalid length");
    this->frame_buffer_.clear();
  }

  // Flush the buffer on every preamble, a new frame always starts with 0xDD.
  if (length >= 1 && data[0] == START_OF_FRAME) {
    this->frame_buffer_.clear();
  }

  this->frame_buffer_.insert(this->frame_buffer_.end(), data, data + length);

  if (this->frame_buffer_.size() < 4)
    return;

  const uint16_t frame_size = 4u + this->frame_buffer_[3] + 2u + 1u;  // header + payload + checksum + EOF
  if (this->frame_buffer_.size() < frame_size)
    return;

  const uint8_t *raw = &this->frame_buffer_[0];
  if (raw[0] != START_OF_FRAME || raw[frame_size - 1] != END_OF_FRAME) {
    ESP_LOGW(TAG, "Frame dropped, unexpected start/end byte");
    this->frame_buffer_.clear();
    return;
  }

  const uint16_t computed_checksum = checksum(raw);
  const uint16_t remote_checksum = (uint16_t(raw[frame_size - 3]) << 8) | uint16_t(raw[frame_size - 2]);
  if (computed_checksum != remote_checksum) {
    ESP_LOGW(TAG, "Checksum check failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
    this->frame_buffer_.clear();
    return;
  }

  std::vector<uint8_t> frame(this->frame_buffer_.begin(), this->frame_buffer_.begin() + frame_size);
  this->decode_(frame);
  this->frame_buffer_.clear();
}

void JkBmsOldBle::decode_(const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  switch (data[1]) {
    case REGISTER_BASIC_INFO:
      this->decode_basic_info_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unsupported register (0x%02X)", data[1]);
  }
}

void JkBmsOldBle::decode_basic_info_(const std::vector<uint8_t> &data) {
  auto get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | uint16_t(data[i + 1]); };

  const uint32_t now = millis();
  if (now - this->last_basic_info_ < this->throttle_) {
    return;
  }
  this->last_basic_info_ = now;

  ESP_LOGI(TAG, "Basic info frame (%zu bytes) received", data.size());

  static const uint8_t BASIC_INFO_PAYLOAD_SIZE = 27;
  if (data.size() < 4u + BASIC_INFO_PAYLOAD_SIZE) {
    ESP_LOGW(TAG, "Basic info frame too short (%zu bytes)", data.size());
    return;
  }

  // Byte  Len  Content                       Coeff.  Unit
  // 0      2   Total voltage                 0.01    V
  // 2      2   Current (signed, + = charge)  0.01    A
  // 4      2   Remaining capacity            0.01    Ah
  // 6      2   Nominal (full) capacity       0.01    Ah
  // 8      2   Cycle count                   1
  // 10     2   Production date (bit-packed)  -
  // 12     2   Balance status (cells 1-16)   -
  // 14     2   Balance status (cells 17-32)  -
  // 16     2   Protection status bitmask     -
  // 18     1   Software version              -
  // 19     1   State of charge               1       %
  // 20     1   FET status (bit0 chg, bit1 dsg)
  // 21     1   Cell count
  // 22     1   Number of NTC temperature sensors
  // 23     2   Temperature sensor 1          0.1     K
  // 25     2   Temperature sensor 2          0.1     K
  const uint8_t payload = 4;

  float total_voltage = (float) get_16bit(payload + 0) * 0.01f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  float current = (float) (int16_t) get_16bit(payload + 2) * 0.01f;
  this->publish_state_(this->current_sensor_, current);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));

  this->publish_state_(this->capacity_remaining_sensor_, (float) get_16bit(payload + 4) * 0.01f);
  this->publish_state_(this->full_charge_capacity_sensor_, (float) get_16bit(payload + 6) * 0.01f);
  this->publish_state_(this->charging_cycles_sensor_, (float) get_16bit(payload + 8));

  uint8_t fet_status = data[payload + 20];
  this->publish_state_(this->charging_binary_sensor_, check_bit_(fet_status, 0x01));
  this->publish_state_(this->discharging_binary_sensor_, check_bit_(fet_status, 0x02));

  this->publish_state_(this->state_of_charge_sensor_, (float) data[payload + 19]);
  this->publish_state_(this->cell_count_sensor_, (float) data[payload + 21]);

  uint8_t ntc_count = data[payload + 22];
  if (ntc_count >= 1) {
    this->publish_state_(this->temperature_sensor_1_sensor_, (float) get_16bit(payload + 23) * 0.1f - 273.15f);
  }
  if (ntc_count >= 2) {
    this->publish_state_(this->temperature_sensor_2_sensor_, (float) get_16bit(payload + 25) * 0.1f - 273.15f);
  }
}

void JkBmsOldBle::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkBmsOldBle::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void JkBmsOldBle::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);

  this->publish_state_(this->total_voltage_sensor_, NAN);
  this->publish_state_(this->current_sensor_, NAN);
  this->publish_state_(this->power_sensor_, NAN);
  this->publish_state_(this->charging_power_sensor_, NAN);
  this->publish_state_(this->discharging_power_sensor_, NAN);
  this->publish_state_(this->state_of_charge_sensor_, NAN);
  this->publish_state_(this->capacity_remaining_sensor_, NAN);
  this->publish_state_(this->full_charge_capacity_sensor_, NAN);
  this->publish_state_(this->charging_cycles_sensor_, NAN);
  this->publish_state_(this->cell_count_sensor_, NAN);
  this->publish_state_(this->temperature_sensor_1_sensor_, NAN);
  this->publish_state_(this->temperature_sensor_2_sensor_, NAN);
}

void JkBmsOldBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBmsOldBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

}  // namespace esphome::jk_bms_old_ble
