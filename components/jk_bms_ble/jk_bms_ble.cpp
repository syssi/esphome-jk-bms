#include "jk_bms_ble.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble";

static const uint8_t MAX_NO_RESPONSE_COUNT = 10;

static const uint8_t FRAME_VERSION_JK04 = 0x01;
static const uint8_t FRAME_VERSION_JK02_24S = 0x02;
static const uint8_t FRAME_VERSION_JK02_32S = 0x03;

static const uint16_t JK_BMS_SERVICE_UUID = 0xFFE0;
static const uint16_t JK_BMS_CHARACTERISTIC_UUID = 0xFFE1;

static const uint8_t COMMAND_CELL_INFO = 0x96;
static const uint8_t COMMAND_DEVICE_INFO = 0x97;

static const uint16_t MIN_RESPONSE_SIZE = 300;
static const uint16_t MAX_RESPONSE_SIZE = 320;

static const uint8_t ERRORS_SIZE = 16;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Charge Overtemperature",               // 0000 0000 0000 0001
    "Charge Undertemperature",              // 0000 0000 0000 0010
    "Coprocessor communication error",      // 0000 0000 0000 0100
    "Cell Undervoltage",                    // 0000 0000 0000 1000
    "Battery pack undervoltage",            // 0000 0000 0001 0000
    "Discharge overcurrent",                // 0000 0000 0010 0000
    "Discharge short circuit",              // 0000 0000 0100 0000
    "Discharge overtemperature",            // 0000 0000 1000 0000
    "Wire resistance",                      // 0000 0001 0000 0000
    "Mosfet overtemperature",               // 0000 0010 0000 0000
    "Cell count is not equal to settings",  // 0000 0100 0000 0000
    "Current sensor anomaly",               // 0000 1000 0000 0000
    "Cell Overvoltage",                     // 0001 0000 0000 0000
    "Battery pack overvoltage",             // 0010 0000 0000 0000
    "Charge overcurrent protection",        // 0100 0000 0000 0000
    "Charge short circuit",                 // 1000 0000 0000 0000
};

uint8_t crc(const uint8_t data[], const uint16_t len) {
  uint8_t crc = 0;
  for (uint16_t i = 0; i < len; i++) {
    crc = crc + data[i];
  }
  return crc;
}

void JkBmsBle::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "JkBmsBle");
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
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging Power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Power Tube Temperature", this->power_tube_temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperatures_[0].temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperatures_[1].temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 3", this->temperatures_[2].temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 4", this->temperatures_[3].temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 5", this->temperatures_[4].temperature_sensor_);
  LOG_SENSOR("", "Balacing", this->balancing_sensor_);
  LOG_SENSOR("", "State Of Charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Capacity Remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Total Battery Capacity Setting", this->total_battery_capacity_setting_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Total Charging Cycle Capacity", this->total_charging_cycle_capacity_sensor_);
  LOG_SENSOR("", "Total Runtime", this->total_runtime_sensor_);
  LOG_SENSOR("", "Heating Current", this->heating_current_sensor_);
  LOG_TEXT_SENSOR("", "Operation Status", this->operation_status_text_sensor_);
  LOG_TEXT_SENSOR("", "Total Runtime Formatted", this->total_runtime_formatted_text_sensor_);
  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Precharging", this->precharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Heating", this->heating_binary_sensor_);
}

void JkBmsBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
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
      this->char_handle_ = 0;

      this->frame_buffer_.clear();

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *chr = this->parent_->get_characteristic(JK_BMS_SERVICE_UUID, JK_BMS_CHARACTERISTIC_UUID);
      if (chr == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an JK BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }

      // Services and characteristic of the old BLE module (C8:47:8C:XX:XX:XX)
      //
      // Found device at MAC address [C8:47:8C:F2:7E:03]
      // Attempting BLE connection to c8:47:8c:f2:7e:03
      // Service UUID: 0x1800
      //   start_handle: 0x1  end_handle: 0x9
      //  characteristic 0x2A00, handle 0x3, properties 0xa
      //  characteristic 0x2A01, handle 0x5, properties 0xa
      //  characteristic 0x2A02, handle 0x7, properties 0x2
      //  characteristic 0x2A04, handle 0x9, properties 0x2
      // Service UUID: 0x1801
      //   start_handle: 0xa  end_handle: 0xd
      //  characteristic 0x2A05, handle 0xc, properties 0x22
      // Service UUID: 0xFFE0
      //   start_handle: 0xe  end_handle: 0x13
      //  characteristic 0xFFE2, handle 0x10, properties 0x4
      //  characteristic 0xFFE1, handle 0x12, properties 0x1c
      // Service UUID: 0x180A
      //   start_handle: 0x14  end_handle: 0x26
      //  characteristic 0x2A29, handle 0x16, properties 0x2
      //  characteristic 0x2A24, handle 0x18, properties 0x2
      //  characteristic 0x2A25, handle 0x1a, properties 0x2
      //  characteristic 0x2A27, handle 0x1c, properties 0x2
      //  characteristic 0x2A26, handle 0x1e, properties 0x2
      //  characteristic 0x2A28, handle 0x20, properties 0x2
      //  characteristic 0x2A23, handle 0x22, properties 0x2
      //  characteristic 0x2A2A, handle 0x24, properties 0x2
      //  characteristic 0x2A50, handle 0x26, properties 0x2
      // Service UUID: 0x180F
      //   start_handle: 0x27  end_handle: 0x2a
      //  characteristic 0x2A19, handle 0x29, properties 0x12
      // Service UUID: F000FFC0-0451-4000-B000-000000000000
      //   start_handle: 0x2b  end_handle: 0x33
      //  characteristic F000FFC1-0451-4000-B000-000000000000, handle 0x2d, properties 0x1c
      //  characteristic F000FFC2-0451-4000-B000-000000000000, handle 0x31, properties 0x1c

      // Services and characteristic of the new BLE module (20:21:11:XX:XX:XX)
      //
      // Found device at MAC address [20:21:11:28:18:DD]
      // Attempting BLE connection to 20:21:11:28:18:dd
      // Service UUID: 0xFFE0
      //   start_handle: 0x1  end_handle: 0xffff
      //  characteristic 0xFFE1, handle 0x3, properties 0xc
      //  characteristic 0xFFE1, handle 0x5, properties 0x12
      //    descriptor 0x2902, handle 0x6
      this->char_handle_ = chr->handle;
      this->notify_handle_ = (chr->handle == 0x03) ? 0x05 : chr->handle;

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

      ESP_LOGI(TAG, "Request device info");
      this->write_register(COMMAND_DEVICE_INFO, 0x00000000, 0x00);

      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      if (param->notify.handle != this->notify_handle_)
        break;

      ESP_LOGVV(TAG, "Notification received: %s",
                format_hex_pretty(param->notify.value, param->notify.value_len).c_str());

      this->assemble(param->notify.value, param->notify.value_len);

      break;
    }
    default:
      break;
  }
}

void JkBmsBle::update() {
  this->track_online_status_();
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  if (!this->status_notification_received_) {
    ESP_LOGI(TAG, "Request status notification");
    this->write_register(COMMAND_CELL_INFO, 0x00000000, 0x00);
  }
}

// TODO: There is no need to assemble frames if the MTU can be increased to > 320 bytes
void JkBmsBle::assemble(const uint8_t *data, uint16_t length) {
  if (this->frame_buffer_.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Frame dropped because of invalid length");
    this->frame_buffer_.clear();
  }

  // Flush buffer on every preamble
  if (data[0] == 0x55 && data[1] == 0xAA && data[2] == 0xEB && data[3] == 0x90) {
    this->frame_buffer_.clear();
  }

  this->frame_buffer_.insert(this->frame_buffer_.end(), data, data + length);

  if (this->frame_buffer_.size() >= MIN_RESPONSE_SIZE) {
    const uint8_t *raw = &this->frame_buffer_[0];
    // Even if the frame is 320 bytes long the CRC is at position 300 in front of 0xAA 0x55 0x90 0xEB
    const uint16_t frame_size = 300;  // this->frame_buffer_.size();

    uint8_t computed_crc = crc(raw, frame_size - 1);
    uint8_t remote_crc = raw[frame_size - 1];
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

void JkBmsBle::decode_(const std::vector<uint8_t> &data) {
  this->reset_online_status_tracker_();

  uint8_t frame_type = data[4];
  switch (frame_type) {
    case 0x01:
      if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
        this->decode_jk04_settings_(data);
      } else {
        this->decode_jk02_settings_(data);
      }
      break;
    case 0x02:
      if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
        this->decode_jk04_cell_info_(data);
      } else {
        this->decode_jk02_cell_info_(data);
      }
      break;
    case 0x03:
      this->decode_device_info_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unsupported message type (0x%02X)", data[4]);
  }
}

