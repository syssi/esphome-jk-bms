#include <gtest/gtest.h>
#include "common.h"
#include "frames_gw-24s4eb.h"

namespace esphome::heltec_balancer_ble::testing {
using namespace gw;

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(GwCellInfoTest, CellVoltages) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cells[4];
  for (int i = 0; i < 4; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(cells[0].state, 3.322f, 0.001f);
  EXPECT_NEAR(cells[1].state, 3.322f, 0.001f);
  EXPECT_NEAR(cells[2].state, 3.323f, 0.001f);
  EXPECT_NEAR(cells[3].state, 3.322f, 0.001f);
}

TEST(GwCellInfoTest, InactiveCellsPublishZero) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell17;
  bms.set_cell_voltage_sensor(16, &cell17);  // index 16 — beyond active 16

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(cell17.state, 0.0f);
}

// ── Cell voltage statistics ───────────────────────────────────────────────────

TEST(GwCellInfoTest, CellVoltageStats) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(min_v.state, 3.322f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.323f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 1.0f);  // cell 1 (1-indexed)
  EXPECT_FLOAT_EQ(max_cell.state, 9.0f);  // cell 9 (1-indexed)
  EXPECT_NEAR(delta.state, 0.001f, 0.001f);
  EXPECT_NEAR(avg.state, 3.322f, 0.001f);
}

// ── Total voltage ─────────────────────────────────────────────────────────────

TEST(GwCellInfoTest, TotalVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(total.state, 53.16f, 0.01f);
}

// ── Operation status and balancing ───────────────────────────────────────────

TEST(GwCellInfoTest, OperationStatus) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor balancing;
  text_sensor::TextSensor op_status;
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_operation_status_text_sensor(&op_status);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_TRUE(balancing.state);
  EXPECT_EQ(op_status.state, "Balancing");
}

// ── Balancing current ─────────────────────────────────────────────────────────

TEST(GwCellInfoTest, BalancingCurrent) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor current;
  bms.set_balancing_current_sensor(&current);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(current.state, 2.056f, 0.001f);
}

// ── Temperatures ──────────────────────────────────────────────────────────────

TEST(GwCellInfoTest, Temperatures) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor temp1, temp2;
  bms.set_temperature_sensor_1_sensor(&temp1);
  bms.set_temperature_sensor_2_sensor(&temp2);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(temp1.state, 26.48f, 0.01f);
  EXPECT_NEAR(temp2.state, 26.48f, 0.01f);
}

// ── Bitmask sensors ───────────────────────────────────────────────────────────

TEST(GwCellInfoTest, BitmaskSensors) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor det_fail, overvolt, undervolt, polarity, line_res;
  bms.set_cell_detection_failed_bitmask_sensor(&det_fail);
  bms.set_cell_overvoltage_bitmask_sensor(&overvolt);
  bms.set_cell_undervoltage_bitmask_sensor(&undervolt);
  bms.set_cell_polarity_error_bitmask_sensor(&polarity);
  bms.set_cell_excessive_line_resistance_bitmask_sensor(&line_res);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(det_fail.state, 0.0f);
  EXPECT_FLOAT_EQ(overvolt.state, 0.0f);
  EXPECT_FLOAT_EQ(undervolt.state, 0.0f);
  EXPECT_FLOAT_EQ(polarity.state, 0.0f);
  EXPECT_FLOAT_EQ(line_res.state, 0.0f);
}

// ── Error binary sensors ──────────────────────────────────────────────────────

TEST(GwCellInfoTest, ErrorBinarySensors) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor sys_overheat, charging_err, discharging_err;
  bms.set_error_system_overheating_binary_sensor(&sys_overheat);
  bms.set_error_charging_binary_sensor(&charging_err);
  bms.set_error_discharging_binary_sensor(&discharging_err);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FALSE(sys_overheat.state);
  EXPECT_FALSE(charging_err.state);
  EXPECT_FALSE(discharging_err.state);
}

// ── Online status (via decode_ dispatch) ─────────────────────────────────────

TEST(GwCellInfoTest, OnlineStatusSetViaDispatch) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor online;
  bms.set_online_status_binary_sensor(&online);

  bms.decode_(CELL_INFO_FRAME);

  EXPECT_TRUE(online.state);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(GwCellInfoTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_cell_info_(CELL_INFO_FRAME);
}

// ── Device info frame ─────────────────────────────────────────────────────────

TEST(GwDeviceInfoTest, TotalRuntime) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 4064370.0f);
}

TEST(GwDeviceInfoTest, TotalRuntimeFormatted) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor formatted;
  bms.set_total_runtime_formatted_text_sensor(&formatted);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(formatted.state, "47d ");
}

TEST(GwDeviceInfoTest, DispatchedViaFrameType) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);

  bms.decode_(DEVICE_INFO_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 4064370.0f);
}

TEST(GwDeviceInfoTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_device_info_(DEVICE_INFO_FRAME);
}

// ── Settings frame ────────────────────────────────────────────────────────────

TEST(GwSettingsTest, CellCount) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(GwSettingsTest, BalancerSwitch) {
  TestableHeltecBalancerBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_TRUE(balancer.state);
}

TEST(GwSettingsTest, BuzzerModeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor buzzer;
  bms.set_buzzer_mode_text_sensor(&buzzer);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_EQ(buzzer.state, "Off");
}

TEST(GwSettingsTest, BatteryTypeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor btype;
  bms.set_battery_type_text_sensor(&btype);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_EQ(btype.state, "LFP");
}

TEST(GwSettingsTest, VoltageNumbers) {
  TestableHeltecBalancerBle bms;
  TestNumber trig, max_cur, sleep, start, stop_diff, cap;
  bms.set_balance_trigger_voltage_number(&trig);
  bms.set_max_balance_current_number(&max_cur);
  bms.set_balance_sleep_voltage_number(&sleep);
  bms.set_balance_start_voltage_number(&start);
  bms.set_balance_stop_diff_voltage_number(&stop_diff);
  bms.set_nominal_battery_capacity_number(&cap);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_NEAR(trig.state, 0.004f, 0.001f);
  EXPECT_NEAR(max_cur.state, 2.0f, 0.001f);
  EXPECT_NEAR(sleep.state, 3.0f, 0.001f);
  EXPECT_NEAR(start.state, 0.0f, 0.001f);
  EXPECT_NEAR(stop_diff.state, 0.0f, 0.001f);
  EXPECT_FLOAT_EQ(cap.state, 280.0f);
}

TEST(GwSettingsTest, DispatchedViaFrameType) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(GwSettingsTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_settings_(SETTINGS_FRAME);
}

// ── Dispatch: write register response ignored ─────────────────────────────────

TEST(GwDispatchTest, WriteRegisterResponseIgnored) {
  TestableHeltecBalancerBle bms;
  std::vector<uint8_t> wr = {0x55, 0xAA, 0x11, 0x01, 0x05, 0x00, 0x14, 0x00};
  wr.resize(20, 0x00);
  bms.decode_(wr);
}

TEST(GwDispatchTest, UnknownFrameTypeIgnored) {
  TestableHeltecBalancerBle bms;
  std::vector<uint8_t> unknown = {0x55, 0xAA, 0x11, 0x01, 0xAB, 0x00, 0x14, 0x00};
  unknown.resize(20, 0x00);
  bms.decode_(unknown);
}

}  // namespace esphome::heltec_balancer_ble::testing
