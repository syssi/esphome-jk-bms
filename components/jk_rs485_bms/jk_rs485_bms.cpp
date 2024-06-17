#include "jk_rs485_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_rs485_bms {

static const char *const TAG = "jk_rs485_bms";

static const uint8_t MAX_NO_RESPONSE_COUNT = 5;

static const uint8_t FUNCTION_READ_ALL = 0x06;
static const uint8_t FUNCTION_WRITE_REGISTER = 0x02;

static const uint8_t FRAME_VERSION_JK04 = 0x01;
static const uint8_t FRAME_VERSION_JK02_24S = 0x02;
static const uint8_t FRAME_VERSION_JK02_32S = 0x03;

static const uint8_t ERRORS_SIZE = 14;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Low capacity",                              // Byte 0.0, warning
    "Powertube overtemperature",                 // Byte 0.1, alarm
    "Charging overvoltage",                      // Byte 0.2, alarm
    "Discharging undervoltage",                  // Byte 0.3, alarm
    "Battery over temperature",                  // Byte 0.4, alarm
    "Charging overcurrent",                      // Byte 0.5, alarm
    "Discharging overcurrent",                   // Byte 0.6, alarm
    "Cell pressure difference",                  // Byte 0.7, alarm
    "Overtemperature alarm in the battery box",  // Byte 1.0, alarm
    "Battery low temperature",                   // Byte 1.1, alarm
    "Cell overvoltage",                          // Byte 1.2, alarm
    "Cell undervoltage",                         // Byte 1.3, alarm
    "309_A protection",                          // Byte 1.4, alarm
    "309_A protection",                          // Byte 1.5, alarm
};

static const uint8_t OPERATION_MODES_SIZE = 4;
static const char *const OPERATION_MODES[OPERATION_MODES_SIZE] = {
    "Charging enabled",     // 0x00
    "Discharging enabled",  // 0x01
    "Balancer enabled",     // 0x02
    "Battery dropped",      // 0x03
};

static const uint8_t BATTERY_TYPES_SIZE = 3;
static const char *const BATTERY_TYPES[BATTERY_TYPES_SIZE] = {
    "Lithium Iron Phosphate",  // 0x00
    "Ternary Lithium",         // 0x01
    "Lithium Titanate",        // 0x02
};

float uint32_to_float(const uint8_t* byteArray) {
    // Combine the bytes into an int32_t
    uint32_t uintValue = (static_cast<uint32_t>(byteArray[0]) << 0) |
                       (static_cast<uint32_t>(byteArray[1]) << 8) |
                       (static_cast<uint32_t>(byteArray[2]) << 16)|
                       (static_cast<uint32_t>(byteArray[3]) << 24);

    float floatValue = static_cast<float>(uintValue);

    return floatValue;
}

float int32_to_float(const uint8_t* byteArray) {
    // Combine the bytes into an int32_t
    int32_t intValue = (static_cast<int32_t>(byteArray[0]) << 0) |
                       (static_cast<int32_t>(byteArray[1]) << 8) |
                       (static_cast<int32_t>(byteArray[2]) << 16)|
                       (static_cast<int32_t>(byteArray[3]) << 24);

    float floatValue = static_cast<float>(intValue);

    return floatValue;
}

float uint16_to_float(const uint8_t* byteArray) {
    // Combine the bytes into an int32_t
    uint32_t uintValue = (static_cast<uint32_t>(byteArray[0]) << 0) |
                       (static_cast<uint32_t>(byteArray[1]) << 8);

    float floatValue = static_cast<float>(uintValue);

    return floatValue;
}

float int16_to_float(const uint8_t* byteArray) {
    // Combine the bytes into an int32_t
    int32_t intValue = (static_cast<int32_t>(byteArray[0]) << 0) |
                       (static_cast<int32_t>(byteArray[1]) << 8);

    float floatValue = static_cast<float>(intValue);

    return floatValue;
}

void JkRS485Bms::on_jk_rs485_sniffer_data(const uint8_t &origin_address, const uint8_t &frame_type,
                                          const std::vector<uint8_t> &data,
                                          const std::string &nodes_available_received) {
  // this->reset_status_online_tracker_();

  if (this->nodes_available != nodes_available_received) {
    this->nodes_available = nodes_available_received;
    this->publish_state_(this->network_nodes_available_text_sensor_, this->nodes_available);
  }

  if (origin_address == this->address_) {
    this->reset_status_online_tracker_();
    ESP_LOGD(TAG, "This BMS address is: %d  and address received %d ==> WORKING (frame type:%d)",
             this->address_, origin_address, frame_type);
    switch (frame_type) {
      case 0x01:
        if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
          // this->decode_jk04_settings_(data);
        } else {
          this->decode_jk02_settings_(data);
        }
        break;
      case 0x02:
        if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
          // this->decode_jk04_cell_info_(data);
        } else {
          this->decode_jk02_cell_info_(data);
        }
        break;
      case 0x03:
        ESP_LOGI(TAG, "Decoding DEVICE "
                      "info............................................................................................"
                      "................................................................");
        this->decode_device_info_(data);
        break;
      default:
        ESP_LOGW(TAG, "Unsupported FRAME TYPE (0x%02X)", frame_type);
        ESP_LOGD(TAG, "Decoding cell info frame.... [ADDRESS: %02X] %d bytes received", origin_address, data.size());
        ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), 150).c_str());
    }
  } else {
    ESP_LOGD(TAG, "This BMS address is: %d  and address received %d ==> IDLE", this->address_, origin_address);
  }
}

