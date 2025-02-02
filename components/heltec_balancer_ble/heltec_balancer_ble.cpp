#include "heltec_balancer_ble.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace heltec_balancer_ble {

static const char *const TAG = "heltec_balancer_ble";

static const uint8_t MAX_NO_RESPONSE_COUNT = 10;

static const uint16_t HELTEC_BALANCER_SERVICE_UUID = 0xFFE0;
static const uint16_t HELTEC_BALANCER_CHARACTERISTIC_UUID = 0xFFE1;

static const uint8_t SOF_REQUEST_BYTE1 = 0xAA;
static const uint8_t SOF_REQUEST_BYTE2 = 0x55;
static const uint8_t SOF_RESPONSE_BYTE1 = 0x55;
static const uint8_t SOF_RESPONSE_BYTE2 = 0xAA;
static const uint8_t DEVICE_ADDRESS = 0x11;

static const uint8_t FUNCTION_WRITE = 0x00;
static const uint8_t FUNCTION_READ = 0x01;

static const uint8_t COMMAND_NONE = 0x00;
static const uint8_t COMMAND_DEVICE_INFO = 0x01;
static const uint8_t COMMAND_CELL_INFO = 0x02;
static const uint8_t COMMAND_FACTORY_DEFAULTS = 0x03;
static const uint8_t COMMAND_SETTINGS = 0x04;
static const uint8_t COMMAND_WRITE_REGISTER = 0x05;

static const uint8_t END_OF_FRAME = 0xFF;

static const uint16_t MIN_RESPONSE_SIZE = 20;   // Write acknowledge frame
static const uint16_t MAX_RESPONSE_SIZE = 300;  // Cell info frame

static const uint8_t OPERATION_STATUS_SIZE = 13;
static const char *const OPERATION_STATUS[OPERATION_STATUS_SIZE] = {
    "Unknown",                                   // 0x00
    "Wrong cell count",                          // 0x01
    "AcqLine Res test",                          // 0x02
    "AcqLine Res exceed",                        // 0x03
    "Systest Completed",                         // 0x04
    "Balancing",                                 // 0x05
    "Balancing finished",                        // 0x06
    "Low voltage",                               // 0x07
    "System Overtemp",                           // 0x08
    "Host fails",                                // 0x09
    "Low battery voltage - balancing stopped",   // 0x0A
    "Temperature too high - balancing stopped",  // 0x0B
    "Self-test completed",                       // 0x0C
};

static const uint8_t BUZZER_MODES_SIZE = 4;
static const char *const BUZZER_MODES[BUZZER_MODES_SIZE] = {
    "Unknown",       // 0x00
    "Off",           // 0x01
    "Beep once",     // 0x02
    "Beep regular",  // 0x03
};

static const uint8_t BATTERY_TYPES_SIZE = 5;
static const char *const BATTERY_TYPES[BATTERY_TYPES_SIZE] = {
    "Unknown",  // 0x00
    "NCM",      // 0x01
    "LFP",      // 0x02
    "LTO",      // 0x03
    "PbAc",     // 0x04
};

static const uint8_t CELL_ERRORS_SIZE = 8;
static const char *const CELL_ERRORS[CELL_ERRORS_SIZE] = {
    "Battery detection failed",  "Overvoltage",        "Undervoltage",   "Polarity error",
    "Excessive line resistance", "System overheating", "Charging fault", "Discharge fault",
};

uint8_t crc(const uint8_t data[], const uint16_t len) {
  uint8_t crc = 0;
  for (uint16_t i = 0; i < len; i++) {
    crc = crc + data[i];
  }
  return crc;
}

void HeltecBalancerBle::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "HeltecBalancerBle");
  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Online Status", this->online_status_binary_sensor_);
  LOG_SENSOR("", "Minimum Cell Voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Maximum Cell Voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Minimum Voltage Cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Maximum Voltage Cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta Cell Voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Average Cell Voltage", this->average_cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 17", this->cells_[16].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 18", this->cells_[17].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 19", this->cells_[18].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 20", this->cells_[19].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 21", this->cells_[20].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 22", this->cells_[21].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 23", this->cells_[22].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 24", this->cells_[23].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Resistance 1", this->cells_[0].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 2", this->cells_[1].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 3", this->cells_[2].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 4", this->cells_[3].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 5", this->cells_[4].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 6", this->cells_[5].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 7", this->cells_[6].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 8", this->cells_[7].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 9", this->cells_[8].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 10", this->cells_[9].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 11", this->cells_[10].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 12", this->cells_[11].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 13", this->cells_[12].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 14", this->cells_[13].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 15", this->cells_[14].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 16", this->cells_[15].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 17", this->cells_[16].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 18", this->cells_[17].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 19", this->cells_[18].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 20", this->cells_[19].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 21", this->cells_[20].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 22", this->cells_[21].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 23", this->cells_[22].cell_resistance_sensor_);
  LOG_SENSOR("", "Cell Resistance 24", this->cells_[23].cell_resistance_sensor_);
  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
  LOG_SENSOR("", "Total Runtime", this->total_runtime_sensor_);
  LOG_TEXT_SENSOR("", "Operation Status", this->operation_status_text_sensor_);
  LOG_TEXT_SENSOR("", "Total Runtime Formatted", this->total_runtime_formatted_text_sensor_);
}

void HeltecBalancerBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                            esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;
      this->status_notification_received_ = false;

      if (this->char_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(this->parent()->get_gattc_if(),
                                                          this->parent()->get_remote_bda(), this->char_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->char_handle_ = 0;

      this->frame_buffer_.clear();

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *chr = this->parent_->get_characteristic(HELTEC_BALANCER_SERVICE_UUID, HELTEC_BALANCER_CHARACTERISTIC_UUID);
      if (chr == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an Heltec/NEEY balancer..?",
                 this->parent_->address_str().c_str());
        break;
      }

      // Services and characteristic of a GW-24S4EB, HW-2.8.0, SW-1.1.0, V1.0.0, 20210915 (3C:A5:51:XX:XX:XX)
      //
      // Service UUID: 0x1800
      //  start_handle: 0x1  end_handle: 0x7
      // Service UUID: 0xFFE0
      //  start_handle: 0x8  end_handle: 0xffff
      // Connected
      //  characteristic 0xFFE1, handle 0xa, properties 0x1c
      //  characteristic 0xFFE2, handle 0xd, properties 0x1c
      //  characteristic 0xFFE3, handle 0x10, properties 0xc
      // cfg_mtu status 0, mtu 244

      // Services and characteristic of a EK-24S4EB, HW-3.2.0, ZH-1.2.9, V1.2.9, 20230608 (3C:A5:51:XX:XX:XX)
      //
      // Service UUID: 0x1800
      //  start_handle: 0x1  end_handle: 0x7
      // Service UUID: 0xFFE0
      //  start_handle: 0x8  end_handle: 0xffff
      // Connected
      //  characteristic 0xFFE1, handle 0xa, properties 0x1c
      //  characteristic 0xFFE2, handle 0xd, properties 0x1c
      //  characteristic 0xFFE3, handle 0x10, properties 0xc
      // cfg_mtu status 0, mtu 244
      this->char_handle_ = chr->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      chr->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;
      this->status_notification_received_ = false;

      ESP_LOGI(TAG, "Request device info");
      this->send_command(FUNCTION_READ, COMMAND_DEVICE_INFO);

      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      if (param->notify.handle != this->char_handle_)
        break;

      this->assemble(param->notify.value, param->notify.value_len);

      break;
    }
    default:
      break;
  }
}

void HeltecBalancerBle::update() {
  this->track_online_status_();
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  if (!this->status_notification_received_) {
    ESP_LOGI(TAG, "Request status notification");
    this->send_command(FUNCTION_READ, COMMAND_CELL_INFO);
  }
}

// TODO: There is no need to assemble frames if the MTU can be increased to > MAX_RESPONSE_SIZE
void HeltecBalancerBle::assemble(const uint8_t *data, uint16_t length) {
  if (this->frame_buffer_.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Frame dropped because of invalid length");
    this->frame_buffer_.clear();
  }

  // Flush buffer on every preamble
  if (data[0] == SOF_RESPONSE_BYTE1 && data[1] == SOF_RESPONSE_BYTE2) {
    this->frame_buffer_.clear();
  }

  this->frame_buffer_.insert(this->frame_buffer_.end(), data, data + length);

  if (this->frame_buffer_.size() >= MIN_RESPONSE_SIZE && this->frame_buffer_.back() == END_OF_FRAME) {
    const uint8_t *raw = &this->frame_buffer_[0];
    const uint16_t frame_size = this->frame_buffer_.size();

    uint8_t computed_crc = crc(raw, frame_size - 2);
    uint8_t remote_crc = raw[frame_size - 2];
    if (computed_crc != remote_crc) {
      ESP_LOGW(TAG, "CRC check failed! 0x%02X != 0x%02X", computed_crc, remote_crc);
      this->frame_buffer_.clear();
      return;
    }

    std::vector<uint8_t> data(this->frame_buffer_.begin(), this->frame_buffer_.end());

    this->decode_(data);
    this->frame_buffer_.clear();
  }
}

void HeltecBalancerBle::decode_(const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  uint8_t frame_type = data[4];
  switch (frame_type) {
    case COMMAND_DEVICE_INFO:
      this->decode_device_info_(data);
      break;
    case COMMAND_CELL_INFO:
      this->decode_cell_info_(data);
      break;
    case COMMAND_FACTORY_DEFAULTS:
      this->decode_factory_defaults_(data);
      break;
    case COMMAND_SETTINGS:
      this->decode_settings_(data);
      break;
    case COMMAND_WRITE_REGISTER:
      ESP_LOGD(TAG, "Write register response received: %s", format_hex_pretty(data.data(), data.size()).c_str());
      break;
    default:
      ESP_LOGW(TAG, "Unsupported message type (0x%02X)", data[4]);
  }
}

