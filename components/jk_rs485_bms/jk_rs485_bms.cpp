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
    "Power tube overtemperature",                // Byte 0.1, alarm
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


void JkRS485Bms::on_jk_rs485_sniffer_data(const uint8_t &origin_address, const uint8_t &frame_type, const std::vector<uint8_t> &data) {
  //this->reset_online_status_tracker_();

  if (origin_address == this->address_){
    //this->reset_online_status_tracker_();
      ESP_LOGV(TAG, "This BMS address is: %d  and address received %d ==> WORKING", this->address_, origin_address);
      switch (frame_type) {
        case 0x01:
          if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
            //this->decode_jk04_settings_(data);
          } else {
            ESP_LOGI(TAG, "Decoding settings............................................................................................................................................................");
            this->decode_jk02_settings_(data);
          }
          break;
        case 0x02:
          if (this->protocol_version_ == PROTOCOL_VERSION_JK04) {
            //this->decode_jk04_cell_info_(data);
          } else {
            ESP_LOGI(TAG, "Decoding cell info............................................................................................................................................................");
            this->decode_jk02_cell_info_(data);
          }
          break;
        case 0x03:
          //this->decode_device_info_(data);
          break;
        default:
          ESP_LOGW(TAG, "Unsupported message type (0x%02X)", data[4]);
      }    
  } else {
         ESP_LOGV(TAG, "This BMS address is: %d  and address received %d ==> IDLE", this->address_, origin_address);
  }  



  

}


void JkRS485Bms::decode_jk02_cell_info_(const std::vector<uint8_t> &data) {
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
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_get_16bit(i * 2 + 6) * 0.001f;
    float cell_resistance = (float) jk_get_16bit(i * 2 + 64 + offset) * 0.001f;
    if (cell_voltage > 0 && cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
    this->publish_state_(this->cells_[i].cell_resistance_sensor_, cell_resistance);
    ESP_LOGV(TAG, "Cell %d voltage:    %f",i,cell_voltage);
    ESP_LOGV(TAG, "Cell %d resistance: %f",i,cell_resistance);
  }
  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);

  ESP_LOGV(TAG, "Cell MAX voltage:    %f",max_cell_voltage);
  ESP_LOGV(TAG, "Cell MAX voltage:    %f",min_cell_voltage);

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
  this->publish_state_(this->average_cell_voltage_sensor_, (float) jk_get_16bit(58 + offset) * 0.001f);

  // 60    2   0x00 0x00              Delta Cell Voltage    0.001        V
  this->publish_state_(this->delta_cell_voltage_sensor_, (float) jk_get_16bit(60 + offset) * 0.001f);

  // 62    1   0x00                   Max voltage cell      1
  this->publish_state_(this->max_voltage_cell_sensor_, (float) data[62 + offset] + 1);
  // 63    1   0x00                   Min voltage cell      1
  this->publish_state_(this->min_voltage_cell_sensor_, (float) data[63 + offset] + 1);
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
  //           0x00 0x04                                                     0000 0000 0000 0100
  //           0x00 0x08                Cell Undervoltage                    0000 0000 0000 1000
  //           0x00 0x10                                                     0000 0000 0001 0000
  //           0x00 0x20                                                     0000 0000 0010 0000
  //           0x00 0x40                                                     0000 0000 0100 0000
  //           0x00 0x80                                                     0000 0000 1000 0000
  //           0x01 0x00                                                     0000 0001 0000 0000
  //           0x02 0x00                                                     0000 0010 0000 0000
  //           0x04 0x00                Cell count is not equal to settings  0000 0100 0000 0000
  //           0x08 0x00                Current sensor anomaly               0000 1000 0000 0000
  //           0x10 0x00                Cell Over Voltage                    0001 0000 0000 0000
  //           0x20 0x00                                                     0010 0000 0000 0000
  //           0x40 0x00                                                     0100 0000 0000 0000
  //           0x80 0x00                                                     1000 0000 0000 0000
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
  this->publish_state_(this->balancing_binary_sensor_, (data[140 + offset] != 0x00));

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

  // 158   1   0x64                   SOCSOH
  ESP_LOGD(TAG, "SOCSOH: 0x%02X (always 0x64?)", data[158 + offset], data[159 + offset]);
  // 159   1   0x00                   Precharge
  ESP_LOGD(TAG, "SOCSOH: 0x%02X (always 0x00?)", data[159 + offset], data[159 + offset]);

  // 160   2   0x79 0x04              UserAlarm 
  ESP_LOGD(TAG, "UserAlarm: 0x%02X 0x%02X (always 0xC5 0x09?)", data[160 + offset], data[161 + offset]);

  // 162   4   0xCA 0x03 0x10 0x00    Total runtime in seconds           s
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_32bit(162 + offset));
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(jk_get_32bit(162 + offset)));

  // 166   1   0x01                   Charging mosfet enabled                      0x00: off, 0x01: on
  this->publish_state_(this->charging_binary_sensor_, (bool) data[166 + offset]);

  // 167   1   0x01                   Discharging mosfet enabled                   0x00: off, 0x01: on
  this->publish_state_(this->discharging_binary_sensor_, (bool) data[167 + offset]);

  ESP_LOGD(TAG, "Unknown168: %s",
           format_hex_pretty(&data.front() + 168 + offset, data.size() - (168 + offset) - 4 - 81 - 1).c_str());

  // 168   1   0xAA                   Unknown168
  // 169   2   0x06 0x00              Unknown169
  // 171   2   0x00 0x00              Unknown171
  // 173   2   0x00 0x00              Unknown173
  // 175   2   0x00 0x00              Unknown175
  // 177   2   0x00 0x00              Unknown177
  // 179   2   0x00 0x00              Unknown179
  // 181   2   0x00 0x07              Unknown181
  // 183   2   0x00 0x01              Unknown183
  // 185   2   0x00 0x00              Unknown185
  // 187   2   0x00 0xD5              Unknown187
  // 189   2   0x02 0x00              Unknown189
  ESP_LOGD(TAG, "Unknown189: 0x%02X 0x%02X", data[189], data[190]);
  // 190   1   0x00                   Unknown190
  // 191   1   0x00                   Balancer status (working: 0x01, idle: 0x00)
  // 192   1   0x00                   Unknown192
  ESP_LOGD(TAG, "Unknown192: 0x%02X", data[192 + offset]);
  // 193   2   0x00 0xAE              Unknown193
  ESP_LOGD(TAG, "Unknown193: 0x%02X 0x%02X (0x00 0x8D)", data[193 + offset], data[194 + offset]);
  // 195   2   0xD6 0x3B              Unknown195
  ESP_LOGD(TAG, "Unknown195: 0x%02X 0x%02X (0x21 0x40)", data[195 + offset], data[196 + offset]);
  // 197   10  0x40 0x00 0x00 0x00 0x00 0x58 0xAA 0xFD 0xFF 0x00
  // 207   7   0x00 0x00 0x01 0x00 0x02 0x00 0x00
  // 214   4   0xEC 0xE6 0x4F 0x00    Uptime 100ms
  //
  // 218   81  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //           0x00

  // 224   1   0x01                   Heating status          0x00: off, 0x01: on
  this->publish_state_(this->heating_binary_sensor_, (bool) data[224 + offset]);

  // 236   2   0x01 0xFD              Heating current         0.001         A
  this->publish_state_(this->heating_current_sensor_, (float) ((int16_t) jk_get_16bit(236 + offset)) * 0.001f);

  if (frame_version == FRAME_VERSION_JK02_32S) {
    uint16_t raw_emergency_time_countdown = jk_get_16bit(186 + offset);
    ESP_LOGI(TAG, "  Emergency switch: %s", (raw_emergency_time_countdown > 0) ? "on" : "off");
    this->publish_state_(this->emergency_switch_, raw_emergency_time_countdown > 0);
    this->publish_state_(this->emergency_time_countdown_sensor_, (float) raw_emergency_time_countdown * 1.0f);

    this->publish_state_(this->temperatures_[3].temperature_sensor_,
                         (float) ((int16_t) jk_get_16bit(224 + offset)) * 0.1f);
    this->publish_state_(this->temperatures_[2].temperature_sensor_,
                         (float) ((int16_t) jk_get_16bit(226 + offset)) * 0.1f);                       
  }

  // 299   1   0xCD                   CHECKSUM

  this->status_notification_received_ = true;
}


