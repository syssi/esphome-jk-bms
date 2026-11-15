#include <gtest/gtest.h>
#include <cmath>
#include "common.h"
#include "frames_jk04.h"
namespace esphome::jk_bms_ble::testing {
TEST(JkFirstCellInfoTest, InitialPacketIsNotThrottled) {
  TestableJkBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_GT(voltage.state, 0);
}
TEST(JkFirstCellInfoTest, RecoveryPacketIsNotThrottled) {
  TestableJkBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  ASSERT_GT(voltage.state, 0);
  bms.publish_device_unavailable_();
  ASSERT_TRUE(std::isnan(voltage.state));
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_GT(voltage.state, 0);
}
TEST(JkFirstCellInfoTest, AlternateProtocolInitialPacketIsNotThrottled) {
  TestableJkBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_throttle(0x7FFFFFFF);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04);
  EXPECT_GT(voltage.state, 0);
}
}  // namespace esphome::jk_bms_ble::testing