void JkBmsBle::decode_jk02_cell_info_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  const uint32_t now = millis();
  if (now - this->last_cell_info_ < this->throttle_) {
    return;
  }
  this->last_cell_info_ = now;

  uint8_t frame_version = FRAME_VERSION_JK02_24S;
  uint8_t offset = 0;
  if (this->protocol_version_ == PROTOCOL_VERSION_JK02_32S) {
    frame_version = FRAME_VERSION_JK02_32S;
    offset = 16;
  }

  ESP_LOGI(TAG, "Cell info frame (version %d, %d bytes) received", frame_version, data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 150).c_str());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 150, data.size() - 150).c_str());

  // 6 example responses (128+128+44 = 300 bytes per frame)
  //
  //
  // 55.AA.EB.90.02.8C.FF.0C.01.0D.01.0D.FF.0C.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.03.D0.00.00.00.00.00.00.00.00
  // 00.00.BE.00.BF.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CA.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.EC.E6.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.CD
  //
  // 55.AA.EB.90.02.8D.FF.0C.01.0D.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.FF.0C.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.04.D0.00.00.00.00.00.00.00.00
  // 00.00.BE.00.BF.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CA.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.F0.E6.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.D3
  //
  // 55.AA.EB.90.02.8E.FF.0C.01.0D.01.0D.FF.0C.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.FF.0C.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.04.D0.00.00.00.00.00.00.00.00
  // 00.00.BE.00.BF.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CA.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.F5.E6.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.D6
  //
  // 55.AA.EB.90.02.91.FF.0C.FF.0C.01.0D.FF.0C.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.01.D0.00.00.00.00.00.00.00.00
  // 00.00.BF.00.C0.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CC.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.01.E7.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.E7
  //
  // 55.AA.EB.90.02.92.01.0D.01.0D.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.03.D0.00.00.00.00.00.00.00.00
  // 00.00.BF.00.C0.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CC.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.06.E7.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.F8
  //
  // 55.AA.EB.90.02.93.FF.0C.01.0D.01.0D.01.0D.01.0D.01.0D.FF.0C.01.0D.01.0D.01.0D.FF.0C.FF.0C.01.0D.01.0D.01.0D.01.0D.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.FF.FF.00.00.00.0D.00.00.00.00.9D.01.96.01.8C.01.87.01.84.01.84.01.83.01.84.01.85.01.81.01.83.01.86.01.82.01.82.01.83.01.85.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.04.D0.00.00.00.00.00.00.00.00
  // 00.00.BE.00.C0.00.D2.00.00.00.00.00.00.54.8E.0B.01.00.68.3C.01.00.00.00.00.00.3D.04.00.00.64.00.79.04.CD.03.10.00.01.01.AA.06.00.00.00.00.00.00.00.00.00.00.00.00.07.00.01.00.00.00.D5.02.00.00.00.00.AE.D6.3B.40.00.00.00.00.58.AA.FD.FF.00.00.00.01.00.02.00.00.0A.E7.4F.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
  // 00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.F8
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     2   0x55 0xAA 0xEB 0x90    Header
  // 4     1   0x02                   Record type
  // 5     1   0x8C                   Frame counter
  // 6     2   0xFF 0x0C              Voltage cell 01       0.001        V
  // 8     2   0x01 0x0D              Voltage cell 02       0.001        V
  // 10    2   0x01 0x0D              Voltage cell 03       0.001        V
  // ...
  uint8_t cells = 24 + (offset / 2);
  uint8_t cells_enabled = 0;
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_get_16bit(i * 2 + 6) * 0.001f;
    float cell_resistance = (float) jk_get_16bit(i * 2 + 64 + offset) * 0.001f;
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
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  // 54    4   0xFF 0xFF 0x00 0x00    Enabled cells bitmask
  //           0x0F 0x00 0x00 0x00    4 cells enabled
  //           0xFF 0x00 0x00 0x00    8 cells enabled
  //           0xFF 0x0F 0x00 0x00    12 cells enabled
  //           0xFF 0x1F 0x00 0x00    13 cells enabled
  //           0xFF 0xFF 0x00 0x00    16 cells enabled
  //           0xFF 0xFF 0xFF 0x00    24 cells enabled
  //           0xFF 0xFF 0xFF 0xFF    32 cells enabled
  ESP_LOGV(TAG, "Enabled cells bitmask: 0x%02X 0x%02X 0x%02X 0x%02X", data[54 + offset], data[55 + offset],
           data[56 + offset], data[57 + offset]);

  // 58    2   0x00 0x0D              Average Cell Voltage  0.001        V
  // this->publish_state_(this->average_cell_voltage_sensor_, (float) jk_get_16bit(58 + offset) * 0.001f);

  // 60    2   0x00 0x00              Delta Cell Voltage    0.001        V
  // this->publish_state_(this->delta_cell_voltage_sensor_, (float) jk_get_16bit(60 + offset) * 0.001f);

  // 62    1   0x00                   Max voltage cell      1
  // this->publish_state_(this->max_voltage_cell_sensor_, (float) data[62 + offset] + 1);

  // 63    1   0x00                   Min voltage cell      1
  // this->publish_state_(this->min_voltage_cell_sensor_, (float) data[63 + offset] + 1);

  // 64    2   0x9D 0x01              Resistance Cell 01    0.001        Ohm
  // 66    2   0x96 0x01              Resistance Cell 02    0.001        Ohm
  // 68    2   0x8C 0x01              Resistance Cell 03    0.001        Ohm
  // ...
  // 110   2   0x00 0x00              Resistance Cell 24    0.001        Ohm

  offset = offset * 2;

  // 112   2   0x00 0x00              Unknown112
  if (frame_version == FRAME_VERSION_JK02_32S) {
    this->publish_state_(this->power_tube_temperature_sensor_, (float) ((int16_t) jk_get_16bit(112 + offset)) * 0.1f);
  } else {
    ESP_LOGD(TAG, "Unknown112: 0x%02X 0x%02X", data[112 + offset], data[113 + offset]);
  }

  // 114   4   0x00 0x00 0x00 0x00    Wire resistance warning bitmask (each bit indicates a warning per cell / wire)
  ESP_LOGD(TAG, "Wire resistance warning bitmask: 0x%02X 0x%02X 0x%02X 0x%02X", data[114 + offset], data[115 + offset],
           data[116 + offset], data[117 + offset]);

  // 118   4   0x03 0xD0 0x00 0x00    Battery voltage       0.001        V
  float total_voltage = (float) jk_get_32bit(118 + offset) * 0.001f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 122   4   0x00 0x00 0x00 0x00    Battery power         0.001        W
  // 126   4   0x00 0x00 0x00 0x00    Charge current        0.001        A
  float current = (float) ((int32_t) jk_get_32bit(126 + offset)) * 0.001f;
  this->publish_state_(this->current_sensor_, (float) ((int32_t) jk_get_32bit(126 + offset)) * 0.001f);

  // Don't use byte 122 because it's unsigned
  // float power = (float) ((int32_t) jk_get_32bit(122 + offset)) * 0.001f;
  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 130   2   0xBE 0x00              Temperature Sensor 1  0.1          °C
  this->publish_state_(this->temperatures_[0].temperature_sensor_,
                       (float) ((int16_t) jk_get_16bit(130 + offset)) * 0.1f);

  // 132   2   0xBF 0x00              Temperature Sensor 2  0.1          °C
  this->publish_state_(this->temperatures_[1].temperature_sensor_,
                       (float) ((int16_t) jk_get_16bit(132 + offset)) * 0.1f);

  // 134   2   0xD2 0x00              MOS Temperature       0.1          °C
  if (frame_version == FRAME_VERSION_JK02_32S) {
    uint16_t raw_errors_bitmask = (uint16_t(data[134 + offset]) << 8) | (uint16_t(data[134 + 1 + offset]) << 0);
    this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
    this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));
  } else {
    this->publish_state_(this->power_tube_temperature_sensor_, (float) ((int16_t) jk_get_16bit(134 + offset)) * 0.1f);
  }

  // 136   2   0x00 0x00              System alarms
  //           0x00 0x01                Charge overtemperature               0000 0000 0000 0001
  //           0x00 0x02                Charge undertemperature              0000 0000 0000 0010
  //           0x00 0x04                Coprocessor communication error      0000 0000 0000 0100
  //           0x00 0x08                Cell undervoltage                    0000 0000 0000 1000
  //           0x00 0x10                Battery pack undervoltage            0000 0000 0001 0000
  //           0x00 0x20                Discharge overcurrent                0000 0000 0010 0000
  //           0x00 0x40                Discharge short circuit              0000 0000 0100 0000
  //           0x00 0x80                Discharge overtemperature            0000 0000 1000 0000
  //           0x01 0x00                Wire resistance                      0000 0001 0000 0000
  //           0x02 0x00                Mosfet overtemperature               0000 0010 0000 0000
  //           0x04 0x00                Cell count is not equal to settings  0000 0100 0000 0000
  //           0x08 0x00                Current sensor anomaly               0000 1000 0000 0000
  //           0x10 0x00                Cell Overvoltage                     0001 0000 0000 0000
  //           0x20 0x00                Battery pack overvoltage             0010 0000 0000 0000
  //           0x40 0x00                Charge overcurrent protection        0100 0000 0000 0000
  //           0x80 0x00                Charge short circuit                 1000 0000 0000 0000
  //
  //           0x14 0x00                Cell Over Voltage +                  0001 0100 0000 0000
  //                                    Cell count is not equal to settings
  //           0x04 0x08                Cell Undervoltage +                  0000 0100 0000 1000
  //                                    Cell count is not equal to settings
  if (frame_version != FRAME_VERSION_JK02_32S) {
    uint16_t raw_errors_bitmask = (uint16_t(data[136 + offset]) << 8) | (uint16_t(data[136 + 1 + offset]) << 0);
    this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
    this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));
  }

  // 138   2   0x00 0x00              Balance current      0.001         A
  this->publish_state_(this->balancing_current_sensor_, (float) ((int16_t) jk_get_16bit(138 + offset)) * 0.001f);

  // 140   1   0x00                   Balancing action                   0x00: Off
  //                                                                     0x01: Charging balancer
  //                                                                     0x02: Discharging balancer
  this->publish_state_(this->balancing_sensor_, (data[140 + offset]));
  this->publish_state_(this->balancing_binary_sensor_, (data[140 + offset] != 0x00));
  ESP_LOGD(TAG, "Balancing indicator (legacy): %s", YESNO(data[140 + offset] != 0x00));

  // 141   1   0x54                   State of charge in   1.0           %
  this->publish_state_(this->state_of_charge_sensor_, (float) data[141 + offset]);

  // 142   4   0x8E 0x0B 0x01 0x00    Capacity_Remain      0.001         Ah
  this->publish_state_(this->capacity_remaining_sensor_, (float) jk_get_32bit(142 + offset) * 0.001f);

  // 146   4   0x68 0x3C 0x01 0x00    Nominal_Capacity     0.001         Ah
  this->publish_state_(this->total_battery_capacity_setting_sensor_, (float) jk_get_32bit(146 + offset) * 0.001f);

  // 150   4   0x00 0x00 0x00 0x00    Cycle_Count          1.0
  this->publish_state_(this->charging_cycles_sensor_, (float) jk_get_32bit(150 + offset));

  // 154   4   0x3D 0x04 0x00 0x00    Cycle_Capacity       0.001         Ah
  this->publish_state_(this->total_charging_cycle_capacity_sensor_, (float) jk_get_32bit(154 + offset) * 0.001f);

  // 158   1   0x64                   SOH                  1.0           %
  ESP_LOGD(TAG, "State of health: %d %%", data[158 + offset]);

  // 159   1   0x00                   Precharge
  ESP_LOGD(TAG, "Precharge: %s", ONOFF(data[159 + offset]));

  // 160   2   0x79 0x04              User alarm
  ESP_LOGD(TAG, "User alarm: 0x%02X 0x%02X (always 0xC5 0x09?)", data[160 + offset], data[161 + offset]);

  // 162   4   0xCA 0x03 0x10 0x00    Total runtime in seconds           s
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_32bit(162 + offset));
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(jk_get_32bit(162 + offset)));

  // 166   1   0x01                   Charging mosfet enabled                      0x00: off, 0x01: on
  this->publish_state_(this->charging_binary_sensor_, (bool) data[166 + offset]);

  // 167   1   0x01                   Discharging mosfet enabled                   0x00: off, 0x01: on
  this->publish_state_(this->discharging_binary_sensor_, (bool) data[167 + offset]);

  // 168   1   0x01                   Precharging                                  0x00: off, 0x01: on
  this->publish_state_(this->precharging_binary_sensor_, (bool) data[168 + offset]);

  // 169   1   0x01                   Balancer working                             0x00: off, 0x01: on
  // this->publish_state_(this->balancing_binary_sensor_, (bool) data[169 + offset]);
  ESP_LOGD(TAG, "Balancing indicator (new): %s", YESNO((bool) data[169 + offset]));

  ESP_LOGD(TAG, "Discharge overcurrent protection release timer: %d", jk_get_16bit(170 + offset));
  ESP_LOGD(TAG, "Discharge short circuit protection release timer: %d", jk_get_16bit(172 + offset));
  ESP_LOGD(TAG, "Charge overcurrent protection release timer: %d", jk_get_16bit(174 + offset));
  ESP_LOGD(TAG, "Charge short circuit protection release timer: %d", jk_get_16bit(176 + offset));
  ESP_LOGD(TAG, "Undervoltage protection release timer: %d", jk_get_16bit(178 + offset));
  ESP_LOGD(TAG, "Overvoltage protection release timer: %d", jk_get_16bit(180 + offset));
  ESP_LOGD(TAG, "Temperature sensor absent bitmask: %d", jk_get_16bit(182 + offset));
  // bit0: Mosfet temperature sensor
  // bit1: Temperature sensor 1
  // bit2: Temperature sensor 2
  // bit3: Temperature sensor 3
  // bit4: Temperature sensor 4
  // bit5: Temperature sensor 5

  ESP_LOGD(TAG, "Heating: %s", ONOFF((bool) data[183 + offset]));
  this->publish_state_(this->heating_binary_sensor_, (bool) data[183 + offset]);

  ESP_LOGD(TAG, "Time emergency: %d s", jk_get_16bit(186 + offset));
  ESP_LOGD(TAG, "Discharge current correction factor: %d", jk_get_16bit(188 + offset));
  ESP_LOGD(TAG, "Charging current sensor voltage: %.3f", jk_get_16bit(190 + offset) * 0.001f);
  ESP_LOGD(TAG, "Discharging current sensor voltage: %.3f", jk_get_16bit(192 + offset) * 0.001f);
  ESP_LOGD(TAG, "Battery voltage correction factor: %f", (float) jk_get_32bit(194 + offset) * 1.0f);

  ESP_LOGD(TAG, "Battery voltage: %.3f", (float) ieee_float_(jk_get_32bit(202 + offset)));
  ESP_LOGD(TAG, "Heating current: %.3f A", (float) ((int16_t) jk_get_16bit(204 + offset)) * 0.001f);
  this->publish_state_(this->heating_current_sensor_, (float) ((int16_t) jk_get_16bit(204 + offset)) * 0.001f);

  ESP_LOGD(TAG, "Charger Plugged: %s", ONOFF((bool) data[213 + offset]));
  ESP_LOGD(TAG, "Temperature sensor 3: %.1f °C", (float) jk_get_16bit(222 + offset) * 0.1f);
  ESP_LOGD(TAG, "Temperature sensor 4: %.1f °C", (float) jk_get_16bit(224 + offset) * 0.1f);
  ESP_LOGD(TAG, "Temperature sensor 5: %.1f °C", (float) jk_get_16bit(226 + offset) * 0.1f);
  ESP_LOGD(TAG, "Time enter sleep: %lu s", (unsigned long) jk_get_32bit(238 + offset));
  ESP_LOGD(TAG, "PCL Module State: %s", ONOFF((bool) data[242 + offset]));

  if (frame_version == FRAME_VERSION_JK02_32S) {
    uint16_t raw_emergency_time_countdown = jk_get_16bit(186 + offset);
    ESP_LOGI(TAG, "Emergency switch: %s", ONOFF(raw_emergency_time_countdown > 0));
    this->publish_state_(this->emergency_switch_, raw_emergency_time_countdown > 0);
    this->publish_state_(this->emergency_time_countdown_sensor_, (float) raw_emergency_time_countdown * 1.0f);

    this->publish_state_(this->temperatures_[4].temperature_sensor_,
                         (float) ((int16_t) jk_get_16bit(222 + offset)) * 0.1f);
    this->publish_state_(this->temperatures_[3].temperature_sensor_,
                         (float) ((int16_t) jk_get_16bit(224 + offset)) * 0.1f);
    this->publish_state_(this->temperatures_[2].temperature_sensor_,
                         (float) ((int16_t) jk_get_16bit(226 + offset)) * 0.1f);
  }

  // 299   1   0xCD                   CRC

  this->status_notification_received_ = true;
}