void JkRS485Bms::decode_jk02_cell_info_(const std::vector<uint8_t> &data) {


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

  ESP_LOGI(TAG, "Decoding cell info frame.... [ADDRESS: %02X] %d bytes received", this->address_, data.size());
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

  float temp_param_value;

  uint8_t cells = 24 + (offset / 2);
  float cell_voltage_min = 100.0f;
  float cell_voltage_max = -100.0f;
  float cell_resistance_min = 1000.0f;
  float cell_resistance_max = -1000.0f;
  uint8_t cell_count_real = 0;
  uint8_t cell_voltage_min_cell_number = 0;
  uint8_t cell_voltage_max_cell_number = 0;
  uint8_t cell_resistance_min_cell_number = 0;
  uint8_t cell_resistance_max_cell_number = 0;  
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage    = uint16_to_float(&data[i * 2 + 6]) * 0.001f;              //(float) jk_get_16bit(i * 2 + 6) * 0.001f;
    float cell_resistance = uint16_to_float(&data[(i * 2 + 64 + offset)]) * 0.001f;  //(float) jk_get_16bit(i * 2 + 64 + offset) * 0.001f;
    if (cell_voltage > 0){
      cell_count_real++;
      if (cell_voltage < cell_voltage_min) {
        cell_voltage_min = cell_voltage;
      }
      if (cell_voltage > cell_voltage_max) {
        cell_voltage_max = cell_voltage;
      }

      if (cell_resistance < cell_resistance_min) {
        cell_resistance_min = cell_resistance;
        cell_resistance_min_cell_number=i;
      }
      if (cell_resistance > cell_resistance_max) {
        cell_resistance_max = cell_resistance;
        cell_resistance_max_cell_number=i;
      } 
    }



    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
    this->publish_state_(this->cells_[i].cell_resistance_sensor_, cell_resistance);
    ESP_LOGV(TAG, "Cell %02d voltage:    %f", i, cell_voltage);
    ESP_LOGV(TAG, "Cell %02d resistance: %f", i, cell_resistance);
  }
  
  this->publish_state_(this->cell_count_real_sensor_, (float) cell_count_real);
  this->publish_state_(this->cell_voltage_min_sensor_, cell_voltage_min);
  this->publish_state_(this->cell_voltage_max_sensor_, cell_voltage_max);
  this->publish_state_(this->cell_resistance_min_sensor_, cell_resistance_min);
  this->publish_state_(this->cell_resistance_max_sensor_, cell_resistance_max);
  this->publish_state_(this->cell_resistance_max_cell_number_sensor_, (float) cell_resistance_max_cell_number+1);
  this->publish_state_(this->cell_resistance_min_cell_number_sensor_, (float) cell_resistance_min_cell_number+1);
  

  ESP_LOGV(TAG, "Cell MAX voltage:    %f", cell_voltage_max);
  ESP_LOGV(TAG, "Cell MAX voltage:    %f", cell_voltage_min);

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

  // 58    2   0x00 0x0D              cell average voltage  0.001        V
  this->publish_state_(this->cell_average_voltage_sensor_, uint16_to_float(&data[58+offset]) * 0.001f);    // (float) jk_get_16bit(58 + offset) * 0.001f);

  // 60    2   0x00 0x00              cell delta voltage    0.001        V
  this->publish_state_(this->cell_delta_voltage_sensor_, uint16_to_float(&data[60+offset]) * 0.001f);    // (float) jk_get_16bit(60 + offset) * 0.001f);

  // 62    1   0x00                   Cell voltage max cell number      1
  this->publish_state_(this->cell_voltage_max_cell_number_sensor_, (float) data[62 + offset] + 1);
  // 63    1   0x00                   Cell voltage min cell number      1
  this->publish_state_(this->cell_voltage_min_cell_number_sensor_, (float) data[63 + offset] + 1);
  // 64    2   0x9D 0x01              Resistance Cell 01    0.001        Ohm
  // 66    2   0x96 0x01              Resistance Cell 02    0.001        Ohm
  // 68    2   0x8C 0x01              Resistance Cell 03    0.001        Ohm
  // ...
  // 110   2   0x00 0x00              Resistance Cell 24    0.001        Ohm

  offset = offset * 2;

  // 112   2   0x00 0x00              Unknown112
  if (frame_version == FRAME_VERSION_JK02_32S) {
    temp_param_value=int16_to_float(&data[112+offset]) * 0.1f;
    this->publish_state_(this->temperature_powertube_sensor_, temp_param_value);
  } else {
    ESP_LOGD(TAG, "Unknown112: 0x%02X 0x%02X", data[112 + offset], data[113 + offset]);
  }

  // 114   4   0x00 0x00 0x00 0x00    Wire resistance warning bitmask (each bit indicates a warning per cell / wire)
  ESP_LOGD(TAG, "Wire resistance warning bitmask: 0x%02X 0x%02X 0x%02X 0x%02X", data[114 + offset], data[115 + offset],
           data[116 + offset], data[117 + offset]);

  // 118 [144=118+26]  4   0x03 0xD0 0x00 0x00    Battery voltage       0.001        V
  float battery_voltage =  uint32_to_float(&data[118+offset]) * 0.001f; //(float) jk_get_32bit(118 + offset) * 0.001f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  ESP_LOGD(TAG, " BATTERY VOLTAGE 144: %f", battery_voltage);


  // 122   4   0x00 0x00 0x00 0x00    Battery power         0.001        W
  // 126   4   0x00 0x00 0x00 0x00    Charge current        0.001        A
  float current = int32_to_float(&data[126+offset]) * 0.001f;
  this->publish_state_(this->battery_current_sensor_, current);

  // Don't use byte 122 because it's unsigned
  // float power = (float) ((int32_t) jk_get_32bit(122 + offset)) * 0.001f;
  float power = battery_voltage * current;
  this->publish_state_(this->battery_power_sensor_, power);
  this->publish_state_(this->battery_power_charging_sensor_, std::max(0.0f, power));  // 500W vs 0W -> 500W
  this->publish_state_(this->battery_power_discharging_sensor_,
                       std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 130 [156 = 130+26]   2   0xBE 0x00              Temperature Sensor 1  0.1          °C
  temp_param_value=int16_to_float(&data[130+offset]) * 0.1f;
  this->publish_state_(this->temperatures_[0].temperature_sensor_,temp_param_value);

  // 132 [158 = 132+26]   2   0xBF 0x00              Temperature Sensor 2  0.1          °C
  temp_param_value=int16_to_float(&data[132+offset]) * 0.1f;
  this->publish_state_(this->temperatures_[1].temperature_sensor_,temp_param_value);

  // 134 [160=134+26]  2   0xD2        Alarms      bit
  // AlarmWireRes                1   (0:normal | 1:alarm)
  // AlarmMosOTP                 2   (0:normal | 1:alarm)
  // AlarmCellQuantity           4   (0:normal | 1:alarm)
  // AlarmCurSensorErr           8   (0:normal | 1:alarm)
  // AlarmCellOVP                16  (0:normal | 1:alarm)
  // AlarmBatOVP                 32  (0:normal | 1:alarm)
  // AlarmChOCP                  64  (0:normal | 1:alarm)
  // AlarmChSCP                  128 (0:normal | 1:alarm)
  if (frame_version == FRAME_VERSION_JK02_32S) {
    this->battery_total_alarms_count_ = 0;
    this->battery_total_alarms_active_ = 0;
    this->publish_alarm_state_(this->alarm_wireres_binary_sensor_, (bool) check_bit_(data[134], 1));
    this->publish_alarm_state_(this->alarm_mosotp_binary_sensor_, (bool) check_bit_(data[134], 2));
    this->publish_alarm_state_(this->alarm_cellquantity_binary_sensor_, (bool) check_bit_(data[134], 4));
    this->publish_alarm_state_(this->alarm_cursensorerr_binary_sensor_, (bool) check_bit_(data[134], 8));
    this->publish_alarm_state_(this->alarm_cellovp_binary_sensor_, (bool) check_bit_(data[134], 16));
    this->publish_alarm_state_(this->alarm_batovp_binary_sensor_, (bool) check_bit_(data[134], 32));
    this->publish_alarm_state_(this->alarm_chocp_binary_sensor_, (bool) check_bit_(data[134], 64));
    this->publish_alarm_state_(this->alarm_chscp_binary_sensor_, (bool) check_bit_(data[134], 128));

    /*    ESP_LOGI(TAG, "alarm_WireRes_binary_sensor_:                  %d", (bool) check_bit_(data[134], 1));
        ESP_LOGI(TAG, "alarm_MosOTP_binary_sensor_:                   %d", (bool) check_bit_(data[134], 2));
        ESP_LOGI(TAG, "alarm_CellQuantity_binary_sensor_:             %d", (bool) check_bit_(data[134], 4));
        ESP_LOGI(TAG, "alarm_CurSensorErr_binary_sensor_:             %d", (bool) check_bit_(data[134], 8));
        ESP_LOGI(TAG, "alarm_CellOVP_binary_sensor_:                  %d", (bool) check_bit_(data[134], 16));
        ESP_LOGI(TAG, "alarm_BatOVP_binary_sensor_:                   %d", (bool) check_bit_(data[134], 32));
        ESP_LOGI(TAG, "alarm_ChOCP_binary_sensor_:                    %d", (bool) check_bit_(data[134], 64));
        ESP_LOGI(TAG, "alarm_ChSCP_binary_sensor_:                    %d", (bool) check_bit_(data[134], 128));*/
  }

  if (frame_version == FRAME_VERSION_JK02_32S) {
  // 134   2   0xD2 0x00              error bitmastk
    uint16_t raw_errors_bitmask = (uint16_t(data[134 + offset]) << 8) | (uint16_t(data[134 + 1 + offset]) << 0);
    this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
    this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));
  } else {
  // 134   2   0xD2 0x00              MOS Temperature       0.1          °C
    this->publish_state_(this->temperature_powertube_sensor_, int16_to_float(&data[134+offset]) * 0.1f);    //  (float) ((int16_t) jk_get_16bit(134 + offset)) * 0.1f);
  }

  // 135 [161=135+26]   2   0xD2        Alarms      bit
  // AlarmChOTP                  1   (0:normal | 1:alarm)
  // AlarmChUTP                  2   (0:normal | 1:alarm)
  // AlarmCPUAuxCommuErr         4   (0:normal | 1:alarm)
  // AlarmCellUVP                8   (0:normal | 1:alarm)
  // AlarmBatUVP                 16  (0:normal | 1:alarm)
  // AlarmDchOCP                 32  (0:normal | 1:alarm)
  // AlarmDchSCP                 64  (0:normal | 1:alarm)
  // AlarmDchOTP                 128 (0:normal | 1:alarm)
  if (frame_version == FRAME_VERSION_JK02_32S) {
    this->publish_alarm_state_(this->alarm_chotp_binary_sensor_, (bool) check_bit_(data[135], 1));
    this->publish_alarm_state_(this->alarm_chutp_binary_sensor_, (bool) check_bit_(data[135], 2));
    this->publish_alarm_state_(this->alarm_cpuauxcommuerr_binary_sensor_, (bool) check_bit_(data[135], 4));
    this->publish_alarm_state_(this->alarm_celluvp_binary_sensor_, (bool) check_bit_(data[135], 8));
    this->publish_alarm_state_(this->alarm_batuvp_binary_sensor_, (bool) check_bit_(data[135], 16));
    this->publish_alarm_state_(this->alarm_dchocp_binary_sensor_, (bool) check_bit_(data[135], 32));
    this->publish_alarm_state_(this->alarm_dchscp_binary_sensor_, (bool) check_bit_(data[135], 64));
    this->publish_alarm_state_(this->alarm_dchotp_binary_sensor_, (bool) check_bit_(data[135], 128));

  }
  // 136 [162]   2   0xD2        Alarms      bit
  // AlarmChargeMOS              1   (0:normal | 1:alarm)
  // AlarmDischargeMOS           2   (0:normal | 1:alarm)
  // GPSDisconneted              4   (0:normal | 1:alarm)
  // ModifyPWDinTime             8   (0:normal | 1:alarm)
  // DischargeOnFailed           16  (0:normal | 1:alarm)
  // BatteryOverTemp             32  (0:normal | 1:alarm)
  // TemperatureSensorAnomaly    64  (0:normal | 1:alarm)
  // PLCModuleAnomaly            128 (0:normal | 1:alarm)
  if (frame_version == FRAME_VERSION_JK02_32S) {
    this->publish_alarm_state_(this->alarm_chargemos_binary_sensor_, (bool) check_bit_(data[136], 1));
    this->publish_alarm_state_(this->alarm_dischargemos_binary_sensor_, (bool) check_bit_(data[136], 2));
    this->publish_alarm_state_(this->alarm_gpsdisconneted_binary_sensor_, (bool) check_bit_(data[136], 4));
    this->publish_alarm_state_(this->alarm_modifypwdintime_binary_sensor_, (bool) check_bit_(data[136], 8));
    this->publish_alarm_state_(this->alarm_dischargeonfailed_binary_sensor_, (bool) check_bit_(data[136], 16));
    this->publish_alarm_state_(this->alarm_batteryovertemp_binary_sensor_, (bool) check_bit_(data[136], 32));
    this->publish_alarm_state_(this->alarm_temperaturesensoranomaly_binary_sensor_, (bool) check_bit_(data[136], 64));
    this->publish_alarm_state_(this->alarm_plcmoduleanomaly_binary_sensor_, (bool) check_bit_(data[136], 128));
  }

  if (frame_version != FRAME_VERSION_JK02_32S) {
    uint16_t raw_errors_bitmask = (uint16_t(data[136 + offset]) << 8) | (uint16_t(data[136 + 1 + offset]) << 0);
    this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
    this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));
  }

  // 138 [164=138+26]  2   0x00 0x00              Balance current      0.001         A
  this->publish_state_(this->balancing_current_sensor_, int16_to_float(&data[138+offset]) * 0.001f);

  // 140 [166=140+26]  1   0x00                   Balancing action                   0x00: Off
  //                                                                                 0x01: Charging balancer
  //                                                                                 0x02: Discharging balancer
  // this->publish_state_(this->balancing_binary_sensor_, (data[140 + offset] != 0x00));
  this->publish_state_(this->balancing_direction_sensor_, (data[140 + offset]));
  if (data[140 + offset] == 1 or data[140 + offset] == 2) {
    this->publish_state_(this->status_balancing_binary_sensor_, (bool) 1);
  } else {
    this->publish_state_(this->status_balancing_binary_sensor_, (bool) 0);
  }
  // ESP_LOGI(TAG, "BALANCER WORKING STATUS 140:  0x%02X", data[140 + offset]);

  // 141 [167=141+26]  1   0x54                   Battery capacity state of charge in   1.0           %
  this->publish_state_(this->battery_capacity_state_of_charge_sensor_, (float) data[141 + offset]);

  // 142 [168=142+26]  4   0x8E 0x0B 0x01 0x00    Capacity_Remain      0.001         Ah
  this->publish_state_(this->battery_capacity_remaining_sensor_, int32_to_float(&data[142+offset]) * 0.001f);

  // 146 [172=146+26]  4   0x68 0x3C 0x01 0x00    Nominal_Capacity     0.001         Ah
  this->publish_state_(this->battery_capacity_total_setting_sensor_, uint32_to_float(&data[146+offset]) * 0.001f);

  // 150 [176=150+26]  4   0x00 0x00 0x00 0x00    Cycle_Count          1.0
  this->publish_state_(this->charging_cycles_sensor_, uint32_to_float(&data[150+offset])); // (float) jk_get_32bit(150 + offset));

  // 154 [180=154+26]  4   0x3D 0x04 0x00 0x00    Cycle_Capacity       0.001         Ah
  this->publish_state_(this->battery_capacity_total_charging_cycle_sensor_, uint32_to_float(&data[154+offset])*0.001f); //  (float) jk_get_32bit(154 + offset) * 0.001f);

  // 158 [184=158+26]  1   0x64                   SOCSOH
  temp_param_value=uint32_to_float(&data[158+offset]);
  ESP_LOGV(TAG, "SOCSOH: 0x%02X (always 0x64?) %f", data[158 + offset],temp_param_value);
  this->publish_state_(this->battery_soh_valuation_sensor_, temp_param_value); //  (float) jk_get_32bit(158 + offset));


  // 159  [185=159+26]  1   0x00                   Precharge
  // ESP_LOGV(TAG, "Precharge: 0x%02X (always 0x00?)", data[159 + offset]);
  this->publish_state_(this->status_precharging_binary_sensor_, (bool) check_bit_(data[159 + offset], 1));
  ESP_LOGV(TAG, "PRECHARGE WORKING STATUS: 0x%02X", data[159 + offset]);

  // 160  [186=160+26]  2   0x79 0x04              UserAlarm
  ESP_LOGD(TAG, "UserAlarm: 0x%02X 0x%02X (always 0xC5 0x09?)", data[160 + offset], data[161 + offset]);

  // 162  [188=160+26]   4   0xCA 0x03 0x10 0x00    Battery total runtime in seconds           s
  temp_param_value=uint32_to_float(&data[162+offset]);
  ESP_LOGV(TAG, "       1] temp_param_value: %f", temp_param_value);
  this->publish_state_(this->battery_total_runtime_sensor_, temp_param_value); //  (float) jk_get_32bit(162 + offset));
  ESP_LOGV(TAG, "       2] temp_param_value: %f", temp_param_value);  
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(temp_param_value));
  ESP_LOGV(TAG, "       3] temp_param_value: %f", temp_param_value);  

  // 166  [192=166+26]   1   0x01                   Charging mosfet enabled                      0x00: off, 0x01: on
  this->publish_state_(this->status_charging_binary_sensor_, (bool) check_bit_(data[166 + offset], 1));
  ESP_LOGV(TAG, "CHARGE WORKING STATUS:    0x%02X", data[166 + offset]);
  // 167  [193=167+26]  1   0x01                   Discharging mosfet enabled                   0x00: off, 0x01: on
  this->publish_state_(this->status_discharging_binary_sensor_, (bool) check_bit_(data[167 + offset], 1));
  ESP_LOGV(TAG, "DISCHARGE WORKING STATUS: 0x%02X", data[167 + offset]);


  // .........  
  // 170  [196=170+26] 2                          TimeDcOCPR   Time for Discharge Overcurrent Protection Release
  this->publish_state_(this->discharging_overcurrent_protection_release_time_sensor_, uint16_to_float(&data[170+offset]));    //  (float) jk_get_16bit(170 + offset));
  // 172  [198=172+26] 2                          TimeDcSCPR   Time for Discharge Short Circuit Protection Release
  this->publish_state_(this->discharging_short_circuit_protection_release_time_sensor_, uint16_to_float(&data[172+offset]));    // (float) jk_get_16bit(172 + offset));
  // 174  [200=174+26] 2                          TimeCOCPR    Time for Charge Overcurrent Protection Release
  this->publish_state_(this->charging_overcurrent_protection_release_time_sensor_, uint16_to_float(&data[174+offset]));    // (float) jk_get_16bit(174 + offset));   
  // 176  [202=176+26] 2                          TimeCSCPR    Time for Charge Short Circuit Protection Release
  this->publish_state_(this->charging_short_circuit_protection_release_time_sensor_, uint16_to_float(&data[176+offset]));    // (float) jk_get_16bit(176 + offset)); 
  // 178  [204=178+26] 2                          TimeUVPR     Time for Single Cell Under Voltage Protection Release
  this->publish_state_(this->cell_undervoltage_protection_release_time_sensor_, uint16_to_float(&data[178+offset]));    //  (float) jk_get_16bit(178 + offset)); 
  // 180  [206=180+26] 2                          TimeOVPR     Time for Single Cell Over Voltage Protection Release
  this->publish_state_(this->cell_overvoltage_protection_release_time_sensor_, uint16_to_float(&data[180+offset]));    // (float) jk_get_16bit(180 + offset)); 
  // .........



  // 183 [209=183+26]   1   0x01                 209.Status heating          0x00: closed/off, 0x01: open/on
  this->publish_state_(this->status_heating_binary_sensor_, (bool) check_bit_(data[183 + offset], 1));
  ESP_LOGD(TAG, "HEATING BINARY SENSOR STATUS:  0x%02X", data[183 + offset]);

  // 184 [210=184+26]  2   0x00 0x00             210.Reserved

  // 186 [212=186+26]                            212.TimeEmergency  (s)         Emergency switch time
  temp_param_value = uint16_to_float(&data[186+offset]);    //jk_get_16bit(186 + offset);
  // ESP_LOGI(TAG, "  Emergency switch: %s", (raw_emergency_time_countdown > 0) ? "on" : "off");
  this->publish_state_(this->emergency_switch_, temp_param_value > 0);
  this->publish_state_(this->emergency_time_countdown_sensor_, temp_param_value);


  // 187   2   0x00 0xD5              Unknown187
  // 189   2   0x02 0x00              Unknown189
  ESP_LOGD(TAG, "Unknown189: 0x%02X 0x%02X", data[189], data[190]);
  // 190   1   0x00                   Unknown190
  // 191   1   0x00                   Balancer status (working: 0x01, idle: 0x00)
  
  // 193   2   0x00 0xAE              Unknown193
  ESP_LOGD(TAG, "Unknown193: 0x%02X 0x%02X (0x00 0x8D)", data[193 + offset], data[194 + offset]);
  // 195   2   0xD6 0x3B              Unknown195
  ESP_LOGD(TAG, "Unknown195: 0x%02X 0x%02X (0x21 0x40)", data[195 + offset], data[196 + offset]);
  // 197   10  0x40 0x00 0x00 0x00 0x00 0x58 0xAA 0xFD 0xFF 0x00

                      
  
  // 202 [228=202+26]                          228.Battery Voltage       0.01         V   (repeated. More precision at 118)
  //battery_voltage = (float) jk_get_32bit(202 + offset) * 0.01f;
  //ESP_LOGI(TAG, " BATTERY VOLTAGE 228: %f", battery_voltage);

  // 204 [230=204+26]    2   0x01 0xFD         230.Heating current         0.001         A
  this->publish_state_(this->heating_current_sensor_, int16_to_float(&data[204+offset])  * 0.001f);
  ESP_LOGV(TAG, "HEATING CURRENT:  %f", int16_to_float(&data[204+offset])  * 0.001f);

  // 207   7   0x00 0x00 0x01 0x00 0x02 0x00 0x00
  
  // 212 [186=160+26]  // 214   4   0xEC 0xE6 0x4F 0x00    Uptime 100ms

  //--------------------------------------------------------------------------------------
  // 214 [240=214+26] 240.SysRunTics
  
  // 222 [248=222+26] 248.Temp Bat 3
  // 224 [250=224+26] 250.Temp Bat 4
  // 226 [252=226+26] 252.Temp Bat 5
  this->publish_state_(this->temperatures_[2].temperature_sensor_, int16_to_float(&data[222+offset]) * 0.1f);
  this->publish_state_(this->temperatures_[3].temperature_sensor_, int16_to_float(&data[224+offset]) * 0.1f);
  this->publish_state_(this->temperatures_[4].temperature_sensor_, int16_to_float(&data[226+offset]) * 0.1f);
  
  
  if (frame_version == FRAME_VERSION_JK02_32S) {
    // 182 [208=182+26]                             208.Alarms...  
    // ** [JK-PB2A16S-20P v14]
    //    bit0: MOSTempSensorAbsent                    1    
    //    bit1: BATTempSensor1Absent                   2     1: normal, 0: alarm
    //    bit2: BATTempSensor2Absent                   4     1: normal, 0: alarm
    //    bit3: BATTempSensor3Absent                   8     1: normal, 0: alarm
    //    bit4: BATTempSensor4Absent                   16    1: normal, 0: alarm
    //    bit5: BATTempSensor5Absent                   32    1: normal, 0: alarm
    //    bit6:                                        64
    //    bit7:                                        128
    this->publish_alarm_state_(this->alarm_mostempsensorabsent_binary_sensor_,  (bool) !this->check_bit_(data[(182 + offset)], 1));
    this->publish_alarm_state_(this->alarm_battempsensor1absent_binary_sensor_, (bool) !this->check_bit_(data[(182 + offset)], 2));
    this->publish_alarm_state_(this->alarm_battempsensor2absent_binary_sensor_, (bool) !this->check_bit_(data[(182 + offset)], 4));
    this->publish_alarm_state_(this->alarm_battempsensor3absent_binary_sensor_, (bool) !this->check_bit_(data[(182 + offset)], 8));
    this->publish_alarm_state_(this->alarm_battempsensor4absent_binary_sensor_, (bool) !this->check_bit_(data[(182 + offset)], 16));
    this->publish_alarm_state_(this->alarm_battempsensor5absent_binary_sensor_, (bool) !this->check_bit_(data[(182 + offset)], 32));

    // 180 [212=180+32]
    
    //          206
    ESP_LOGV(TAG, "  TimeUVPR??:  %f", uint16_to_float(&data[178+offset]));  // ((int16_t) jk_get_16bit(178 + offset)));
    ESP_LOGV(TAG, "  TimeOVPR??:  %f", uint16_to_float(&data[180+offset]));  // ((int16_t) jk_get_16bit(180 + offset)));

    // 186 [212]
    // uint16_t raw_emergency_time_countdown = jk_get_16bit(186 + offset);
    // ESP_LOGV(TAG, "  Emergency switch: %s", (raw_emergency_time_countdown > 0) ? "on" : "off");
    // this->publish_state_(this->emergency_switch_, (bool) (raw_emergency_time_countdown > 0));

    // 202 Battery Voltage (better 118 measurement --> more decimals)
    //if (frame_version == FRAME_VERSION_JK02_32S) {
    //}

    // 207 [239] ChargerPlugged
    ESP_LOGV(TAG, "  Charger plugged: %d", (data[207 + offset]));
    // 208 [240] SysRunTicks
    ESP_LOGV(TAG, "  SysRunTicks:  %f", uint32_to_float(&data[208+offset]));   //(int32_t) jk_get_32bit(208 + offset)));
  }


  // 286   4                          RUNTIME??
  // this->publish_state_(this->battery_total_runtime_sensor_, (float) jk_get_32bit(286));
  // this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(jk_get_32bit(286)));

  // ESP_LOGI(TAG, "RUNTIME SENSOR:          %f", (float) jk_get_32bit(286));
  // ESP_LOGI(TAG, "RUNTIME SENSOR FORMATED: %s", this->total_runtime_formatted_text_sensor_);
  //  299   1   0xCD                   CHECKSUM

  if (frame_version == FRAME_VERSION_JK02_32S) {
    this->publish_state_(this->battery_total_alarms_count_sensor_, (float) this->battery_total_alarms_count_);
    this->publish_state_(this->battery_total_alarms_active_sensor_, (float) this->battery_total_alarms_active_);
  }
  this->status_notification_received_ = true;
}

