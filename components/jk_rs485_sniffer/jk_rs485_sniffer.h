#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_rs485_sniffer {

enum ProtocolVersion {
  PROTOCOL_VERSION_JK04,
  PROTOCOL_VERSION_JK02_24S,
  PROTOCOL_VERSION_JK02_32S,
};

class JkRS485SnifferDevice;

class JkRS485Sniffer : public uart::UARTDevice, public Component {
 public:
  JkRS485Sniffer() = default;

  void loop() override;
  void loop2() ;

  void dump_config() override;

  void register_device(JkRS485SnifferDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  ProtocolVersion protocol_version_{PROTOCOL_VERSION_JK02_32S};
  bool parse_jk_rs485_sniffer_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint16_t rx_timeout_{50};
  uint32_t last_jk_rs485_sniffer_byte_{0};
  std::vector<JkRS485SnifferDevice *> devices_;
};

class JkRS485SnifferDevice {
 public:
  void set_parent(JkRS485Sniffer *parent) { parent_ = parent; }
//  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_rs485_sniffer_data(const uint8_t &origin_address, const uint8_t &frame_type, const std::vector<uint8_t> &data) = 0;

 protected:
  friend JkRS485Sniffer;

  JkRS485Sniffer *parent_;
  //uint8_t address_;
};

}  // namespace jk_rs485_sniffer
}  // namespace esphome