void JkBmsBle::decode_jk04_cell_info_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  const uint32_t now = millis();
  if (now - this->last_cell_info_ < this->throttle_) {
    return;
  }
  this->last_cell_info_ = now;

  ESP_LOGI(TAG, "Cell info frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 150).c_str());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 150, data.size() - 150).c_str());

  // 0x55 0xAA 0xEB 0x90 0x02 0x4B 0xC0 0x61 0x56 0x40 0x1F 0xAA 0x56 0x40 0xFF 0x91 0x56 0x40 0xFF 0x91 0x56 0x40 0x1F
  // 0xAA 0x56 0x40 0xFF 0x91 0x56 0x40 0xFF 0x91 0x56 0x40 0xFF 0x91 0x56 0x40 0x1F 0xAA 0x56 0x40 0xFF 0x91 0x56 0x40
  // 0xFF 0x91 0x56 0x40 0xFF 0x91 0x56 0x40 0xFF 0x91 0x56 0x40 0x1F 0xAA 0x56 0x40 0xE0 0x79 0x56 0x40 0xE0 0x79 0x56
  // 0x40 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x7C 0x1D 0x23 0x3E 0x1B 0xEB 0x08 0x3E 0x56 0xCE 0x14 0x3E 0x4D
  // 0x9B 0x15 0x3E 0xE0 0xDB 0xCD 0x3D 0x72 0x33 0xCD 0x3D 0x94 0x88 0x01 0x3E 0x5E 0x1E 0xEA 0x3D 0xE5 0x17 0xCD 0x3D
  // 0xE3 0xBB 0xD7 0x3D 0xF5 0x44 0xD2 0x3D 0xBE 0x7C 0x01 0x3E 0x27 0xB6 0x00 0x3E 0xDA 0xB5 0xFC 0x3D 0x6B 0x51 0xF8
  // 0x3D 0xA2 0x93 0xF3 0x3D 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x03 0x95 0x56 0x40 0x00
  // 0xBE 0x90 0x3B 0x00 0x00 0x00 0x00 0xFF 0xFF 0x00 0x00 0x01 0x00 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x66 0xA0 0xD2 0x4A 0x40 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x01 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x53 0x96 0x1C 0x00 0x00 0x00 0x00 0x00 0x00 0x48 0x22 0x40 0x00
  // 0x13
  //
  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     4   0x55 0xAA 0xEB 0x90    Header
  // 4     1   0x02                   Frame type
  // 5     1   0x4B                   Frame counter
  // 6     4   0xC0 0x61 0x56 0x40    Cell voltage 1                     V
  // 10    4   0x1F 0xAA 0x56 0x40    Cell voltage 2                     V
  // 14    4   0xFF 0x91 0x56 0x40    Cell voltage 3                     V
  // 18    4   0xFF 0x91 0x56 0x40    Cell voltage 4                     V
  // 22    4   0x1F 0xAA 0x56 0x40    Cell voltage 5                     V
  // 26    4   0xFF 0x91 0x56 0x40    Cell voltage 6                     V
  // 30    4   0xFF 0x91 0x56 0x40    Cell voltage 7                     V
  // 34    4   0xFF 0x91 0x56 0x40    Cell voltage 8                     V
  // 38    4   0x1F 0xAA 0x56 0x40    Cell voltage 9                     V
  // 42    4   0xFF 0x91 0x56 0x40    Cell voltage 10                    V
  // 46    4   0xFF 0x91 0x56 0x40    Cell voltage 11                    V
  // 50    4   0xFF 0x91 0x56 0x40    Cell voltage 12                    V
  // 54    4   0xFF 0x91 0x56 0x40    Cell voltage 13                    V
  // 58    4   0x1F 0xAA 0x56 0x40    Cell voltage 14                    V
  // 62    4   0xE0 0x79 0x56 0x40    Cell voltage 15                    V
  // 66    4   0xE0 0x79 0x56 0x40    Cell voltage 16                    V
  // 70    4   0x00 0x00 0x00 0x00    Cell voltage 17                    V
  // 74    4   0x00 0x00 0x00 0x00    Cell voltage 18                    V
  // 78    4   0x00 0x00 0x00 0x00    Cell voltage 19                    V
  // 82    4   0x00 0x00 0x00 0x00    Cell voltage 20                    V
  // 86    4   0x00 0x00 0x00 0x00    Cell voltage 21                    V
  // 90    4   0x00 0x00 0x00 0x00    Cell voltage 22                    V
  // 94    4   0x00 0x00 0x00 0x00    Cell voltage 23                    V
  // 98    4   0x00 0x00 0x00 0x00    Cell voltage 24                    V
  // 102   4   0x7C 0x1D 0x23 0x3E    Cell resistance 1                  Ohm
  // 106   4   0x1B 0xEB 0x08 0x3E    Cell resistance 2                  Ohm
  // 110   4   0x56 0xCE 0x14 0x3E    Cell resistance 3                  Ohm
  // 114   4   0x4D 0x9B 0x15 0x3E    Cell resistance 4                  Ohm
  // 118   4   0xE0 0xDB 0xCD 0x3D    Cell resistance 5                  Ohm
  // 122   4   0x72 0x33 0xCD 0x3D    Cell resistance 6                  Ohm
  // 126   4   0x94 0x88 0x01 0x3E    Cell resistance 7                  Ohm
  // 130   4   0x5E 0x1E 0xEA 0x3D    Cell resistance 8                  Ohm
  // 134   4   0xE5 0x17 0xCD 0x3D    Cell resistance 9                  Ohm
  // 138   4   0xE3 0xBB 0xD7 0x3D    Cell resistance 10                 Ohm
  // 142   4   0xF5 0x44 0xD2 0x3D    Cell resistance 11                 Ohm
  // 146   4   0xBE 0x7C 0x01 0x3E    Cell resistance 12                 Ohm
  // 150   4   0x27 0xB6 0x00 0x3E    Cell resistance 13                 Ohm
  // 154   4   0xDA 0xB5 0xFC 0x3D    Cell resistance 14                 Ohm
  // 158   4   0x6B 0x51 0xF8 0x3D    Cell resistance 15                 Ohm
  // 162   4   0xA2 0x93 0xF3 0x3D    Cell resistance 16                 Ohm
  // 166   4   0x00 0x00 0x00 0x00    Cell resistance 17                 Ohm
  // 170   4   0x00 0x00 0x00 0x00    Cell resistance 18                 Ohm
  // 174   4   0x00 0x00 0x00 0x00    Cell resistance 19                 Ohm
  // 178   4   0x00 0x00 0x00 0x00    Cell resistance 20                 Ohm
  // 182   4   0x00 0x00 0x00 0x00    Cell resistance 21                 Ohm
  // 186   4   0x00 0x00 0x00 0x00    Cell resistance 22                 Ohm
  // 190   4   0x00 0x00 0x00 0x00    Cell resistance 23                 Ohm
  // 194   4   0x00 0x00 0x00 0x00    Cell resistance 24                 Ohm
  // 198   4   0x00 0x00 0x00 0x00    Cell resistance 25                 Ohm
  //                                  https://github.com/jblance/mpp-solar/issues/98#issuecomment-823701486
  uint8_t cells = 24;
  uint8_t cells_enabled = 0;
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  float total_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) ieee_float_(jk_get_32bit(i * 4 + 6));
    float cell_resistance = (float) ieee_float_(jk_get_32bit(i * 4 + 102));
    total_voltage = total_voltage + cell_voltage;
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
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 202   4   0x03 0x95 0x56 0x40    Average Cell Voltage               V
  // this->publish_state_(this->average_cell_voltage_sensor_, (float) ieee_float_(jk_get_32bit(202)));

  // 206   4   0x00 0xBE 0x90 0x3B    Delta Cell Voltage                 V
  // this->publish_state_(this->delta_cell_voltage_sensor_, (float) ieee_float_(jk_get_32bit(206)));

  // 210   4   0x00 0x00 0x00 0x00    Unknown210
  ESP_LOGD(TAG, "Unknown210: 0x%02X 0x%02X 0x%02X 0x%02X (always 0x00 0x00 0x00 0x00)", data[210], data[211], data[212],
           data[213]);

  // 214   4   0xFF 0xFF 0x00 0x00    Unknown214
  ESP_LOGD(TAG, "Unknown214: 0x%02X 0x%02X 0x%02X 0x%02X (0xFF 0xFF: 24 cells?)", data[214], data[215], data[216],
           data[217]);

  // 218   1   0x01                   Unknown218
  ESP_LOGD(TAG, "Unknown218: 0x%02X", data[218]);

  // 219   1   0x00                   Unknown219
  ESP_LOGD(TAG, "Unknown219: 0x%02X", data[219]);

  // 220   1   0x00                  Blink cells (0x00: Off, 0x01: Charging balancer, 0x02: Discharging balancer)
  bool balancing = (data[220] != 0x00);
  this->publish_state_(this->balancing_binary_sensor_, balancing);
  this->publish_state_(this->operation_status_text_sensor_, (balancing) ? "Balancing" : "Idle");

  // 221   1   0x01                  Unknown221
  ESP_LOGD(TAG, "Unknown221: 0x%02X", data[221]);

  // 222   4   0x00 0x00 0x00 0x00    Balancing current
  this->publish_state_(this->balancing_current_sensor_, (float) ieee_float_(jk_get_32bit(222)));

  // 226   7   0x00 0x00 0x00 0x00 0x00 0x00 0x00    Unknown226
  ESP_LOGD(TAG, "Unknown226: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X (always 0x00...0x00?)", data[226],
           data[227], data[228], data[229], data[230], data[231], data[232]);

  // 233   4   0x66 0xA0 0xD2 0x4A    Unknown233
  ESP_LOGD(TAG, "Unknown233: 0x%02X 0x%02X 0x%02X 0x%02X (%f)", data[233], data[234], data[235], data[236],
           ieee_float_(jk_get_32bit(233)));

  // 237   4   0x40 0x00 0x00 0x00    Unknown237
  ESP_LOGD(TAG, "Unknown237: 0x%02X 0x%02X 0x%02X 0x%02X (always 0x40 0x00 0x00 0x00?)", data[237], data[238],
           data[239], data[240]);

  // 241   45  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x01 0x01 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00
  // 286   3   0x53 0x96 0x1C 0x00        Uptime
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_32bit(286));
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(jk_get_32bit(286)));

  // 290   4   0x00 0x00 0x00 0x00    Unknown290
  ESP_LOGD(TAG, "Unknown290: 0x%02X 0x%02X 0x%02X 0x%02X (always 0x00 0x00 0x00 0x00?)", data[290], data[291],
           data[292], data[293]);

  // 294   4   0x00 0x48 0x22 0x40    Unknown294
  ESP_LOGD(TAG, "Unknown294: 0x%02X 0x%02X 0x%02X 0x%02X", data[294], data[295], data[296], data[297]);

  // 298   1   0x00                   Unknown298
  ESP_LOGD(TAG, "Unknown298: 0x%02X", data[298]);

  // 299   1   0x13                   Checksm

  status_notification_received_ = true;
}

