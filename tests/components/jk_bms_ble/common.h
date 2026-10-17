#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/jk_bms_ble/jk_bms_ble.h"

namespace esphome::jk_bms_ble::testing {

struct TestNumber : number::Number {
  void control(float) override {}
};

struct TestSwitch : switch_::Switch {
  void write_state(bool) override {}
};

struct TestSelect : select::Select {
  explicit TestSelect(std::initializer_list<const char *> options) { this->traits.set_options(options); }
  void control(size_t) override {}
};

// Exposes protected decoder methods for direct testing.
class TestableJkBmsBle : public JkBmsBle {
 public:
  void update() override {}
  using JkBmsBle::decode_;
  using JkBmsBle::decode_device_info_;
  using JkBmsBle::decode_jk02_cell_info_;
  using JkBmsBle::decode_jk04_cell_info_;
  using JkBmsBle::decode_jk02_settings_;
  using JkBmsBle::decode_jk04_settings_;
  using JkBmsBle::decode_logbook_;

  using JkBmsBle::queue_command_;

  uint8_t queue_size() const { return queue_.size(); }
  bool command_pending() const { return queue_.pending(); }
  const CommandQueue::Command &queue_front() const { return queue_.front(); }
  bool queue_on_frame(uint8_t frame_type) { return this->queue_.on_frame(frame_type); }

  // send_next_command_() is a no-op off ESP32, so tests mark the head in-flight themselves.
  void mark_command_pending() { this->queue_.mark_pending(millis()); }
  void set_response_timeout(uint32_t ms) { this->queue_.set_response_timeout(ms); }
  void set_write_settle(uint32_t ms) { this->queue_.set_write_settle(ms); }

  bool settings_verification_pending() const { return settings_verification_.pending(); }
  void schedule_settings_verification() { this->settings_verification_.schedule(millis()); }
  // Rearms with zero delay, making an already scheduled verification due immediately.
  void expire_settings_verification() {
    this->settings_verification_.set_delay(0);
    this->settings_verification_.schedule(millis());
  }

  bool status_notification_received() const { return status_notification_received_; }
};

}  // namespace esphome::jk_bms_ble::testing

// Traffic from esp32-ble-example-faker.yaml
#include "frames_jk02_24s_v10.h"
