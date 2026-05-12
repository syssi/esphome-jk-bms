#include <gtest/gtest.h>
#include "common.h"
#include "frames_ek-b24s8e200a.h"

namespace esphome::heltec_balancer_ble::testing {
using namespace ek200a;

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, CellVoltage1) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell1;
  bms.set_cell_voltage_sensor(0, &cell1);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(cell1.state, 3.272f, 0.001f);
}

TEST(EkB24S8E200ACellInfoTest, InactiveCellsPublishZero) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell17;
  bms.set_cell_voltage_sensor(16, &cell17);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(cell17.state, 0.0f);
}

// ── Total voltage ─────────────────────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, TotalVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(total.state, 52.40f, 0.01f);
}

// ── Operation status: balancing ───────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, OperationStatus) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor balancing;
  text_sensor::TextSensor op_status;
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_operation_status_text_sensor(&op_status);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_TRUE(balancing.state);
  EXPECT_EQ(op_status.state, "Balancing");
}

// ── Balancing current at shifted offset [218] ─────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, BalancingCurrent) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor current;
  bms.set_balancing_current_sensor(&current);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(current.state, -8.055f, 0.01f);
}

// ── Temperatures at shifted offsets ───────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, Temperatures) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor temp1, temp2;
  bms.set_temperature_sensor_1_sensor(&temp1);
  bms.set_temperature_sensor_2_sensor(&temp2);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(temp1.state, 28.20f, 0.05f);
  EXPECT_NEAR(temp2.state, 28.09f, 0.05f);
}

// ── New temperature sensors (MOS and Balancer) ────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, TemperaturesMosAndBalancer) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor temp3, temp4;
  bms.set_mosfet_temperature_sensor(&temp3);
  bms.set_balancer_temperature_sensor(&temp4);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(temp3.state, 31.40f, 0.05f);
  EXPECT_NEAR(temp4.state, 61.90f, 0.05f);
}

// ── SOC sensors (new in SW-2.0.0) ────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, SocSensors) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total_ah, remain_ah, soc_pct;
  bms.set_nominal_capacity_sensor(&total_ah);
  bms.set_capacity_remaining_sensor(&remain_ah);
  bms.set_state_of_charge_sensor(&soc_pct);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(total_ah.state, 320.0f);
  EXPECT_NEAR(remain_ah.state, 177.4f, 0.1f);
  EXPECT_NEAR(soc_pct.state, 55.45f, 0.05f);
}

// ── Min/max cell voltage and cell index ──────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, MinMaxCellVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(min_v.state, 3.2683f, 0.0001f);
  EXPECT_NEAR(max_v.state, 3.2808f, 0.0001f);
  EXPECT_FLOAT_EQ(min_cell.state, 10.0f);
  EXPECT_FLOAT_EQ(max_cell.state, 12.0f);
}

// ── Delta and average cell voltage ────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, DeltaAndAverageCellVoltage) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor delta, avg;
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(delta.state, 0.0125f, 0.0001f);
  EXPECT_NEAR(avg.state, 3.2753f, 0.0001f);
}

// ── Cell resistance ───────────────────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, CellResistance1) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor r1;
  bms.set_cell_resistance_sensor(0, &r1);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_NEAR(r1.state, 0.08489f, 0.0001f);
}

// ── Protection bitmasks (all zero — no fault in capture) ─────────────────────

TEST(EkB24S8E200ACellInfoTest, ProtectionBitmasks) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor det_failed, overvoltage, undervoltage, polarity, excess_resistance;
  bms.set_cell_detection_failed_bitmask_sensor(&det_failed);
  bms.set_cell_overvoltage_bitmask_sensor(&overvoltage);
  bms.set_cell_undervoltage_bitmask_sensor(&undervoltage);
  bms.set_cell_polarity_error_bitmask_sensor(&polarity);
  bms.set_cell_excessive_line_resistance_bitmask_sensor(&excess_resistance);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_FLOAT_EQ(det_failed.state, 0.0f);
  EXPECT_FLOAT_EQ(overvoltage.state, 0.0f);
  EXPECT_FLOAT_EQ(undervoltage.state, 0.0f);
  EXPECT_FLOAT_EQ(polarity.state, 0.0f);
  EXPECT_FLOAT_EQ(excess_resistance.state, 0.0f);
}

// ── Error status (all clear — no fault in capture) ────────────────────────────

TEST(EkB24S8E200ACellInfoTest, ErrorStatus) {
  TestableHeltecBalancerBle bms;
  binary_sensor::BinarySensor sys_overheat, chg_fault, dsg_fault;
  bms.set_error_system_overheating_binary_sensor(&sys_overheat);
  bms.set_error_charging_binary_sensor(&chg_fault);
  bms.set_error_discharging_binary_sensor(&dsg_fault);

  bms.decode_cell_info_v2_(CELL_INFO_FRAME);

  EXPECT_FALSE(sys_overheat.state);
  EXPECT_FALSE(chg_fault.state);
  EXPECT_FALSE(dsg_fault.state);
}

// ── dispatch: decode_ routes cell info to v2 when protocol_version is V2 ─────

TEST(EkB24S8E200ACellInfoTest, DispatchesToV2WhenProtocolVersionSet) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor current;
  bms.set_balancing_current_sensor(&current);
  bms.set_protocol_version(PROTOCOL_VERSION_V2);

  bms.decode_(CELL_INFO_FRAME);

  EXPECT_NEAR(current.state, -8.055f, 0.01f);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(EkB24S8E200ACellInfoTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_cell_info_v2_(CELL_INFO_FRAME);
}

// ── Device info frame (same decode as legacy) ─────────────────────────────────

