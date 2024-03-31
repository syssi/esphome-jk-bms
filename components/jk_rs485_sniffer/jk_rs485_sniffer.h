#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"
//#include "gpio_binary_output.h"
//#include "binary_output.h"
//#include "esphome.h"
#include "binary_output.h"


namespace esphome {
namespace jk_rs485_sniffer {

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

class JkRS485SnifferDevice;

class JkRS485Sniffer : public uart::UARTDevice, public output::BinaryOutput, public Component {
 public:
  JkRS485Sniffer() = default;


  void set_talk_pin(GPIOPin *pin) { talk_pin_ = pin; }

  void setup() override {
    this->turn_off();
    this->talk_pin_->setup();
    this->turn_off();

    for (uint8_t cont=0;cont<16;cont++){
        rs485_network_node[cont].available=0;
        rs485_network_node[cont].last_device_info_request_sent=0;
        rs485_network_node[cont].last_device_info_received=0;
        rs485_network_node[cont].last_message_received=0;
    }    
  }

  void loop() override;

  void dump_config() override;

  void register_device(JkRS485SnifferDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_32S};
  
  void printBuffer(uint16_t max_length);
  bool parse_jk_rs485_sniffer_byte_(uint8_t byte);
  void set_node_availability(uint8_t address,bool value);
  std::string nodes_available_to_string();

  std::vector<uint8_t> rx_buffer_;
  uint16_t rx_timeout_{50};
  uint32_t last_jk_rs485_sniffer_byte_{0};
  std::vector<JkRS485SnifferDevice *> devices_;  

  void write_state(bool state) override { this->talk_pin_->digital_write(state); }
  GPIOPin *talk_pin_;

  struct rs485_network_nodes {
     bool available;
     uint32_t last_message_received;
     uint32_t last_device_info_received;
     uint32_t last_device_info_request_sent;
  };
  struct rs485_network_nodes rs485_network_node[16];
  
  std::string nodes_available;
};

class JkRS485SnifferDevice {
 public:
  void set_parent(JkRS485Sniffer *parent) { parent_ = parent; }
//  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_rs485_sniffer_data(const uint8_t &origin_address, const uint8_t &frame_type, const std::vector<uint8_t> &data, const std::string &nodes_available) = 0;

 protected:
  friend JkRS485Sniffer;

  JkRS485Sniffer *parent_;
  //uint8_t address_;
};

}  // namespace jk_rs485_sniffer
}  // namespace esphome
