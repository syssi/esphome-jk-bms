#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_modbus {

class JkModbusDevice;

class JkModbus : public uart::UARTDevice, public Component {
 public:
  JkModbus() = default;

  void loop() override;

  void dump_config() override;

  void register_device(JkModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t function, uint8_t address, uint8_t value);
  void write_register(uint8_t address, uint8_t value);
  void read_registers();
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  void authenticate_();
  bool parse_jk_modbus_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint16_t rx_timeout_{50};
  uint32_t last_jk_modbus_byte_{0};
  std::vector<JkModbusDevice *> devices_;
};

class JkModbusDevice {
 public:
  void set_parent(JkModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(int8_t function, uint8_t address, uint8_t value) { this->parent_->send(function, address, value); }
  void write_register(uint8_t address, uint8_t value) { this->parent_->write_register(address, value); }
  void read_registers() { this->parent_->read_registers(); }

 protected:
  friend JkModbus;

  JkModbus *parent_;
  uint8_t address_;
};

}  // namespace jk_modbus
}  // namespace esphome
