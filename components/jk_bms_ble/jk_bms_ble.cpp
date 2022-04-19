#include "jk_bms_ble.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace jk_bms_ble {

static const char *const TAG = "jk_bms_ble";

static const uint16_t JK_BMS_SERVICE_UUID = 0xFFE0;
static const uint16_t JK_BMS_CHARACTERISTIC_UUID = 0xFFE1;

static const uint16_t MIN_RESPONSE_SIZE = 300;
static const uint16_t MAX_RESPONSE_SIZE = 320;

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
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
  LOG_SENSOR("", "State Of Charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Capacity Remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Total Battery Capacity Setting", this->total_battery_capacity_setting_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Total Charging Cycle Capacity", this->total_charging_cycle_capacity_sensor_);
  LOG_SENSOR("", "Total Runtime", this->total_runtime_sensor_);
  LOG_TEXT_SENSOR("", "Total Runtime Formatted", this->total_runtime_formatted_text_sensor_);
  LOG_BINARY_SENSOR("", "Balancing Switch", this->balancing_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging Switch", this->charging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging Switch", this->discharging_switch_binary_sensor_);
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

      // this->publish_state_(this->voltage_sensor_, NAN);
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *chr = this->parent_->get_characteristic(JK_BMS_SERVICE_UUID, JK_BMS_CHARACTERISTIC_UUID);
      if (chr == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an JK BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_handle_ = chr->handle;

      auto status =
          esp_ble_gattc_register_for_notify(this->parent()->gattc_if, this->parent()->remote_bda, chr->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;
      this->status_notification_received_ = false;

      ESP_LOGW(TAG, "Request device info");
      // Request extracted from btsnoop_hci_jk-bd6a17s6p_hw72_sw710h.log
      static uint8_t req_device_info[20] = {0xaa, 0x55, 0x90, 0xeb, 0x97, 0x00, 0xf4, 0x5f, 0xe2, 0x03,
                                            0x6d, 0xfb, 0xe0, 0x38, 0xaa, 0xbc, 0x44, 0x12, 0x34, 0xb9};
      auto status = esp_ble_gattc_write_char(this->parent_->gattc_if, this->parent_->conn_id, this->char_handle_,
                                             sizeof(req_device_info), req_device_info, ESP_GATT_WRITE_TYPE_NO_RSP,
                                             ESP_GATT_AUTH_REQ_NONE);
      if (status)
        ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);

      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      if (param->notify.handle != this->char_handle_)
        break;

      this->assemble_(param->notify.value, param->notify.value_len);

      break;
    }
    default:
      break;
  }
}

