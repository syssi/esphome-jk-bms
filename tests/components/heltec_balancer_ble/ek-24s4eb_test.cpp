#include <gtest/gtest.h>
#include "common.h"
#include "frames_ek-24s4eb.h"

namespace esphome::heltec_balancer_ble::testing {
using namespace ek;

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(EkCellInfoTest, CellVoltage1) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell1;
  bms.set_cell_voltage_sensor(0, &cell1);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(cell1.state, 3.325f, 0.001f);
}

TEST(EkCellInfoTest, InactiveCellsPublishZero) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell17;
  bms.set_cell_voltage_sensor(16, &cell17);  // index 16 — beyond active 16

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(cell17.state, 0.0f);
}

// ── Total voltage ─────────────────────────────────────────────────────────────

TEST(EkCellInfoTest, TotalVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Operation status: stopped, not balancing ──────────────────────────────────

TEST(EkCellInfoTest, OperationStatus) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor balancing;
  text_sensor::TextSensor op_status;
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_operation_status_text_sensor(&op_status);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FALSE(balancing.state);
  EXPECT_EQ(op_status.state, "Low battery voltage - balancing stopped");
}

// ── Balancing current ─────────────────────────────────────────────────────────

TEST(EkCellInfoTest, BalancingCurrent) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor current;
  bms.set_balancing_current_sensor(&current);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(current.state, 0.0f);
}

// ── Temperatures ──────────────────────────────────────────────────────────────

TEST(EkCellInfoTest, Temperatures) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor temp1, temp2;
  bms.set_temperature_sensor_1_sensor(&temp1);
  bms.set_temperature_sensor_2_sensor(&temp2);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(temp1.state, 25.71f, 0.01f);
  EXPECT_NEAR(temp2.state, 25.71f, 0.01f);
}

// ── Cell resistance ───────────────────────────────────────────────────────────

TEST(EkCellInfoTest, CellResistance1) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor r1;
  bms.set_cell_resistance_sensor(0, &r1);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(r1.state, 0.19487f, 0.0001f);
}

// ── Min/max cell voltage and cell index ──────────────────────────────────────

TEST(EkCellInfoTest, MinMaxCellVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(min_v.state, 3.3253f, 0.0001f);
  EXPECT_NEAR(max_v.state, 3.3265f, 0.0001f);
  EXPECT_FLOAT_EQ(min_cell.state, 1.0f);
  EXPECT_FLOAT_EQ(max_cell.state, 4.0f);
}

// ── Delta and average cell voltage ────────────────────────────────────────────

TEST(EkCellInfoTest, DeltaAndAverageCellVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor delta, avg;
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_NEAR(delta.state, 0.0012f, 0.0001f);
  EXPECT_NEAR(avg.state, 3.3261f, 0.0001f);
}

// ── Protection bitmasks (cells 18-24 unpopulated — flagged as detection failed) ─

TEST(EkCellInfoTest, ProtectionBitmasks) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor det_failed, overvoltage, undervoltage, polarity, excess_resistance;
  bms.set_cell_detection_failed_bitmask_sensor(&det_failed);
  bms.set_cell_overvoltage_bitmask_sensor(&overvoltage);
  bms.set_cell_undervoltage_bitmask_sensor(&undervoltage);
  bms.set_cell_polarity_error_bitmask_sensor(&polarity);
  bms.set_cell_excessive_line_resistance_bitmask_sensor(&excess_resistance);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(det_failed.state, 16646144.0f);
  EXPECT_FLOAT_EQ(overvoltage.state, 0.0f);
  EXPECT_FLOAT_EQ(undervoltage.state, 0.0f);
  EXPECT_FLOAT_EQ(polarity.state, 0.0f);
  EXPECT_FLOAT_EQ(excess_resistance.state, 0.0f);
}

// ── Error status (all clear — no fault in capture) ────────────────────────────

