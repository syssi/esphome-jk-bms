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
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS_FOR_FRAME_TYPE_x01 = 264;
static const uint16_t JKPB_RS485_FRAME_COUNTER_ADDRESS = 5;
static const uint16_t JKPB_RS485_CHECKSUM_INDEX = 299;
static const uint16_t JKPB_RS485_ADDRESS_OF_RS485_ADDRESS = 300;
static const uint16_t JKPB_RS485_MASTER_SHORT_REQUEST_SIZE = 8;
static const uint16_t JKPB_RS485_MASTER_REQUEST_SIZE = 11;
static const uint16_t MIN_SILENCE_MILLISECONDS = 150;                           //MIN TIME THAT MEANS THAT THERE IS A SILENCE
static const uint16_t MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS = 250;

static const uint32_t TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS = 30000;
static const uint32_t TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS = 10000;
static const uint32_t TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS=60000;

static const uint16_t SILENCE_BEFORE_ACTING_AS_MASTER = 5000;
static const uint16_t SILENCE_BEFORE_REUSING_NETWORK_ACTING_AS_MASTER=250;

static const uint16_t TIME_BETWEEN_NETWORK_SCAN_MILLISECONDS=500;  // mejorar
static const uint16_t NO_MESSAGE_RECEIVED_TIME_SET_AS_UNAVAILABLE_MILLISECONDS = 10000;

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