void JkRS485Bms::decode_jk02_settings_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 2)) << 16) | (uint32_t(jk_get_16bit(i + 0)) << 0);
  };
  ESP_LOGI(TAG, "Decoding settings  frame.... [ADDRESS: %02X] %d bytes received", this->address_, data.size());
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
  // 6  [0]   4   0x58 0x02 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE SMART SLEEP
  ESP_LOGV(TAG, "  Voltage Smart Sleep: %f", (float) jk_get_32bit(6) * 0.001f);
  this->publish_state_(this->cell_smart_sleep_voltage_sensor_, (float) jk_get_32bit(6) * 0.001f);

  // 10 [4]   4   0x54 0x0B 0x00 0x00    Cell UVP
  ESP_LOGV(TAG, "  Cell UVP: %f V", (float) jk_get_32bit(10) * 0.001f);
  this->publish_state_(this->cell_undervoltage_protection_sensor_, (float) jk_get_32bit(10) * 0.001f);

  // 14 [8]   4   0x80 0x0C 0x00 0x00    Cell UVP Recovery
  ESP_LOGV(TAG, "  Cell UVPR: %f V", (float) jk_get_32bit(14) * 0.001f);
  this->publish_state_(this->cell_undervoltage_protection_recovery_sensor_, (float) jk_get_32bit(14) * 0.001f);

  // 18 [12]   4   0xCC 0x10 0x00 0x00    Cell OVP
  ESP_LOGV(TAG, "%02X%02X%02X%02X Cell OVP: %f V", data[18], data[19], data[20], data[21], (float) jk_get_32bit(18));
  this->publish_state_(this->cell_overvoltage_protection_sensor_, (float) jk_get_32bit(18) * 0.001f);

  // 22 [16]   4   0x68 0x10 0x00 0x00    Cell OVP Recovery
  ESP_LOGI(TAG, "  Cell OVPR: %f V", (float) jk_get_32bit(22) * 0.001f);
  this->publish_state_(this->cell_overvoltage_protection_recovery_sensor_, (float) jk_get_32bit(22) * 0.001f);

  // 26 [20]   4   0x0A 0x00 0x00 0x00    Balance trigger voltage
  ESP_LOGV(TAG, "  Balance trigger voltage: %f V", (float) jk_get_32bit(26) * 0.001f);
  this->publish_state_(this->balancing_trigger_voltage_sensor_, (float) jk_get_32bit(26) * 0.001f);

  // 30 [24]   4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC-100% VOLTAGE
  ESP_LOGV(TAG, "  SOC-100 VOLTAGE: %f V", (float) jk_get_32bit(30) * 0.001f);
  this->publish_state_(this->cell_soc100_voltage_sensor_, (float) jk_get_32bit(30) * 0.001f);
  
  // 34 [28]   4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC-0% VOLTAGE
  ESP_LOGV(TAG, "  SOC-0 VOLTAGE: %f V", (float) jk_get_32bit(34) * 0.001f);
  this->publish_state_(this->cell_soc0_voltage_sensor_, (float) jk_get_32bit(34) * 0.001f);
  
  // 38 [32]   4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE CELL REQUEST CHARGE VOLTAGE [RCV]
  ESP_LOGV(TAG, "  CELL REQUEST CHARGE VOLTAGE [RCV]: %f V", (float) jk_get_32bit(38) * 0.001f);
  this->publish_state_(this->cell_request_charge_voltage_sensor_, (float) jk_get_32bit(38) * 0.001f);
  
  // 42 [36]   4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE CELL REQUEST FLOAT VOLTAGE
  ESP_LOGV(TAG, "  CELL REQUEST FLOAT VOLTAGE [RFV]: %f V", (float) jk_get_32bit(42) * 0.001f);
  this->publish_state_(this->cell_request_float_voltage_sensor_, (float) jk_get_32bit(42) * 0.001f);
  
  // 46 [40]   4   0xF0 0x0A 0x00 0x00    Power off voltage
  ESP_LOGV(TAG, "  Sys Power off voltage: %f V", (float) jk_get_32bit(46) * 0.001f);
  this->publish_state_(this->cell_power_off_voltage_sensor_, (float) jk_get_32bit(46) * 0.001f);

  // 50 [44]   4   0xA8 0x61 0x00 0x00    Max. charge current                 CurBatCOC
  ESP_LOGV(TAG, "  Max. charging current: %f A", (float) jk_get_32bit(50) * 0.001f);
  this->publish_state_(this->max_charging_current_sensor_, (float) jk_get_32bit(50) * 0.001f);

  // 54 [48]   4   0x1E 0x00 0x00 0x00    Charge OCP delay                    TIMBatCOCPDly   Charging Overcurrent
  // Protection Delay (s)
  ESP_LOGV(TAG, "  Charge OCP delay: %f s", (float) jk_get_32bit(54));
  this->publish_state_(this->charging_overcurrent_protection_delay_sensor_, (float) jk_get_32bit(54) * 0.001f);
  
  // 58 [52]   4   0x3C 0x00 0x00 0x00    Charge OCP recovery time            TIMBatCOCPRDly  Charging Overcurrent
  // Protection Release Delay (s)
  ESP_LOGV(TAG, "  Charge OCP recovery delay: %f s", (float) jk_get_32bit(58));
  this->publish_state_(this->charging_overcurrent_protection_recovery_delay_sensor_, (float) jk_get_32bit(58) * 0.001f);
  
  // 62 [56]   4   0xF0 0x49 0x02 0x00    Max. discharge current CurBatDcOC
  ESP_LOGV(TAG, "  Max. discharging current: %f A", (float) jk_get_32bit(62) * 0.001f);
  this->publish_state_(this->max_discharging_current_sensor_, (float) jk_get_32bit(62) * 0.001f);

  // 66 [60]   4   0x2C 0x01 0x00 0x00    Discharge OCP delay                 TIMBatDcOCPDly   Discharge Overcurrent Protection Delay
  ESP_LOGV(TAG, "  Discharge OCP delay: %f s", (float) jk_get_32bit(66));
  this->publish_state_(this->discharging_overcurrent_protection_delay_sensor_, (float) jk_get_32bit(66) * 0.001f);
  
  // 70 [64]   4   0x3C 0x00 0x00 0x00    Discharge OCP recovery time         TIMBatDcOCPRDly  Discharge Overcurrent
  // Protection Release Delay
  ESP_LOGV(TAG, "  Discharge OCP recovery time: %f s", (float) jk_get_32bit(70));
  this->publish_state_(this->discharging_overcurrent_protection_recovery_delay_sensor_, (float) jk_get_32bit(70) * 0.001f);
  
  // 74 [68]   4   0x3C 0x00 0x00 0x00    SCPR delay                          TIMBatSCPRDly    Short Circuit Protection Recovery Delay
  ESP_LOGV(TAG, "  SCP recovery time: %f s", (float) jk_get_32bit(74));
  this->publish_state_(this->short_circuit_protection_recovery_delay_sensor_, (float) jk_get_32bit(74) * 0.001f);

  // 78 [72]   4   0xD0 0x07 0x00 0x00    Max balance current
  ESP_LOGV(TAG, "  Max. balance current: %f A", (float) jk_get_32bit(78) * 0.001f);
  this->publish_state_(this->max_balancing_current_sensor_, (float) jk_get_32bit(78) * 0.001f);

  // 82 [76]   4   0xBC 0x02 0x00 0x00    Charge OTP                          TMPBatCOT        Charging Over Temperature Protection
  float value_f=int32_to_float(&data[82])*0.1f;
  ESP_LOGV(TAG, "  Charging OTP: %f °C", value_f);
  this->publish_state_(this->charging_overtemperature_protection_sensor_, value_f);
  
  // 86 [80]   4   0x58 0x02 0x00 0x00    Charge OTP Recovery                 TMPBatCOTPR      Charging Over Temperature Protection Recovery
  value_f=int32_to_float(&data[86])*0.1f;
  ESP_LOGV(TAG, "  Charge OTP recovery: %f °C", value_f);
  this->publish_state_(this->charging_overtemperature_protection_recovery_sensor_, value_f);

  // 90 [84]    4   0xBC 0x02 0x00 0x00    Discharge OTP                       TMPBatDcOT
  value_f=int32_to_float(&data[90])*0.1f;
  ESP_LOGV(TAG, "  Discharge OTP: %f °C", value_f);
  this->publish_state_(this->discharging_overtemperature_protection_sensor_, value_f);

  // 94 [88]    4   0x58 0x02 0x00 0x00    Discharge OTP Recovery              TMPBatDcOTPR
  value_f=int32_to_float(&data[94])*0.1f;
  ESP_LOGV(TAG, "  Discharge OTP recovery: %f °C", value_f);
  this->publish_state_(this->discharging_overtemperature_protection_recovery_sensor_, value_f);

  // 98 [92]   4   0x38 0xFF 0xFF 0xFF    Charge UTP                          TMPBatCUT        Charging Low Temperature Protection
  value_f=int32_to_float(&data[98])*0.1f;
  ESP_LOGI(TAG, "  Charge UTP: %f °C", value_f);
  this->publish_state_(this->charging_lowtemperature_protection_sensor_, value_f);
  
  // 102 [96]   4   0x9C 0xFF 0xFF 0xFF    Charge UTP Recovery                 TMPBatCUTPR      Charging Low Temperature Protection Recovery
  value_f=int32_to_float(&data[102])*0.1f;
  ESP_LOGI(TAG, "  Charge UTP recovery: %f °C", value_f);
  this->publish_state_(this->charging_lowtemperature_protection_recovery_sensor_,value_f);
  
  // 106 [100]  4   0x84 0x03 0x00 0x00    MOS OTP                             TMPMosOT         MOS Overtemperature Protection
  value_f=int32_to_float(&data[106])*0.1f;
  ESP_LOGI(TAG, "  MOS OTP: %f °C", value_f);
  this->publish_state_(this->mos_overtemperature_protection_sensor_, value_f);
  
  // 110 [104]   4   0xBC 0x02 0x00 0x00    MOS OTP Recovery
  value_f=int32_to_float(&data[110])*0.1f;
  ESP_LOGI(TAG, "  MOS OTP recovery: %f °C", value_f);
  this->publish_state_(this->mos_overtemperature_protection_recovery_sensor_, value_f);

  // 114 [108]  4   0x0D 0x00 0x00 0x00    cell count settings
  ESP_LOGI(TAG, "  cell count settings: %f", (float) jk_get_32bit(114));
  this->publish_state_(this->cell_count_settings_sensor_, (float) data[114]);

  // 118 [112]  4   0x01 0x00 0x00 0x00    Charge switch BatChargeEN
  //  ESP_LOGI(TAG, "  Charge switch: %s", ((bool) data[118]) ? "on" : "off");
  this->publish_state_(this->charging_switch_, (bool) data[118]);

  // 122 [116]  4   0x01 0x00 0x00 0x00    Discharge switch
  // ESP_LOGI(TAG, "  Discharge switch: %s", ((bool) data[122]) ? "on" : "off");
  this->publish_state_(this->discharging_switch_, (bool) data[122]);

  // 126 [120]  4   0x01 0x00 0x00 0x00    Balancer switch
  // ESP_LOGI(TAG, "  Balancer switch: %s", ((bool) data[126]) ? "on" : "off");
  this->publish_state_(this->balancer_switch_, (bool) (data[126]));

  // 130 [124]  4   0x88 0x13 0x00 0x00    Nominal battery capacity CapBatCell  [Nominal_Capacity] (CellInfo)
  // ESP_LOGI(TAG, "  Nominal battery capacity: %f Ah", (float) jk_get_32bit(130) * 0.001f);
  // this->publish_state_(this->battery_capacity_total_setting_sensor_, (float) jk_get_32bit(130) * 0.001f);

  // 134 [128] 4   0xDC 0x05 0x00 0x00    SCP DELAY (us)
  // ESP_LOGI(TAG, "  SCP DELAY: %f us", (float) jk_get_32bit(134) * 0.001f);
  this->publish_state_(this->short_circuit_protection_delay_sensor_, (float) jk_get_32bit(134) * 0.001f);

  // 138 [132]  4   0xE4 0x0C 0x00 0x00    Start balance voltage
  // ESP_LOGI(TAG, "  Start balance voltage: %f V", (float) jk_get_32bit(138) * 0.001f);
  this->publish_state_(this->cell_balancing_starting_voltage_sensor_, (float) jk_get_32bit(138) * 0.001f);

  // 142   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         142: %02X%02X%02X%02X",data[142],data[143],data[144],data[145]);
  // 146   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         146: %02X%02X%02X%02X",data[146],data[147],data[148],data[149]);
  // 150   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         150: %02X%02X%02X%02X",data[150],data[151],data[152],data[153]);
  // 154   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         154: %02X%02X%02X%02X",data[154],data[155],data[156],data[157]);
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
  // for (uint8_t i = 0; i < 24; i++) {
  //  ESP_LOGV(TAG, "  Con. wire resistance %d: %f Ohm", i + 1, (float) jk_get_32bit(i * 4 + 158) * 0.001f);
  //}

  // 254   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         254: %02X%02X%02X%02X",data[254],data[255],data[256],data[257]);
  // 258   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         258: %02X%02X%02X%02X",data[258],data[259],data[260],data[261]);
  // 262   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         262: %02X%02X%02X%02X",data[262],data[263],data[264],data[265]);
  // 266   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "         266: %02X%02X%02X%02X",data[266],data[267],data[268],data[269]);
  // 270 [264]   4   0x00 0x00 0x00 0x00 (USED BY SNIFFER)
  // ESP_LOGI(TAG, "  Device address: 0x%02X", data[270]);
  // ESP_LOGI(TAG, "         270: %02X%02X%02X%02X",data[270],data[271],data[272],data[273]);
  
  // 274 [268]  4   0x00 0x00 0x00 0x00    TIMProdischarge: Discharge pre-charging time
  // ESP_LOGI(TAG, "         274: %02X%02X%02X%02X",data[274],data[275],data[276],data[277]);
  ESP_LOGI(TAG, "  Precharging time from discharged: %f s", (float) ((int32_t) jk_get_32bit(274)));
  this->publish_state_(this->precharging_time_from_discharge_sensor_,(float) ((int32_t) jk_get_32bit(274)));  

  // 278   4   0x00 0x00 0x00 0x00  //60 e3 16 00          10023c3218feffffffbfe90102000000000001
  ESP_LOGI(TAG, "      unkown 278: %02X%02X%02X%02X",data[278],data[279],data[280],data[281]);

  // 282 [276]   1   0x00                   New controls bitmask
  // ** [JK-PB2A16S-20P v14]
  //    bit0: HEATING_SWITCH_ENABLED                 1
  //    bit1: DISABLE_TEMP_SENSOR_SWITCH_ENABLED     2
  //    bit2: GPS Heartbeat                          4
  //    bit3: port switch (1:RS485|0:CAN)            8
  //    bit4: DISPLAY_ALWAYS_ON_SWITCH_ENABLED       16
  //    bit5: Special Charger                        32
  //    bit6: SMART_SLEEP_ON_SWITCH_ENABLED          64
  //    bit7: DISABLE_PCL_MODULE_SWITCH_ENABLED      128
  this->publish_state_(this->heating_switch_, (bool) this->check_bit_(data[282], 1));
  // ESP_LOGI(TAG, "  heating switch: %s", ( this->check_bit_(data[282], 1)) ? "on" : "off");
  this->publish_state_(this->disable_temperature_sensors_switch_, this->check_bit_(data[282], 2));
  this->publish_state_(this->gps_heartbeat_switch_, this->check_bit_(data[282], 4));
  this->publish_state_(this->port_selection_switch_, this->check_bit_(data[282], 8));
  // ESP_LOGI(TAG, "  Port switch: %s", this->check_bit_(data[282], 8) ? "RS485" : "CAN");
  this->publish_state_(this->display_always_on_switch_, this->check_bit_(data[282], 16));
  this->publish_state_(this->special_charger_switch_, this->check_bit_(data[282], 32));
  this->publish_state_(this->smart_sleep_on_switch_, check_bit_(data[282], 64));
  this->publish_state_(this->disable_pcl_module_switch_, check_bit_(data[282], 128));

  // 283 [28?]   1   0x00                   New controls bitmask
  // ** [JK-PB2A16S-20P v14]
  //    bit0: TIMED_STORED_DATA_SWITCH_ENABLED       1
  //    bit1: CHARGING_FLOAT_MODE_SWITCH_ENABLED     2
  //    bit2: ?                                      4
  //    bit3: ?                                      8
  //    bit4: ?                                      16
  //    bit5: ?                                      32
  //    bit6: ?                                      64
  //    bit7: ?                                      128
  this->publish_state_(this->timed_stored_data_switch_, (bool) this->check_bit_(data[283], 1));
  // ESP_LOGI(TAG, "  timed_stored_data_switch: %s", ( this->check_bit_(data[283], 1)) ? "on" : "off");
  this->publish_state_(this->charging_float_mode_switch_, (bool) this->check_bit_(data[283], 2));
  // ESP_LOGI(TAG, "  charging_float_mode_switch: %s", ( this->check_bit_(data[283], 2)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit2: %s", ( this->check_bit_(data[283], 3)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit3: %s", ( this->check_bit_(data[283], 4)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit4: %s", ( this->check_bit_(data[283], 5)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit5: %s", ( this->check_bit_(data[283], 6)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit6: %s", ( this->check_bit_(data[283], 7)) ? "on" : "off");
  // ESP_LOGI(TAG, "  switch bit7: %s", ( this->check_bit_(data[283], 8)) ? "on" : "off");

  // 284   2   0X00 0X00
  // 286   4   0x00 0x00 0x00 0x00
  // ESP_LOGI(TAG, "  TIMSmartSleep: %d H", (uint8_t) (data[286]));
  this->publish_state_(this->smart_sleep_time_sensor_, (uint8_t) (data[286]));
  ESP_LOGI(TAG, "  Data field enable control 0: %d", (uint8_t) (data[287]));
  

  // 290   4   0x00 0x00 0x00 0x00
  // 294   4   0x00 0x00 0x00 0x00
  // 298   1   0x00
  // 299   1   0x40                   CHECKSUM
}

