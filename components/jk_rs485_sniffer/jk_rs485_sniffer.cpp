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

static const uint16_t JKPB_RS485_RESPONSE_SIZE = 308;
static const uint16_t JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM = 299;
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS = 4;
static const uint16_t JKPB_RS485_FRAME_COUNTER_ADDRESS = 5;
static const uint16_t JKPB_RS485_CHECKSUM_ADDRESS = 299;
static const uint16_t JKPB_RS485_ADDRESS_OF_RS485_ADDRESS = 300;

std::vector<unsigned char> pattern_header = {0x55, 0xAA, 0xEB, 0x90};

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

  if (this->rx_buffer_.size()>=JKPB_RS485_RESPONSE_SIZE*2){
    ESP_LOGI(TAG, "Buffer cleared buffer size: %d",this->rx_buffer_.size());
    this->rx_buffer_.clear();
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_jk_rs485_sniffer_byte_(byte)) {
      this->last_jk_rs485_sniffer_byte_ = now;
    } else {
      ESP_LOGVV(TAG, "Buffer cleared due to reset: %s",
                format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
      //this->rx_buffer_.clear();
    }
  }

}


void JkRS485Sniffer::loop2() {

  const uint32_t now = millis();
  if (now - this->last_jk_rs485_sniffer_byte_ > this->rx_timeout_) {
    ESP_LOGI(TAG, "Entering now..."); 
    this->last_jk_rs485_sniffer_byte_ = now;    
    ESP_LOGI(TAG, "Buffer cleared due to %d timeout: %s", this->rx_timeout_,
              format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    this->rx_buffer_.clear();

  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);


    if (this->rx_buffer_.size()==800){
      ESP_LOGI(TAG, "Clearing buffer... size: %d",this->rx_buffer_.size());
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


void JkRS485Sniffer::printBuffer(void){
  std::string bufferHex;

  bufferHex="";
  // Volcar el contenido del buffer en el string en formato hexadecimal
  for (auto byte : this->rx_buffer_) {
      char hexByte[3];
      sprintf(hexByte, "%02X", byte);
      bufferHex += hexByte;
  }  
  ESP_LOGI("BUFFER", "(%d): %s",this->rx_buffer_.size(), bufferHex.c_str());
}

bool JkRS485Sniffer::parse_jk_rs485_sniffer_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);


  if (at>=JKPB_RS485_RESPONSE_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_header.begin(), pattern_header.end());

    // Si se encontró la secuencia
    if (it != this->rx_buffer_.end()) {
      // Calcular el índice de la secuencia

      size_t index = std::distance(this->rx_buffer_.begin(), it);
      
//      printBuffer();
      // Eliminar los elementos anteriores a la secuencia
      this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + index);
//      printBuffer();

      if (this->rx_buffer_.size()>=JKPB_RS485_RESPONSE_SIZE){
        //continue
        ESP_LOGI(TAG, "Sequence found SIZE: %d",(at));     
      } else {
        return(false);
      }
    }    
  } else {
    return(false);
  }


  const uint8_t *raw = &this->rx_buffer_[0];

  uint8_t address = 0;

  // Byte 0: Start sequence (0x4E) //55aaeb900105

  // Byte 4: Frame Type //55aaeb90 01 05

  // Byte 5: Frame Counter //55aaeb9001 05


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
//      if ((found==false) && (address==7) && (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==1)){
//          printBuffer();
//          found = true;
//      }
      device->on_jk_rs485_sniffer_data(address, raw[JKPB_RS485_FRAME_TYPE_ADDRESS], data);   
//        found = true;
    //}
  }
  found=true;  

  if (!found) {
    ESP_LOGW(TAG, "Got JkRS485 frame from unknown address 0x%02X!", address);
  }
//  // Delete processed frame from rx_buffer
//  std::string bufferHex;

//  bufferHex="BEFORE:";
//  // Volcar el contenido del buffer en el string en formato hexadecimal
//  for (auto byte : rx_buffer_) {
//    char hexByte[3];
//    sprintf(hexByte, "%02X", byte);
//    bufferHex += hexByte;
//  }
//  ESP_LOGI("UART", "(%d) %s",this->rx_buffer_.size(), bufferHex.c_str());
//  ESP_LOGI("UART", "(%d) to delete", JKPB_RS485_RESPONSE_SIZE);

  this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_RESPONSE_SIZE);
  
//  bufferHex="AFTER:";
//  for (auto byte : rx_buffer_) {
//    char hexByte2[3];
//    sprintf(hexByte2, "%02X", byte);
//    bufferHex += hexByte2;
//  }
//  ESP_LOGI("UART", "(%d) %s",this->rx_buffer_.size(), bufferHex.c_str());


  return(true);
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
