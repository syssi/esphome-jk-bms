#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_balancer::testing {

static constexpr uint8_t FUNCTION_READ_ALL = 0xFF;
static constexpr uint8_t FUNCTION_SET_CELL_COUNT = 0xF0;
static constexpr uint8_t FUNCTION_SET_TRIGGER_VOLTAGE = 0xF2;
static constexpr uint8_t FUNCTION_SET_MAX_BALANCE_CURRENT = 0xF4;

// ── Total voltage and average ─────────────────────────────────────────────────

TEST(JkBalancerStatusTest, TotalAndAverageVoltage) {
  TestableJkBalancer bms;
  sensor::Sensor total, avg;
  bms.set_total_voltage_sensor(&total);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_NEAR(total.state, 56.49f, 0.001f);
  EXPECT_NEAR(avg.state, 3.323f, 0.001f);
}

// ── Cell count ────────────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, CellCountDetected) {
  TestableJkBalancer bms;
  sensor::Sensor detected;
  bms.set_cell_count_detected_sensor(&detected);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_FLOAT_EQ(detected.state, 17.0f);
}

// ── Balancing state ───────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, BalancingState) {
  TestableJkBalancer bms;
  sensor::Sensor status_sensor;
  binary_sensor::BinarySensor balancing;
  bms.set_balancer_status_sensor(&status_sensor);
  bms.set_balancing_binary_sensor(&balancing);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_FLOAT_EQ(status_sensor.state, 0.0f);  // 0 = idle
  EXPECT_FALSE(balancing.state);
}

// ── Errors ────────────────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, Errors) {
  TestableJkBalancer bms;
  sensor::Sensor bitmask;
  text_sensor::TextSensor errors_text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&errors_text);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_FLOAT_EQ(bitmask.state, 1.0f);  // bit 0 = "Wrong cell count"
  EXPECT_EQ(errors_text.state, "Wrong cell count");
}

// ── Balancing current ─────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, BalancingCurrent) {
  TestableJkBalancer bms;
  sensor::Sensor current;
  bms.set_balancing_current_sensor(&current);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_NEAR(current.state, 0.0f, 0.001f);
}

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, CellVoltages) {
  TestableJkBalancer bms;
  sensor::Sensor cells[4];
  for (int i = 0; i < 4; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_NEAR(cells[0].state, 3.321f, 0.001f);
  EXPECT_NEAR(cells[1].state, 3.323f, 0.001f);
  EXPECT_NEAR(cells[2].state, 3.323f, 0.001f);
  EXPECT_NEAR(cells[3].state, 3.323f, 0.001f);
}

// ── Cell voltage statistics ───────────────────────────────────────────────────

TEST(JkBalancerStatusTest, CellVoltageStats) {
  TestableJkBalancer bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_NEAR(min_v.state, 3.321f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.324f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 1.0f);  // cell 1 has minimum voltage
  EXPECT_FLOAT_EQ(max_cell.state, 6.0f);  // cell 6 has maximum voltage
  EXPECT_NEAR(delta.state, 0.003f, 0.001f);
}

// ── Battery temperature ───────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, BatteryTemperature) {
  TestableJkBalancer bms;
  sensor::Sensor temp;
  bms.set_battery_temperature_sensor(&temp);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_NEAR(temp.state, 16.3f, 0.001f);  // 163 × 0.1
}

// ── Online status ─────────────────────────────────────────────────────────────

TEST(JkBalancerStatusTest, OnlineStatus) {
  TestableJkBalancer bms;
  binary_sensor::BinarySensor online;
  bms.set_online_status_binary_sensor(&online);

  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);

  EXPECT_TRUE(online.state);  // reset_online_status_tracker_ sets it true
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(JkBalancerStatusTest, NullSensorsDoNotCrash) {
  TestableJkBalancer bms;
  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_17S);
}

// ── Function dispatch ─────────────────────────────────────────────────────────

TEST(JkBalancerDispatchTest, WriteRegisterResponsesIgnored) {
  TestableJkBalancer bms;
  std::vector<uint8_t> wr_data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  bms.on_jk_balancer_modbus_data(FUNCTION_SET_CELL_COUNT, wr_data);
  bms.on_jk_balancer_modbus_data(FUNCTION_SET_TRIGGER_VOLTAGE, wr_data);
  bms.on_jk_balancer_modbus_data(FUNCTION_SET_MAX_BALANCE_CURRENT, wr_data);
}

TEST(JkBalancerDispatchTest, UnknownFunctionIgnored) {
  TestableJkBalancer bms;
  bms.on_jk_balancer_modbus_data(0xAB, {0x01, 0x02, 0x03});
}

}  // namespace esphome::jk_balancer::testing