TEST(EkB24S8E200ADeviceInfoTest, TotalRuntime) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 53081.0f);
}

TEST(EkB24S8E200ADeviceInfoTest, TotalRuntimeFormatted) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor formatted;
  bms.set_total_runtime_formatted_text_sensor(&formatted);

  bms.decode_device_info_(DEVICE_INFO_FRAME);

  EXPECT_EQ(formatted.state, "14h");
}

// ── Settings frame SW-2.0.0 (150 bytes) ──────────────────────────────────────

TEST(EkB24S8E200ASettingsTest, CellCount) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(EkB24S8E200ASettingsTest, BalancerSwitch) {
  TestableHeltecBalancerBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_TRUE(balancer.state);
}

TEST(EkB24S8E200ASettingsTest, BuzzerModeText) {
  TestableHeltecBalancerBle bms;
  text_sensor::TextSensor buzzer;
  bms.set_buzzer_mode_text_sensor(&buzzer);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_EQ(buzzer.state, "Off");
}

TEST(EkB24S8E200ASettingsTest, VoltageAndCurrentNumbers) {
  TestableHeltecBalancerBle bms;
  TestNumber cap, trig, stop_vol, start_vol, max_cur;
  bms.set_nominal_battery_capacity_number(&cap);
  bms.set_balance_trigger_voltage_number(&trig);
  bms.set_balance_sleep_voltage_number(&stop_vol);
  bms.set_balance_start_voltage_number(&start_vol);
  bms.set_max_balance_current_number(&max_cur);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_NEAR(cap.state, 320.0f, 0.1f);
  EXPECT_NEAR(trig.state, 0.003f, 0.0001f);
  EXPECT_NEAR(stop_vol.state, 3.40f, 0.01f);
  EXPECT_NEAR(start_vol.state, 3.38f, 0.01f);
  EXPECT_NEAR(max_cur.state, 8.0f, 0.001f);
}

TEST(EkB24S8E200ASettingsTest, NullSensorsDoNotCrash) {
  TestableHeltecBalancerBle bms;
  bms.decode_settings_v2_(SETTINGS_FRAME);
}

TEST(EkB24S8E200ASettingsTest, BmsProtectionVoltages) {
  TestableHeltecBalancerBle bms;
  TestNumber ovp, ocv, uvp, uvr, dpp, dpr, shutdown;
  bms.set_cell_overvoltage_protection_number(&ovp);
  bms.set_cell_overvoltage_recovery_number(&ocv);
  bms.set_cell_undervoltage_protection_number(&uvp);
  bms.set_cell_undervoltage_recovery_number(&uvr);
  bms.set_cell_voltage_difference_protection_number(&dpp);
  bms.set_cell_voltage_difference_recovery_number(&dpr);
  bms.set_cell_shutdown_voltage_number(&shutdown);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_NEAR(ovp.state, 3.65f, 0.001f);
  EXPECT_NEAR(ocv.state, 3.55f, 0.001f);
  EXPECT_NEAR(uvp.state, 2.60f, 0.001f);
  EXPECT_NEAR(uvr.state, 2.90f, 0.001f);
  EXPECT_NEAR(dpp.state, 1.0f, 0.001f);
  EXPECT_NEAR(dpr.state, 0.7f, 0.001f);
  EXPECT_NEAR(shutdown.state, 2.0f, 0.001f);
}

TEST(EkB24S8E200ASettingsTest, BmsProtectionCurrentAndTime) {
  TestableHeltecBalancerBle bms;
  TestNumber chg_ocp, chg_delay, chg_recovery, dsg_ocp, short_ocp, short_delay;
  bms.set_charging_overcurrent_protection_number(&chg_ocp);
  bms.set_charging_overcurrent_delay_number(&chg_delay);
  bms.set_charging_overcurrent_recovery_number(&chg_recovery);
  bms.set_discharging_overcurrent_protection_number(&dsg_ocp);
  bms.set_short_circuit_protection_number(&short_ocp);
  bms.set_short_circuit_detection_delay_number(&short_delay);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_NEAR(chg_ocp.state, 170.0f, 0.1f);
  EXPECT_FLOAT_EQ(chg_delay.state, 5.0f);
  EXPECT_FLOAT_EQ(chg_recovery.state, 300.0f);
  EXPECT_NEAR(dsg_ocp.state, 200.0f, 0.1f);
  EXPECT_NEAR(short_ocp.state, 800.0f, 0.1f);
  EXPECT_FLOAT_EQ(short_delay.state, 500.0f);
}

TEST(EkB24S8E200ASettingsTest, BmsProtectionTemperatures) {
  TestableHeltecBalancerBle bms;
  TestNumber chg_otp, chg_ltp, dsg_otp, dsg_ltp;
  bms.set_charging_overtemperature_protection_number(&chg_otp);
  bms.set_charging_undertemperature_protection_number(&chg_ltp);
  bms.set_discharging_overtemperature_protection_number(&dsg_otp);
  bms.set_discharging_undertemperature_protection_number(&dsg_ltp);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_NEAR(chg_otp.state, 60.0f, 0.1f);
  EXPECT_NEAR(chg_ltp.state, -2.0f, 0.1f);
  EXPECT_NEAR(dsg_otp.state, 60.0f, 0.1f);
  EXPECT_NEAR(dsg_ltp.state, -10.0f, 0.1f);
}

// ── dispatch: decode_settings_ auto-detects V2 from 150-byte frame ───────────

TEST(EkB24S8E200ASettingsTest, DecodesV2Settings) {
  TestableHeltecBalancerBle bms;
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);

  bms.decode_settings_v2_(SETTINGS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

}  // namespace esphome::heltec_balancer_ble::testing