void JkBmsBle::decode_jk02_settings_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Settings frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 160).c_str());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 160, data.size() - 160).c_str());

  // JK02_24S response example:
  //
  // 0x55 0xAA 0xEB 0x90 0x01 0x4F 0x58 0x02 0x00 0x00 0x54 0x0B 0x00 0x00 0x80 0x0C 0x00 0x00 0xCC 0x10 0x00 0x00 0x68
  // 0x10 0x00 0x00 0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0xF0 0x0A 0x00 0x00 0xA8 0x61 0x00 0x00 0x1E 0x00 0x00 0x00 0x3C 0x00 0x00 0x00 0xF0 0x49 0x02 0x00 0x2C 0x01 0x00
  // 0x00 0x3C 0x00 0x00 0x00 0x3C 0x00 0x00 0x00 0xD0 0x07 0x00 0x00 0xBC 0x02 0x00 0x00 0x58 0x02 0x00 0x00 0xBC 0x02
  // 0x00 0x00 0x58 0x02 0x00 0x00 0x38 0xFF 0xFF 0xFF 0x9C 0xFF 0xFF 0xFF 0x84 0x03 0x00 0x00 0xBC 0x02 0x00 0x00 0x0D
  // 0x00 0x00 0x00 0x01 0x00 0x00 0x00 0x01 0x00 0x00 0x00 0x01 0x00 0x00 0x00 0x88 0x13 0x00 0x00 0xDC 0x05 0x00 0x00
  // 0xE4 0x0C 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x40

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     4   0x55 0xAA 0xEB 0x90    Header
  // 4     1   0x01                   Frame type
  // 5     1   0x4F                   Frame counter

  // 6     4   0x58 0x02 0x00 0x00    ** [JK-PB2A16S-20P v14] Smart sleep voltage
  ESP_LOGV(TAG, "  Smart sleep voltage: %f", (float) jk_get_32bit(6) * 0.001f);
  this->publish_state_(this->smart_sleep_voltage_number_, (float) jk_get_32bit(6) * 0.001f);

  // 10    4   0x54 0x0B 0x00 0x00    Cell UVP
  ESP_LOGV(TAG, "  Cell UVP: %f V", (float) jk_get_32bit(10) * 0.001f);
  this->publish_state_(this->cell_voltage_undervoltage_protection_number_, (float) jk_get_32bit(10) * 0.001f);

  // 14    4   0x80 0x0C 0x00 0x00    Cell UVP recovery
  ESP_LOGV(TAG, "  Cell UVPR: %f V", (float) jk_get_32bit(14) * 0.001f);
  this->publish_state_(this->cell_voltage_undervoltage_recovery_number_, (float) jk_get_32bit(14) * 0.001f);

  // 18    4   0xCC 0x10 0x00 0x00    Cell OVP
  ESP_LOGV(TAG, "  Cell OVP: %f V", (float) jk_get_32bit(18) * 0.001f);
  this->publish_state_(this->cell_voltage_overvoltage_protection_number_, (float) jk_get_32bit(18) * 0.001f);

  // 22    4   0x68 0x10 0x00 0x00    Cell OVP recovery
  ESP_LOGV(TAG, "  Cell OVPR: %f V", (float) jk_get_32bit(22) * 0.001f);
  this->publish_state_(this->cell_voltage_overvoltage_recovery_number_, (float) jk_get_32bit(22) * 0.001f);

  // 26    4   0x0A 0x00 0x00 0x00    Balance trigger voltage
  ESP_LOGV(TAG, "  Balance trigger voltage: %f V", (float) jk_get_32bit(26) * 0.001f);
  this->publish_state_(this->balance_trigger_voltage_number_, (float) jk_get_32bit(26) * 0.001f);

  // 30    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC 100% voltage
  ESP_LOGV(TAG, "  SOC 100%% voltage: %f V", (float) jk_get_32bit(30) * 0.001f);
  this->publish_state_(this->cell_soc100_voltage_number_, (float) jk_get_32bit(30) * 0.001f);

  // 34    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC 0% voltage
  ESP_LOGV(TAG, "  SOC 0%% voltage: %f V", (float) jk_get_32bit(34) * 0.001f);
  this->publish_state_(this->cell_soc0_voltage_number_, (float) jk_get_32bit(34) * 0.001f);

  // 38    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] Voltage cell request charge voltage
  ESP_LOGV(TAG, "  Voltage cell request charge voltage [RCV]: %f V", (float) jk_get_32bit(38) * 0.001f);
  this->publish_state_(this->cell_request_charge_voltage_number_, (float) jk_get_32bit(38) * 0.001f);

  // 42    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] Voltage cell request float voltage
  ESP_LOGV(TAG, "  Voltage cell request float voltage [RFV]: %f V", (float) jk_get_32bit(42) * 0.001f);
  this->publish_state_(this->cell_request_float_voltage_number_, (float) jk_get_32bit(42) * 0.001f);

  // 46    4   0xF0 0x0A 0x00 0x00    Power off voltage
  ESP_LOGV(TAG, "  Power off voltage: %f V", (float) jk_get_32bit(46) * 0.001f);
  this->publish_state_(this->power_off_voltage_number_, (float) jk_get_32bit(46) * 0.001f);

  // 50    4   0xA8 0x61 0x00 0x00    Max. charge current
  ESP_LOGV(TAG, "  Max. charge current: %f A", (float) jk_get_32bit(50) * 0.001f);
  this->publish_state_(this->max_charge_current_number_, (float) jk_get_32bit(50) * 0.001f);

  // 54    4   0x1E 0x00 0x00 0x00    Charge OCP delay
  ESP_LOGV(TAG, "  Charge OCP delay: %f s", (float) jk_get_32bit(54));
  this->publish_state_(this->charge_overcurrent_protection_delay_number_, (float) jk_get_32bit(54));

  // 58    4   0x3C 0x00 0x00 0x00    Charge OCP recovery time
  ESP_LOGV(TAG, "  Charge OCP recovery time: %f s", (float) jk_get_32bit(58));
  this->publish_state_(this->charge_overcurrent_protection_recovery_time_number_, (float) jk_get_32bit(58));

  // 62    4   0xF0 0x49 0x02 0x00    Max. discharge current
  ESP_LOGV(TAG, "  Max. discharge current: %f A", (float) jk_get_32bit(62) * 0.001f);
  this->publish_state_(this->max_discharge_current_number_, (float) jk_get_32bit(62) * 0.001f);

  // 66    4   0x2C 0x01 0x00 0x00    Discharge OCP delay
  ESP_LOGV(TAG, "  Discharge OCP delay: %f s", (float) jk_get_32bit(66));
  this->publish_state_(this->discharge_overcurrent_protection_delay_number_, (float) jk_get_32bit(66));

  // 70    4   0x3C 0x00 0x00 0x00    Discharge OCP recovery time
  ESP_LOGV(TAG, "  Discharge OCP recovery time: %f s", (float) jk_get_32bit(70));
  this->publish_state_(this->discharge_overcurrent_protection_recovery_time_number_, (float) jk_get_32bit(70));

  // 74    4   0x3C 0x00 0x00 0x00    SCPR time
  ESP_LOGV(TAG, "  Short circuit protection recovery time: %f s", (float) jk_get_32bit(74));
  this->publish_state_(this->short_circuit_protection_recovery_time_number_, (float) jk_get_32bit(74));

  // 78    4   0xD0 0x07 0x00 0x00    Max balance current
  ESP_LOGV(TAG, "  Max. balance current: %f A", (float) jk_get_32bit(78) * 0.001f);
  this->publish_state_(this->max_balance_current_number_, (float) jk_get_32bit(78) * 0.001f);

  // 82    4   0xBC 0x02 0x00 0x00    Charge OTP
  ESP_LOGV(TAG, "  Charge OTP: %f °C", (float) jk_get_32bit(82) * 0.1f);
  this->publish_state_(this->charge_overtemperature_protection_number_, (float) jk_get_32bit(82) * 0.1f);

  // 86    4   0x58 0x02 0x00 0x00    Charge OTP Recovery
  ESP_LOGV(TAG, "  Charge OTP recovery: %f °C", (float) jk_get_32bit(86) * 0.1f);
  this->publish_state_(this->charge_overtemperature_protection_recovery_number_, (float) jk_get_32bit(86) * 0.1f);

  // 90    4   0xBC 0x02 0x00 0x00    Discharge OTP
  ESP_LOGV(TAG, "  Discharge OTP: %f °C", (float) jk_get_32bit(90) * 0.1f);
  this->publish_state_(this->discharge_overtemperature_protection_number_, (float) jk_get_32bit(90) * 0.1f);

  // 94    4   0x58 0x02 0x00 0x00    Discharge OTP Recovery
  ESP_LOGV(TAG, "  Discharge OTP recovery: %f °C", (float) jk_get_32bit(94) * 0.1f);
  this->publish_state_(this->discharge_overtemperature_protection_recovery_number_, (float) jk_get_32bit(94) * 0.1f);

  // 98    4   0x38 0xFF 0xFF 0xFF    Charge UTP
  ESP_LOGV(TAG, "  Charge UTP: %f °C", (float) ((int32_t) jk_get_32bit(98)) * 0.1f);
  this->publish_state_(this->charge_undertemperature_protection_number_, (float) ((int32_t) jk_get_32bit(98)) * 0.1f);

  // 102   4   0x9C 0xFF 0xFF 0xFF    Charge UTP Recovery
  ESP_LOGV(TAG, "  Charge UTP recovery: %f °C", (float) ((int32_t) jk_get_32bit(102)) * 0.1f);
  this->publish_state_(this->charge_undertemperature_protection_recovery_number_,
                       (float) ((int32_t) jk_get_32bit(102)) * 0.1f);

  // 106   4   0x84 0x03 0x00 0x00    MOS OTP
  ESP_LOGV(TAG, "  Mosfet OTP: %f °C", (float) ((int32_t) jk_get_32bit(106)) * 0.1f);
  this->publish_state_(this->power_tube_overtemperature_protection_number_,
                       (float) ((int32_t) jk_get_32bit(106)) * 0.1f);

  // 110   4   0xBC 0x02 0x00 0x00    MOS OTP Recovery
  ESP_LOGV(TAG, "  Mosfet OTP recovery: %f °C", (float) ((int32_t) jk_get_32bit(110)) * 0.1f);
  this->publish_state_(this->power_tube_overtemperature_protection_recovery_number_,
                       (float) ((int32_t) jk_get_32bit(110)) * 0.1f);

  // 114   4   0x0D 0x00 0x00 0x00    Cell count
  ESP_LOGV(TAG, "  Cell count: %f", (float) jk_get_32bit(114));
  this->publish_state_(this->cell_count_number_, (float) data[114]);

  // 118   4   0x01 0x00 0x00 0x00    Charge switch
  ESP_LOGV(TAG, "  Charge switch: %s", ONOFF((bool) data[118]));
  this->publish_state_(this->charging_switch_, (bool) data[118]);

  // 122   4   0x01 0x00 0x00 0x00    Discharge switch
  ESP_LOGV(TAG, "  Discharge switch: %s", ONOFF((bool) data[122]));
  this->publish_state_(this->discharging_switch_, (bool) data[122]);

  // 126   4   0x01 0x00 0x00 0x00    Balancer switch
  ESP_LOGV(TAG, "  Balancer switch: %s", ONOFF((bool) data[126]));
  this->publish_state_(this->balancer_switch_, (bool) data[126]);

  // 130   4   0x88 0x13 0x00 0x00    Nominal battery capacity
  ESP_LOGV(TAG, "  Nominal battery capacity: %f Ah", (float) jk_get_32bit(130) * 0.001f);
  this->publish_state_(this->total_battery_capacity_number_, (float) jk_get_32bit(130) * 0.001f);

  // 134   4   0xDC 0x05 0x00 0x00    SCP delay
  ESP_LOGV(TAG, "  Short circuit protection delay: %f us", (float) jk_get_32bit(134) * 1.0f);
  this->publish_state_(this->short_circuit_protection_delay_number_, (float) jk_get_32bit(134) * 1.0f);

  // 138   4   0xE4 0x0C 0x00 0x00    Start balance voltage
  ESP_LOGV(TAG, "  Start balance voltage: %f V", (float) jk_get_32bit(138) * 0.001f);
  this->publish_state_(this->balance_starting_voltage_number_, (float) jk_get_32bit(138) * 0.001f);

  if (this->protocol_version_ == PROTOCOL_VERSION_JK02_24S) {
    ESP_LOGD(TAG, "  Unknown142: %f", (float) jk_get_32bit(142) * 0.001f);
    ESP_LOGD(TAG, "  Unknown146: %f", (float) jk_get_32bit(146) * 0.001f);
    ESP_LOGD(TAG, "  Unknown150: %f", (float) jk_get_32bit(150) * 0.001f);
    ESP_LOGD(TAG, "  Unknown154: %f", (float) jk_get_32bit(154) * 0.001f);
    // 142   4   0x00 0x00 0x00 0x00
    // 146   4   0x00 0x00 0x00 0x00
    // 150   4   0x00 0x00 0x00 0x00
    // 154   4   0x00 0x00 0x00 0x00
    // 158   4   0x00 0x00 0x00 0x00    Con. wire resistance 1
    // 162   4   0x00 0x00 0x00 0x00    Con. wire resistance 2
    // 166   4   0x00 0x00 0x00 0x00    Con. wire resistance 3
    // 170   4   0x00 0x00 0x00 0x00    Con. wire resistance 4
    // 174   4   0x00 0x00 0x00 0x00    Con. wire resistance 5
    // 178   4   0x00 0x00 0x00 0x00    Con. wire resistance 6
    // 182   4   0x00 0x00 0x00 0x00    Con. wire resistance 7
    // 186   4   0x00 0x00 0x00 0x00    Con. wire resistance 8
    // 190   4   0x00 0x00 0x00 0x00    Con. wire resistance 9
    // 194   4   0x00 0x00 0x00 0x00    Con. wire resistance 10
    // 198   4   0x00 0x00 0x00 0x00    Con. wire resistance 11
    // 202   4   0x00 0x00 0x00 0x00    Con. wire resistance 12
    // 206   4   0x00 0x00 0x00 0x00    Con. wire resistance 13
    // 210   4   0x00 0x00 0x00 0x00    Con. wire resistance 14
    // 214   4   0x00 0x00 0x00 0x00    Con. wire resistance 15
    // 218   4   0x00 0x00 0x00 0x00    Con. wire resistance 16
    // 222   4   0x00 0x00 0x00 0x00    Con. wire resistance 17
    // 226   4   0x00 0x00 0x00 0x00    Con. wire resistance 18
    // 230   4   0x00 0x00 0x00 0x00    Con. wire resistance 19
    // 234   4   0x00 0x00 0x00 0x00    Con. wire resistance 20
    // 238   4   0x00 0x00 0x00 0x00    Con. wire resistance 21
    // 242   4   0x00 0x00 0x00 0x00    Con. wire resistance 22
    // 246   4   0x00 0x00 0x00 0x00    Con. wire resistance 23
    // 250   4   0x00 0x00 0x00 0x00    Con. wire resistance 24
    for (uint8_t i = 0; i < 24; i++) {
      ESP_LOGI(TAG, "  Con. wire resistance %d: %f Ohm", i + 1, (float) jk_get_32bit(i * 4 + 158) * 0.001f);
    }

  } else {
    // 142   4   0x00 0x00 0x00 0x00    Con. wire resistance 1
    // 146   4   0x00 0x00 0x00 0x00    Con. wire resistance 2
    // 150   4   0x00 0x00 0x00 0x00    Con. wire resistance 3
    // 154   4   0x00 0x00 0x00 0x00    Con. wire resistance 4
    // ...   4   0x00 0x00 0x00 0x00    ...
    // 252   4   0x00 0x00 0x00 0x00    Con. wire resistance 29
    // 256   4   0x00 0x00 0x00 0x00    Con. wire resistance 30
    // 260   4   0x00 0x00 0x00 0x00    Con. wire resistance 31
    // 266   4   0x00 0x00 0x00 0x00    Con. wire resistance 32
    for (uint8_t i = 0; i < 32; i++) {
      ESP_LOGI(TAG, "  Con. wire resistance %d: %f Ohm", i + 1, (float) jk_get_32bit(i * 4 + 142) * 0.001f);
    }

    // 254   4   0x00 0x00 0x00 0x00
    // 258   4   0x00 0x00 0x00 0x00
    // 262   4   0x00 0x00 0x00 0x00
    // 266   4   0x00 0x00 0x00 0x00
    // 270   4   0x00 0x00 0x00 0x00
    ESP_LOGI(TAG, "  Device address: %d", data[270]);
    // 274   4   0x00 0x00 0x00 0x00
    ESP_LOGI(TAG, "  Precharge time: %d s", data[274]);
    // 278   4   0x00 0x00 0x00 0x00
    // 282   2   0x00 0x00                 New controls bitmask
    // ** [JK-PB2A16S-20P v14]
    //    bit0: Heating enabled                       1
    //    bit1: Disable temperature sensors           2
    //    bit2: GPS Heartbeat                         4
    //    bit3: Port switch (1: RS485, 0: CAN)        8
    //    bit4: Display always on                    16
    //    bit5: Special charger                      32
    //    bit6: Smart sleep                          64
    //    bit7: Disable PCL module                  128
    //    bit8: Timed stored data                   256
    //    bit9: Charging float mode                 512
    //    bit10: Reserved                          1024
    //    bit11: Reserved                          2048
    //    bit12: Reserved                          4096
    //    bit13: Reserved                          8192
    //    bit14: Reserved                         16384
    //    bit15: Reserved                         32768
    ESP_LOGI(TAG, "  Heating switch: %s", ONOFF(check_bit_(data[282], 1)));
    this->publish_state_(this->heating_switch_, check_bit_(data[282], 1));
    this->publish_state_(this->disable_temperature_sensors_switch_, check_bit_(data[282], 2));
    ESP_LOGI(TAG, "  GPS Heartbeat: %s", ONOFF(check_bit_(data[282], 4)));
    ESP_LOGI(TAG, "  Port switch: %s", check_bit_(data[282], 8) ? "RS485" : "CAN");
    this->publish_state_(this->display_always_on_switch_, check_bit_(data[282], 16));
    ESP_LOGI(TAG, "  Special charger: %s", ONOFF(check_bit_(data[282], 32)));
    this->publish_state_(this->smart_sleep_switch_, check_bit_(data[282], 64));
    this->publish_state_(this->disable_pcl_module_switch_, check_bit_(data[282], 128));
    this->publish_state_(this->timed_stored_data_switch_, check_bit_(data[283], 1));
    this->publish_state_(this->charging_float_mode_switch_, check_bit_(data[283], 2));

    // 284   2   0x00 0x00
    // 286   1   0x00
    ESP_LOGI(TAG, "  Smart sleep: %d h", data[286]);
    // 287   1   0x00
    ESP_LOGI(TAG, "  Data field enable control 0: %d", data[287]);
    // 288   2   0x00 0x00
    // 290   4   0x00 0x00 0x00 0x00
    // 294   4   0x00 0x00 0x00 0x00
    // 298   1   0x00
    // 299   1   0x40                   CRC
  }
}

