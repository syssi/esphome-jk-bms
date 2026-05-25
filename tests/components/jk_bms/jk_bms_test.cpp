#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms::testing {

static constexpr uint8_t FUNCTION_READ_ALL = 0x06;
static constexpr uint8_t FUNCTION_WRITE_REGISTER = 0x02;

// ── Cell voltages ────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, CellVoltages) {
  TestableJkBms bms;
  sensor::Sensor cells[14];
  for (int i = 0; i < 14; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(cells[0].state, 3.821f, 0.001f);   // cell 1
  EXPECT_NEAR(cells[1].state, 3.834f, 0.001f);   // cell 2
  EXPECT_NEAR(cells[2].state, 3.831f, 0.001f);   // cell 3
  EXPECT_NEAR(cells[3].state, 3.820f, 0.001f);   // cell 4
  EXPECT_NEAR(cells[8].state, 3.811f, 0.001f);   // cell 9 (min)
  EXPECT_NEAR(cells[11].state, 3.835f, 0.001f);  // cell 12 (max)
}

// ── Cell voltage statistics ──────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, CellVoltageStats) {
  TestableJkBms bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(min_v.state, 3.811f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.835f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 9.0f);   // cell 9 has minimum voltage
  EXPECT_FLOAT_EQ(max_cell.state, 12.0f);  // cell 12 has maximum voltage
  EXPECT_NEAR(delta.state, 0.024f, 0.001f);
  EXPECT_NEAR(avg.state, 3.828f, 0.001f);  // 53595 mV / 14 cells
}

// ── Total voltage ────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, TotalVoltage) {
  TestableJkBms bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(total.state, 53.59f, 0.001f);
}

// ── Current and power ────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, ChargingCurrentAndPower) {
  TestableJkBms bms;
  sensor::Sensor current, power, charging_power, discharging_power;
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(current.state, 2.08f, 0.001f);         // 208 × 0.01, charging (bit 15 set)
  EXPECT_NEAR(power.state, 111.47f, 0.1f);           // 53.59 × 2.08
  EXPECT_NEAR(charging_power.state, 111.47f, 0.1f);  // max(0, power)
  EXPECT_FLOAT_EQ(discharging_power.state, 0.0f);    // abs(min(0, power))
}

// ── Temperatures ─────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, Temperatures) {
  TestableJkBms bms;
  sensor::Sensor tube, t1, t2, count;
  bms.set_mosfet_temperature_sensor(&tube);
  bms.set_temperature_sensor_1_sensor(&t1);
  bms.set_temperature_sensor_2_sensor(&t2);
  bms.set_temperature_sensor_count_sensor(&count);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(tube.state, 29.0f);
  EXPECT_FLOAT_EQ(t1.state, 30.0f);
  EXPECT_FLOAT_EQ(t2.state, 28.0f);
  EXPECT_FLOAT_EQ(count.state, 2.0f);
}

// ── Capacity ─────────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, Capacity) {
  TestableJkBms bms;
  sensor::Sensor soc, capacity_remaining, full_charge_capacity;
  bms.set_state_of_charge_sensor(&soc);
  bms.set_capacity_remaining_sensor(&capacity_remaining);
  bms.set_full_charge_capacity_sensor(&full_charge_capacity);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(soc.state, 15.0f);                    // 15 %
  EXPECT_NEAR(capacity_remaining.state, 2.1f, 0.001f);  // 14 Ah × 15 %
  EXPECT_FLOAT_EQ(full_charge_capacity.state, 14.0f);   // 14 Ah
}

// ── Balancing configuration ──────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, BalancingConfig) {
  TestableJkBms bms;
  sensor::Sensor starting_voltage, delta_voltage;
  bms.set_balancing_start_voltage_sensor(&starting_voltage);
  bms.set_balancing_delta_voltage_sensor(&delta_voltage);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(starting_voltage.state, 3.300f, 0.001f);  // 3300 × 0.001
  EXPECT_NEAR(delta_voltage.state, 0.008f, 0.001f);     // 8 × 0.001
}

