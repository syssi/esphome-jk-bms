#include <gtest/gtest.h>
#include <cmath>
#include "common.h"
namespace esphome::jk_balancer::testing {
TEST(JkBalancerCellCountTest, RemovedCellsBecomeUnavailable) {
  TestableJkBalancer bms;
  sensor::Sensor last;
  bms.set_cell_voltage_sensor(16, &last);
  bms.on_jk_balancer_modbus_data(0xFF, STATUS_FRAME_17S);
  ASSERT_GT(last.state, 0);
  auto frame = STATUS_FRAME_17S;
  frame[22] = 1;
  bms.on_jk_balancer_modbus_data(0xFF, frame);
  EXPECT_TRUE(std::isnan(last.state));
  bms.on_jk_balancer_modbus_data(0xFF, STATUS_FRAME_17S);
  EXPECT_GT(last.state, 0);
}
}  // namespace esphome::jk_balancer::testing