void JkBmsBle::update() {
  if (this->enable_fake_traffic_) {
    const uint8_t frame[300] = {
        0x55, 0xAA, 0xEB, 0x90, 0x02, 0x8C, 0xFF, 0x0C, 0x01, 0x0D, 0x01, 0x0D, 0xFF, 0x0C, 0x01, 0x0D, 0x01, 0x0D,
        0xFF, 0x0C, 0x01, 0x0D, 0x01, 0x0D, 0x01, 0x0D, 0x01, 0x0D, 0xFF, 0x0C, 0x01, 0x0D, 0x01, 0x0D, 0x01, 0x0D,
        0x01, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x9D, 0x01, 0x96, 0x01, 0x8C, 0x01, 0x87, 0x01,
        0x84, 0x01, 0x84, 0x01, 0x83, 0x01, 0x84, 0x01, 0x85, 0x01, 0x81, 0x01, 0x83, 0x01, 0x86, 0x01, 0x82, 0x01,
        0x82, 0x01, 0x83, 0x01, 0x85, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xBE, 0x00, 0xBF, 0x00, 0xD2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x8E, 0x0B,
        0x01, 0x00, 0x68, 0x3C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x04, 0x00, 0x00, 0x64, 0x00, 0x79, 0x04,
        0xCA, 0x03, 0x10, 0x00, 0x01, 0x01, 0xAA, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD5, 0x02, 0x00, 0x00, 0x00, 0x00, 0xAE, 0xD6, 0x3B, 0x40,
        0x00, 0x00, 0x00, 0x00, 0x58, 0xAA, 0xFD, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0xEC, 0xE6,
        0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCD};
    this->assemble_(frame, 300);
    return;
  }

  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  if (!this->status_notification_received_) {
    ESP_LOGW(TAG, "Request status notification");
    // Request extracted from btsnoop_hci_jk-bd6a17s6p_hw72_sw710h.log
    static uint8_t req_status[20] = {0xaa, 0x55, 0x90, 0xeb, 0x96, 0x00, 0x79, 0x62, 0x96, 0xed,
                                     0xe3, 0xd0, 0x82, 0xa1, 0x9b, 0x5b, 0x3c, 0x9c, 0x4b, 0x5d};
    auto status =
        esp_ble_gattc_write_char(this->parent_->gattc_if, this->parent_->conn_id, this->char_handle_,
                                 sizeof(req_status), req_status, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
    if (status)
      ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }
}

// TODO: There is no need to assemble frames if the MTU can be increased to > 320 bytes
void JkBmsBle::assemble_(const uint8_t *data, uint16_t length) {
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
    const uint16_t frame_size = this->frame_buffer_.size();

    uint16_t computed_crc = crc(raw, frame_size - 1);
    uint16_t remote_crc = raw[frame_size - 1];
    if (computed_crc != remote_crc) {
      ESP_LOGW(TAG, "JkBmsBle CRC Check failed! %02X!=%02X", computed_crc, remote_crc);
      this->frame_buffer_.clear();
      return;
    }

    std::vector<uint8_t> data(this->frame_buffer_.begin(), this->frame_buffer_.end());

    this->decode_(data);
    this->frame_buffer_.clear();
  }
}

void JkBmsBle::decode_(const std::vector<uint8_t> &data) {
  uint8_t frame_type = data[4];
  switch (frame_type) {
    case 0x02:
      this->decode_cell_info_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unsupported message type (%02X)", data[4]);
  }
}

