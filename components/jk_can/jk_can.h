#pragma once

#include "esphome/core/component.h"
#include "esphome/components/canbus/canbus.h"

namespace esphome {
namespace jk_can {

class JkCANDevice;

class JkCAN : public uart::UARTDevice, public Component {
 public:
  JkCAN() = default;

  void loop() override;

  void dump_config() override;

  void register_device(JkCANDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t function, uint8_t address, uint8_t value);
  void read_registers(uint8_t function, uint8_t address);

 protected:
  bool parse_jk_can_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_jk_can_byte_{0};
  std::vector<JkCANDevice *> devices_;
};

class JkCANDevice {
 public:
  void set_parent(JkCAN *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_can_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(int8_t function, uint8_t address, uint8_t value) { this->parent_->send(function, address, value); }
  void read_registers(uint8_t function, uint8_t address) { this->parent_->read_registers(function, address); }

 protected:
  friend JkCAN;

  JkCAN *parent_;
  uint8_t address_;
};

}  // namespace jk_can
}  // namespace esphome
