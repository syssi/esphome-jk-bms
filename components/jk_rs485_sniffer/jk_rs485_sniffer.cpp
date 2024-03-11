#include "jk_rs485_sniffer.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_rs485_sniffer {

static const char *const TAG = "jk_rs485_sniffer";

static const uint8_t FUNCTION_WRITE_REGISTER = 0x02;
static const uint8_t FUNCTION_READ_REGISTER = 0x03;
static const uint8_t FUNCTION_PASSWORD = 0x05;
static const uint8_t FUNCTION_READ_ALL_REGISTERS = 0x06;

static const uint8_t ADDRESS_READ_ALL = 0x00;

static const uint8_t FRAME_SOURCE_GPS = 0x02;

static const uint16_t JKPB_RS485_RESPONSE_SIZE = 310;
static const uint16_t JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM = 299;
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS = 4;
static const uint16_t JKPB_RS485_FRAME_COUNTER_ADDRESS = 5;
static const uint16_t JKPB_RS485_CHECKSUM_ADDRESS = 299;
static const uint16_t JKPB_RS485_ADDRESS_OF_RS485_ADDRESS = 300;

std::vector<uint8_t> hexStringToVector(const std::string& hexString) {
  std::vector<uint8_t> result;
  for (size_t i = 0; i < hexString.length(); i += 2) {
    std::string byteString = hexString.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
    result.push_back(byte);
  }
  return result;
}

void JkRS485Sniffer::loop() {

  const uint32_t now = millis();
  if (now - this->last_jk_rs485_sniffer_byte_ > this->rx_timeout_) {
    ESP_LOGVV(TAG, "Buffer cleared due to %d timeout: %s", this->rx_timeout_,
              format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    this->rx_buffer_.clear();
    this->last_jk_rs485_sniffer_byte_ = now;
  }
   
//  if (now - this->last_jk_rs485_sniffer_byte_ > 5000) {
//    ESP_LOGVV(TAG, "SENDING...");
//    this->last_jk_rs485_sniffer_byte_ = now;
//
//    static const char *data2="55aaeb900205120d120d120d120d120d110d120d110d120d120d110d110d120d110d120d110d0000000000000000000000000000000000000000000000000000000000000000ffff0000120d010000064d004b004d004b004d004a004c0049004c004b004e004c0050004e0050004e000000000000000000000000000000000000000000000000000000000000000000ce00000000001fd100000000000000000000c000c7000000000000000064aa9e040080a3040000000000df04000064000000771f0c0001010000000000000000000000000000ff00010000009303000000000bc23f4000000000e91400000001010100060000946f5c0000000000ce00bf00c1008d03873e0900090000008051010000000000000000000000000000feff7fdd2f0101b0070000006e011016200001044b";
//    static const char *data1="55aaeb900105ac0d0000280a00005a0a00001a0e0000780d000005000000790d0000500a00007a0d0000160d0000c4090000a8610000030000003c000000400d03002c0100003c00000005000000d0070000bc02000058020000bc0200005802000038ffffff9cffffffe8030000200300001000000001000000010000000100000080a30400dc0500007a0d00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000500000060e3160010023c3218feffffffbfe9010200000000000110161e00016587";
//
//    for (auto *device : this->devices_) {
//        device->on_jk_rs485_sniffer_data(5, 2, hexStringToVector(data2));   
//        device->on_jk_rs485_sniffer_data(5, 1, hexStringToVector(data1));
//    }
//
//  }


  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_rs485_sniffer_byte_(byte)) {
      this->last_jk_rs485_sniffer_byte_ = now;
    } else {
      ESP_LOGVV(TAG, "Buffer cleared due to reset: %s",
                format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
      this->rx_buffer_.clear();
    }
  }
}