void JkBmsBle::decode_cell_info_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_24bit = [&](size_t i) -> uint32_t {
    return (uint32_t(data[i + 2]) << 16) | (uint32_t(data[i + 1]) << 8) | (uint32_t(data[i + 0]) << 0);
  };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };

  const uint32_t now = millis();
  if (now - this->last_cell_info_ < this->throttle_) {
    return;
  }
  this->last_cell_info_ = now;

  ESP_LOGVV(TAG, "Cell info frame:");
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
  // Address Content: Description      Decoded content                         Coeff./Unit
  // 0x55 0xAA 0xEB 0x90: Header
  // 0x02: Record type
  // 0x8C: Frame counter
  // 0xFF 0x0C: Voltage cell 01                                                0.001
  // 0x01 0x0D: Voltage cell 02                                                0.001
  // 0x01 0x0D: Voltage cell 03                                                0.001
  // ...
  uint8_t cells = 24;
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_get_16bit(i * 2 + 6) * 0.001f;
    float cell_resistance = (float) jk_get_16bit(i * 2 + 64) * 0.001f;
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
  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);

  // 0xFF 0xFF 0x00 0x00: Static separator?

  // 0x00 0x0D: Average_Cell_Voltage?                                          0.001
  this->publish_state_(this->average_cell_voltage_sensor_, (float) jk_get_16bit(58) * 0.001f);

  // 0x00 0x00: Delta_Cell_Voltage?                                            0.001
  this->publish_state_(this->delta_cell_voltage_sensor_, (float) jk_get_16bit(60) * 0.001f);

  // 0x00 0x00: Current_Balancer?                                              0.001
  // 0x9D 0x01: Resistance Cell 01
  // 0x96 0x01: Resistance Cell 02
  // 0x8C 0x01: Resistance Cell 03
  // ...
  // 0x00 0x00: Resistance Cell 24

  // 0x00 0x00 0x00 0x00 0x00 0x00: Unknown2

  // 0x03 0xD0 0x00 0x00: Battery voltage                                      0.001
  float total_voltage = (float) jk_get_16bit(118) * 0.001f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 0x00 0x00 0x00 0x00: Battery power                                        0.001
  // 0x00 0x00 0x00 0x00: Charge current                                       0.001
  float current = (float) ((int) jk_get_32bit(126)) * 0.001f;
  this->publish_state_(this->current_sensor_, (float) ((int) jk_get_32bit(126)) * 0.001f);

  // Don't use byte 122 because it's unsigned
  // float power = (float) ((int) jk_get_32bit(122)) * 0.001f;
  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 0xBE 0x00: Temperature Sensor 1                                           0.1
  this->publish_state_(this->temperature_sensor_1_sensor_, (float) jk_get_16bit(130) * 0.1f);

  // 0xBF 0x00: Temperature Sensor 2                                           0.1
  this->publish_state_(this->temperature_sensor_2_sensor_, (float) jk_get_16bit(132) * 0.1f);

  // 0xD2 0x00: MOS Temperature                                                0.1
  this->publish_state_(this->power_tube_temperature_sensor_, (float) jk_get_16bit(134) * 0.1f);

  // 0x00 0x00:
  // 0x00 0x00: Balance current                                               0.001
  this->publish_state_(this->balancing_current_sensor_, (float) ((int16_t) jk_get_16bit(138)) * 0.001f);

  // 0x00: Blink cells?

  // 0x54: Percent_Remain
  this->publish_state_(this->state_of_charge_sensor_, (float) data[141]);

  // 0x8E 0x0B 0x01 0x00: Capacity_Remain                                      0.001
  this->publish_state_(this->capacity_remaining_sensor_, (float) jk_get_32bit(142) * 0.001f);

  // 0x68 0x3C 0x01 0x00: Nominal_Capacity                                     0.001
  this->publish_state_(this->total_battery_capacity_setting_sensor_, (float) jk_get_32bit(146) * 0.001f);

  // 0x00 0x00 0x00 0x00: Cycle_Count
  this->publish_state_(this->charging_cycles_sensor_, (float) jk_get_32bit(150));

  // 0x3D 0x04 0x00 0x00: Cycle_Capacity                                       0.001
  this->publish_state_(this->total_charging_cycle_capacity_sensor_, (float) jk_get_32bit(154));

  // 0x64 0x00: Unknown12
  // 0x79 0x04: Unknown13 (Cycle capacity?)
  // 0xCA 0x03 0x10: Total runtime in seconds
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_24bit(162));
  if (this->total_runtime_formatted_text_sensor_ != nullptr) {
    this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(jk_get_32bit(162)));
  }

  // 0x00: Unknown
  // 0x01: Charging switch enabled                                             0x00: false, 0x01: true
  this->publish_state_(this->charging_switch_binary_sensor_, (bool) data[166]);
  // 0x01: Discharging switch enabled                                          0x00: false, 0x01: true
  this->publish_state_(this->discharging_switch_binary_sensor_, (bool) data[167]);

  // 0xAA: Unknown
  // 0x06 0x00: Unknown17
  // 0x00 0x00: discard6
  // 0x00 0x00: Unknown18
  // 0x00 0x00: Unknown19
  // 0x00 0x00: Unknown20
  // 0x00 0x00: Current_Charge
  // 0x00 0x07: Current_Discharge
  // 0x00 0x01: Unknown23
  // 0x00 0x00: Unknown24
  // 0x00 0xD5: Unknown25
  // 0x02 0x00: Unknown26
  // 0x00 0x00: Unknown27
  // 0x00 0xAE: Unknown28
  // 0xD6 0x3B: Unknown29
  // 0x40 0x00 0x00 0x00 0x00 0x58 0xAA 0xFD 0xFF 0x00
  // 0x00 0x00 0x01 0x00 0x02 0x00 0x00 0xEC 0xE6 0x4F
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  // 0x00 0x00: Unknown30 (92 bytes)
  // 0xCD: CRC

  status_notification_received_ = true;
}

void JkBmsBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkBmsBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkBmsBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

}  // namespace jk_bms_ble
}  // namespace esphome

#endif