void JkRS485Bms::decode_jk02_settings_(const std::vector<uint8_t> &data) {
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
  // 6     4   0x58 0x02 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE SMART SLEEP
  ESP_LOGD(TAG, "  Voltage Smart Sleep: %f", (float) jk_get_32bit(6) * 0.001f);
  this->publish_state_(this->voltage_smart_sleep_number_, (float) jk_get_32bit(6) * 0.001f);
  // 10    4   0x54 0x0B 0x00 0x00    Cell UVP
  ESP_LOGI(TAG, "  Cell UVP: %f V", (float) jk_get_32bit(10) * 0.001f);
  this->publish_state_(this->cell_voltage_undervoltage_protection_number_, (float) jk_get_32bit(10) * 0.001f);

  // 14    4   0x80 0x0C 0x00 0x00    Cell UVP Recovery
  ESP_LOGI(TAG, "  Cell UVPR: %f V", (float) jk_get_32bit(14) * 0.001f);
  this->publish_state_(this->cell_voltage_undervoltage_recovery_number_, (float) jk_get_32bit(14) * 0.001f);

  // 18    4   0xCC 0x10 0x00 0x00    Cell OVP
  ESP_LOGI(TAG, "  Cell OVP: %f V", (float) jk_get_32bit(18) * 0.001f);
  this->publish_state_(this->cell_voltage_overvoltage_protection_number_, (float) jk_get_32bit(18) * 0.001f);

  // 22    4   0x68 0x10 0x00 0x00    Cell OVP Recovery
  ESP_LOGI(TAG, "  Cell OVPR: %f V", (float) jk_get_32bit(22) * 0.001f);
  this->publish_state_(this->cell_voltage_overvoltage_recovery_number_, (float) jk_get_32bit(22) * 0.001f);

  // 26    4   0x0A 0x00 0x00 0x00    Balance trigger voltage
  ESP_LOGI(TAG, "  Balance trigger voltage: %f V", (float) jk_get_32bit(26) * 0.001f);
  this->publish_state_(this->balance_trigger_voltage_number_, (float) jk_get_32bit(26) * 0.001f);

  // 30    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC-100% VOLTAGE
  ESP_LOGI(TAG, "  SOC-100 VOLTAGE: %f V", (float) jk_get_32bit(30) * 0.001f);
  this->publish_state_(this->soc100_voltage_number_, (float) jk_get_32bit(30) * 0.001f);
  // 34    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] SOC-0% VOLTAGE
  ESP_LOGI(TAG, "  SOC-0 VOLTAGE: %f V", (float) jk_get_32bit(34) * 0.001f);
  this->publish_state_(this->soc0_voltage_number_, (float) jk_get_32bit(34) * 0.001f);  
  // 38    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE CELL REQUEST CHARGE VOLTAGE [RCV]
  ESP_LOGI(TAG, "  VOLTAGE CELL REQUEST CHARGE VOLTAGE [RCV]: %f V", (float) jk_get_32bit(38) * 0.001f);
  this->publish_state_(this->voltage_cell_request_charge_voltage_number_, (float) jk_get_32bit(38) * 0.001f);  
  // 42    4   0x00 0x00 0x00 0x00    ** [JK-PB2A16S-20P v14] VOLTAGE CELL REQUEST FLOAT VOLTAGE
  ESP_LOGI(TAG, "  VOLTAGE CELL REQUEST FLOAT VOLTAGE [RFV]: %f V", (float) jk_get_32bit(42) * 0.001f);
  this->publish_state_(this->voltage_cell_request_float_voltage_number_, (float) jk_get_32bit(42) * 0.001f);   
  // 46    4   0xF0 0x0A 0x00 0x00    Power off voltage
  ESP_LOGI(TAG, "  Power off voltage: %f V", (float) jk_get_32bit(46) * 0.001f);
  this->publish_state_(this->power_off_voltage_number_, (float) jk_get_32bit(46) * 0.001f);

  // 50    4   0xA8 0x61 0x00 0x00    Max. charge current
  ESP_LOGI(TAG, "  Max. charge current: %f A", (float) jk_get_32bit(50) * 0.001f);
  this->publish_state_(this->max_charge_current_number_, (float) jk_get_32bit(50) * 0.001f);

  // 54    4   0x1E 0x00 0x00 0x00    Charge OCP delay
  ESP_LOGI(TAG, "  Charge OCP delay: %f s", (float) jk_get_32bit(54));
  this->publish_state_(this->charge_ocp_delay_number_, (float) jk_get_32bit(54) * 0.001f);
  // 58    4   0x3C 0x00 0x00 0x00    Charge OCP recovery time
  ESP_LOGI(TAG, "  Charge OCP recovery delay: %f s", (float) jk_get_32bit(58));
  this->publish_state_(this->charge_ocp_recovery_delay_number_, (float) jk_get_32bit(58) * 0.001f);
  // 62    4   0xF0 0x49 0x02 0x00    Max. discharge current
  ESP_LOGI(TAG, "  Max. discharge current: %f A", (float) jk_get_32bit(62) * 0.001f);
  this->publish_state_(this->max_discharge_current_number_, (float) jk_get_32bit(62) * 0.001f);

  // 66    4   0x2C 0x01 0x00 0x00    Discharge OCP delay
  ESP_LOGI(TAG, "  Discharge OCP delay: %f s", (float) jk_get_32bit(66));
  this->publish_state_(this->discharge_ocp_delay_number_, (float) jk_get_32bit(66) * 0.001f);  
  // 70    4   0x3C 0x00 0x00 0x00    Discharge OCP recovery time
  ESP_LOGI(TAG, "  Discharge OCP recovery time: %f s", (float) jk_get_32bit(70));
  this->publish_state_(this->discharge_ocp_recovery_time_number_, (float) jk_get_32bit(70) * 0.001f);    
  // 74    4   0x3C 0x00 0x00 0x00    SCPR time
  ESP_LOGI(TAG, "  SCP recovery time: %f s", (float) jk_get_32bit(74));
  this->publish_state_(this->scp_recovery_time_number_, (float) jk_get_32bit(74) * 0.001f);  
  // 78    4   0xD0 0x07 0x00 0x00    Max balance current
  ESP_LOGI(TAG, "  Max. balance current: %f A", (float) jk_get_32bit(78) * 0.001f);
  this->publish_state_(this->max_balance_current_number_, (float) jk_get_32bit(78) * 0.001f);

  // 82    4   0xBC 0x02 0x00 0x00    Charge OTP
  ESP_LOGI(TAG, "  Charge OTP: %f °C", (float) jk_get_32bit(82) * 0.1f);
  // 86    4   0x58 0x02 0x00 0x00    Charge OTP Recovery
  ESP_LOGI(TAG, "  Charge OTP recovery: %f °C", (float) jk_get_32bit(86) * 0.1f);
  // 90    4   0xBC 0x02 0x00 0x00    Discharge OTP
  ESP_LOGI(TAG, "  Discharge OTP: %f °C", (float) jk_get_32bit(90) * 0.1f);
  // 94    4   0x58 0x02 0x00 0x00    Discharge OTP Recovery
  ESP_LOGI(TAG, "  Discharge OTP recovery: %f °C", (float) jk_get_32bit(94) * 0.1f);
  // 98    4   0x38 0xFF 0xFF 0xFF    Charge UTP
  ESP_LOGI(TAG, "  Charge UTP: %f °C", (float) ((int32_t) jk_get_32bit(98)) * 0.1f);
  // 102   4   0x9C 0xFF 0xFF 0xFF    Charge UTP Recovery
  ESP_LOGI(TAG, "  Charge UTP recovery: %f °C", (float) ((int32_t) jk_get_32bit(102)) * 0.1f);
  // 106   4   0x84 0x03 0x00 0x00    MOS OTP
  ESP_LOGI(TAG, "  MOS OTP: %f °C", (float) ((int32_t) jk_get_32bit(106)) * 0.1f);
  // 110   4   0xBC 0x02 0x00 0x00    MOS OTP Recovery
  ESP_LOGI(TAG, "  MOS OTP recovery: %f °C", (float) ((int32_t) jk_get_32bit(110)) * 0.1f);
  // 114   4   0x0D 0x00 0x00 0x00    Cell count
  ESP_LOGI(TAG, "  Cell count: %f", (float) jk_get_32bit(114));
  this->publish_state_(this->cell_count_number_, (float) data[114]);

  // 118   4   0x01 0x00 0x00 0x00    Charge switch
  ESP_LOGI(TAG, "  Charge switch: %s", ((bool) data[118]) ? "on" : "off");
  this->publish_state_(this->charging_switch_, (bool) data[118]);

  // 122   4   0x01 0x00 0x00 0x00    Discharge switch
  ESP_LOGI(TAG, "  Discharge switch: %s", ((bool) data[122]) ? "on" : "off");
  this->publish_state_(this->discharging_switch_, (bool) data[122]);

  // 126   4   0x01 0x00 0x00 0x00    Balancer switch
  ESP_LOGI(TAG, "  Balancer switch: %s", ((bool) data[126]) ? "on" : "off");
  this->publish_state_(this->balancer_switch_, (bool) (data[126]));

  // 130   4   0x88 0x13 0x00 0x00    Nominal battery capacity
  ESP_LOGI(TAG, "  Nominal battery capacity: %f Ah", (float) jk_get_32bit(130) * 0.001f);
  this->publish_state_(this->total_battery_capacity_number_, (float) jk_get_32bit(130) * 0.001f);

  // 134   4   0xDC 0x05 0x00 0x00    SCP DELAY (us) 
  ESP_LOGI(TAG, "  SCP DELAY: %f us", (float) jk_get_32bit(134) * 0.001f);
  //this->publish_state_(this->scp_delay_number_, (float) jk_get_32bit(134) * 0.001f);
  // 138   4   0xE4 0x0C 0x00 0x00    Start balance voltage
  ESP_LOGI(TAG, "  Start balance voltage: %f V", (float) jk_get_32bit(138) * 0.001f);
  this->publish_state_(this->balance_starting_voltage_number_, (float) jk_get_32bit(138) * 0.001f);

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
    ESP_LOGV(TAG, "  Con. wire resistance %d: %f Ohm", i + 1, (float) jk_get_32bit(i * 4 + 158) * 0.001f);
  }

  // 254   4   0x00 0x00 0x00 0x00
  // 258   4   0x00 0x00 0x00 0x00
  // 262   4   0x00 0x00 0x00 0x00
  // 266   4   0x00 0x00 0x00 0x00
  // 270   4   0x00 0x00 0x00 0x00
  ESP_LOGI(TAG, "  Device address: 0x%02X", data[270]);
  // 274   4   0x00 0x00 0x00 0x00
  // 278   4   0x00 0x00 0x00 0x00
  // 282   1   0x00                   New controls bitmask
  // ** [JK-PB2A16S-20P v14] 
  //    bit0: HEATING_SWITCH_ENABLED
  //    bit1: DISABLE_TEMP_SENSOR_SWITCH_ENABLED
  //    bit2: ?
  //    bit3: ?
  //    bit4: DISPLAY_ALWAYS_ON_SWITCH_ENABLED
  //    bit5: ?
  //    bit6: SMART_SLEEP_ON_SWITCH_ENABLED
  //    bit7: DISABLE_PCL_MODULE_SWITCH_ENABLED
  this->publish_state_(this->heating_switch_, this->check_bit_(data[282], 1));
  ESP_LOGI(TAG, "  heating switch: %s", ((bool) this->check_bit_(data[282], 1)) ? "on" : "off");
  this->publish_state_(this->disable_temperature_sensors_switch_, this->check_bit_(data[282], 2));
  this->publish_state_(this->display_always_on_switch_, this->check_bit_(data[282], 16));
  ESP_LOGI(TAG, "  Port switch: %s", this->check_bit_(data[282], 8) ? "RS485" : "CAN");

  // 283   1   0x00
  // ** [JK-PB2A16S-20P v14] 
  //    bit0: TIME_STORED_DATA_SWITCH_ENABLED
  //    bit1: CHARGING_FLOAT_MODE_SWITCH_ENABLED
  // 284   2   0X00 0X00
  // 286   4   0x00 0x00 0x00 0x00
  // 290   4   0x00 0x00 0x00 0x00
  // 294   4   0x00 0x00 0x00 0x00
  // 298   1   0x00
  // 299   1   0x40                   CHECKSUM
}