uint16_t chksum(const uint8_t data[], const uint16_t len) {
  uint16_t checksum = 0;
  for (uint16_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

// Function to print the content of the array in hexadecimal
void printArrayInHex(const uint8_t *array, size_t length) {
    printf("HEX: ");
    for (int i = 0; i < length; i++) {
        if (i<5000 || i>300){
           printf("%02x", array[i]);
        } else if (i==100){
          printf("...");
        }
    }
    printf("\n"); // Newline at the end
}


bool JkRS485Sniffer::parse_jk_rs485_sniffer_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  uint8_t address = 0;

  // Byte 0: Start sequence (0x4E) //55aaeb900105
  if (at == 0) {
    if (raw[0] == 0x55){
        return true;
    } else {
//         ESP_LOGW(TAG, "Invalid header 1: 0x%02X", raw[0]);
        return false;
    }
  }

  // Byte 1: Start sequence (0x57) //55aaeb900105
  if (at == 1) {
    if (raw[1] != 0xAA) {
//      ESP_LOGW(TAG, "Invalid header 2: 0x%02X 0x%02X", raw[0], raw[1]);
      // return false to reset buffer
      return false;
    }
    return true;
  }

  // Byte 2: Start sequence //55aaeb900105
  if (at == 2) {
    if (raw[2] != 0xEB) {
      ESP_LOGW(TAG, "Invalid header 3: 0x%02X 0x%02X 0x%02X", raw[0], raw[1], raw[2]);
      // return false to reset buffer
      return false;
    }
    return true;
  }

  // Byte 3: Start sequence //55aaeb900105
  if (at == 3) {
    if (raw[3] != 0x90) {
      ESP_LOGW(TAG, "Invalid header 4: 0x%02X 0x%02X 0x%02X 0x%02X", raw[0], raw[1], raw[2], raw[3]);
      // return false to reset buffer
      return false;
    }
    return true;
  }

  // Byte 4: Frame Type //55aaeb90 01 05
  if (at == JKPB_RS485_FRAME_TYPE_ADDRESS) {
      ESP_LOGVV(TAG, "Frame Type: 0x%02X", raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);
      return true;
  }

  // Byte 5: Frame Counter //55aaeb9001 05
  if (at == JKPB_RS485_FRAME_COUNTER_ADDRESS) {
      ESP_LOGVV(TAG, "Frame Counter: 0x%02X", raw[JKPB_RS485_FRAME_COUNTER_ADDRESS]);
      return true;
  }

  if (at>JKPB_RS485_FRAME_COUNTER_ADDRESS && at<(JKPB_RS485_RESPONSE_SIZE-1)){  //308-1
    if (at == JKPB_RS485_ADDRESS_OF_RS485_ADDRESS){
      uint8_t computed_checksum = chksum(raw, JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM);
      uint8_t remote_checksum = raw[JKPB_RS485_CHECKSUM_ADDRESS];
      address=raw[JKPB_RS485_ADDRESS_OF_RS485_ADDRESS];
      if (computed_checksum != remote_checksum) {
        ESP_LOGW(TAG, "CHECKSUM failed! 0x%02X != 0x%02X", computed_checksum, remote_checksum);
        return false;
      } else {
        ESP_LOGV(TAG, "(at:%03d) len:%03d [address 0x%02X] Frame Type 0x%02X | CHECKSUM is correct",at,at+1,address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);
      }
    }
    return true;
  }

  if (at==(JKPB_RS485_RESPONSE_SIZE-1)){  //308-1    
     //printArrayInHex(raw, at+1);
     uint16_t data_len=at+1;

     address=raw[JKPB_RS485_ADDRESS_OF_RS485_ADDRESS];

  //  std::vector<uint8_t> data(this->rx_buffer_.begin() + 11, this->rx_buffer_.begin() + data_len - 3);
    std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + data_len);

    ESP_LOGI(TAG, "Frame received (type:%d, %d bytes)", raw[4], data.size());
    ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), 150).c_str());
    ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front() + 150, data.size() - 150).c_str());

    bool found = false;
//    address=this->get_last_detected_address();
    for (auto *device : this->devices_) {
//      if (device->address_ == address) {
        //device->on_jk_rs485_sniffer_data(FUNCTION_READ_ALL, data);
        device->on_jk_rs485_sniffer_data(address, raw[JKPB_RS485_FRAME_TYPE_ADDRESS], data);   
//        found = true;
      //}
    }
    found=true;  

    if (!found) {
      ESP_LOGW(TAG, "Got JkRS485 frame from unknown address 0x%02X!", address);
    }
  }
  // return false to reset buffer
  return false;
}


void JkRS485Sniffer::dump_config() {
  ESP_LOGCONFIG(TAG, "JkRS485Sniffer:");
  ESP_LOGCONFIG(TAG, "  RX timeout: %d ms", this->rx_timeout_);
}
float JkRS485Sniffer::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

}  // namespace jk_rs485_sniffer
}  // namespace esphome
