#include <gtest/gtest.h>
#include "common.h"
#include "frames_ek-24s4eb.h"

namespace esphome::heltec_balancer_ble::testing {
TEST(HeltecOfflineTextTest, ClearsAndRestoresRuntimeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor runtime;
  bms.set_total_runtime_formatted_text_sensor(&runtime);
  bms.decode_device_info_(ek::DEVICE_INFO_FRAME);
  const auto online = runtime.state;
  ASSERT_FALSE(online.empty());
  bms.publish_device_unavailable_();
  EXPECT_EQ(runtime.state, "Offline");
  bms.decode_device_info_(ek::DEVICE_INFO_FRAME);
  EXPECT_EQ(runtime.state, online);
}
}  // namespace esphome::heltec_balancer_ble::testing