uint16_t chksum(const uint8_t data[], const uint16_t len) {
  uint16_t checksum = 0;
  for (uint16_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

void JkRS485Sniffer::send_request_to_slave(uint8_t address, uint8_t frame_type){

    uint8_t frame[11];
    frame[0] = address ;     // start sequence
    frame[1] = 0x10;     // start sequence
    frame[2] = 0x16;     // start sequence
    
    if (frame_type==1){
      frame[3] = 0x1E;     // start sequence
    } else if (frame_type==2){
      frame[3] = 0x20;     // start sequence
    } else if (frame_type==3){
      frame[3] = 0x1C;     // start sequence
    } else {
      return;
    }
    frame[4] = 0x00;     // holding register
    frame[5] = 0x01;     // size of the value in byte
    frame[6] = 0x02;
    frame[7] = 0x00;
    frame[8] = 0x00;
    uint16_t computed_checksum = crc16_c(frame, 9);
    frame[9] = ((computed_checksum & 0xFF00)>>8);
    frame[10] = ((computed_checksum & 0x00FF)>>0);

    ESP_LOGV(TAG, "MESSAGE REQUEST TO SEND>>: %s",format_hex_pretty(frame, 11).c_str());
    // Enviar el array de bytes por UART
    std::vector<uint8_t> data_to_send(frame, frame + sizeof(frame) / sizeof(frame[0]));

    this->talk_pin_->digital_write(1);
    delayMicroseconds(500); //50us
    this->write_array(data_to_send);
    this->flush();
    this->talk_pin_->digital_write(0); 
    delayMicroseconds(500); //50us

    const uint32_t now=millis();

    if (frame_type==1){
      this->rs485_network_node[address].last_device_settings_request_sent=now;  
    } else if (frame_type==2){
      this->rs485_network_node[address].last_cell_info_request_sent=now;  
    } else if (frame_type==3){
      this->rs485_network_node[address].last_device_info_request_sent=now;  
    } else {
  
    }
    
    this->last_jk_rs485_network_activity_=now;
    if (this->act_as_master==true){
      this->last_message_sent_acting_as_master=now;
    }
 

}

bool JkRS485Sniffer::calculate_next_pooling(void){
  //NORMAL POOLING MODE: SAME NODE
  bool found=false;

  const uint32_t now=millis();

  //PENDING INFO FROM ACTUAL ADDRESS NODE?
  if (this->rs485_network_node[this->pooling_index.node_address].available){
    if (this->pooling_index.frame_type==3){ //DEVICE INFO
        if (now-this->rs485_network_node[pooling_index.node_address].last_device_settings_request_sent>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_device_settings_request_sent==0){
          this->pooling_index.frame_type=1; //DEVICE SETTINGS
          found=true;
        } else {
          if (now-this->rs485_network_node[pooling_index.node_address].last_cell_info_request_sent>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_cell_info_request_sent==0 ){
            this->pooling_index.frame_type=2; //CELL INFO
            found=true;
          }          
        }

    } else if (this->pooling_index.frame_type==1){
        if (now-this->rs485_network_node[pooling_index.node_address].last_cell_info_request_sent>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_cell_info_request_sent==0){
          this->pooling_index.frame_type=2; //CELL INFO
          found=true;
        }          

    } else if (this->pooling_index.frame_type==2){


    } else {
      //try_next_address
    }
  } else {
    //try_next_address
  }


  if (found==false){
    //try other address
    uint8_t found_index=0;
    for (uint8_t j = this->pooling_index.node_address+1; j < 16; ++j) {
      if (rs485_network_node[j].available) {
        if (now-this->rs485_network_node[j].last_device_info_request_sent>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_info_request_sent==0){
          this->pooling_index.frame_type=3; //DEVICE INFO
          found=true;
        } else { //DEVICE SETTINGS
          if (now-this->rs485_network_node[j].last_device_settings_request_sent>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_settings_request_sent==0){
            this->pooling_index.frame_type=1; //DEVICE SETTINGS
            found=true;
          } else {
            if (now-this->rs485_network_node[j].last_cell_info_request_sent>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_cell_info_request_sent==0){
              this->pooling_index.frame_type=2; //CELL INFO
              found=true;
            }          
          }
        }        
      }
      if (found==true){
        found_index=j;
        break;
      }      
    }

    if (found==false){
      for (uint8_t j = 1; j <= this->pooling_index.node_address; ++j) {
        if (rs485_network_node[j].available) {
          if (now-this->rs485_network_node[j].last_device_info_request_sent>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_info_request_sent==0){
            this->pooling_index.frame_type=3; //DEVICE INFO
            found=true;
          } else { //DEVICE SETTINGS
            if (now-this->rs485_network_node[j].last_device_settings_request_sent>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_settings_request_sent==0){
              this->pooling_index.frame_type=1; //DEVICE SETTINGS
              found=true;
            } else {
              if (now-this->rs485_network_node[j].last_cell_info_request_sent>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_cell_info_request_sent==0){
                this->pooling_index.frame_type=2; //CELL INFO
                found=true;
              }          
            }
          }
        }
        if (found==true){
          found_index=j;
          break;
        }        
      }
    }    

    if (found==true){
      this->pooling_index.node_address=found_index;
    } else {

    } 
  }

  if (found==true){
    const uint32_t now=millis();
    ESP_LOGI(TAG, "POOLING NEXT AVAILABLE...0x%02X @ %d [%d,%d,%d]",this->pooling_index.node_address,this->pooling_index.frame_type,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_cell_info_request_sent,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_device_settings_request_sent,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_device_info_request_sent);
  } else {

  } 

  return(found);
}

void JkRS485Sniffer::loop() {
  uint32_t now = millis();

  if (this->rx_buffer_.size()==this->rx_buffer_.max_size()){
    ESP_LOGW(TAG, "### Buffer cleared buffer size: %d",this->rx_buffer_.size());
    this->rx_buffer_.clear();
  }

  if (this->available()){
    if ((now-this->last_jk_rs485_network_activity_)>MIN_SILENCE_MILLISECONDS){
      if (this->act_as_master==false){
        ESP_LOGD(TAG, "SILENCE: %f ms",(float)(now-this->last_jk_rs485_network_activity_));
      } else {
        ESP_LOGI(TAG, "SILENCE: %f ms",(float)(now-this->last_jk_rs485_network_activity_));
      }
    
    }

    //bulk to Received data to "rx_buffer_"
    uint8_t byte;
    while (this->available()) {
      this->read_byte(&byte);
      this->rx_buffer_.push_back(byte);
    }

    //manage buffer
    uint8_t response=0;
    uint16_t buffer_size=rx_buffer_.size();
    uint8_t cont_manage=0;
    bool changed=false;
    ESP_LOGD(TAG, "..........................................");
    do {
        cont_manage++;
        ESP_LOGD(TAG, "Buffer before number %d:    %s",cont_manage,format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
        response=this->manage_rx_buffer_();
        ESP_LOGD(TAG, "Response:            %d:",response);
        if (buffer_size==rx_buffer_.size()){
           changed=false;
        } else {
           changed=true;
           buffer_size=this->rx_buffer_.size();   
        }
    } while (cont_manage<5 && changed==true && buffer_size>0);
    
    if (buffer_size==0){
      ESP_LOGD(TAG,     "Buffer clear");
//    } else {
//      ESP_LOGD(TAG,     "Buffer after at the end:   %s",format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    }
    
    
    
    //if (cont_manage==5){
    //  this->rx_buffer_.clear();
    //}
    


    this->last_jk_rs485_network_activity_ = now;    
  } else {
    //NO RX DATA
    if ((now-this->last_jk_rs485_network_activity_)>MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS){
      //CAN TX REQUEST IF NEEDED
      if (now-last_master_activity>SILENCE_BEFORE_ACTING_AS_MASTER){
        if (this->act_as_master==false){
              this->act_as_master=true;
              ESP_LOGI(TAG, "NO JK MASTER DETECTED IN THE NETWORK. ESP WILL ACT AS MASTER");
        }
      }
      
      if (this->act_as_master) {
        
        //NO MASTER HAS BEEN DETECTED IN THE NETWORK --> ESP WILL ACT AS MASTER
        if (now-last_message_sent_acting_as_master>SILENCE_BEFORE_REUSING_NETWORK_ACTING_AS_MASTER){
          // Is an special message to send in the queue?
          // if so, do it and return. TO DO!!!
          
          bool scan_sent=false;
          //SCAN NEXT UNAVAILABLE NODE
          if (now-this->last_network_scan>TIME_BETWEEN_NETWORK_SCAN_MILLISECONDS){
            int found_index=-1;
            for (uint8_t j = this->pooling_index.scan_address+1; j < 16; ++j) {
                if (!rs485_network_node[j].available) {
                    found_index=j;
                    break;
                }
            }
            if (found_index==-1){
              for (uint8_t j = 1; j <= this->pooling_index.scan_address; ++j) {
                  if (!rs485_network_node[j].available) {
                      found_index=j;
                      break;
                  }
              }
            }
            if (found_index==-1){
              //all nodes are available now
              ESP_LOGD(TAG, "SCANNING TO DISCOVER...NO INDEX FOUND");
            } else {
              ESP_LOGD(TAG, "SCANNING TO DISCOVER...0x%02X",found_index);
              this->pooling_index.scan_address=found_index;
              this->send_request_to_slave(found_index,2);
              this->last_network_scan=millis();
              scan_sent=true;
            }
          }
          
          if (scan_sent==false){
            if (this->nodes_available_number>0){
              //NORMAL POOLING LOOP AS MASTER
              //FIRST CHECK IF BASIC INFO ARRIVED FROM EACH SLAVE
              for (uint8_t cont=0;cont<16;cont++){
                if (this->rs485_network_node[cont].available){
                  if (this->rs485_network_node[cont].counter_device_info_received==0){
                    this->rs485_network_node[cont].last_device_info_request_sent=0;
                  }
                  if (this->rs485_network_node[cont].counter_device_settings_received==0){
                    this->rs485_network_node[cont].last_device_settings_request_sent=0;
                  }                  
                }
              }

              if (this->calculate_next_pooling()==true){
                ESP_LOGD(TAG, "CALCULATED NEXT POOLING...0x%02X @ %d",this->pooling_index.node_address,this->pooling_index.frame_type);
                this->send_request_to_slave(this->pooling_index.node_address,this->pooling_index.frame_type);
              }
            }
          }
        }

      } else {
        //SPEAK WHEN A MASTER IS IN THE NETWORK
        for (uint8_t cont=0;cont<16;cont++){
          if (this->rs485_network_node[cont].available==true){
            //repeat device info request
            if (now-rs485_network_node[cont].last_device_info_request_sent>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS ||
               this->rs485_network_node[cont].last_device_info_request_sent==0){

              send_request_to_slave(cont,03);

              now=millis();
              rs485_network_node[cont].last_device_info_request_sent=now;          
              this->last_jk_rs485_network_activity_=now; 
              break;
            }

          }
        }

        //decide if node is available (if none info recieved during a time from that address)
        for (uint8_t cont=0;cont<16;cont++){
          if (now-this->rs485_network_node[cont].last_message_received>NO_MESSAGE_RECEIVED_TIME_SET_AS_UNAVAILABLE_MILLISECONDS){
            this->set_node_availability(cont,0);
          }

          //periodically test !!!!!!
          if (this->rs485_network_node[cont].available && cont>0){
            if (this->rs485_network_node[cont].counter_device_info_received==0){
              this->rs485_network_node[cont].last_device_info_request_sent=0;
            }                
          }
          
        }
      
      }
    }
  }
}

//void JkRS485Sniffer::send_rs485_message(uint8_t message[])
//}




std::string JkRS485Sniffer::nodes_available_to_string() {
    std::string bufferHex;
    bufferHex.reserve(17); // Reservar espacio para 16 caracteres + 1 para el carácter nulo
    uint8_t number_of_nodes_available=0;
    for (uint8_t cont = 0; cont < 16; cont++) {
        // Volcar el contenido del buffer en el string en formato hexadecimal
        if (this->rs485_network_node[cont].available) {
            bufferHex.push_back('1');
            number_of_nodes_available++;
        } else {
            bufferHex.push_back('0');
        }
    }
    
    bufferHex.push_back('\0'); // Agregar el carácter nulo al final de la cadena
    this->nodes_available_number=number_of_nodes_available;
    return bufferHex;
}



void JkRS485Sniffer::set_node_availability(uint8_t address,bool value){
  if (this->rs485_network_node[address].available==value){
    //no changes
  } else {
    uint8_t previous_value=this->rs485_network_node[address].available;
    this->rs485_network_node[address].available=value;

    std::string previous=this->nodes_available;
    this->nodes_available=this->nodes_available_to_string();
    ESP_LOGI(TAG, "NODES AVAILABLE CHANGED: address 0x%02X (%d->%d) [%s] --> [%s]",address,previous_value,value,previous.c_str(),this->nodes_available.c_str());
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

void JkRS485Sniffer::detected_master_activity_now(void){
  const uint32_t now = millis();

  if (this->act_as_master){
    this->act_as_master=false;
    ESP_LOGI(TAG, "JK MASTER DETECTED IN THE NETWORK");
  }
  this->last_master_activity=now;
}



uint8_t JkRS485Sniffer::manage_rx_buffer_(void) {
  size_t at = this->rx_buffer_.size();

  const uint8_t *raw = &this->rx_buffer_[0];
  uint8_t address = 0;

  const uint32_t now = millis();

  if (at>=JKPB_RS485_MASTER_SHORT_REQUEST_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
    if (it == this->rx_buffer_.end()) {
      // Start sequence NOT FOUND (0x55AAEB90) --> maybe short response to a real master request?

      uint16_t computed_checksum = crc16_c(raw, 6); 
      uint16_t remote_checksum = ((uint16_t(raw[6]) << 8) | (uint16_t(raw[7]) << 0) );

      if (computed_checksum != remote_checksum) {
        ESP_LOGV(TAG, "CHECKSUM failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
        //NO, OR THERE WAS A COMM. ERROR
      } else {
        address=raw[0];
        ESP_LOGI(TAG, "REAL master is speaking to address 0x%02X (short request)",address);

        this->rs485_network_node[0].last_message_received=now;
        this->detected_master_activity_now();
        
        this->set_node_availability(0,1);
        std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE-1);
        ESP_LOGD(TAG, "Frame received from MASTER (type: SHORT REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE); 
        at = this->rx_buffer_.size();
        //continue with next;
        return(7);
      }
    }
  }

  if (at>=JKPB_RS485_MASTER_REQUEST_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
    bool try_with_master_request_size=false;
    if (it == this->rx_buffer_.end()) {
      //no sequence
      try_with_master_request_size=true;
    } else {
      //sequence found, but where?
      size_t index = std::distance(this->rx_buffer_.begin(), it);
      if (index>=JKPB_RS485_MASTER_REQUEST_SIZE){
        try_with_master_request_size=true;
      }      
    }

    if (try_with_master_request_size==true){
      // Start sequence NOT FOUND (0x55AAEB90) --> maybe short response to a real master request?

      uint16_t computed_checksum = crc16_c(raw, 9); 
      uint16_t remote_checksum = ((uint16_t(raw[9]) << 8) | (uint16_t(raw[10]) << 0) );

      if (computed_checksum != remote_checksum) {
        ESP_LOGV(TAG, "CHECKSUM failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
        //NO, OR THERE WAS A COMM. ERROR
      } else {
        address=raw[0];
        ESP_LOGI(TAG, "REAL master is speaking to address 0x%02X (request)",address);
        this->rs485_network_node[0].last_message_received=now;
        this->detected_master_activity_now();
        this->set_node_availability(0,1);
        std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE-1);
        ESP_LOGD(TAG, "Frame received from MASTER (type: REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE); 
        at = this->rx_buffer_.size();
        //continue with next;
        return(6);
      }
    }
  }

  if (at>=JKPB_RS485_RESPONSE_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());

    if (it != this->rx_buffer_.end()) {
      //Sequence found, but where?

      size_t index = std::distance(this->rx_buffer_.begin(), it);
      
      if (index>0){
        //printBuffer(index);
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + index);    
        at = this->rx_buffer_.size();
        //continue with next;            
      }

      if (at>=JKPB_RS485_RESPONSE_SIZE){
        //continue
        ESP_LOGV(TAG, "###############################Sequence found SIZE: %d",(at));     
      } else {
        return(3);
      }
    } else {
      return(4);
    }  
  } else {
    return(5);
  }


  // Start sequence (0x55AAEB90) //55aaeb90 0105

  if (at >= JKPB_RS485_RESPONSE_SIZE){
    uint8_t computed_checksum = chksum(raw, JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM);
    uint8_t remote_checksum = raw[JKPB_RS485_CHECKSUM_INDEX];

    if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==1){
      address=raw[JKPB_RS485_FRAME_TYPE_ADDRESS_FOR_FRAME_TYPE_x01+6];
    } else {
      address=raw[JKPB_RS485_ADDRESS_OF_RS485_ADDRESS];
    }
    //ESP_LOGI(TAG, "(at:%03d) [address 0x%02X] Frame Type 0x%02X ",at,address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);

    if (computed_checksum != remote_checksum) {
      ESP_LOGW(TAG, "CHECKSUM failed! 0x%02X != 0x%02X", computed_checksum, remote_checksum);
      return(10);
    } else {
      this->rs485_network_node[address].last_message_received=now;
      if (address==0){
        last_master_activity=now;
      } else if (address>15){
        ESP_LOGV(TAG, "(at:%03d) [address 0x%02X] Frame Type 0x%02X | CHECKSUM is correct",at,address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);
        //printBuffer(0);
        this->rx_buffer_.clear();
        return(11);
      } else {
        this->set_node_availability(address,1);
      }
      
    }

    //counter++
    if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==0x02){
      this->rs485_network_node[address].counter_cell_info_received++;
    } else if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==0x01){
      this->rs485_network_node[address].counter_device_settings_received++;
    } else if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==0x03){
      this->rs485_network_node[address].counter_device_info_received++;
    }


    uint16_t data_len=at+1;

    std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + data_len);
    
    ESP_LOGD(TAG, "Frame received from SLAVE (type: 0x%02X, %d bytes) %02X address", raw[4], data.size(),address);
    ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

    bool found = false;
    for (auto *device : this->devices_) {
        device->on_jk_rs485_sniffer_data(address, raw[JKPB_RS485_FRAME_TYPE_ADDRESS], data, this->nodes_available );   
        found = true;
    }

    if (!found) {
      ESP_LOGW(TAG, "Got JkRS485 but no recipients to send [frame type:0x%02X] 0x%02X!",raw[JKPB_RS485_FRAME_TYPE_ADDRESS], address);
    }
  } else {
    //    ESP_LOGD(TAG, "at=%02d",at); 
  }

  this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_RESPONSE_SIZE);
  at = this->rx_buffer_.size();
  return(12);
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
