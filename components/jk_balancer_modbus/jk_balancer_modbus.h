#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_balancer_modbus {

class JkBalancerModbusDevice;

class JkBalancerModbus : public uart::UARTDevice, public Component {
 public:
  JkBalancerModbus() = default;

  void loop() override;

  void dump_config() override;

  void register_device(JkBalancerModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t address, uint8_t function, uint16_t value);
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  bool parse_jk_balancer_modbus_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint16_t rx_timeout_{100};
  uint32_t last_jk_balancer_modbus_byte_{0};
  std::vector<JkBalancerModbusDevice *> devices_;
};

class JkBalancerModbusDevice {
 public:
  void set_parent(JkBalancerModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_jk_balancer_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void send(uint8_t function, uint16_t value) { this->parent_->send(this->address_, function, value); }

 protected:
  friend JkBalancerModbus;

  JkBalancerModbus *parent_;
  uint8_t address_;
};

}  // namespace jk_balancer_modbus
}  // namespace esphome
