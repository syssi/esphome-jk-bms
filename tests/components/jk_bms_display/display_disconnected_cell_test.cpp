#include <gtest/gtest.h>
#include <cmath>
#include "common.h"
namespace esphome::jk_bms_display::testing {
TEST(JkDisplayAvailabilityTest, DisconnectedCellDoesNotKeepOldVoltage) {
  TestableJkBmsDisplay bms;
  sensor::Sensor cell;
  bms.set_cell_voltage_sensor(0, &cell);
  bms.on_jk_bms_display_data(STATUS_FRAME);
  ASSERT_GT(cell.state, 3.0f);
  auto frame = STATUS_FRAME;
  frame[30] = 0;
  frame[31] = 0;
  bms.on_jk_bms_display_data(frame);
  EXPECT_TRUE(std::isnan(cell.state));
  bms.on_jk_bms_display_data(STATUS_FRAME);
  EXPECT_GT(cell.state, 3.0f);
}
}  // namespace esphome::jk_bms_display::testing
