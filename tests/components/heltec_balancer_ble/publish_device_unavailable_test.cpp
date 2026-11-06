#include <gtest/gtest.h>
#include "common.h"

namespace esphome::heltec_balancer_ble::testing {

TEST(HeltecBalancerBlePublishDeviceUnavailableTest, InvalidatesBinarySensors) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor online, balancing, error_charging, error_discharging, error_system_overheating;
  bms.set_online_status_binary_sensor(&online);
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_error_charging_binary_sensor(&error_charging);
  bms.set_error_discharging_binary_sensor(&error_discharging);
  bms.set_error_system_overheating_binary_sensor(&error_system_overheating);
  const std::vector<binary_sensor::BinarySensor *> binary_sensors = {&balancing, &error_charging, &error_discharging,
                                                                     &error_system_overheating};

  online.publish_state(true);
  for (auto *binary_sensor : binary_sensors) {
    binary_sensor->publish_state(true);
  }

  bms.publish_device_unavailable_();

  EXPECT_TRUE(online.has_state());
  EXPECT_FALSE(online.state);
  for (size_t i = 0; i < binary_sensors.size(); i++) {
    EXPECT_FALSE(binary_sensors[i]->has_state()) << "binary sensor index " << i;
  }
}

}  // namespace esphome::heltec_balancer_ble::testing