void JkRS485Bms::decode_jk04_cell_info_(const std::vector<uint8_t> &data) {
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
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float total_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) ieee_float_(jk_get_32bit(i * 4 + 6));
    float cell_resistance = (float) ieee_float_(jk_get_32bit(i * 4 + 102));
    total_voltage = total_voltage + cell_voltage;
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
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 202   4   0x03 0x95 0x56 0x40    Average Cell Voltage               V
  this->publish_state_(this->average_cell_voltage_sensor_, (float) ieee_float_(jk_get_32bit(202)));

  // 206   4   0x00 0xBE 0x90 0x3B    Delta Cell Voltage                 V
  this->publish_state_(this->delta_cell_voltage_sensor_, (float) ieee_float_(jk_get_32bit(206)));

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

  // 290   1   0x00    Unknown290
  // 291   1   0x00    ** [JK-PB2A16S-20P v14] CHARGE IS WORKING 
  // 292   1   0x00    ** [JK-PB2A16S-20P v14] DISCHARGE IS WORKING
  // 293   1   0x00    ** [JK-PB2A16S-20P v14] PREDISCHARGE IS WORKING (?)
  // 294   1   0x00    ** [JK-PB2A16S-20P v14] BALANCE IS WORKING (?)     
  ESP_LOGD(TAG, "Unknown290: 0x%02X 0x%02X 0x%02X 0x%02X (always 0x00 0x00 0x00 0x00?)", data[290], data[291],
           data[292], data[293]);

  // 294   4   0x00 0x48 0x22 0x40    Unknown294
  ESP_LOGD(TAG, "Unknown294: 0x%02X 0x%02X 0x%02X 0x%02X", data[294], data[295], data[296], data[297]);

  // 298   1   0x00                   Unknown298
  ESP_LOGD(TAG, "Unknown298: 0x%02X", data[298]);

  // 299   1   0x13                   Checksm

  status_notification_received_ = true;
}



