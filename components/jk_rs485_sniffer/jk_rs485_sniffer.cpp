#include "jk_rs485_sniffer.h"


namespace esphome {
namespace jk_rs485_sniffer {

static const char *const TAG = "jk_rs485_sniffer";

static const uint16_t JKPB_RS485_MASTER_SHORT_REQUEST_SIZE = 8;
static const uint16_t JKPB_RS485_MASTER_REQUEST_SIZE = 11;
static const uint16_t JKPB_RS485_RESPONSE_SIZE = 308;

static const uint16_t JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM = 299;
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS = 4;
static const uint16_t JKPB_RS485_FRAME_TYPE_ADDRESS_FOR_FRAME_TYPE_x01 = 264;
static const uint16_t JKPB_RS485_FRAME_COUNTER_ADDRESS = 5;
static const uint16_t JKPB_RS485_CHECKSUM_INDEX = 299;
static const uint16_t JKPB_RS485_ADDRESS_OF_RS485_ADDRESS = 300;

static const uint16_t MIN_SILENCE_MILLISECONDS = 150;                           //MIN TIME THAT MEANS THAT THERE IS A SILENCE
static const uint16_t MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS = 250;

static const uint32_t TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS       =  5000;
static const uint32_t TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS = 10000; //5000
static const uint32_t TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS     = 3600000; //3600000

static const uint16_t SILENCE_BEFORE_ACTING_AS_MASTER = 2000;
static const uint16_t SILENCE_BEFORE_REUSING_NETWORK_ACTING_AS_MASTER=400;
static const uint16_t TIME_BEFORE_NEXT_POOLING_MILLISENCONDS=750;
static const uint16_t TIME_BETWEEN_CONSECUTIVE_REQUEST_SENDINGS_TO_SAME_SLAVE=2500;


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


void JkRS485Sniffer::set_broadcast_changes_to_all_bms(bool state) {
    this->broadcast_changes_to_all_bms_ = state;
    if (state) {
      ESP_LOGD(TAG, "Broadcast to All BMSs ACTIVATED");
      // Aquí puedes añadir el código que se ejecuta cuando se activa
    } else {
      ESP_LOGD(TAG, "Broadcast to All BMSs DEACTIVATED");
      // Aquí puedes añadir el código que se ejecuta cuando se desactiva
    }
}



bool JkRS485Sniffer::get_broadcast_changes_to_all_bms() const {
    return this->broadcast_changes_to_all_bms_;
}


void JkRS485Sniffer::handle_bms2sniffer_event(std::uint8_t slave_address, std::string event, std::uint8_t frame_type){
  // Maneja el evento aquí. Por ejemplo, puedes imprimir el evento:
  ESP_LOGD(TAG,"Received Event from BMS.. [address:0x%02X] @ %d -->  %s", slave_address, frame_type, event.c_str());
  const uint32_t now=millis();

  if (frame_type==1){
    this->rs485_network_node[slave_address].last_device_settings_request_received_OK=now;  
    this->rs485_network_node[slave_address].counter_device_settings_received++;
    ESP_LOGD(TAG, "updated last_device_settings_request_received_OK");
  } else if (frame_type==2){
    this->rs485_network_node[slave_address].last_cell_info_request_received_OK=now;  
    this->rs485_network_node[slave_address].counter_cell_info_received++;
    ESP_LOGD(TAG, "updated last_cell_info_request_received_OK");
  } else if (frame_type==3){
    this->rs485_network_node[slave_address].last_device_info_request_received_OK=now;  
    this->rs485_network_node[slave_address].counter_device_info_received++;
    ESP_LOGD(TAG, "updated last_device_info_request_received_OK");
  } else {

  }


  this->last_jk_rs485_network_activity_=now;
  if (this->act_as_master==true){
    this->last_message_received_acting_as_master=now;
  }  
}

void JkRS485Sniffer::handle_bms2sniffer_switch_or_number_uint32_event(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::uint32_t value) {

  // 02.10.10.78.00.02.04.00.00.00.00.37.A9
  // 02.10.10.78.00.02.04.00.00.00.01.F6.69.

  // 02.10.10.78.00.02.04.00.00.00.00.37.A9

  // 02.10.10.00.00.02.04.00.00.0D.AC.35.C6 (13)

  if (rs485_network_node[slave_address].available) {
    send_command_switch_or_number_to_slave_uint32(slave_address,third_element_of_frame,register_address,value);
  }

  if (this->broadcast_changes_to_all_bms_==true){
    for (uint8_t j = 1; j < 16; ++j) {
        if (rs485_network_node[j].available && slave_address!=j) {
            delayMicroseconds(50000);
            send_command_switch_or_number_to_slave_uint32(j,third_element_of_frame,register_address,value);
        }
    }
  }


  

//  return (status == 0);
}


void JkRS485Sniffer::handle_bms2sniffer_switch_or_number_int32_event(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::int32_t value) {

  // 02.10.10.78.00.02.04.00.00.00.00.37.A9
  // 02.10.10.78.00.02.04.00.00.00.01.F6.69.

  // 02.10.10.78.00.02.04.00.00.00.00.37.A9

  if (rs485_network_node[slave_address].available) {
    send_command_switch_or_number_to_slave_int32(slave_address,third_element_of_frame,register_address,value);
  }

  if (this->broadcast_changes_to_all_bms_==true){
    for (uint8_t j = 1; j < 16; ++j) {
        if (rs485_network_node[j].available && slave_address!=j) {
            delayMicroseconds(50000);
            send_command_switch_or_number_to_slave_int32(j,third_element_of_frame,register_address,value);
        }
    }
  }


  

//  return (status == 0);
}

void JkRS485Sniffer::handle_bms2sniffer_switch_or_number_uint16_event(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::uint16_t value) {

  if (rs485_network_node[slave_address].available) {
    send_command_switch_or_number_to_slave_uint16(slave_address,third_element_of_frame,register_address,value);
    rs485_network_node[slave_address].last_device_info_request_received_OK=0;
  }

  if (this->broadcast_changes_to_all_bms_==true){
    for (uint8_t j = 1; j < 16; ++j) {
        if (rs485_network_node[j].available && slave_address!=j) {
            delayMicroseconds(50000);
            send_command_switch_or_number_to_slave_uint16(j,third_element_of_frame,register_address,value);
            rs485_network_node[j].last_device_info_request_received_OK=0;
        }
    }
  }


  

//  return (status == 0);
}

void JkRS485Sniffer::send_command_switch_or_number_to_slave_uint32(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::uint32_t value) {
  uint8_t frame[13];
  uint8_t size=0;

  //[0x0000, 0x10,   0x04,  3,  0],
  //SENT:02.10.00.04.00.02.04.00.00.0D.AC.F9.F5 (13)
      // 02.10.10.78.00.02.04.00.00.00.00.37.A9
      // 02.10.10.78.00.02.04.00.00.00.01.F6.69.
      // 02.10.10.78.00.02.04.00.00.00.00.37.A9


  frame[0]  = slave_address ;                   // Slave Address
  frame[1]  = 0x10;                             // 
  frame[2]  = third_element_of_frame;           // 
  frame[3]  = (register_address & 0x00FF) >> 0;                 // Register address
  frame[4]  = 0x00;                             // 
  frame[5]  = 0x02;                             // 
  frame[6]  = 4;                                // Length of data in number of Bytes
  frame[7]  = (value & 0xFF000000) >> 24;       // Data Byte 1
  frame[8]  = (value & 0x00FF0000) >> 16;       // Data Byte 2
  frame[9]  = (value & 0x0000FF00) >> 8;        // Data Byte 3
  frame[10] = (value & 0x000000FF) >> 0;        // Data Byte 4
  size=13;

  uint16_t computed_checksum = crc16_c(frame, size-2);
  frame[11] = ((computed_checksum & 0xFF00)>>8);
  frame[12] = ((computed_checksum & 0x00FF)>>0);


  ESP_LOGD(TAG, "MESSAGE REQUEST TO SEND switch >>: %s",format_hex_pretty(frame, size).c_str());
//  // Enviar el array de bytes por UART
  std::vector<uint8_t> data_to_send(frame, frame + size / sizeof(frame[0]));

  if (talk_pin_needed_) { this->talk_pin_->digital_write(1); }
  delayMicroseconds(50); //50us
  this->write_array(data_to_send);
  this->flush();
  if (talk_pin_needed_) { this->talk_pin_->digital_write(0); } 
  delayMicroseconds(50); //50us
   
  const uint32_t now=millis();
  this->rs485_network_node[slave_address].last_request_sent=now;  
  this->last_jk_rs485_network_activity_=now;   
}

void JkRS485Sniffer::send_command_switch_or_number_to_slave_uint16(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::uint16_t value) {
  uint8_t frame[13];
  uint8_t size=0;

  //02.10.11.14.00.01.   02.  02.10.   B0.19

  frame[0]  = slave_address ;                   // Slave Address
  frame[1]  = 0x10;                             // 
  frame[2]  = third_element_of_frame;                      // 
  frame[3]  = register_address;                 // Register address
  frame[4]  = 0x00;                             // 
  frame[5]  = 0x01;                             // 
  frame[6]  = 2;                                // Length of data in number of Bytes
  frame[7]  = (value & 0xFF00) >> 8;            // Data Byte 1
  frame[8]  = (value & 0x00FF) >> 0;            // Data Byte 2
  size=11; 

  uint16_t computed_checksum = crc16_c(frame, size-2);
  frame[9] = ((computed_checksum & 0xFF00)>>8);
  frame[10] = ((computed_checksum & 0x00FF)>>0);
     


  ESP_LOGD(TAG, "MESSAGE REQUEST TO SEND switch or number >>: %s",format_hex_pretty(frame, size).c_str());
//  // Enviar el array de bytes por UART
  std::vector<uint8_t> data_to_send(frame, frame + size / sizeof(frame[0]));

  if (talk_pin_needed_) { this->talk_pin_->digital_write(1); }
  delayMicroseconds(50); //50us
  this->write_array(data_to_send);
  this->flush();
  if (talk_pin_needed_) { this->talk_pin_->digital_write(0); } 
  delayMicroseconds(50); //50us
   
  const uint32_t now=millis();
  this->rs485_network_node[slave_address].last_request_sent=now;  
  this->last_jk_rs485_network_activity_=now;   
}

void JkRS485Sniffer::send_command_switch_or_number_to_slave_int32(std::uint8_t slave_address, std::uint8_t third_element_of_frame, std::uint16_t register_address, std::int32_t value) {
  uint8_t frame[13];
  uint8_t size=0;

  frame[0]  = slave_address ;                   // Slave Address
  frame[1]  = 0x10;                             // 
  frame[2]  = third_element_of_frame;           // 
  frame[3]  = register_address;                 // Register address
  frame[4]  = 0x00;                             // 
  frame[5]  = 0x02;                             // 
  frame[6]  = 4;                                // Length of data in number of Bytes
  frame[7]  = (value & 0xFF000000) >> 24;       // Data Byte 1
  frame[8]  = (value & 0x00FF0000) >> 16;       // Data Byte 2
  frame[9]  = (value & 0x0000FF00) >> 8;        // Data Byte 3
  frame[10] = (value & 0x000000FF) >> 0;        // Data Byte 4
  size=13;

  uint16_t computed_checksum = crc16_c(frame, size-2);
  frame[11] = ((computed_checksum & 0xFF00)>>8);
  frame[12] = ((computed_checksum & 0x00FF)>>0);



  ESP_LOGD(TAG, "MESSAGE REQUEST TO SEND switch or number >>: %s",format_hex_pretty(frame, size).c_str());
//  // Enviar el array de bytes por UART
  std::vector<uint8_t> data_to_send(frame, frame + size / sizeof(frame[0]));

  if (talk_pin_needed_) { this->talk_pin_->digital_write(1); }
  delayMicroseconds(50); //50us
  this->write_array(data_to_send);
  this->flush();
  if (talk_pin_needed_) { this->talk_pin_->digital_write(0); } 
  delayMicroseconds(50); //50us
   
  const uint32_t now=millis();
  this->rs485_network_node[slave_address].last_request_sent=now;  
  this->last_jk_rs485_network_activity_=now;   
}


void JkRS485Sniffer::send_request_to_slave(uint8_t address, uint8_t frame_type){

    uint8_t frame[11];
    frame[0] = address ;        // start sequence
    frame[1] = 0x10;            // start sequence
    frame[2] = 0x16;            // start sequence
    
    if (frame_type==1){
      frame[3] = 0x1E;          // start sequence
    } else if (frame_type==2){
      frame[3] = 0x20;          // start sequence
    } else if (frame_type==3){
      frame[3] = 0x1C;          // start sequence
    } else {
      return;
    }  
    frame[4] = 0x00;            // holding register
    frame[5] = 0x01;            // size of the value in byte
    frame[6] = 0x02;            // command word: 0x01 (activation), 0x02 (write), 0x03 (read), 0x05 (password), 0x06 (read all)
    frame[7] = 0x00;
    frame[8] = 0x00;
    
    uint16_t computed_checksum = crc16_c(frame, 9);
    frame[9] = ((computed_checksum & 0xFF00)>>8);
    frame[10] = ((computed_checksum & 0x00FF)>>0);

    ESP_LOGV(TAG, "MESSAGE REQUEST TO SEND>>: %s",format_hex_pretty(frame, 11).c_str());
    // Enviar el array de bytes por UART
    std::vector<uint8_t> data_to_send(frame, frame + sizeof(frame) / sizeof(frame[0]));

    if (talk_pin_needed_) { this->talk_pin_->digital_write(1); }
    delayMicroseconds(50); //50us
    this->write_array(data_to_send);
    this->flush();
    if (talk_pin_needed_) { this->talk_pin_->digital_write(0); } 
    delayMicroseconds(50); //50us


    const uint32_t now=millis();
    this->rs485_network_node[address].last_request_sent=now;  
    this->last_jk_rs485_network_activity_=now;     
//    this->last_jk_rs485_network_activity_=now;
//    if (this->act_as_master==true){
//      this->last_message_received_acting_as_master=now;
//    }  

}




bool JkRS485Sniffer::calculate_next_pooling(void){
  //NORMAL POOLING MODE: SAME NODE
  bool found=false;

  const uint32_t now=millis();

  //PENDING INFO FROM ACTUAL ADDRESS NODE?
  if (this->rs485_network_node[this->pooling_index.node_address].available && (now-this->rs485_network_node[this->pooling_index.node_address].last_request_sent)>TIME_BETWEEN_CONSECUTIVE_REQUEST_SENDINGS_TO_SAME_SLAVE){
    if (now-this->rs485_network_node[pooling_index.node_address].last_device_info_request_received_OK>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_device_info_request_received_OK==0){
      this->pooling_index.frame_type=3; //DEVICE INFO
      found=true;
    } else {
        if (now-this->rs485_network_node[pooling_index.node_address].last_device_settings_request_received_OK>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_device_settings_request_received_OK==0){
          this->pooling_index.frame_type=1; //DEVICE SETTINGS
          found=true;
        } else {
            if (now-this->rs485_network_node[pooling_index.node_address].last_cell_info_request_received_OK>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[pooling_index.node_address].last_cell_info_request_received_OK==0){
              this->pooling_index.frame_type=2; //CELL INFO
              found=true;
            }
        }
    }
  }


  if (found==false){
    //try other address
    uint8_t found_index=0;
    for (uint8_t j = this->pooling_index.node_address+1; j < 16; ++j) {
      if (rs485_network_node[j].available && (now-this->rs485_network_node[j].last_request_sent)>TIME_BETWEEN_CONSECUTIVE_REQUEST_SENDINGS_TO_SAME_SLAVE) {
        if (now-this->rs485_network_node[j].last_device_info_request_received_OK>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_info_request_received_OK==0){
          this->pooling_index.frame_type=3; //DEVICE INFO
          found=true;
        } else { //DEVICE SETTINGS
          if (now-this->rs485_network_node[j].last_device_settings_request_received_OK>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_settings_request_received_OK==0){
            this->pooling_index.frame_type=1; //DEVICE SETTINGS
            found=true;
          } else {
            if (now-this->rs485_network_node[j].last_cell_info_request_received_OK>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_cell_info_request_received_OK==0){
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
        if (rs485_network_node[j].available && (now-this->rs485_network_node[j].last_request_sent)>TIME_BETWEEN_CONSECUTIVE_REQUEST_SENDINGS_TO_SAME_SLAVE) {
          if (now-this->rs485_network_node[j].last_device_info_request_received_OK>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_info_request_received_OK==0){
            this->pooling_index.frame_type=3; //DEVICE INFO
            found=true;
          } else { //DEVICE SETTINGS
            if (now-this->rs485_network_node[j].last_device_settings_request_received_OK>TIME_BETWEEN_DEVICE_SETTINGS_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_device_settings_request_received_OK==0){
              this->pooling_index.frame_type=1; //DEVICE SETTINGS
              found=true;
            } else {
              if (now-this->rs485_network_node[j].last_cell_info_request_received_OK>TIME_BETWEEN_CELL_INFO_REQUESTS_MILLISECONDS || this->rs485_network_node[j].last_cell_info_request_received_OK==0){
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
    ESP_LOGI(TAG, "POOLING NEXT AVAILABLE... [address:0x%02X] @ %d [%u,%u,%u]",this->pooling_index.node_address,this->pooling_index.frame_type,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_device_settings_request_received_OK,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_cell_info_request_received_OK,
                                                          now-this->rs485_network_node[this->pooling_index.node_address].last_device_info_request_received_OK);
  } else {

  } 

  return(found);
}

int JkRS485Sniffer::found_next_node_to_discover(void){
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
  return(found_index);
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
    while (this->available() && (this->rx_buffer_.size()<this->rx_buffer_.max_size())) {
      this->read_byte(&byte);
      this->rx_buffer_.push_back(byte);
    }
    now = millis();
    this->last_jk_rs485_network_activity_ = now; 

    //manage buffer
    uint8_t response=0;
    uint16_t original_buffer_size=rx_buffer_.size();
    uint8_t cont_manage=0;
    bool changed=true;
    ESP_LOGD(TAG, "..........................................");

    do {
        cont_manage++;
        ESP_LOGV(TAG, "Buffer before number %d:    %s",cont_manage,format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());  
        response=this->manage_rx_buffer_();
        ESP_LOGV(TAG, "Response:            %d:",response);
        if (original_buffer_size==rx_buffer_.size()){
           changed=false;
        } else {
           changed=true;
           original_buffer_size=this->rx_buffer_.size();   
        }
    } while (cont_manage<5 && changed==true && original_buffer_size>=JKPB_RS485_MASTER_SHORT_REQUEST_SIZE);
    
    if (original_buffer_size==0){
      ESP_LOGV(TAG,     "Buffer empty");
//    } else {
//      ESP_LOGD(TAG,     "Buffer after at the end:   %s",format_hex_pretty(&this->rx_buffer_.front(), this->rx_buffer_.size()).c_str());
    }
    
    
       
  } else {
    //NO RX DATA
    if ((now-this->last_jk_rs485_network_activity_)>MIN_SILENCE_NEEDED_BEFORE_SPEAKING_MILLISECONDS){
      //CAN TX REQUEST IF NEEDED
      if (now-last_master_activity>SILENCE_BEFORE_ACTING_AS_MASTER){
        if (this->act_as_master==false){
          //NO MASTER HAS BEEN DETECTED IN THE NETWORK --> ESP WILL ACT AS MASTER
          this->act_as_master=true;
          this->set_node_availability(0,0);
          ESP_LOGI(TAG, "NO JK MASTER DETECTED IN THE NETWORK. ESP WILL ACT AS MASTER");
        }
      }
      
      if (this->act_as_master) {
        if (now-last_message_received_acting_as_master>SILENCE_BEFORE_REUSING_NETWORK_ACTING_AS_MASTER){
          // Is an special message to send in the queue?
          // if so, do it and return. TO DO!!!
          this->last_message_received_acting_as_master=now;      
          
          bool scan_sent=false;
          //SCAN NEXT UNAVAILABLE NODE
          if (now-this->last_network_scan>TIME_BETWEEN_NETWORK_SCAN_MILLISECONDS){
            int found_index=-1;
            found_index=this->found_next_node_to_discover();

            if (found_index==-1){
              //all nodes are available now
              ESP_LOGD(TAG, "SCANNING TO DISCOVER...ALL NODES ARE AVAILABLE");
            } else {
              ESP_LOGD(TAG, "SCANNING TO DISCOVER...0x%02X [%s]",found_index,this->nodes_available_to_string().c_str());
              this->pooling_index.scan_address=found_index;
              this->send_request_to_slave(found_index,2);

              this->last_network_scan=now;
              scan_sent=true;
            }
          }
          
          if (scan_sent==false){
            if (this->nodes_available_number>0 && now-this->last_jk_rs485_pooling_trial_>TIME_BEFORE_NEXT_POOLING_MILLISENCONDS){
              this->last_jk_rs485_pooling_trial_=now;
              //NORMAL POOLING LOOP AS MASTER
              if (this->calculate_next_pooling()==true){
                //ESP_LOGI(TAG, "CALCULATED NEXT POOLING...0x%02X @ %d",this->pooling_index.node_address,this->pooling_index.frame_type);
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
            if (now-rs485_network_node[cont].last_device_info_request_received_OK>TIME_BETWEEN_DEVICE_INFO_REQUESTS_MILLISECONDS){

              send_request_to_slave(cont,03);



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
              this->rs485_network_node[cont].last_device_info_request_received_OK=0;
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

  const uint8_t *raw = &this->rx_buffer_[0];
  uint8_t address = 0;

  const uint32_t now = millis();

  /*
  const size_t free_heap = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
  ESP_LOGV(TAG, "free_heap %f kBytes [buffer: %d bytes]",((float)free_heap/1024),this->rx_buffer_.size());
  */
  if (this->rx_buffer_.size()>=JKPB_RS485_MASTER_SHORT_REQUEST_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
    if (it == this->rx_buffer_.end()) {
      // Start sequence NOT FOUND (0x55AAEB90) --> maybe short response to a real master request?
      // no squence
      uint16_t computed_checksum = crc16_c(raw, 6); 
      uint16_t remote_checksum = ((uint16_t(raw[6]) << 8) | (uint16_t(raw[7]) << 0) );

      if (computed_checksum != remote_checksum) {
        ESP_LOGV(TAG, "CHECKSUM failed! 0x%04X != 0x%04X", computed_checksum, remote_checksum);
        //IT IS NOT A SHORT REQUEST OR THERE WAS A COMM. ERROR --> continue whith manage_rx_buffer code
      } else {
        address=raw[0];
        //ESP_LOGI(TAG, "REAL master is speaking to address 0x%02X (short request)",address);

        //this->rs485_network_node[0].last_message_received=now;
        //this->detected_master_activity_now();
        
        //this->set_node_availability(0,1);
        std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE-1);
        ESP_LOGD(TAG, "Answer received for MASTER (type: SHORT REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_SHORT_REQUEST_SIZE-1); 
        //continue with next;
        return(7);
      }
    }
  }

  if (this->rx_buffer_.size()>=JKPB_RS485_MASTER_REQUEST_SIZE){
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
        //std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE-1);
        //ESP_LOGD(TAG, "Frame received from MASTER (type: REQUEST for address %02X, %d bytes)",address, data.size());
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_MASTER_REQUEST_SIZE); 
        //continue with next;
        return(6);
      }
    }
  }

  if (this->rx_buffer_.size()>=JKPB_RS485_RESPONSE_SIZE){
    auto it = std::search(this->rx_buffer_.begin(), this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());

    if (it != this->rx_buffer_.end()) {
      //Sequence found, but where?

      size_t index = std::distance(this->rx_buffer_.begin(), it);
      
      if (index>0){
        //printBuffer(index);
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + index);    
        //continue with next;            
      }

      if (this->rx_buffer_.size()>=JKPB_RS485_RESPONSE_SIZE){
        //continue
        ESP_LOGD(TAG, "###############################Sequence found SIZE: %d",(this->rx_buffer_.size()));     
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

  if (this->rx_buffer_.size() >= JKPB_RS485_RESPONSE_SIZE){
    uint8_t computed_checksum = chksum(raw, JKPB_RS485_NUMBER_OF_ELEMENTS_TO_COMPUTE_CHECKSUM);
    uint8_t remote_checksum = raw[JKPB_RS485_CHECKSUM_INDEX];

    if (raw[JKPB_RS485_FRAME_TYPE_ADDRESS]==1){
      address=raw[JKPB_RS485_FRAME_TYPE_ADDRESS_FOR_FRAME_TYPE_x01+6];
    } else {
      address=raw[JKPB_RS485_ADDRESS_OF_RS485_ADDRESS];
    }
    //ESP_LOGI(TAG, "(this->rx_buffer_.size():%03d) [address 0x%02X] Frame Type 0x%02X ",this->rx_buffer_.size(),address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);

    if (computed_checksum != remote_checksum) {
      ESP_LOGW(TAG, "CHECKSUM failed! 0x%02X != 0x%02X", computed_checksum, remote_checksum);
      auto it_next = std::search(this->rx_buffer_.begin()+1, this->rx_buffer_.end(), pattern_response_header.begin(), pattern_response_header.end());
      size_t index_next = std::distance(this->rx_buffer_.begin(), it_next);
      
      if (index_next>0){
        //printBuffer(index);
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + index_next);        
      } else {
          this->rx_buffer_.clear();
      }

      return(10);
    } else {
      this->rs485_network_node[address].last_message_received=now;
      if (address==0){
        last_master_activity=now;
      } else if (address>15){
        ESP_LOGV(TAG, "(this->rx_buffer_.size():%03d) [address 0x%02X] Frame Type 0x%02X | CHECKSUM is correct",this->rx_buffer_.size(),address,raw[JKPB_RS485_FRAME_TYPE_ADDRESS]);
        //printBuffer(0);
        this->rx_buffer_.clear();
        return(11);
      } else {
        this->set_node_availability(address,1);
      }
      
    }





    std::vector<uint8_t> data(this->rx_buffer_.begin() + 0, this->rx_buffer_.begin() + this->rx_buffer_.size()+1);
    
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
    //    ESP_LOGD(TAG, "rx_buffer_.size=%02d",this->rx_buffer_.size()); 
  }

  this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + JKPB_RS485_RESPONSE_SIZE);
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
