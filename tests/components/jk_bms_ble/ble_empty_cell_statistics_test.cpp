#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include "common.h"
#include "frames_jk04.h"
namespace esphome::jk_bms_ble::testing {
static void expect_unavailable(TestableJkBmsBle &bms, std::vector<uint8_t> frame, bool jk04) {
  sensor::Sensor minimum, maximum, delta, minimum_index, maximum_index;
  bms.set_min_cell_voltage_sensor(&minimum);
  bms.set_max_cell_voltage_sensor(&maximum);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_min_voltage_cell_sensor(&minimum_index);
  bms.set_max_voltage_cell_sensor(&maximum_index);
  std::fill(frame.begin() + 6, frame.begin() + (jk04 ? 102 : 54), 0);
  if (jk04)
    bms.decode_jk04_cell_info_(frame);
  else
    bms.decode_jk02_cell_info_(frame);
  EXPECT_TRUE(std::isnan(minimum.state));
  EXPECT_TRUE(std::isnan(maximum.state));
  EXPECT_TRUE(std::isnan(delta.state));
  EXPECT_TRUE(std::isnan(minimum_index.state));
  EXPECT_TRUE(std::isnan(maximum_index.state));
}
TEST(JkEmptyCellStatisticsTest, JK02) {
  TestableJkBmsBle bms;
  expect_unavailable(bms, CELL_INFO_JK02_24S_V10, false);
}
TEST(JkEmptyCellStatisticsTest, JK04) {
  TestableJkBmsBle bms;
  expect_unavailable(bms, CELL_INFO_JK04, true);
}
}  // namespace esphome::jk_bms_ble::testing