void JkRS485Bms::on_status_data_(const std::vector<uint8_t> &data) {
  auto jk_get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };
  auto jk_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(jk_get_16bit(i + 0)) << 16) | (uint32_t(jk_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");

  // Status request
  // -> 0x4E 0x57 0x00 0x13 0x00 0x00 0x00 0x00 0x06 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x68 0x00 0x00 0x01 0x29
  //
  // Status response
  // <- 0x4E 0x57 0x01 0x1B 0x00 0x00 0x00 0x00 0x06 0x00 0x01: Header
  //
  // *Data*
  //
  // Address Content: Description      Decoded content                         Coeff./Unit
  // 0x79: Individual Cell voltage
  // 0x2A: Cell count               42 / 3 bytes = 14 cells
  // 0x01 0x0E 0xED: Cell 1         3821 * 0.001 = 3.821V                        0.001 V
  // 0x02 0x0E 0xFA: Cell 2         3834 * 0.001 = 3.834V                        0.001 V
  // 0x03 0x0E 0xF7: Cell 3         3831 * 0.001 = 3.831V                        0.001 V
  // 0x04 0x0E 0xEC: Cell 4         ...                                          0.001 V
  // 0x05 0x0E 0xF8: Cell 5         ...                                          0.001 V
  // 0x06 0x0E 0xFA: Cell 6         ...                                          0.001 V
  // 0x07 0x0E 0xF1: Cell 7         ...                                          0.001 V
  // 0x08 0x0E 0xF8: Cell 8         ...                                          0.001 V
  // 0x09 0x0E 0xE3: Cell 9         ...                                          0.001 V
  // 0x0A 0x0E 0xFA: Cell 10        ...                                          0.001 V
  // 0x0B 0x0E 0xF1: Cell 11        ...                                          0.001 V
  // 0x0C 0x0E 0xFB: Cell 12        ...                                          0.001 V
  // 0x0D 0x0E 0xFB: Cell 13        ...                                          0.001 V
  // 0x0E 0x0E 0xF2: Cell 14        3826 * 0.001 = 3.826V                        0.001 V
  uint8_t cells = data[1] / 3;

  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = (float) jk_get_16bit(i * 3 + 3) * 0.001f;
    average_cell_voltage = average_cell_voltage + cell_voltage;
    if (cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
  }
  average_cell_voltage = average_cell_voltage / cells;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  uint16_t offset = data[1] + 3;

  // 0x80 0x00 0x1D: Read power tube temperature                 29°C                      1.0 °C
  // --->  99 = 99°C, 100 = 100°C, 101 = -1°C, 140 = -40°C
  this->publish_state_(this->power_tube_temperature_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 0)));

  // 0x81 0x00 0x1E: Read the temperature in the battery box     30°C                      1.0 °C
  this->publish_state_(this->temperature_sensor_1_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 1)));

  // 0x82 0x00 0x1C: Read battery temperature                    28°C                      1.0 °C
  this->publish_state_(this->temperature_sensor_2_sensor_, get_temperature_(jk_get_16bit(offset + 3 * 2)));

  // 0x83 0x14 0xEF: Total battery voltage                       5359 * 0.01 = 53.59V      0.01 V
  float total_voltage = (float) jk_get_16bit(offset + 3 * 3) * 0.01f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // 0x84 0x80 0xD0: Current data                                32976                     0.01 A
  // this->publish_state_(this->current_sensor_, get_current_(jk_get_16bit(offset + 3 * 4), 0x01) * 0.01f);
  float current = get_current_(jk_get_16bit(offset + 3 * 4), data[offset + 84 + 3 * 45]) * 0.01f;
  this->publish_state_(this->current_sensor_, current);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 0x85 0x0F: Battery remaining capacity                       15 %
  uint8_t raw_battery_remaining_capacity = data[offset + 3 * 5];
  this->publish_state_(this->capacity_remaining_sensor_, (float) raw_battery_remaining_capacity);

  // 0x86 0x02: Number of battery temperature sensors             2                        1.0  count
  this->publish_state_(this->temperature_sensors_sensor_, (float) data[offset + 2 + 3 * 5]);

  // 0x87 0x00 0x04: Number of battery cycles                     4                        1.0  count
  this->publish_state_(this->charging_cycles_sensor_, (float) jk_get_16bit(offset + 4 + 3 * 5));

  // 0x89 0x00 0x00 0x00 0x00: Total battery cycle capacity
  this->publish_state_(this->total_charging_cycle_capacity_sensor_, (float) jk_get_32bit(offset + 4 + 3 * 6));

  // 0x8A 0x00 0x0E: Total number of battery strings             14                        1.0  count
  this->publish_state_(this->battery_strings_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 7));

  // 0x8B 0x00 0x00: Battery warning message                     0000 0000 0000 0000
  //
  // Bit 0    Low capacity                                1 (alarm), 0 (normal)    warning
  // Bit 1    Power tube overtemperature                  1 (alarm), 0 (normal)    alarm
  // Bit 2    Charging overvoltage                        1 (alarm), 0 (normal)    alarm
  // Bit 3    Discharging undervoltage                    1 (alarm), 0 (normal)    alarm
  // Bit 4    Battery over temperature                    1 (alarm), 0 (normal)    alarm
  // Bit 5    Charging overcurrent                        1 (alarm), 0 (normal)    alarm
  // Bit 6    Discharging overcurrent                     1 (alarm), 0 (normal)    alarm
  // Bit 7    Cell pressure difference                    1 (alarm), 0 (normal)    alarm
  // Bit 8    Overtemperature alarm in the battery box    1 (alarm), 0 (normal)    alarm
  // Bit 9    Battery low temperature                     1 (alarm), 0 (normal)    alarm
  // Bit 10   Cell overvoltage                            1 (alarm), 0 (normal)    alarm
  // Bit 11   Cell undervoltage                           1 (alarm), 0 (normal)    alarm
  // Bit 12   309_A protection                            1 (alarm), 0 (normal)    alarm
  // Bit 13   309_A protection                            1 (alarm), 0 (normal)    alarm
  // Bit 14   Reserved
  // Bit 15   Reserved
  //
  // Examples:
  // 0x0001 = 00000000 00000001: Low capacity alarm
  // 0x0002 = 00000000 00000010: MOS tube over-temperature alarm
  // 0x0003 = 00000000 00000011: Low capacity alarm AND power tube over-temperature alarm
  uint16_t raw_errors_bitmask = jk_get_16bit(offset + 6 + 3 * 8);
  this->publish_state_(this->errors_bitmask_sensor_, (float) raw_errors_bitmask);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_errors_bitmask));

  // 0x8C 0x00 0x07: Battery status information                  0000 0000 0000 0111
  // Bit 0: Charging enabled        1 (on), 0 (off)
  // Bit 1: Discharging enabled     1 (on), 0 (off)
  // Bit 2: Balancer enabled        1 (on), 0 (off)
  // Bit 3: Battery dropped(?)      1 (normal), 0 (offline)
  // Bit 4...15: Reserved

  // Example: 0000 0000 0000 0111 -> Charging + Discharging + Balancer enabled
  uint16_t raw_modes_bitmask = jk_get_16bit(offset + 6 + 3 * 9);
  this->publish_state_(this->operation_mode_bitmask_sensor_, (float) raw_modes_bitmask);
  this->publish_state_(this->operation_mode_text_sensor_, this->mode_bits_to_string_(raw_modes_bitmask));
  this->publish_state_(this->charging_binary_sensor_, check_bit_(raw_modes_bitmask, 1));
  this->publish_state_(this->discharging_binary_sensor_, check_bit_(raw_modes_bitmask, 2));
  this->publish_state_(this->balancing_binary_sensor_, check_bit_(raw_modes_bitmask, 4));

  // 0x8E 0x16 0x26: Total voltage overvoltage protection        5670 * 0.01 = 56.70V     0.01 V
  this->publish_state_(this->total_voltage_overvoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 10) * 0.01f);

  // 0x8F 0x10 0xAE: Total voltage undervoltage protection       4270 * 0.01 = 42.70V     0.01 V
  this->publish_state_(this->total_voltage_undervoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 11) * 0.01f);

  // 0x90 0x0F 0xD2: Cell overvoltage protection voltage         4050 * 0.001 = 4.050V     0.001 V
  this->publish_state_(this->cell_voltage_overvoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 12) * 0.001f);

  // 0x91 0x0F 0xA0: Cell overvoltage recovery voltage           4000 * 0.001 = 4.000V     0.001 V
  this->publish_state_(this->cell_voltage_overvoltage_recovery_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 13) * 0.001f);

  // 0x92 0x00 0x05: Cell overvoltage protection delay            5s                         1.0 s
  this->publish_state_(this->cell_voltage_overvoltage_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 14));

  // 0x93 0x0B 0xEA: Cell undervoltage protection voltage        3050 * 0.001 = 3.050V     0.001 V
  this->publish_state_(this->cell_voltage_undervoltage_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 15) * 0.001f);

  // 0x94 0x0C 0x1C: Cell undervoltage recovery voltage          3100 * 0.001 = 3.100V     0.001 V
  this->publish_state_(this->cell_voltage_undervoltage_recovery_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 16) * 0.001f);

  // 0x95 0x00 0x05: Cell undervoltage protection delay           5s                         1.0 s
  this->publish_state_(this->cell_voltage_undervoltage_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 17));

  // 0x96 0x01 0x2C: Cell pressure difference protection value    300 * 0.001 = 0.300V     0.001 V     0.000-1.000V
  this->publish_state_(this->cell_pressure_difference_protection_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 18) * 0.001f);

  // 0x97 0x00 0x07: Discharge overcurrent protection value       7A                         1.0 A
  this->publish_state_(this->discharging_overcurrent_protection_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 19));

  // 0x98 0x00 0x03: Discharge overcurrent delay                  3s                         1.0 s
  this->publish_state_(this->discharging_overcurrent_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 20));

  // 0x99 0x00 0x05: Charging overcurrent protection value        5A                         1.0 A
  this->publish_state_(this->charging_overcurrent_protection_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 21));

  // 0x9A 0x00 0x05: Charge overcurrent delay                     5s                         1.0 s
  this->publish_state_(this->charging_overcurrent_delay_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 22));

  // 0x9B 0x0C 0xE4: Balanced starting voltage                   3300 * 0.001 = 3.300V     0.001 V
  this->publish_state_(this->balance_starting_voltage_sensor_, (float) jk_get_16bit(offset + 6 + 3 * 23) * 0.001f);

  // 0x9C 0x00 0x08: Balanced opening pressure difference           8 * 0.001 = 0.008V     0.001 V     0.01-1V
  this->publish_state_(this->balance_opening_pressure_difference_sensor_,
                       (float) jk_get_16bit(offset + 6 + 3 * 24) * 0.001f);

  // 0x9D 0x01: Active balance switch                              1 (on)                     Bool     0 (off), 1 (on)
  this->publish_state_(this->balancing_switch_binary_sensor_, (bool) data[offset + 6 + 3 * 25]);
  this->publish_state_(this->balancer_switch_, (bool) data[offset + 6 + 3 * 25]);

  // 0x9E 0x00 0x5A: Power tube temperature protection value                90°C            1.0 °C     0-100°C
  this->publish_state_(this->power_tube_temperature_protection_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 25));

  // 0x9F 0x00 0x46: Power tube temperature recovery value                  70°C            1.0 °C     0-100°C
  this->publish_state_(this->power_tube_temperature_recovery_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 26));

  // 0xA0 0x00 0x64: Temperature protection value in the battery box       100°C            1.0 °C     40-100°C
  this->publish_state_(this->temperature_sensor_temperature_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 27));

  // 0xA1 0x00 0x64: Temperature recovery value in the battery box         100°C            1.0 °C     40-100°C
  this->publish_state_(this->temperature_sensor_temperature_recovery_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 28));

  // 0xA2 0x00 0x14: Battery temperature difference protection value        20°C            1.0 °C     5-10°C
  this->publish_state_(this->temperature_sensor_temperature_difference_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 29));

  // 0xA3 0x00 0x46: Battery charging high temperature protection value     70°C            1.0 °C     0-100°C
  this->publish_state_(this->charging_high_temperature_protection_sensor_, (float) jk_get_16bit(offset + 8 + 3 * 30));

  // 0xA4 0x00 0x46: Battery discharge high temperature protection value    70°C            1.0 °C     0-100°C
  this->publish_state_(this->discharging_high_temperature_protection_sensor_,
                       (float) jk_get_16bit(offset + 8 + 3 * 31));

  // 0xA5 0xFF 0xEC: Charging low temperature protection value             -20°C            1.0 °C     -45...25°C
  this->publish_state_(this->charging_low_temperature_protection_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 32));

  // 0xA6 0xFF 0xF6: Charging low temperature protection recovery value    -10°C            1.0 °C     -45...25°C
  this->publish_state_(this->charging_low_temperature_recovery_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 33));

  // 0xA7 0xFF 0xEC: Discharge low temperature protection value            -20°C            1.0 °C     -45...25°C
  this->publish_state_(this->discharging_low_temperature_protection_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 34));

  // 0xA8 0xFF 0xF6: Discharge low temperature protection recovery value   -10°C            1.0 °C     -45...25°C
  this->publish_state_(this->discharging_low_temperature_recovery_sensor_,
                       (float) (int16_t) jk_get_16bit(offset + 8 + 3 * 35));

  // 0xA9 0x0E: Battery string setting                                      14              1.0 count
  // this->publish_state_(this->battery_string_setting_sensor_, (float) data[offset + 8 + 3 * 36]);
  // 0xAA 0x00 0x00 0x02 0x30: Total battery capacity setting              560 Ah           1.0 Ah
  uint32_t raw_total_battery_capacity_setting = jk_get_32bit(offset + 10 + 3 * 36);
  this->publish_state_(this->total_battery_capacity_setting_sensor_, (float) raw_total_battery_capacity_setting);
  this->publish_state_(this->capacity_remaining_derived_sensor_,
                       (float) (raw_total_battery_capacity_setting * (raw_battery_remaining_capacity * 0.01f)));

  // 0xAB 0x01: Charging MOS tube switch                                     1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->charging_switch_binary_sensor_, (bool) data[offset + 15 + 3 * 36]);
  this->publish_state_(this->charging_switch_, (bool) data[offset + 15 + 3 * 36]);

  // 0xAC 0x01: Discharge MOS tube switch                                    1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->discharging_switch_binary_sensor_, (bool) data[offset + 17 + 3 * 36]);
  this->publish_state_(this->discharging_switch_, (bool) data[offset + 17 + 3 * 36]);

  // 0xAD 0x04 0x11: Current calibration                       1041mA * 0.001 = 1.041A     0.001 A     0.1-2.0A
  this->publish_state_(this->current_calibration_sensor_, (float) jk_get_16bit(offset + 19 + 3 * 36) * 0.001f);

  // 0xAE 0x01: Protection board address                                     1              1.0
  this->publish_state_(this->device_address_sensor_, (float) data[offset + 19 + 3 * 37]);

  // 0xAF 0x01: Battery Type                                                 1              1.0
  // ---> 0 (lithium iron phosphate), 1 (ternary), 2 (lithium titanate)
  uint8_t raw_battery_type = data[offset + 21 + 3 * 37];
  if (raw_battery_type < BATTERY_TYPES_SIZE) {
    this->publish_state_(this->battery_type_text_sensor_, BATTERY_TYPES[raw_battery_type]);
  } else {
    this->publish_state_(this->battery_type_text_sensor_, "Unknown");
  }

  // 0xB0 0x00 0x0A: Sleep waiting time                                      10s            1.0 s
  this->publish_state_(this->sleep_wait_time_sensor_, (float) jk_get_16bit(offset + 23 + 3 * 37));

  // 0xB1 0x14: Low volume alarm                                             20%            1.0 %      0-80%
  this->publish_state_(this->alarm_low_volume_sensor_, (float) data[offset + 23 + 3 * 38]);

  // 0xB2 0x31 0x32 0x33 0x34 0x35 0x36 0x00 0x00 0x00 0x00: Modify parameter password
  this->publish_state_(this->password_text_sensor_,
                       std::string(data.begin() + offset + 25 + 3 * 38, data.begin() + offset + 35 + 3 * 38));

  // 0xB3 0x00: Dedicated charger switch                                     1 (on)         Bool       0 (off), 1 (on)
  this->publish_state_(this->dedicated_charger_switch_binary_sensor_, (bool) data[offset + 36 + 3 * 38]);

  // 0xB4 0x49 0x6E 0x70 0x75 0x74 0x20 0x55 0x73: Device ID code
  this->publish_state_(this->device_type_text_sensor_,
                       std::string(data.begin() + offset + 38 + 3 * 38, data.begin() + offset + 46 + 3 * 38));

  // 0xB5 0x32 0x31 0x30 0x31: Date of manufacture
  // 0xB6 0x00 0x00 0xE2 0x00: System working hours
  this->publish_state_(this->total_runtime_sensor_, (float) jk_get_32bit(offset + 46 + 3 * 40) * 0.0166666666667);
  this->publish_state_(this->total_runtime_formatted_text_sensor_,
                       format_total_runtime_(jk_get_32bit(offset + 46 + 3 * 40) * 60));

  // 0xB7 0x48 0x36 0x2E 0x58 0x5F 0x5F 0x53
  //      0x36 0x2E 0x31 0x2E 0x33 0x53 0x5F 0x5F: Software version number
  this->publish_state_(this->software_version_text_sensor_,
                       std::string(data.begin() + offset + 51 + 3 * 40, data.begin() + offset + 51 + 3 * 45));

  // 0xB8 0x00: Whether to start current calibration
  // 0xB9 0x00 0x00 0x00 0x00: Actual battery capacity
  //   Firmware version >= 10.10 required
  //   See https://github.com/syssi/esphome-jk-bms/issues/212 for details
  this->publish_state_(this->actual_battery_capacity_sensor_, (float) jk_get_32bit(offset + 54 + 3 * 45));

  // 0xBA 0x42 0x54 0x33 0x30 0x37 0x32 0x30 0x32 0x30 0x31 0x32 0x30
  //      0x30 0x30 0x30 0x32 0x30 0x30 0x35 0x32 0x31 0x30 0x30 0x31: Manufacturer ID naming
  this->publish_state_(this->manufacturer_text_sensor_,
                       std::string(data.begin() + offset + 59 + 3 * 45, data.begin() + offset + 83 + 3 * 45));

  // 0xC0 0x01: Protocol version number
  this->publish_state_(this->protocol_version_sensor_, (float) data[offset + 84 + 3 * 45]);

  // 00 00 00 00 68 00 00 54 D1: End of frame
}