void JkBmsBle::decode_jk04_settings_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Settings frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 160).c_str());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 160, data.size() - 160).c_str());

  // JK04 (JK-B2A16S v3) response example:
  //
  // 0x55 0xAA 0xEB 0x90 0x01 0x50 0x00 0x00 0x80 0x3F 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x10 0x00 0x00 0x00 0x00 0x00 0x40 0x40 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0xA3 0xFD 0x40 0x40 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x88 0x40 0x9A 0x99 0x59 0x40 0x0A 0xD7 0xA3 0x3B 0x00 0x00 0x00 0x40 0x01
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0xCE

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     4   0x55 0xAA 0xEB 0x90    Header
  // 4     1   0x01                   Frame type
  // 5     1   0x50                   Frame counter
  // 6     4   0x00 0x00 0x80 0x3F
  ESP_LOGD(TAG, "  Unknown6: 0x%02X 0x%02X 0x%02X 0x%02X (%f)", data[6], data[7], data[8], data[9],
           (float) ieee_float_(jk_get_32bit(6)));

  // 10    4   0x00 0x00 0x00 0x00
  // 14    4   0x00 0x00 0x00 0x00
  // 18    4   0x00 0x00 0x00 0x00
  // 22    4   0x00 0x00 0x00 0x00
  // 26    4   0x00 0x00 0x00 0x00
  // 30    4   0x00 0x00 0x00 0x00
  // 34    4   0x10 0x00 0x00 0x00    Cell count
  ESP_LOGI(TAG, "  Cell count: %d", data[34]);

  // 38    4   0x00 0x00 0x40 0x40    Power off voltage
  ESP_LOGI(TAG, "  Power off voltage: %f V", (float) ieee_float_(jk_get_32bit(38)));

  // 42    4   0x00 0x00 0x00 0x00
  // 46    4   0x00 0x00 0x00 0x00
  // 50    4   0x00 0x00 0x00 0x00
  // 54    4   0x00 0x00 0x00 0x00
  // 58    4   0x00 0x00 0x00 0x00
  // 62    4   0x00 0x00 0x00 0x00
  // 66    4   0x00 0x00 0x00 0x00
  // 70    4   0x00 0x00 0x00 0x00
  // 74    4   0xA3 0xFD 0x40 0x40
  ESP_LOGD(TAG, "  Unknown74: %f", (float) ieee_float_(jk_get_32bit(74)));

  // 78    4   0x00 0x00 0x00 0x00
  // 82    4   0x00 0x00 0x00 0x00
  // 86    4   0x00 0x00 0x00 0x00
  // 90    4   0x00 0x00 0x00 0x00
  // 94    4   0x00 0x00 0x00 0x00
  // 98    4   0x00 0x00 0x88 0x40    Start balance voltage
  ESP_LOGI(TAG, "  Start balance voltage: %f V", (float) ieee_float_(jk_get_32bit(98)));

  // 102   4   0x9A 0x99 0x59 0x40
  ESP_LOGD(TAG, "  Unknown102: %f", (float) ieee_float_(jk_get_32bit(102)));

  // 106   4   0x0A 0xD7 0xA3 0x3B    Trigger delta voltage
  ESP_LOGI(TAG, "  Trigger Delta Voltage: %f V", (float) ieee_float_(jk_get_32bit(106)));

  // 110   4   0x00 0x00 0x00 0x40    Max. balance current
  ESP_LOGI(TAG, "  Max. balance current: %f A", (float) ieee_float_(jk_get_32bit(110)));
  // 114   4   0x01 0x00 0x00 0x00    Balancer switch
  this->publish_state_(this->balancer_switch_, (bool) (data[114]));

  ESP_LOGI(TAG, "  ADC Vref: unknown V");  // 53.67 V?

  // 118  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 138  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 158  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 178  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 198  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 218  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 238  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 258  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 278  20   0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 298   2   0x00 0xCE
}