void JkRS485Bms::update() { this->track_status_online_(); }

void JkRS485Bms::decode_device_info_(const std::vector<uint8_t> &data) {
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

  ESP_LOGV(TAG, "  Vendor ID: %s", std::string(data.begin() + 6, data.begin() + 6 + 16).c_str());
  ESP_LOGV(TAG, "  Hardware version: %s", std::string(data.begin() + 22, data.begin() + 22 + 8).c_str());
  ESP_LOGV(TAG, "  Software version: %s", std::string(data.begin() + 30, data.begin() + 30 + 8).c_str());
  ESP_LOGV(TAG, "  Uptime: %d s", jk_get_32bit(38));
  ESP_LOGV(TAG, "  Power on count: %d", jk_get_32bit(42));
  ESP_LOGV(TAG, "  Device name: %s", std::string(data.begin() + 46, data.begin() + 46 + 16).c_str());
  ESP_LOGV(TAG, "  Device passcode: %s", std::string(data.begin() + 62, data.begin() + 62 + 16).c_str());
  ESP_LOGV(TAG, "  Manufacturing date: %s", std::string(data.begin() + 78, data.begin() + 78 + 8).c_str());
  ESP_LOGV(TAG, "  Serial number: %s", std::string(data.begin() + 86, data.begin() + 86 + 11).c_str());
  ESP_LOGV(TAG, "  Passcode: %s", std::string(data.begin() + 97, data.begin() + 97 + 5).c_str());
  ESP_LOGV(TAG, "  User data: %s", std::string(data.begin() + 102, data.begin() + 102 + 16).c_str());
  ESP_LOGV(TAG, "  Setup passcode: %s", std::string(data.begin() + 118, data.begin() + 118 + 16).c_str());

  ESP_LOGV(TAG, "  UART1 Protocol Number:     0x%02X", ((uint8_t) data[178]));
  ESP_LOGV(TAG, "  CAN   Protocol Number:     0x%02X", ((uint8_t) data[179]));  
  ESP_LOGV(TAG, "  UART2 Protocol Number:     0x%02X", ((uint8_t) data[212]));
  ESP_LOGV(TAG, "  UART2 Protocol Enabled[0]: 0x%02X", ((uint8_t) data[213]));

  ESP_LOGV(TAG, "  RCV Time: %f h", (float) ((uint8_t) data[266]) * 0.1f);
  ESP_LOGV(TAG, "  RFV Time: %f h", (float) ((uint8_t) data[267]) * 0.1f);
  ESP_LOGV(TAG, "  CAN Protocol Library Version: %f", (float) ((uint8_t) data[268]));
  ESP_LOGV(TAG, "  RVD: %f", (float) ((uint8_t) data[269]));
  ESP_LOGV(TAG, "  ---------------------------------------");


  this->publish_state_(this->info_vendorid_text_sensor_, std::string(data.begin() + 6, data.begin() + 6 + 16).c_str());
  this->publish_state_(this->info_hardware_version_text_sensor_, std::string(data.begin() + 22, data.begin() + 22 + 8).c_str());
  this->publish_state_(this->info_software_version_text_sensor_, std::string(data.begin() + 30, data.begin() + 30 + 8).c_str());
  this->publish_state_(this->info_device_name_text_sensor_, std::string(data.begin() + 46, data.begin() + 46 + 16).c_str());
  this->publish_state_(this->info_device_password_text_sensor_, std::string(data.begin() + 62, data.begin() + 62 + 16).c_str());
  this->publish_state_(this->info_device_serial_number_text_sensor_, std::string(data.begin() + 86, data.begin() + 86 + 11).c_str());
  this->publish_state_(this->info_device_setup_passcode_text_sensor_, std::string(data.begin() + 118, data.begin() + 118 + 16).c_str());

  this->publish_state_(this->uart1_protocol_number_sensor_, (uint8_t) data[178]);
  this->publish_state_(this->uart2_protocol_number_sensor_, (uint8_t) data[212]);

  this->publish_state_(this->cell_request_charge_voltage_time_sensor_, (float) data[266]*0.1f);
  this->publish_state_(this->cell_request_float_voltage_time_sensor_, (float) data[267]*0.1f);


}