void HeltecBalancerBle::decode_cell_info_(const std::vector<uint8_t> &data) {
  auto heltec_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto heltec_get_24bit = [&](size_t i) -> uint32_t {
    return (uint32_t(data[i + 2]) << 16) | (uint32_t(data[i + 1]) << 8) | (uint32_t(data[i + 0]) << 0);
  };
  auto heltec_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(heltec_get_16bit(i + 2)) << 16) | (uint32_t(heltec_get_16bit(i + 0)) << 0);
  };

  const uint32_t now = millis();
  if (now - this->last_cell_info_ < this->throttle_) {
    return;
  }
  this->last_cell_info_ = now;

  ESP_LOGI(TAG, "Cell info frame (%d bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), 150).c_str());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front() + 150, data.size() - 150).c_str());

  // Cell info frame (300 bytes)
  // 0x55 0xAA 0x11 0x01 0x02 0x00 0x2C 0x01 0x38 0xE7 0xFA 0x50 0x40 0xB6 0x04 0x51 0x40 0x85 0x0E 0x51
  // 0x40 0xF0 0x05 0x51 0x40 0xB6 0x04 0x51 0x40 0x75 0x1E 0x51 0x40 0x7F 0x4F 0x51 0x40 0x43 0x02 0x51
  // 0x40 0x1C 0x3D 0x51 0x40 0x78 0x6A 0x51 0x40 0xFE 0x82 0x51 0x40 0x16 0x7E 0x51 0x40 0xBC 0x76 0x51
  // 0x40 0x16 0x7E 0x51 0x40 0x8B 0x80 0x51 0x40 0xCA 0x66 0x51 0x40 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x35 0x93 0x24 0x3E 0x68 0x94 0x26 0x3E 0x3D 0x25 0x1B 0x3E 0x90 0x8E 0x1B
  // 0x3E 0xB3 0xF3 0x23 0x3E 0x2E 0x91 0x25 0x3E 0xC6 0x1B 0x1A 0x3E 0x4A 0x7C 0x1C 0x3E 0x6F 0x1B 0x1A
  // 0x3E 0xC2 0x43 0x1B 0x3E 0x85 0x1E 0x18 0x3E 0x4B 0x27 0x19 0x3E 0x5E 0xDF 0x18 0x3E 0xD0 0xEB 0x1A
  // 0x3E 0xE6 0xD4 0x18 0x3E 0x0C 0xFE 0x18 0x3E 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0xDE 0x40 0x51 0x42 0xDE 0x40 0x51 0x40 0x00 0x17 0x08 0x3C 0x0A 0x00 0x0F 0x05 0x19 0xA1 0x82
  // 0xC0 0xC3 0xF5 0x48 0x42 0xC3 0xF5 0x48 0x42 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x76 0x2E 0x09 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xB6 0xFF
  //
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     2   0x55 0xAA              Header
  // 2     1   0x11                   Device address
  // 3     1   0x01                   Function (read)
  // 4     2   0x02 0x00              Command (cell info)
  // 6     2   0x2C 0x01              Length (300 bytes)
  // 8     1   0x38                   Frame counter
  ESP_LOGD(TAG, "  Frame counter: %d", data[8]);
  // 9     4   0xE7 0xFA 0x50 0x40              Cell 1 voltage
  // 13    4   0xB6 0x04 0x51 0x40              Cell 2 voltage
  // 17    4   0x85 0x0E 0x51 0x40              Cell 3 voltage
  // 21    4   0xF0 0x05 0x51 0x40              Cell 4 voltage
  // 25    4   0xB6 0x04 0x51 0x40              Cell 5 voltage
  // 29    4   0x75 0x1E 0x51 0x40              Cell 6 voltage
  // 33    4   0x7F 0x4F 0x51 0x40              Cell 7 voltage
  // 37    4   0x43 0x02 0x51 0x40              Cell 8 voltage
  // 41    4   0x1C 0x3D 0x51 0x40              Cell 9 voltage
  // 45    4   0x78 0x6A 0x51 0x40              Cell 10 voltage
  // 49    4   0xFE 0x82 0x51 0x40              Cell 11 voltage
  // 53    4   0x16 0x7E 0x51 0x40              Cell 12 voltage
  // 57    4   0xBC 0x76 0x51 0x40              Cell 13 voltage
  // 61    4   0x16 0x7E 0x51 0x40              Cell 14 voltage
  // 65    4   0x8B 0x80 0x51 0x40              Cell 15 voltage
  // 69    4   0xCA 0x66 0x51 0x40              Cell 16 voltage
  // 73    4   0x00 0x00 0x00 0x00              Cell 17 voltage
  // 77    4   0x00 0x00 0x00 0x00              Cell 18 voltage
  // 81    4   0x00 0x00 0x00 0x00              Cell 19 voltage
  // 85    4   0x00 0x00 0x00 0x00              Cell 20 voltage
  // 89    4   0x00 0x00 0x00 0x00              Cell 21 voltage
  // 93    4   0x00 0x00 0x00 0x00              Cell 22 voltage
  // 97    4   0x00 0x00 0x00 0x00              Cell 23 voltage
  // 101   4   0x00 0x00 0x00 0x00              Cell 24 voltage
  // 105   4   0x35 0x93 0x24 0x3E              Cell 1 resistance
  // 109   4   0x68 0x94 0x26 0x3E              Cell 2 resistance
  // 113   4   0x3D 0x25 0x1B 0x3E              Cell 3 resistance
  // 117   4   0x90 0x8E 0x1B 0x3E              Cell 4 resistance
  // 121   4   0xB3 0xF3 0x23 0x3E              Cell 5 resistance
  // 125   4   0x2E 0x91 0x25 0x3E              Cell 6 resistance
  // 127   4   0xC6 0x1B 0x1A 0x3E              Cell 7 resistance
  // 133   4   0x4A 0x7C 0x1C 0x3E              Cell 8 resistance
  // 137   4   0x6F 0x1B 0x1A 0x3E              Cell 9 resistance
  // 141   4   0xC2 0x43 0x1B 0x3E              Cell 10 resistance
  // 145   4   0x85 0x1E 0x18 0x3E              Cell 11 resistance
  // 149   4   0x4B 0x27 0x19 0x3E              Cell 12 resistance
  // 153   4   0x5E 0xDF 0x18 0x3E              Cell 13 resistance
  // 157   4   0xD0 0xEB 0x1A 0x3E              Cell 14 resistance
  // 161   4   0xE6 0xD4 0x18 0x3E              Cell 15 resistance
  // 165   4   0x0C 0xFE 0x18 0x3E              Cell 16 resistance
  // 169   4   0x00 0x00 0x00 0x00              Cell 17 resistance
  // 173   4   0x00 0x00 0x00 0x00              Cell 18 resistance
  // 177   4   0x00 0x00 0x00 0x00              Cell 19 resistance
  // 181   4   0x00 0x00 0x00 0x00              Cell 20 resistance
  // 185   4   0x00 0x00 0x00 0x00              Cell 21 resistance
  // 189   4   0x00 0x00 0x00 0x00              Cell 22 resistance
  // 193   4   0x00 0x00 0x00 0x00              Cell 23 resistance
  // 197   4   0x00 0x00 0x00 0x00              Cell 24 resistance
  // 201   4   0xDE 0x40 0x51 0x42              Total voltage
  this->publish_state_(this->total_voltage_sensor_, ieee_float_(heltec_get_32bit(201)));

  // 205   4   0xDE 0x40 0x51 0x40              Average cell voltage
  // this->publish_state_(this->average_cell_voltage_sensor_, ieee_float_(heltec_get_32bit(205)));

  // 209   4   0x00 0x17 0x08 0x3C              Delta Cell Voltage
  // this->publish_state_(this->delta_cell_voltage_sensor_, ieee_float_(heltec_get_32bit(209)));

  // 213   1   0x0A                             Max voltage cell
  // this->publish_state_(this->max_voltage_cell_sensor_, (float) data[213] + 1);

  // 214   1   0x00                             Min voltage cell
  // this->publish_state_(this->min_voltage_cell_sensor_, (float) data[214] + 1);

  uint8_t cells = 24;
  uint8_t cells_enabled = 0;
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = ieee_float_(heltec_get_32bit(i * 4 + 9));
    float cell_resistance = ieee_float_(heltec_get_32bit(i * 4 + 105));
    if (cell_voltage > 0) {
      average_cell_voltage = average_cell_voltage + cell_voltage;
      cells_enabled++;
    }
    if (cell_voltage > 0 && cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
    this->publish_state_(this->cells_[i].cell_resistance_sensor_, cell_resistance);
  }
  average_cell_voltage = average_cell_voltage / cells_enabled;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  // 215   1   0x0F                             Single number (not exposed at the android app)
  // ESP_LOGI(TAG, "  Cell count?: %d", data[215] + 1);

  // 216   1   0x05                             Operation status
  //                                              0x01: Wrong cell count
  //                                              0x02: AcqLine Res test
  //                                              0x03: AcqLine Res exceed
  //                                              0x04: Systest Completed
  //                                              0x05: Balancing
  //                                              0x06: Balancing finished
  //                                              0x07: Low voltage
  //                                              0x08: System Overtemp
  //                                              0x09: Host fails
  //                                              0x0A: Low battery voltage - balancing stopped
  //                                              0x0B: Temperature too high - balancing stopped
  //                                              0x0C: Self-test completed
  uint8_t raw_operation_status = data[216];
  this->publish_state_(this->balancing_binary_sensor_, (raw_operation_status == 0x05));
  if (raw_operation_status < OPERATION_STATUS_SIZE) {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS[raw_operation_status]);
  } else {
    this->publish_state_(this->operation_status_text_sensor_, "Unknown");
  }

  // 217   4   0x19 0xA1 0x82 0xC0              Balancing current
  this->publish_state_(this->balancing_current_sensor_, ieee_float_(heltec_get_32bit(217)));

  // 221   4   0xC3 0xF5 0x48 0x42              Temperature 1
  this->publish_state_(this->temperature_sensor_1_sensor_, ieee_float_(heltec_get_32bit(221)));

  // 225   4   0xC3 0xF5 0x48 0x42              Temperature 2
  this->publish_state_(this->temperature_sensor_2_sensor_, ieee_float_(heltec_get_32bit(225)));

  // 229   3   0x00 0x00 0x00                   Cell detection failed bitmask (24 bits = 1 bit per cell)
  this->publish_state_(this->cell_detection_failed_bitmask_sensor_, heltec_get_24bit(229));
  // 232   3   0x00 0x00 0x00                   Cell overvoltage bitmask (24 cells)
  this->publish_state_(this->cell_overvoltage_bitmask_sensor_, heltec_get_24bit(232));
  // 235   3   0x00 0x00 0x00                   Cell undervoltage bitmask (24 cells)
  this->publish_state_(this->cell_undervoltage_bitmask_sensor_, heltec_get_24bit(235));
  // 238   3   0x00 0x00 0x00                   Cell polarity error bitmask (24 cells)
  this->publish_state_(this->cell_polarity_error_bitmask_sensor_, heltec_get_24bit(238));
  // 241   3   0x00 0x00 0x00                   Excessive line resistance bitmask (24 cells)
  this->publish_state_(this->cell_excessive_line_resistance_bitmask_sensor_, heltec_get_24bit(241));
  // 244   1   0x00                             System overheating
  this->publish_state_(this->error_system_overheating_binary_sensor_, data[244] != 0x00);
  //                                              Bit0: Temperature sensor 1 warning
  //                                              Bit1: Temperature sensor 2 warning
  // 245   1   0x00                             Charging fault
  //                                              0x00: Off
  //                                              0x01: On
  this->publish_state_(this->error_charging_binary_sensor_, (bool) data[245]);
  // 246   1   0x00                             Discharge fault
  //                                              0x00: Off
  //                                              0x01: On
  this->publish_state_(this->error_discharging_binary_sensor_, (bool) data[246]);
  // 247   1   0x00                             Unknown
  //                                              Bit0: Read failed
  //                                              Bit1: Write failed
  // 248   6   0x00 0x00 0x00 0x00 0x00 0x00    Reserved
  // 254   4   0x76 0x2E 0x09 0x00              Uptime?
  ESP_LOGI(TAG, "  Uptime: %s (%lus)", format_total_runtime_(heltec_get_32bit(254)).c_str(),
           (unsigned long) heltec_get_32bit(254));

  // 258   40  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 298   1   0xB6
  // 299   1   0xFF

  this->status_notification_received_ = true;
}