void JkBmsBle::decode_device_info_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Device info frame (%d bytes) received", data.size());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 160).c_str());
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 160, data.size() - 160).c_str());

  // JK04 (JK-B2A16S v3) response example:
  //
  // 0x55 0xAA 0xEB 0x90 0x03 0xE7 0x4A 0x4B 0x2D 0x42 0x32 0x41 0x31 0x36 0x53 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x33
  // 0x2E 0x30 0x00 0x00 0x00 0x00 0x00 0x33 0x2E 0x33 0x2E 0x30 0x00 0x00 0x00 0x10 0x8E 0x32 0x02 0x13 0x00 0x00 0x00
  // 0x42 0x4D 0x53 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x31 0x32 0x33 0x34 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0xA9
  //
  // Device info frame (300 bytes):
  //   Vendor ID: JK-B2A16S
  //   Hardware version: 3.0
  //   Software version: 3.3.0
  //   Uptime: 36867600 s
  //   Power on count: 19
  //   Device name: BMS
  //   Device passcode: 1234
  //   Manufacturing date:
  //   Serial number:
  //   Passcode:
  //   User data:
  //   Setup passcode:

  // JK02_24S response example:
  //
  // 0x55 0xAA 0xEB 0x90 0x03 0x9F 0x4A 0x4B 0x2D 0x42 0x32 0x41 0x32 0x34 0x53 0x31 0x35 0x50 0x00 0x00 0x00 0x00 0x31
  // 0x30 0x2E 0x58 0x57 0x00 0x00 0x00 0x31 0x30 0x2E 0x30 0x37 0x00 0x00 0x00 0x40 0xAF 0x01 0x00 0x06 0x00 0x00 0x00
  // 0x4A 0x4B 0x2D 0x42 0x32 0x41 0x32 0x34 0x53 0x31 0x35 0x50 0x00 0x00 0x00 0x00 0x31 0x32 0x33 0x34 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x32 0x32 0x30 0x34 0x30 0x37 0x00 0x00 0x32 0x30 0x32 0x31 0x36 0x30
  // 0x32 0x30 0x39 0x36 0x00 0x30 0x30 0x30 0x30 0x00 0x49 0x6E 0x70 0x75 0x74 0x20 0x55 0x73 0x65 0x72 0x64 0x61 0x74
  // 0x61 0x00 0x00 0x31 0x32 0x33 0x34 0x35 0x36 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x65

  // JK02_32S response example:
  //
  // 0x55 0xAA 0xEB 0x90 0x03 0xC9 0x4A 0x4B 0x5F 0x50 0x42 0x32 0x41 0x31 0x36 0x53 0x31 0x35 0x50 0x00 0x00 0x00 0x31
  // 0x34 0x2E 0x58 0x41 0x00 0x00 0x00 0x31 0x34 0x2E 0x32 0x30 0x00 0x00 0x00 0x54 0xE6 0x01 0x00 0x9C 0x00 0x00 0x00
  // 0x4A 0x4B 0x5F 0x50 0x42 0x32 0x41 0x31 0x36 0x53 0x31 0x35 0x50 0x00 0x00 0x00 0x31 0x32 0x33 0x34 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x32 0x33 0x31 0x31 0x31 0x38 0x00 0x00 0x33 0x30 0x39 0x32 0x35 0x37
  // 0x32 0x31 0x33 0x34 0x00 0x30 0x30 0x30 0x30 0x00 0x49 0x6E 0x70 0x75 0x74 0x20 0x55 0x73 0x65 0x72 0x64 0x61 0x74
  // 0x61 0x00 0x00 0x31 0x32 0x33 0x34 0x35 0x37 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x49 0x6E 0x70 0x75
  // 0x74 0x20 0x55 0x73 0x65 0x72 0x64 0x61 0x74 0x61 0x00 0x00 0xFE 0xFF 0xFF 0xFF 0xAF 0xE9 0x01 0x02 0x00 0x00 0x00
  // 0x00 0x90 0x1F 0x00 0x00 0x00 0x00 0xC0 0xD8 0xE7 0xFE 0x1F 0x00 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x01 0x04 0xCF 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xDF 0x07 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x01 0xCF 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x0B 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x09 0x00 0x00 0x00 0x0B 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x80 0x51 0x00 0x00 0x0A 0x50 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE 0x9F 0xE9 0xFE 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x1B

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     4   0x55 0xAA 0xEB 0x90    Header
  // 4     1   0x03                   Frame type
  // 5     1   0xC9                   Frame counter
  // 6    16   0x4A 0x4B 0x5F 0x50 0x42 0x32 0x41 0x31 0x36 0x53 0x31 0x35 0x50 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Vendor ID: %s", std::string(data.begin() + 6, data.begin() + 6 + 16).c_str());

  // 22    8   0x31 0x34 0x2E 0x58 0x41 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Hardware version: %s", std::string(data.begin() + 22, data.begin() + 22 + 8).c_str());

  // 30    8   0x31 0x34 0x2E 0x32 0x30 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Software version: %s", std::string(data.begin() + 30, data.begin() + 30 + 8).c_str());

  // 38    4   0x54 0xE6 0x01 0x00
  ESP_LOGI(TAG, "  Uptime: %lu s", (unsigned long) jk_get_32bit(38));

  // 42    4   0x9C 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Power on count: %lu", (unsigned long) jk_get_32bit(42));

  // 46   16   0x4A 0x4B 0x5F 0x50 0x42 0x32 0x41 0x31 0x36 0x53 0x31 0x35 0x50 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Device name: %s", std::string(data.begin() + 46, data.begin() + 46 + 16).c_str());

  // 62   16   0x31 0x32 0x33 0x34 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Device passcode: %s", std::string(data.begin() + 62, data.begin() + 62 + 16).c_str());

  // 78    8   0x32 0x33 0x31 0x31 0x31 0x38 0x00 0x00
  ESP_LOGI(TAG, "  Manufacturing date: %s", std::string(data.begin() + 78, data.begin() + 78 + 8).c_str());

  // 86    8   0x33 0x30 0x39 0x32 0x35 0x37 0x32 0x31 0x33 0x34 0x00
  ESP_LOGI(TAG, "  Serial number: %s", std::string(data.begin() + 86, data.begin() + 86 + 11).c_str());

  // 97    5   0x30 0x30 0x30 0x30 0x00
  ESP_LOGI(TAG, "  Passcode: %s", std::string(data.begin() + 97, data.begin() + 97 + 5).c_str());

  // 102  16   0x49 0x6E 0x70 0x75 0x74 0x20 0x55 0x73 0x65 0x72 0x64 0x61 0x74 0x61 0x00 0x00
  ESP_LOGI(TAG, "  User data: %s", std::string(data.begin() + 102, data.begin() + 102 + 16).c_str());

  // 118  16   0x31 0x32 0x33 0x34 0x35 0x37 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Setup passcode: %s", std::string(data.begin() + 118, data.begin() + 118 + 16).c_str());

  if (this->protocol_version_ != PROTOCOL_VERSION_JK02_32S)
    return;

  // 134  0x49
  // 135  0x6E
  // 136  0x70
  // 137  0x75
  // 138  0x74
  // 139  0x20
  // 140  0x55
  // 141  0x73
  // 142  0x65
  // 143  0x72
  // 144  0x64
  // 145  0x61
  // 146  0x74
  // 147  0x61
  // 148  0x00
  // 149  0x00
  // 150  0xFE
  // 151  0xFF
  // 152  0xFF
  // 153  0xFF
  // 154  0xAF
  // 155  0xE9
  // 156  0x01
  // 157  0x02
  // 158  0x00
  // 159  0x00
  // 160  0x00
  // 161  0x00
  // 162  0x90
  // 163  0x1F
  // 164  0x00
  // 165  0x00
  // 166  0x00
  // 167  0x00
  // 168  0xC0
  // 169  0xD8
  // 170  0xE7
  // 171  0xFE
  // 172  0x1F
  // 173  0x00
  // 174  0x00
  // 175  0x01
  // 176  0x00
  // 177  0x00
  // 178  0x00
  // 179  0x00
  // 180  0x00
  // 181  0x00
  // 182  0x00
  // 183  0x00
  // 184  0x01                  UART1M Protocol number
  ESP_LOGI(TAG, "  UART1 Protocol: %d", data[184]);
  // 185  0x04                  CANM Protocol number
  ESP_LOGI(TAG, "  CAN Protocol: %d", data[185]);
  // 186  0xCF
  // 187  0x03
  // 188  0x00
  // 189  0x00
  // 190  0x00
  // 191  0x00
  // 192  0x00
  // 193  0x00
  // 194  0x00
  // 195  0x00
  // 196  0x00
  // 197  0x00
  // 198  0x00
  // 199  0x00
  // 200  0x00
  // 201  0x00
  // 202  0xDF
  // 203  0x07
  // 204  0x00
  // 205  0x00
  // 206  0x00
  // 207  0x00
  // 208  0x00
  // 209  0x00
  // 210  0x00
  // 211  0x00
  // 212  0x00
  // 213  0x00
  // 214  0x00
  // 215  0x00
  // 216  0x00
  // 217  0x00
  // 218  0x01                  UART2M Protocol number
  ESP_LOGI(TAG, "  UART2 Protocol: %d", data[218]);
  // 219  0xCF                  UART2M Protocol enable
  // 220  0x03
  // 221  0x00
  // 222  0x00
  // 223  0x00
  // 224  0x00
  // 225  0x00
  // 226  0x00
  // 227  0x00
  // 228  0x00
  // 229  0x00
  // 230  0x00
  // 231  0x00
  // 232  0x00
  // 233  0x00
  // 234  0x00                  LCD buzzer trigger
  // 235  0x0B                  DRY1 Trigger
  // 236  0x00                  DRY2 Trigger
  // 237  0x01                  UART protocol library version
  // 238  0x00 0x00 0x00 0x00   LCD Buzzer Trigger Value
  // 242  0x00 0x00 0x00 0x00   LCD Buzzer Release Value
  // 246  0x09 0x00 0x00 0x00   DRY1 Trigger Value
  // 250  0x0B 0x00 0x00 0x00   DRY1 Release Value
  // 254  0x00 0x00 0x00 0x00   DRY2 Trigger Value
  // 258  0x00 0x00 0x00 0x00   DRY2 Release Value
  // 262  0x80 0x51 0x00 0x00   Data Stored Period

  // 266  0x0A                  RCV Time    0.1h
  ESP_LOGI(TAG, "  RCV Time: %.1f h", (float) data[266] * 0.1f);
  this->publish_state_(this->cell_request_charge_voltage_time_number_, (float) data[266] * 0.1f);

  // 267  0x50                  RFV Time    0.1h
  ESP_LOGI(TAG, "  RFV Time: %.1f h", (float) data[267] * 0.1f);
  this->publish_state_(this->cell_request_float_voltage_time_number_, (float) data[267] * 0.1f);

  // 268  0x01  CAN protocol library version
  // 269  0x00  Reserved
  // 270  0x00
  // 271  0x00
  // 272  0x00
  // 273  0x00
  // 274  0x00
  // 275  0x00
  // 276  0x00
  // 277  0x00
  // 278  0x00
  // 279  0x00
  // 280  0x00
  // 281  0x00
  // 282  0x00
  // 283  0x00
  // 284  0x00
  // 285  0x00
  // 286  0x00
  // 287  0xFE
  // 288  0x9F
  // 289  0xE9
  // 290  0xFE
  // 291  0x03
  // 292  0x00
  // 293  0x00
  // 294  0x00
  // 295  0x00
  // 296  0x00
  // 297  0x00
  // 298  0x00
  // 299  0x1B  CRC
}