void JkRS485Bms::track_status_online_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkRS485Bms::reset_status_online_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->status_online_binary_sensor_, true);
}

void JkRS485Bms::publish_device_unavailable_() {
  this->publish_state_(this->status_online_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(cell_voltage_min_sensor_, NAN);
  this->publish_state_(cell_voltage_max_sensor_, NAN);
  this->publish_state_(cell_voltage_min_cell_number_sensor_, NAN);
  this->publish_state_(cell_voltage_max_cell_number_sensor_, NAN);
  this->publish_state_(cell_delta_voltage_sensor_, NAN);
  this->publish_state_(cell_average_voltage_sensor_, NAN);
  this->publish_state_(temperature_powertube_sensor_, NAN);
  this->publish_state_(temperature_sensor_1_sensor_, NAN);
  this->publish_state_(temperature_sensor_2_sensor_, NAN);
  this->publish_state_(battery_voltage_sensor_, NAN);
  this->publish_state_(battery_current_sensor_, NAN);
  this->publish_state_(battery_power_sensor_, NAN);
  this->publish_state_(battery_power_charging_sensor_, NAN);
  this->publish_state_(battery_power_discharging_sensor_, NAN);
  this->publish_state_(battery_capacity_remaining_sensor_, NAN);
  this->publish_state_(battery_capacity_remaining_derived_sensor_, NAN);
  this->publish_state_(temperature_sensors_sensor_, NAN);
  this->publish_state_(charging_cycles_sensor_, NAN);
  this->publish_state_(battery_capacity_total_charging_cycle_sensor_, NAN);
  this->publish_state_(battery_strings_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(operation_mode_bitmask_sensor_, NAN);
  this->publish_state_(total_voltage_overvoltage_protection_sensor_, NAN);
  this->publish_state_(total_voltage_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_overvoltage_protection_sensor_, NAN);
  this->publish_state_(cell_overvoltage_protection_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_delay_sensor_, NAN);
  this->publish_state_(cell_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_undervoltage_protection_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_delay_sensor_, NAN);
  this->publish_state_(cell_pressure_difference_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(charging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(charging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(cell_balancing_starting_voltage_sensor_, NAN);
  this->publish_state_(balancing_opening_pressure_difference_sensor_, NAN);
  this->publish_state_(powertube_temperature_protection_sensor_, NAN);
  this->publish_state_(powertube_temperature_protection_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_protection_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_difference_protection_sensor_, NAN);
  this->publish_state_(charging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(battery_capacity_total_setting_sensor_, NAN);
  this->publish_state_(charging_sensor_, NAN);
  this->publish_state_(discharging_sensor_, NAN);
  this->publish_state_(current_calibration_sensor_, NAN);
  this->publish_state_(device_address_sensor_, NAN);
  this->publish_state_(sleep_wait_time_sensor_, NAN);
  this->publish_state_(alarm_low_volume_sensor_, NAN);
  this->publish_state_(password_sensor_, NAN);
  this->publish_state_(manufacturing_date_sensor_, NAN);
  this->publish_state_(battery_total_runtime_sensor_, NAN);
  this->publish_state_(total_runtime_formatted_text_sensor_, "NAN");

  this->publish_state_(start_current_calibration_sensor_, NAN);
  this->publish_state_(actual_battery_capacity_sensor_, NAN);
  this->publish_state_(protocol_version_sensor_, NAN);
  this->publish_state_(cell_request_float_voltage_time_sensor_, NAN);
  this->publish_state_(cell_request_charge_voltage_time_sensor_, NAN);
  

  for (auto &cell : this->cells_) {
    this->publish_state_(cell.cell_voltage_sensor_, NAN);
  }
}

void JkRS485Bms::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void JkRS485Bms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void JkRS485Bms::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void JkRS485Bms::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void JkRS485Bms::publish_alarm_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;
  battery_total_alarms_count_++;
  if (state) {
    battery_total_alarms_active_++;
  }
  binary_sensor->publish_state(state);
}
std::string JkRS485Bms::error_bits_to_string_(const uint16_t mask) {
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

std::string JkRS485Bms::mode_bits_to_string_(const uint16_t mask) {
  bool first = true;
  std::string modes_list = "";

  if (mask) {
    for (int i = 0; i < OPERATION_MODES_SIZE; i++) {
      if (mask & (1 << i)) {
        if (first) {
          first = false;
        } else {
          modes_list.append(";");
        }
        modes_list.append(OPERATION_MODES[i]);
      }
    }
  }

  return modes_list;
}

void JkRS485Bms::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "JkRS485Bms:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Minimum Cell Voltage", this->cell_voltage_min_sensor_);
  LOG_SENSOR("", "Maximum Cell Voltage", this->cell_voltage_max_sensor_);
  LOG_SENSOR("", "Minimum Voltage Cell", this->cell_voltage_min_cell_number_sensor_);
  LOG_SENSOR("", "Maximum Voltage Cell", this->cell_voltage_max_cell_number_sensor_);
  LOG_SENSOR("", "cell delta voltage", this->cell_delta_voltage_sensor_);
  LOG_SENSOR("", "cell average voltage", this->cell_average_voltage_sensor_);
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
  LOG_SENSOR("", "Temperature powertube", this->temperature_powertube_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
  LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Current", this->battery_current_sensor_);
  LOG_SENSOR("", "Power", this->battery_power_sensor_);
  LOG_SENSOR("", "Battery power charging", this->battery_power_charging_sensor_);
  LOG_SENSOR("", "Battery power discharging", this->battery_power_discharging_sensor_);
  LOG_SENSOR("", "Battery capacity remaining", this->battery_capacity_remaining_sensor_);
  LOG_SENSOR("", "Battery capacity remaining Derived", this->battery_capacity_remaining_derived_sensor_);
  LOG_SENSOR("", "Temperature Sensors", this->temperature_sensors_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Battery capacity total charging cycle", this->battery_capacity_total_charging_cycle_sensor_);
  LOG_SENSOR("", "Battery Strings", this->battery_strings_sensor_);
  LOG_SENSOR("", "Errors Bitmask", this->errors_bitmask_sensor_);
  LOG_SENSOR("", "Operation Mode Bitmask", this->operation_mode_bitmask_sensor_);
  LOG_SENSOR("", "Battery voltage Overvoltage Protection", this->total_voltage_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Battery voltage Undervoltage Protection", this->total_voltage_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Overvoltage Protection", this->cell_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Overvoltage Protection Recovery", this->cell_overvoltage_protection_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Delay", this->cell_voltage_overvoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Undervoltage Protection", this->cell_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Undervoltage Protection Recovery", this->cell_undervoltage_protection_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Delay", this->cell_voltage_undervoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Pressure Difference Protection", this->cell_pressure_difference_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Protection", this->discharging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Delay", this->discharging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Protection", this->charging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Delay", this->charging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Balance Starting Voltage", this->cell_balancing_starting_voltage_sensor_);
  LOG_SENSOR("", "BALANCING OPENING PRESSURE DIFFERENCE", this->balancing_opening_pressure_difference_sensor_);
  LOG_SENSOR("", "Temperature powertube Protection", this->powertube_temperature_protection_sensor_);
  LOG_SENSOR("", "Powertube temperature protection recovery", this->powertube_temperature_protection_recovery_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Protection", this->temperature_sensor_temperature_protection_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Recovery", this->temperature_sensor_temperature_recovery_sensor_);
  LOG_SENSOR("", "Temperature Sensor Temperature Difference Protection",
             this->temperature_sensor_temperature_difference_protection_sensor_);
  LOG_SENSOR("", "Charging High Temperature Protection", this->charging_high_temperature_protection_sensor_);
  LOG_SENSOR("", "Discharging High Temperature Protection", this->discharging_high_temperature_protection_sensor_);
  LOG_SENSOR("", "Charging Low Temperature Protection", this->charging_low_temperature_protection_sensor_);
  LOG_SENSOR("", "Charging Low Temperature Recovery", this->charging_low_temperature_recovery_sensor_);
  LOG_SENSOR("", "Discharging Low Temperature Protection", this->discharging_low_temperature_protection_sensor_);
  LOG_SENSOR("", "Discharging Low Temperature Recovery", this->discharging_low_temperature_recovery_sensor_);
  LOG_SENSOR("", "Battery capacity total setting", this->battery_capacity_total_setting_sensor_);
  LOG_SENSOR("", "Current Calibration", this->current_calibration_sensor_);
  LOG_SENSOR("", "Device Address", this->device_address_sensor_);
  LOG_TEXT_SENSOR("", "Battery Type", this->battery_type_text_sensor_);
  LOG_SENSOR("", "Sleep Wait Time", this->sleep_wait_time_sensor_);
  LOG_SENSOR("", "Alarm Low Volume", this->alarm_low_volume_sensor_);
  LOG_TEXT_SENSOR("", "Password", this->password_text_sensor_);
  LOG_TEXT_SENSOR("", "Device Type", this->device_type_text_sensor_);
  LOG_SENSOR("", "Manufacturing Date", this->manufacturing_date_sensor_);
  LOG_SENSOR("", "Battery total runtime", this->battery_total_runtime_sensor_);
  LOG_TEXT_SENSOR("", "Software Version", this->software_version_text_sensor_);
  LOG_SENSOR("", "Start Current Calibration", this->start_current_calibration_sensor_);
  LOG_TEXT_SENSOR("", "Manufacturer", this->manufacturer_text_sensor_);
  LOG_SENSOR("", "Protocol Version", this->protocol_version_sensor_);
  LOG_SENSOR("", "Protocol Version", this->cell_request_charge_voltage_time_sensor_);
  LOG_SENSOR("", "Protocol Version", this->cell_request_float_voltage_time_sensor_);
  LOG_BINARY_SENSOR("", "Status Balancing", this->status_balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Balancing Switch", this->balancing_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Status Charging", this->status_charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging Switch", this->charging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Status Discharging", this->status_discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging Switch", this->discharging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Dedicated Charger Switch", this->dedicated_charger_switch_binary_sensor_);
  LOG_TEXT_SENSOR("", "Battery total runtime Formatted", this->total_runtime_formatted_text_sensor_);
}

}  // namespace jk_rs485_bms
}  // namespace esphome