// ── Errors ───────────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, NoErrors) {
  TestableJkBms bms;
  sensor::Sensor errors_bitmask;
  text_sensor::TextSensor errors_text;
  bms.set_errors_bitmask_sensor(&errors_bitmask);
  bms.set_errors_text_sensor(&errors_text);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(errors_bitmask.state, 0.0f);
  EXPECT_EQ(errors_text.state, "");
}

// ── Operation modes ──────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, OperationModes) {
  TestableJkBms bms;
  sensor::Sensor modes_bitmask;
  binary_sensor::BinarySensor charging, discharging, balancing;
  bms.set_operation_mode_bitmask_sensor(&modes_bitmask);
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.set_balancing_binary_sensor(&balancing);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(modes_bitmask.state, 7.0f);  // 0x07: charging+discharging+balancer
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
  EXPECT_TRUE(balancing.state);
}

// ── Balancing and MOSFET switches ────────────────────────────────────────────

TEST(JkBmsStatusDataTest, SwitchStates) {
  TestableJkBms bms;
  TestSwitch charging_sw, discharging_sw;
  binary_sensor::BinarySensor balancing_sw;
  bms.set_charging_switch(&charging_sw);
  bms.set_discharging_switch(&discharging_sw);
  bms.set_balancing_switch_binary_sensor(&balancing_sw);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_TRUE(charging_sw.state);
  EXPECT_TRUE(discharging_sw.state);
  EXPECT_TRUE(balancing_sw.state);
}

// ── Battery info ─────────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, BatteryInfo) {
  TestableJkBms bms;
  sensor::Sensor cell_count, cycles;
  text_sensor::TextSensor battery_type;
  bms.set_cell_count_sensor(&cell_count);
  bms.set_charging_cycles_sensor(&cycles);
  bms.set_battery_type_text_sensor(&battery_type);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(cell_count.state, 14.0f);
  EXPECT_FLOAT_EQ(cycles.state, 4.0f);
  EXPECT_EQ(battery_type.state, "Ternary Lithium");
}

// ── Alarm thresholds ─────────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, LowSocAlarm) {
  TestableJkBms bms;
  sensor::Sensor low_soc_alarm_threshold;
  bms.set_low_soc_alarm_threshold_sensor(&low_soc_alarm_threshold);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_FLOAT_EQ(low_soc_alarm_threshold.state, 20.0f);  // 0x14 = 20 %
}

// ── Version and runtime ──────────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, SoftwareVersion) {
  TestableJkBms bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_EQ(sw.state, "H6.X__S6.1.3S__");
}

TEST(JkBmsStatusDataTest, TotalRuntime) {
  TestableJkBms bms;
  sensor::Sensor runtime;
  text_sensor::TextSensor runtime_fmt;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_runtime_formatted_text_sensor(&runtime_fmt);

  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);

  EXPECT_NEAR(runtime.state, 964.267f, 0.01f);  // 57856 min × (1/60)
  EXPECT_EQ(runtime_fmt.state, "40d 4h");       // 57856 min × 60 = 3471360 s
}

// ── Null sensors do not crash ────────────────────────────────────────────────

TEST(JkBmsStatusDataTest, NullSensorsDoNotCrash) {
  TestableJkBms bms;
  bms.on_jk_modbus_data(FUNCTION_READ_ALL, STATUS_FRAME_14S);
}

// ── Write-register response is ignored gracefully ────────────────────────────

TEST(JkBmsFunctionDispatchTest, WriteRegisterWithDataIgnored) {
  TestableJkBms bms;
  bms.on_jk_modbus_data(FUNCTION_WRITE_REGISTER, {0x8E});
}

TEST(JkBmsFunctionDispatchTest, WriteRegisterEmptyIgnored) {
  TestableJkBms bms;
  bms.on_jk_modbus_data(FUNCTION_WRITE_REGISTER, {});
}

TEST(JkBmsFunctionDispatchTest, UnknownFunctionIgnored) {
  TestableJkBms bms;
  bms.on_jk_modbus_data(0xFF, {0x01, 0x02, 0x03});
}

}  // namespace esphome::jk_bms::testing
