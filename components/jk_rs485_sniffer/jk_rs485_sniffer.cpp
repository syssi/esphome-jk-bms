#include "jk_rs485_sniffer.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace jk_rs485_sniffer {

static const char *const TAG = "jk_rs485_sniffer";

static const uint16_t JKPB_RS485_RESPONSE_SIZE = 308;
static const uint16_t JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM = 299;
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS = 4;
static const uint16_t JKPB_RS485_FRAME_COUNTER_ADDRESS = 5;
static const uint16_t JKPB_RS485_CHECKSUM_INDEX = 299;
static const uint16_t JKPB_RS485_ADDRESS_OF_RS485_ADDRESS = 300;
static const uint16_t JKPB_RS485_MASTER_SHORT_REQUEST_SIZE = 8;
static const uint16_t JKPB_RS485_MASTER_REQUEST_SIZE = 11;
static const uint16_t MIN_SILENCE_MILLISECONDS = 100;
static const uint16_t MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS = 250;
static const uint16_t TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS = 30000;
static const uint16_t NO_MESSAGE_RECEIVED_TIME_SET_AS_UNAVAILABLE_MILLISECONDS = 60000;

std::vector<unsigned char> pattern_response_header = {0x55, 0xAA, 0xEB, 0x90};

std::vector<uint8_t> hexStringToVector(const std::string& hexString) {
  std::vector<uint8_t> result;
  for (size_t i = 0; i < hexString.length(); i += 2) {
    std::string byteString = hexString.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
    result.push_back(byte);
  }
  return result;
}

