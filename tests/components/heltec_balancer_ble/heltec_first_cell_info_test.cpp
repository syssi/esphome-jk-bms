#include <gtest/gtest.h>
#include <cmath>
#include "common.h"
#include "frames_gw-24s4eb.h"
#include "frames_ek-24s4eb.h"
namespace esphome::heltec_balancer_ble::testing {
TEST(HeltecFirstCellInfoTest, InitialPacketIsNotThrottled) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_cell_info_(ek::CELL_INFO_FRAME);
  EXPECT_GT(voltage.state, 0);
}
TEST(HeltecFirstCellInfoTest, RecoveryPacketIsNotThrottled) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0);
  bms.decode_cell_info_(ek::CELL_INFO_FRAME);
  ASSERT_GT(voltage.state, 0);
  bms.publish_device_unavailable_();
  ASSERT_TRUE(std::isnan(voltage.state));
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_cell_info_(ek::CELL_INFO_FRAME);
  EXPECT_GT(voltage.state, 0);
}
TEST(HeltecFirstCellInfoTest, AlternateProtocolInitialPacketIsNotThrottled) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_cell_info_v2_(gw::CELL_INFO_FRAME);
  EXPECT_GT(voltage.state, 0);
}
}  // namespace esphome::heltec_balancer_ble::testing