bool JkBmsBle::write_register(uint8_t address, uint32_t value, uint8_t length) {
  uint8_t frame[20];
  frame[0] = 0xAA;     // start sequence
  frame[1] = 0x55;     // start sequence
  frame[2] = 0x90;     // start sequence
  frame[3] = 0xEB;     // start sequence
  frame[4] = address;  // holding register
  frame[5] = length;   // size of the value in byte
  frame[6] = value >> 0;
  frame[7] = value >> 8;
  frame[8] = value >> 16;
  frame[9] = value >> 24;
  frame[10] = 0x00;
  frame[11] = 0x00;
  frame[12] = 0x00;
  frame[13] = 0x00;
  frame[14] = 0x00;
  frame[15] = 0x00;
  frame[16] = 0x00;
  frame[17] = 0x00;
  frame[18] = 0x00;
  frame[19] = crc(frame, sizeof(frame) - 1);

  ESP_LOGD(TAG, "Write register: %s", format_hex_pretty(frame, sizeof(frame)).c_str());
  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }

  return (status == 0);
}

void JkBmsBle::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkBmsBle::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void JkBmsBle::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(min_cell_voltage_sensor_, NAN);
  this->publish_state_(max_cell_voltage_sensor_, NAN);
  this->publish_state_(min_voltage_cell_sensor_, NAN);
  this->publish_state_(max_voltage_cell_sensor_, NAN);
  this->publish_state_(delta_cell_voltage_sensor_, NAN);
  this->publish_state_(average_cell_voltage_sensor_, NAN);
  this->publish_state_(total_voltage_sensor_, NAN);
  this->publish_state_(current_sensor_, NAN);
  this->publish_state_(power_sensor_, NAN);
  this->publish_state_(charging_power_sensor_, NAN);
  this->publish_state_(discharging_power_sensor_, NAN);
  this->publish_state_(power_tube_temperature_sensor_, NAN);
  this->publish_state_(balancing_sensor_, NAN);
  this->publish_state_(state_of_charge_sensor_, NAN);
  this->publish_state_(capacity_remaining_sensor_, NAN);
  this->publish_state_(total_battery_capacity_setting_sensor_, NAN);
  this->publish_state_(charging_cycles_sensor_, NAN);
  this->publish_state_(total_charging_cycle_capacity_sensor_, NAN);
  this->publish_state_(total_runtime_sensor_, NAN);
  this->publish_state_(balancing_current_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(heating_current_sensor_, NAN);

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
    this->publish_state_(cell.cell_resistance_sensor_, NAN);
  }
  for (auto &temperature : this->temperatures_) {
    this->publish_state_(temperature.temperature_sensor_, NAN);
  }
}

void JkBmsBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBmsBle::publish_state_(number::Number *number, float value) {
  if (number == nullptr)
    return;

  number->publish_state(value);
}

void JkBmsBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkBmsBle::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void JkBmsBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

std::string JkBmsBle::error_bits_to_string_(const uint16_t mask) {
  bool first = true;
  std::string errors_list = "";

  if (mask) {
    for (int i = 0; i < ERRORS_SIZE; i++) {
      if (mask & (1 << i)) {
        if (first) {
          first = false;
        } else {
          errors_list.append(";");
        }
        errors_list.append(ERRORS[i]);
      }
    }
  }

  return errors_list;
}

}  // namespace jk_bms_ble
}  // namespace esphome

#endif