void JkRS485Bms::update() {
  this->track_online_status_();
}

void JkRS485Bms::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->no_response_count_++;
  }
  if (this->no_response_count_ == MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    this->no_response_count_++;
  }
}

void JkRS485Bms::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void JkRS485Bms::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
  this->publish_state_(this->errors_text_sensor_, "Offline");

  this->publish_state_(min_cell_voltage_sensor_, NAN);
  this->publish_state_(max_cell_voltage_sensor_, NAN);
  this->publish_state_(min_voltage_cell_sensor_, NAN);
  this->publish_state_(max_voltage_cell_sensor_, NAN);
  this->publish_state_(delta_cell_voltage_sensor_, NAN);
  this->publish_state_(average_cell_voltage_sensor_, NAN);
  this->publish_state_(power_tube_temperature_sensor_, NAN);
  this->publish_state_(temperature_sensor_1_sensor_, NAN);
  this->publish_state_(temperature_sensor_2_sensor_, NAN);
  this->publish_state_(total_voltage_sensor_, NAN);
  this->publish_state_(current_sensor_, NAN);
  this->publish_state_(power_sensor_, NAN);
  this->publish_state_(charging_power_sensor_, NAN);
  this->publish_state_(discharging_power_sensor_, NAN);
  this->publish_state_(capacity_remaining_sensor_, NAN);
  this->publish_state_(capacity_remaining_derived_sensor_, NAN);
  this->publish_state_(temperature_sensors_sensor_, NAN);
  this->publish_state_(charging_cycles_sensor_, NAN);
  this->publish_state_(total_charging_cycle_capacity_sensor_, NAN);
  this->publish_state_(battery_strings_sensor_, NAN);
  this->publish_state_(errors_bitmask_sensor_, NAN);
  this->publish_state_(operation_mode_bitmask_sensor_, NAN);
  this->publish_state_(total_voltage_overvoltage_protection_sensor_, NAN);
  this->publish_state_(total_voltage_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_overvoltage_delay_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_protection_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_recovery_sensor_, NAN);
  this->publish_state_(cell_voltage_undervoltage_delay_sensor_, NAN);
  this->publish_state_(cell_pressure_difference_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(discharging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(charging_overcurrent_protection_sensor_, NAN);
  this->publish_state_(charging_overcurrent_delay_sensor_, NAN);
  this->publish_state_(balance_starting_voltage_sensor_, NAN);
  this->publish_state_(balance_opening_pressure_difference_sensor_, NAN);
  this->publish_state_(power_tube_temperature_protection_sensor_, NAN);
  this->publish_state_(power_tube_temperature_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_protection_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_recovery_sensor_, NAN);
  this->publish_state_(temperature_sensor_temperature_difference_protection_sensor_, NAN);
  this->publish_state_(charging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_high_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(charging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_protection_sensor_, NAN);
  this->publish_state_(discharging_low_temperature_recovery_sensor_, NAN);
  this->publish_state_(total_battery_capacity_setting_sensor_, NAN);
  this->publish_state_(charging_sensor_, NAN);
  this->publish_state_(discharging_sensor_, NAN);
  this->publish_state_(current_calibration_sensor_, NAN);
  this->publish_state_(device_address_sensor_, NAN);
  this->publish_state_(sleep_wait_time_sensor_, NAN);
  this->publish_state_(alarm_low_volume_sensor_, NAN);
  this->publish_state_(password_sensor_, NAN);
  this->publish_state_(manufacturing_date_sensor_, NAN);
  this->publish_state_(total_runtime_sensor_, NAN);
  this->publish_state_(start_current_calibration_sensor_, NAN);
  this->publish_state_(actual_battery_capacity_sensor_, NAN);
  this->publish_state_(protocol_version_sensor_, NAN);

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
  LOG_SENSOR("", "Power Tube Temperature", this->power_tube_temperature_sensor_);
  LOG_SENSOR("", "Temperature Sensor 1", this->temperature_sensor_1_sensor_);
  LOG_SENSOR("", "Temperature Sensor 2", this->temperature_sensor_2_sensor_);
  LOG_SENSOR("", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging Power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging Power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Capacity Remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Capacity Remaining Derived", this->capacity_remaining_derived_sensor_);
  LOG_SENSOR("", "Temperature Sensors", this->temperature_sensors_sensor_);
  LOG_SENSOR("", "Charging Cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Total Charging Cycle Capacity", this->total_charging_cycle_capacity_sensor_);
  LOG_SENSOR("", "Battery Strings", this->battery_strings_sensor_);
  LOG_SENSOR("", "Errors Bitmask", this->errors_bitmask_sensor_);
  LOG_SENSOR("", "Operation Mode Bitmask", this->operation_mode_bitmask_sensor_);
  LOG_SENSOR("", "Total Voltage Overvoltage Protection", this->total_voltage_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Total Voltage Undervoltage Protection", this->total_voltage_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Protection", this->cell_voltage_overvoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Recovery", this->cell_voltage_overvoltage_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Overvoltage Delay", this->cell_voltage_overvoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Protection", this->cell_voltage_undervoltage_protection_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Recovery", this->cell_voltage_undervoltage_recovery_sensor_);
  LOG_SENSOR("", "Cell Voltage Undervoltage Delay", this->cell_voltage_undervoltage_delay_sensor_);
  LOG_SENSOR("", "Cell Pressure Difference Protection", this->cell_pressure_difference_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Protection", this->discharging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Discharging Overcurrent Delay", this->discharging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Protection", this->charging_overcurrent_protection_sensor_);
  LOG_SENSOR("", "Charging Overcurrent Delay", this->charging_overcurrent_delay_sensor_);
  LOG_SENSOR("", "Balance Starting Voltage", this->balance_starting_voltage_sensor_);
  LOG_SENSOR("", "Balance Opening Pressure Difference", this->balance_opening_pressure_difference_sensor_);
  LOG_SENSOR("", "Power Tube Temperature Protection", this->power_tube_temperature_protection_sensor_);
  LOG_SENSOR("", "Power Tube Temperature Recovery", this->power_tube_temperature_recovery_sensor_);
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
  LOG_SENSOR("", "Total Battery Capacity Setting", this->total_battery_capacity_setting_sensor_);
  LOG_SENSOR("", "Current Calibration", this->current_calibration_sensor_);
  LOG_SENSOR("", "Device Address", this->device_address_sensor_);
  LOG_TEXT_SENSOR("", "Battery Type", this->battery_type_text_sensor_);
  LOG_SENSOR("", "Sleep Wait Time", this->sleep_wait_time_sensor_);
  LOG_SENSOR("", "Alarm Low Volume", this->alarm_low_volume_sensor_);
  LOG_TEXT_SENSOR("", "Password", this->password_text_sensor_);
  LOG_TEXT_SENSOR("", "Device Type", this->device_type_text_sensor_);
  LOG_SENSOR("", "Manufacturing Date", this->manufacturing_date_sensor_);
  LOG_SENSOR("", "Total Runtime", this->total_runtime_sensor_);
  LOG_TEXT_SENSOR("", "Software Version", this->software_version_text_sensor_);
  LOG_SENSOR("", "Start Current Calibration", this->start_current_calibration_sensor_);
  LOG_TEXT_SENSOR("", "Manufacturer", this->manufacturer_text_sensor_);
  LOG_SENSOR("", "Protocol Version", this->protocol_version_sensor_);
  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Balancing Switch", this->balancing_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging Switch", this->charging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging Switch", this->discharging_switch_binary_sensor_);
  LOG_BINARY_SENSOR("", "Dedicated Charger Switch", this->dedicated_charger_switch_binary_sensor_);
  LOG_TEXT_SENSOR("", "Total Runtime Formatted", this->total_runtime_formatted_text_sensor_);
}

}  // namespace jk_rs485_bms
}  // namespace esphome