TEST(EkCellInfoTest, ErrorStatus) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor sys_overheat, chg_fault, dsg_fault;
  bms.set_error_system_overheating_binary_sensor(&sys_overheat);
  bms.set_error_charging_binary_sensor(&chg_fault);
  bms.set_error_discharging_binary_sensor(&dsg_fault);

  bms.decode_cell_info_(CELL_INFO_FRAME);

  EXPECT_FALSE(sys_overheat.state);
  EXPECT_FALSE(chg_fault.state);
  EXPECT_FALSE(dsg_fault.state);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(EkCellInfoTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_cell_info_(CELL_INFO_FRAME);
}

// ── Device info frame (ZH-2.1.1) ─────────────────────────────────────────────

TEST(EkDeviceInfoTest, TotalRuntime) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 127854.0f);
}

TEST(EkDeviceInfoTest, TotalRuntimeFormatted) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor formatted;
  bms.set_total_runtime_formatted_text_sensor(&formatted);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(formatted.state, "1d 11h");
}

TEST(EkDeviceInfoTest, DeviceModel) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor device_model;
  bms.set_device_model_text_sensor(&device_model);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(device_model.state, "EK-24S4EB");
}

TEST(EkDeviceInfoTest, HardwareVersion) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor hardware_version;
  bms.set_hardware_version_text_sensor(&hardware_version);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(hardware_version.state, "HW-3.2.0");
}

TEST(EkDeviceInfoTest, SoftwareVersion) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor software_version;
  bms.set_software_version_text_sensor(&software_version);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(software_version.state, "ZH-2.1.1");
}

TEST(EkDeviceInfoTest, ProtocolVersion) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor protocol_version;
  bms.set_protocol_version_text_sensor(&protocol_version);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(protocol_version.state, "V1.3.13");
}

TEST(EkDeviceInfoTest, ManufacturingDate) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor manufacturing_date;
  bms.set_manufacturing_date_text_sensor(&manufacturing_date);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(manufacturing_date.state, "20260201");
}

TEST(EkDeviceInfoTest, DispatchedViaFrameType) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);

  bms.decode_(DEVICE_INFO_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 127854.0f);
}

TEST(EkDeviceInfoTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_device_info_(DEVICE_INFO_FRAME);
}

// ── Settings frame (ZH-2.1.1): stop_diff and new voltages ────────────────────

TEST(EkSettingsTest, CellCount) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(EkSettingsTest, BalancerSwitch) {
  TestableHeltecBalancerBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_TRUE(balancer.state);
}

TEST(EkSettingsTest, BuzzerModeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor buzzer;
  bms.set_buzzer_mode_text_sensor(&buzzer);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_EQ(buzzer.state, "Off");
}

TEST(EkSettingsTest, BatteryTypeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor btype;
  bms.set_battery_type_text_sensor(&btype);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_EQ(btype.state, "LFP");
}

TEST(EkSettingsTest, VoltageNumbers) {
  TestableHeltecBalancerBle bms;
  TestNumber trig, max_cur, sleep, start, stop_diff, cap;
  bms.set_balance_trigger_voltage_number(&trig);
  bms.set_max_balance_current_number(&max_cur);
  bms.set_balance_sleep_voltage_number(&sleep);
  bms.set_balance_start_voltage_number(&start);
  bms.set_balance_stop_diff_voltage_number(&stop_diff);
  bms.set_nominal_battery_capacity_number(&cap);

  bms.decode_settings_(SETTINGS_FRAME);

  EXPECT_NEAR(trig.state, 0.005f, 0.001f);
  EXPECT_NEAR(max_cur.state, 4.0f, 0.001f);
  EXPECT_NEAR(sleep.state, 3.4f, 0.001f);        // StopVol — differs from GW
  EXPECT_NEAR(start.state, 3.45f, 0.001f);       // RunVol — differs from GW
  EXPECT_NEAR(stop_diff.state, 0.003f, 0.001f);  // Stop DifVol — new in ZH-2.1.1
  EXPECT_FLOAT_EQ(cap.state, 314.0f);
}

TEST(EkSettingsTest, DispatchedViaFrameType) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(EkSettingsTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_settings_(SETTINGS_FRAME);
}

}  // namespace esphome::heltec_balancer_ble::testing
