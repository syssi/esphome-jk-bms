#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/jk_modbus/jk_modbus.h"

namespace esphome {
namespace jk_bms {

class JkBms : public PollingComponent, public jk_modbus::JkModbusDevice {
 public:
  void set_firmware_version_sensor(sensor::Sensor *firmware_version_sensor) {
    firmware_version_sensor_ = firmware_version_sensor;
  }

  void dump_config() override;

  void on_jk_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override;

  void update() override;

 protected:
  sensor::Sensor *firmware_version_sensor_;

  void on_status_data_(const std::vector<uint8_t> &data);
  void publish_state_(sensor::Sensor *sensor, float value);
};

}  // namespace jk_bms
}  // namespace esphome
