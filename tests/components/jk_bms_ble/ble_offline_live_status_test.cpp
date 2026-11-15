#include <gtest/gtest.h>
#include <cmath>
#include "common.h"
namespace esphome::jk_bms_ble::testing {
TEST(JkOfflineLiveStatusTest, CountdownsBecomeUnavailable) {
  TestableJkBmsBle bms;
  sensor::Sensor emergency, sleep, charge_id, elapsed;
  bms.set_emergency_time_countdown_sensor(&emergency);
  bms.set_smart_sleep_countdown_sensor(&sleep);
  bms.set_charge_status_id_sensor(&charge_id);
  bms.set_charge_status_time_elapsed_sensor(&elapsed);
  for (auto *s : {&emergency, &sleep, &charge_id, &elapsed})
    s->publish_state(100);
  bms.publish_device_unavailable_();
  for (auto *s : {&emergency, &sleep, &charge_id, &elapsed})
    EXPECT_TRUE(std::isnan(s->state));
}
TEST(JkOfflineLiveStatusTest, LiveTextDoesNotKeepChargingState) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor charge, balancer, runtime;
  bms.set_charge_status_text_sensor(&charge);
  bms.set_balancer_status_text_sensor(&balancer);
  bms.set_total_runtime_formatted_text_sensor(&runtime);
  charge.publish_state("Charging");
  balancer.publish_state("Balancing");
  runtime.publish_state("1h");
  bms.publish_device_unavailable_();
  EXPECT_EQ(charge.state, "Offline");
  EXPECT_EQ(balancer.state, "Offline");
  EXPECT_EQ(runtime.state, "Offline");
}
}  // namespace esphome::jk_bms_ble::testing