void HeltecBalancerBle::decode_settings_(const std::vector<uint8_t> &data) {
  auto heltec_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto heltec_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(heltec_get_16bit(i + 2)) << 16) | (uint32_t(heltec_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Settings frame (%d bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(data.data(), data.size()).c_str());

  // Settings frame (100 bytes)
  // 0x55 0xAA 0x11 0x01 0x04 0x00 0x64 0x00 0x10 0x0A 0xD7 0xA3 0x3B 0x00 0x00 0x80 0x40 0x00 0x00 0x20
  // 0x40 0x01 0x01 0x02 0x18 0x01 0x00 0x00 0x66 0x66 0x26 0x40 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xB7 0xFF
  //
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     2   0x55 0xAA              Header
  // 2     1   0x11                   Device address
  // 3     1   0x01                   Function (read)
  // 4     2   0x01 0x00              Command (device info)
  // 6     2   0x64 0x00              Length (100 bytes)
  // 8     1   0x10                   Cell count
  this->publish_state_(this->cell_count_number_, (float) data[8]);

  // 9     4   0x0A 0xD7 0xA3 0x3B    Balance trigger voltage                      0.005 V
  this->publish_state_(this->balance_trigger_voltage_number_, ieee_float_(heltec_get_32bit(9)));

  // 13    4   0x00 0x00 0x80 0x40    Max. balance current                         4 A
  this->publish_state_(this->max_balance_current_number_, ieee_float_(heltec_get_32bit(13)));

  // 17    4   0x00 0x00 0x20 0x40    Balancing stop voltage                       2.5 V
  this->publish_state_(this->balance_sleep_voltage_number_, ieee_float_(heltec_get_32bit(17)));

  // 21    1   0x01                   Balancing enabled
  uint8_t raw_balancer_enabled = data[21];
  this->publish_state_(this->balancer_switch_, (bool) raw_balancer_enabled);

  // 22    1   0x01                   Buzzer mode (0x01: Off, 0x02: Beep once, 0x03: Beep regular)
  uint8_t raw_buzzer_mode = data[22];
  if (raw_buzzer_mode < BUZZER_MODES_SIZE) {
    this->publish_state_(this->buzzer_mode_text_sensor_, BUZZER_MODES[raw_buzzer_mode]);
  } else {
    this->publish_state_(this->buzzer_mode_text_sensor_, "Unknown");
  }

  // 23    1   0x02                   Battery type (0x01: NCM, 0x02: LFP, 0x03: LTO, 0x04: PbAc)
  uint8_t raw_battery_type = data[23];
  if (raw_battery_type < BATTERY_TYPES_SIZE) {
    this->publish_state_(this->battery_type_text_sensor_, BATTERY_TYPES[raw_battery_type]);
  } else {
    this->publish_state_(this->battery_type_text_sensor_, "Unknown");
  }

  // 24    4   0x18 0x01 0x00 0x00    Nominal battery capacity
  this->publish_state_(this->nominal_battery_capacity_number_, (float) heltec_get_32bit(24));

  // 28    4   0x66 0x66 0x26 0x40    Start balance voltage                        2.6 V
  this->publish_state_(this->balance_start_voltage_number_, ieee_float_(heltec_get_32bit(28)));

  // 32    66  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00
  // 98    1   0xB7
  // 99    1   0xFF
}

void HeltecBalancerBle::decode_factory_defaults_(const std::vector<uint8_t> &data) {
  auto heltec_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto heltec_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(heltec_get_16bit(i + 2)) << 16) | (uint32_t(heltec_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Factory defaults frame (%d bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(data.data(), data.size()).c_str());

  // Skip the ackowledge frame
  if (data.size() == 20) {
    return;
  }

  // A factory settings request returns 3 responses?
  //
  //   -> 0xAA 0x55 0x11 0x01 0x03 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xF8 0xFF (20)
  //   <- 0x55 0xAA 0x11 0x01 0x03 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x28 0xFF (20)
  //   <- 0x55 0xAA 0x11 0x01 0x03 0x00 0x64 0x00 0x7B 0xD2 0xBF 0x3F 0x35 0xCC 0xBF 0x3F 0x51 0x82 0x54 0x40 0x33 0x33
  // 0x73 0x40 0xAA 0xC0 0xDB 0x3F 0x7B 0xE1 0xDB 0x3F 0x61 0xD6 0xF0 0x3F 0x00 0x00 0x00 0x00 0x00 0x00 0x80 0x3F 0x33
  // 0x33 0xD3 0x3F 0x5C 0x8F 0xD2 0x3F 0x48 0xE1 0xBA 0x3F 0x00 0x00 0xAA 0x42 0x00 0x00 0x82 0x42 0x08 0x00 0x00 0x00
  // 0xF6 0xE1 0x0B 0x00 0x32 0x30 0x32 0x32 0x30 0x35 0x33 0x31 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x20 0xFF (100)
  //   <- 0x55 0xAA 0x11 0x01 0x03 0x00 0x64 0x00 0x75 0xD5 0xBF 0x3F 0x35 0xCC 0xBF 0x3F 0x17 0x81 0x54 0x40 0x33 0x33
  // 0x73 0x40 0x74 0xB6 0xDB 0x3F 0x7B 0xE1 0xDB 0x3F 0x62 0xD0 0xF0 0x3F 0x00 0x00 0x00 0x00 0x00 0x00 0x80 0x3F 0x33
  // 0x33 0xD3 0x3F 0x5C 0x8F 0xD2 0x3F 0x48 0xE1 0xBA 0x3F 0x00 0x00 0xAA 0x42 0x00 0x00 0x82 0x42 0x08 0x00 0x00 0x00
  // 0xF6 0xE1 0x0B 0x00 0x32 0x30 0x32 0x32 0x30 0x35 0x33 0x31 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x9D 0xFF (100)
  //
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     2   0x55 0xAA              Header
  // 2     1   0x11                   Device address
  // 3     1   0x01                   Function (read)
  // 4     2   0x03 0x00              Command (factory settings)
  // 6     2   0x64 0x00              Length (100 bytes)
  // 8     4   0x7B 0xD2 0xBF 0x3F    Standard voltage 1
  ESP_LOGI(TAG, "  Standard voltage 1: %.3f V", ieee_float_(heltec_get_32bit(8)));

  // 12    4   0x35 0xCC 0xBF 0x3F    Standard voltage 2
  ESP_LOGI(TAG, "  Standard voltage 2: %.3f V", ieee_float_(heltec_get_32bit(2)));

  // 16    4   0x51 0x82 0x54 0x40    Battery voltage 1
  ESP_LOGI(TAG, "  Battery voltage 1: %.3f V", ieee_float_(heltec_get_32bit(16)));

  // 20    4   0x33 0x33 0x73 0x40    Battery voltage 2
  ESP_LOGI(TAG, "  Battery voltage 2: %.3f V", ieee_float_(heltec_get_32bit(20)));

  // 24    4   0xAA 0xC0 0xDB 0x3F    Standard current 1
  ESP_LOGI(TAG, "  Standard current 1: %.3f A", ieee_float_(heltec_get_32bit(24)));

  // 28    4   0x7B 0xE1 0xDB 0x3F    Standard current 2
  ESP_LOGI(TAG, "  Standard current 2: %.3f A", ieee_float_(heltec_get_32bit(28)));

  // 32    4   0x61 0xD6 0xF0 0x3F    SuperBat 1
  ESP_LOGI(TAG, "  SuperBat 1: %.3f", ieee_float_(heltec_get_32bit(32)));

  // 36    4   0x00 0x00 0x00 0x00    SuperBat 2
  ESP_LOGI(TAG, "  SuperBat 2: %.3f", ieee_float_(heltec_get_32bit(36)));

  // 40    4   0x00 0x00 0x80 0x3F    Resistor 1
  ESP_LOGI(TAG, "  Resistor 1: %.3f", ieee_float_(heltec_get_32bit(40)));

  // 44    4   0x33 0x33 0xD3 0x3F    Battery status
  ESP_LOGI(TAG, "  Battery status: %.3f", ieee_float_(heltec_get_32bit(44)));

  // 48    4   0x5C 0x8F 0xD2 0x3F    Max voltage
  ESP_LOGI(TAG, "  Max voltage: %.3f V", ieee_float_(heltec_get_32bit(48)));

  // 52    4   0x48 0xE1 0xBA 0x3F    Min voltage
  ESP_LOGI(TAG, "  Min voltage: %.3f V", ieee_float_(heltec_get_32bit(52)));

  // 56    4   0x00 0x00 0xAA 0x42    Max temperature
  ESP_LOGI(TAG, "  Max temperature: %.3f °C", ieee_float_(heltec_get_32bit(56)));

  // 60    4   0x00 0x00 0x82 0x42    Min temperature
  ESP_LOGI(TAG, "  Min temperature: %.3f °C", ieee_float_(heltec_get_32bit(60)));

  // 64    4   0x08 0x00 0x00 0x00    Power on counter
  ESP_LOGI(TAG, "  Power on counter: %lu", (unsigned long) heltec_get_32bit(64));

  // 68    4   0xF6 0xE1 0x0B 0x00    Total runtime
  ESP_LOGI(TAG, "  Total runtime: %lu", (unsigned long) heltec_get_32bit(68));

  // 72    8   0x32 0x30 0x32 0x32 0x30 0x35 0x33 0x31    Production date
  ESP_LOGI(TAG, "  Protocol version: %s", std::string(data.begin() + 72, data.begin() + 72 + 8).c_str());

  // 80   18   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 98    1   0x20                   CRC
  // 99    1   0xFF                   EOF
}

void HeltecBalancerBle::decode_device_info_(const std::vector<uint8_t> &data) {
  auto heltec_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto heltec_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(heltec_get_16bit(i + 2)) << 16) | (uint32_t(heltec_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Device info frame (%d bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(data.data(), data.size()).c_str());

  // Device info frame (100 bytes)
  // 0x55 0xAA 0x11 0x01 0x01 0x00 0x64 0x00 0x47 0x57 0x2D 0x32 0x34 0x53 0x34 0x45 0x42 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x48 0x57 0x2D 0x32 0x2E 0x38 0x2E 0x30 0x5A 0x48 0x2D 0x31 0x2E 0x32 0x2E 0x33
  // 0x56 0x31 0x2E 0x30 0x2E 0x30 0x00 0x00 0x32 0x30 0x32 0x32 0x30 0x35 0x33 0x31 0x05 0x00 0x00 0x00
  // 0x01 0x91 0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xAB 0xFF
  //
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     2   0x55 0xAA              Header
  // 2     1   0x11                   Device address
  // 3     1   0x01                   Function (read)
  // 4     2   0x01 0x00              Command (device info)
  // 6     2   0x64 0x00              Length (100 bytes)
  // 8    16   0x47 0x57 0x2D 0x32 0x34 0x53 0x34 0x45 0x42 0x00 0x00 0x00 0x00 0x00 0x00 0x00    Model    GW-24S4EB
  ESP_LOGI(TAG, "  Model: %s", std::string(data.begin() + 8, data.begin() + 8 + 16).c_str());
  // 24    8   0x48 0x57 0x2D 0x32 0x2E 0x38 0x2E 0x30    Hardware version           HW-2.8.0
  ESP_LOGI(TAG, "  Hardware version: %s", std::string(data.begin() + 24, data.begin() + 24 + 8).c_str());
  // 32    8   0x5A 0x48 0x2D 0x31 0x2E 0x32 0x2E 0x33    Software version           ZH-1.2.3
  ESP_LOGI(TAG, "  Software version: %s", std::string(data.begin() + 32, data.begin() + 32 + 8).c_str());
  // 40    8   0x56 0x31 0x2E 0x30 0x2E 0x30 0x00 0x00    Protocol version           V1.0.0
  ESP_LOGI(TAG, "  Protocol version: %s", std::string(data.begin() + 40, data.begin() + 40 + 8).c_str());
  // 48    8   0x32 0x30 0x32 0x32 0x30 0x35 0x33 0x31    Production date            20220531
  ESP_LOGI(TAG, "  Manufacturing date: %s", std::string(data.begin() + 48, data.begin() + 48 + 8).c_str());
  // 56    4   0x05 0x00 0x00 0x00    Power on count                                 5
  ESP_LOGI(TAG, "  Power on count: %d", heltec_get_16bit(56));
  // 60    4   0x01 0x91 0x0A 0x00    Total runtime                                  7D
  ESP_LOGI(TAG, "  Total runtime: %s (%lus)", format_total_runtime_(heltec_get_32bit(60)).c_str(),
           (unsigned long) heltec_get_32bit(60));
  this->publish_state_(this->total_runtime_sensor_, (float) heltec_get_32bit(60));
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(heltec_get_32bit(60)));

  //                                  (0x0A9101 = 692481 / 3600 = 192.35h = 8.01d)
  // 64   34   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 98    1   0xAB                    CRC
  // 99    1   0xFF                    EOF
}

bool HeltecBalancerBle::send_command(uint8_t function, uint8_t command, uint8_t register_address, uint32_t value) {
  // Request device info:
  //
  // (GW-24S4EB, checksum_xor)
  // 0xAA 0x55 0x11 0x01 0x01 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFA 0xFF
  // 0xAA 0x55 0x11 0x01 0x01 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x26 0xFF
  // (EK-24S4EB, crc)
  //
  // Request cell info:
  //
  // (GW-24S4EB, checksum_xor, wrong data_len position)
  // 0xAA 0x55 0x11 0x01 0x02 0x00 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xF9 0xFF
  // 0xAA 0x55 0x11 0x01 0x02 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x27 0xFF
  // (EK-24S4EB, crc)
  //
  // Request factory settings:
  //
  // (GW-24S4EB, checksum_xor)
  // 0xAA 0x55 0x11 0x01 0x03 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xF8 0xFF
  //
  // Request settings:
  //
  // (GW-24S4EB, checksum_xor)
  // 0xAA 0x55 0x11 0x01 0x04 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFF 0xFF
  // 0xAA 0x55 0x11 0x01 0x04 0x00 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x29 0xFF
  // (EK-24S4EB, crc)
  //
  // Enable balancer:
  //
  // (GW-24S4EB, checksum_xor)
  // 0xAA 0x55 0x11 0x00 0x05 0x0D 0x14 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xF3 0xFF
  //
  // Disable balancer:
  //
  // (GW-24S4EB, checksum_xor)
  // 0xAA 0x55 0x11 0x00 0x05 0x0D 0x14 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xF2 0xFF
  uint16_t length = 0x0014;

  uint8_t frame[20];
  frame[0] = SOF_REQUEST_BYTE1;  // Start sequence
  frame[1] = SOF_REQUEST_BYTE2;  // Start sequence
  frame[2] = DEVICE_ADDRESS;     // Device address
  frame[3] = function;           // Function (read or write)
  frame[4] = command >> 0;       // Command
  frame[5] = register_address;   // Register address
  frame[6] = length >> 0;        // Data length
  frame[7] = length >> 8;        // Data length
  frame[8] = value >> 0;         // Data Byte 1
  frame[9] = value >> 8;         // Data Byte 2
  frame[10] = value >> 16;       // Data Byte 3
  frame[11] = value >> 24;       // Data Byte 4
  frame[12] = 0x00;              // Data Byte 5
  frame[13] = 0x00;              // Data Byte 6
  frame[14] = 0x00;              // Data Byte 7
  frame[15] = 0x00;              // Data Byte 8
  frame[16] = 0x00;              // Data Byte 9
  frame[17] = 0x00;              // Data Byte 10
  frame[18] = crc(frame, sizeof(frame) - 2);
  frame[19] = END_OF_FRAME;  // End sequence

  ESP_LOGD(TAG, "Write register: %s", format_hex_pretty(frame, sizeof(frame)).c_str());
  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status)
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);

  return (status == 0);
}