uint16_t crc16_c(const uint8_t data[], const uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if ((crc & 0x1) == 1) {
                crc = (crc >> 1) ^ 40961;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return (((crc & 0x00FF)<<8) | ((crc & 0xFF00))>>8);
}

void JkRS485Sniffer::loop() {
  const uint32_t now = millis();

  if (this->rx_buffer_.size()==this->rx_buffer_.max_size()){
    ESP_LOGI(TAG, "Buffer cleared buffer size: %d",this->rx_buffer_.size());
    this->rx_buffer_.clear();
  }

  if (this->available()){
    if ((now-this->last_jk_rs485_sniffer_byte_)>MIN_SILENCE_MILLISECONDS){
      ESP_LOGI(TAG, "SILENCE: %f ms",(float)(now-this->last_jk_rs485_sniffer_byte_));
    
    }
    while (this->available()) {
      uint8_t byte;
      this->read_byte(&byte);
      if (this->parse_jk_rs485_sniffer_byte_(byte)) {
      } else {
        ESP_LOGVV(TAG, "Buffer cleared due to reset: %s",
                  format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
        //this->rx_buffer_.clear();
      }
    }
    this->last_jk_rs485_sniffer_byte_ = now;    
  } else {
    //NO RX DATA
    if ((now-this->last_jk_rs485_sniffer_byte_)>MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS){
      //CAN TX REQUEST IF NEEDED
      //output.turn_on: talk_gpio
      //this->write_state(1);

      bool talk_activated = false;
      //SPEAK
      for (uint8_t cont=0;cont<16;cont++){
        if (this->rs485_network_node[cont].available==1){
          //repeat device info request
          if (now-rs485_network_node[cont].last_device_info_request_sent>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS){
            if (talk_activated==false){
              talk_activated=true;
              this->talk_pin_->digital_write(1);
              delayMicroseconds(50);
            }

            rs485_network_node[cont].last_device_info_request_sent=now;
            ESP_LOGI(TAG, ".................................SENDING DEVICE INFO REQUEST TO address 0x%02X",cont);
            delayMicroseconds(100000); //////////////////////////////////////////////////
            uint8_t frame[20];
            frame[0] = cont;     // start sequence
            frame[1] = 0x10;     // start sequence
            frame[2] = 0x16;     // start sequence
            frame[3] = 0x1C;     // start sequence
            frame[4] = 0x00;     // holding register
            frame[5] = 0x01;     // size of the value in byte
            frame[6] = 0x02;
            frame[7] = 0x00;
            frame[8] = 0x00;
            uint16_t computed_checksum = crc16_c(frame, 9);
            frame[9] = ((computed_checksum & 0xFF00)>>8);
            frame[10] = ((computed_checksum & 0x00FF)>>0);

            ESP_LOGI(TAG, "MESSAGE TO SEND: %s",
                  format_hex_pretty(frame, 11).c_str());

            break;
          }
          //decide if device is available (if none info recieved during a time from that address)
          if (now-this->rs485_network_node[cont].last_message_received>NO_MESSAGE_RECEIVED_TIME_SET_AS_UNAVAILABLE_MILLISECONDS){
            this->set_node_availability(cont,0);
          }
        }

      }
      this->talk_pin_->digital_write(0);  
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



std::string JkRS485Sniffer::nodes_available_to_string() {
    std::string bufferHex="";
    for (uint8_t cont=0;cont<16;cont++){
      // Volcar el contenido del buffer en el string en formato hexadecimal
      if (this->rs485_network_node[cont].available){
        bufferHex.append("1");
      } else {
        bufferHex.append("0");
      }
    }
    ESP_LOGI(TAG, "NODES AVAILABLE CHANGED TO [%s]",this->nodes_available.c_str());
    return bufferHex;
}

void JkRS485Sniffer::set_node_availability(uint8_t address,bool value){
  if (this->rs485_network_node[address].available==value){
    //no changes
  } else {
    this->rs485_network_node[address].available=value;

    this->nodes_available=this->nodes_available_to_string();
  }
}




void JkRS485Sniffer::printBuffer(uint16_t max_length){
  std::string bufferHex;

  bufferHex="";
  // Volcar el contenido del buffer en el string en formato hexadecimal
  for (auto byte : this->rx_buffer_) {
      char hexByte[3];
      sprintf(hexByte, "%02X", byte);
      bufferHex += hexByte;
      if (max_length>0 and 2*max_length<=bufferHex.length()){
        break;
      }
  }  
  ESP_LOGI("BUFFER", "(%d): %s",this->rx_buffer_.size(), bufferHex.c_str());
}

bool JkRS485Sniffer::parse_jk_rs485_sniffer_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);

  const uint8_t *raw = &this->rx_buffer_[0];
  uint8_t address = 0;

  const uint32_t now = millis();

  if (at==JKPB_RS485_MASTER_SHORT_REQUEST_SIZE-1){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
    if (it == this->rx_buffer_.end()) {
      // Calcular el índice de la secuencia

      uint16_t computed_checksum = crc16_c(raw, 6); 
      uint16_t remote_checksum = ((uint16_t(raw[6]) << 8) | (uint16_t(raw[7]) << 0) );

      if (computed_checksum != remote_checksum) {
        ESP_LOGD(TAG, "CHECKSUM failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
        return false;
      } else {
        ESP_LOGI(TAG, " CHECKSUM is correct");
        address=raw[0];
        rs485_network_node[0].last_message_received=now;
        this->set_node_availability(0,1);
        std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE-1);
        ESP_LOGI(TAG, "Frame received from MASTER (type: SHORT REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE); 
      }
    }
  }

  if (at==JKPB_RS485_MASTER_REQUEST_SIZE-1){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
    if (it == this->rx_buffer_.end()) {
      // Calcular el índice de la secuencia

      uint16_t computed_checksum = crc16_c(raw, 9); 
      uint16_t remote_checksum = ((uint16_t(raw[9]) << 8) | (uint16_t(raw[10]) << 0) );

      if (computed_checksum != remote_checksum) {
        ESP_LOGD(TAG, "CHECKSUM failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
        return false;
      } else {
        ESP_LOGD(TAG, " CHECKSUM is correct");
        address=raw[0];
        this->rs485_network_node[0].last_message_received=now;
        this->set_node_availability(0,1);
        std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE-1);
        ESP_LOGI(TAG, "Frame received from MASTER (type: REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE); 
      }
    }
  }

  if (at>=JKPB_RS485_RESPONSE_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());

    // Si se encontró la secuencia
    if (it != this->rx_buffer_.end()) {
      // Calcular el índice de la secuencia

      size_t index = std::distance(this->rx_buffer_.begin(), it);
      
      if (index>0){
        //printBuffer(index);
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + index);        
        return(false);
      }

      if (this->rx_buffer_.size()>=JKPB_RS485_RESPONSE_SIZE){
        //continue
        ESP_LOGD(TAG, "Sequence found SIZE: %d",(at));     
      } else {
        return(false);
      }
    } else {
      return(false);
    }  
  } else {
    return(false);
  }


  //const uint8_t *raw = &this->rx_buffer_[0];
  //uint8_t address = 0;

  // Start sequence (0x55AAEB90) //55aaeb90 0105

  if (at == JKPB_RS485_RESPONSE_SIZE){
    uint8_t computed_checksum = chksum(raw, JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM);
    uint8_t remote_checksum = raw[JKPB_RS485_CHECKSUM_INDEX];

    address=raw[JKPB_RS485_ADDRESS_OF_RS485_ADDRESS];
    if (computed_checksum != remote_checksum) {
      ESP_LOGW(TAG, "CHECKSUM failed! 0x%02X != 0x%02X", computed_checksum, remote_checksum);
      return false;
    } else {
      this->rs485_network_node[address].last_message_received=now;
      this->set_node_availability(address,1);
      ESP_LOGD(TAG, "(at:%03d) [address 0x%02X] Frame Type 0x%02X | CHECKSUM is correct",at,address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);
    }

    uint16_t data_len=at+1;

    std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + data_len);
    
    ESP_LOGI(TAG, "Frame received from SLAVE (type: 0x%02X, %d bytes) %02X address", raw[4], data.size(),address);
    ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

    bool found = false;
    for (auto *device : this->devices_) {
        device->on_jk_rs485_sniffer_data(address, raw[JKPB_RS485_FRAME_TYPE_ADDRESS], data, this->nodes_available );   
        if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==0x03){
          rs485_network_node[address].last_device_info_received=now;
        }
        found = true;
    }

    if (!found) {
      ESP_LOGW(TAG, "Got JkRS485 but no recipients to send [frame type:0x%02X] 0x%02X!",raw[JKPB_RS485_FRAME_TYPE_ADDRESS], address);
    }
  } else {
    //    ESP_LOGI(TAG, "at=%02d",at); 
  }

  this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_RESPONSE_SIZE);

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