void HeltecBalancerBle::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void HeltecBalancerBle::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void HeltecBalancerBle::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->operation_status_text_sensor_, "Offline");

  this->publish_state_(min_cell_voltage_sensor_, NAN);
  this->publish_state_(max_cell_voltage_sensor_, NAN);
  this->publish_state_(min_voltage_cell_sensor_, NAN);
  this->publish_state_(max_voltage_cell_sensor_, NAN);
  this->publish_state_(delta_cell_voltage_sensor_, NAN);
  this->publish_state_(average_cell_voltage_sensor_, NAN);
  this->publish_state_(total_voltage_sensor_, NAN);
  this->publish_state_(temperature_sensor_1_sensor_, NAN);
  this->publish_state_(temperature_sensor_2_sensor_, NAN);
  this->publish_state_(total_runtime_sensor_, NAN);
  this->publish_state_(balancing_current_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(cell_detection_failed_bitmask_sensor_, NAN);
  this->publish_state_(cell_overvoltage_bitmask_sensor_, NAN);
  this->publish_state_(cell_undervoltage_bitmask_sensor_, NAN);
  this->publish_state_(cell_polarity_error_bitmask_sensor_, NAN);
  this->publish_state_(cell_excessive_line_resistance_bitmask_sensor_, NAN);

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
  }
}

void HeltecBalancerBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void HeltecBalancerBle::publish_state_(number::Number *number, float value) {
  if (number == nullptr)
    return;

  number->publish_state(value);
}

void HeltecBalancerBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void HeltecBalancerBle::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void HeltecBalancerBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

}  // namespace heltec_balancer_ble
}  // namespace esphome

#endif
