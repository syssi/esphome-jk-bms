#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// ── Device info frame (frame type 0x03) ─────────────────────────────────────

TEST(JkBmsBleDeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(hw.state, "10.XG");
}

TEST(JkBmsBleDeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);

  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(sw.state, "10.07");
}

TEST(JkBmsBleDeviceInfoTest, NullSensorsDoNotCrash) {
  TestableJkBmsBle bms;
  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);
}

TEST(JkBmsBleDeviceInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);

  bms.decode_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(hw.state, "10.XG");
}

// ── JK02_24S cell info frame (frame type 0x02) ───────────────────────────────

TEST(JkBmsBleJk02CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  sensor::Sensor cells[24];
  for (int i = 0; i < 24; i++) {
    bms.set_cell_voltage_sensor(i, &cells[i]);
  }

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(cells[0].state, 3.284f, 0.001f);   // cell 1
  EXPECT_NEAR(cells[1].state, 3.280f, 0.001f);   // cell 2
  EXPECT_NEAR(cells[2].state, 3.279f, 0.001f);   // cell 3
  EXPECT_NEAR(cells[3].state, 3.280f, 0.001f);   // cell 4
  EXPECT_NEAR(cells[4].state, 3.280f, 0.001f);   // cell 5
  EXPECT_NEAR(cells[5].state, 3.279f, 0.001f);   // cell 6
  EXPECT_NEAR(cells[6].state, 3.279f, 0.001f);   // cell 7
  EXPECT_NEAR(cells[7].state, 3.281f, 0.001f);   // cell 8
  EXPECT_NEAR(cells[8].state, 3.280f, 0.001f);   // cell 9
  EXPECT_NEAR(cells[9].state, 3.279f, 0.001f);   // cell 10
  EXPECT_NEAR(cells[10].state, 3.280f, 0.001f);  // cell 11
  EXPECT_NEAR(cells[11].state, 3.279f, 0.001f);  // cell 12
  EXPECT_NEAR(cells[12].state, 3.282f, 0.001f);  // cell 13
  EXPECT_NEAR(cells[13].state, 3.284f, 0.001f);  // cell 14
  EXPECT_NEAR(cells[14].state, 3.279f, 0.001f);  // cell 15
  EXPECT_NEAR(cells[15].state, 3.279f, 0.001f);  // cell 16
  EXPECT_NEAR(cells[16].state, 3.280f, 0.001f);  // cell 17
  EXPECT_NEAR(cells[17].state, 3.279f, 0.001f);  // cell 18
  EXPECT_NEAR(cells[18].state, 3.281f, 0.001f);  // cell 19
  EXPECT_NEAR(cells[19].state, 3.282f, 0.001f);  // cell 20
  EXPECT_NEAR(cells[20].state, 3.282f, 0.001f);  // cell 21
  EXPECT_NEAR(cells[21].state, 3.282f, 0.001f);  // cell 22
  EXPECT_NEAR(cells[22].state, 3.285f, 0.001f);  // cell 23 (max)
  EXPECT_NEAR(cells[23].state, 3.281f, 0.001f);  // cell 24
}

TEST(JkBmsBleJk02CellInfoTest, CellVoltageStats) {
  TestableJkBmsBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(min_v.state, 3.279f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.285f, 0.001f);
  EXPECT_NEAR(min_cell.state, 3.0f, 0.1f);   // first min-voltage cell = cell 3
  EXPECT_NEAR(max_cell.state, 23.0f, 0.1f);  // first max-voltage cell = cell 23
  EXPECT_NEAR(delta.state, 0.006f, 0.001f);
  EXPECT_NEAR(avg.state, 3.281f, 0.001f);
}

TEST(JkBmsBleJk02CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsBleJk02CellInfoTest, ZeroCurrent) {
  TestableJkBmsBle bms;
  sensor::Sensor current, power, charging_power, discharging_power;
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(current.state, 0.0f);
  EXPECT_FLOAT_EQ(power.state, 0.0f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_FLOAT_EQ(discharging_power.state, 0.0f);
}

TEST(JkBmsBleJk02CellInfoTest, Temperatures) {
  TestableJkBmsBle bms;
  sensor::Sensor t1, t2, tube;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_mosfet_temperature_sensor(&tube);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(t1.state, 20.7f, 0.1f);
  EXPECT_NEAR(t2.state, 20.7f, 0.1f);
  EXPECT_NEAR(tube.state, 23.4f, 0.1f);
}

TEST(JkBmsBleJk02CellInfoTest, NoErrors) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor errors_bitmask_hex;
  text_sensor::TextSensor errors_text;
  bms.set_errors_bitmask_hex_text_sensor(&errors_bitmask_hex);
  bms.set_errors_text_sensor(&errors_text);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_EQ(errors_bitmask_hex.state, "0x00000000");
  EXPECT_EQ(errors_text.state, "");
}

TEST(JkBmsBleJk02CellInfoTest, BalancingOff) {
  TestableJkBmsBle bms;
  sensor::Sensor balancer_status_bitmask;
  binary_sensor::BinarySensor balancing_binary;
  bms.set_balancer_status_bitmask_sensor(&balancer_status_bitmask);
  bms.set_balancing_binary_sensor(&balancing_binary);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(balancer_status_bitmask.state, 0.0f);
  EXPECT_FALSE(balancing_binary.state);
}

TEST(JkBmsBleJk02CellInfoTest, StateOfCharge) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soc.state, 85.0f);
}

TEST(JkBmsBleJk02CellInfoTest, StateOfHealth) {
  TestableJkBmsBle bms;
  sensor::Sensor soh;
  bms.set_state_of_health_sensor(&soh);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soh.state, 100.0f);
}

TEST(JkBmsBleJk02CellInfoTest, Capacity) {
  TestableJkBmsBle bms;
  sensor::Sensor remaining, nominal, cycles;
  bms.set_capacity_remaining_sensor(&remaining);
  bms.set_full_charge_capacity_sensor(&nominal);
  bms.set_charging_cycles_sensor(&cycles);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(remaining.state, 42.804f, 0.001f);
  EXPECT_NEAR(nominal.state, 50.0f, 0.001f);
  EXPECT_FLOAT_EQ(cycles.state, 0.0f);
}

TEST(JkBmsBleJk02CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  sensor::Sensor runtime;
  text_sensor::TextSensor runtime_fmt;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_runtime_formatted_text_sensor(&runtime_fmt);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(runtime.state, 269340.0f);
  EXPECT_EQ(runtime_fmt.state, "3d 2h");
}

TEST(JkBmsBleJk02CellInfoTest, MosfetState) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor charging, discharging, precharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.set_precharging_binary_sensor(&precharging);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(JkBmsBleJk02CellInfoTest, HeatingOff) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor heating;
  bms.set_heating_binary_sensor(&heating);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_FALSE(heating.state);
}

TEST(JkBmsBleJk02CellInfoTest, CellResistances) {
  TestableJkBmsBle bms;
  sensor::Sensor r1, r2;
  bms.set_cell_resistance_sensor(0, &r1);
  bms.set_cell_resistance_sensor(1, &r2);

  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(r1.state, 0.100f, 0.001f);  // cell 1 = 0x0064 = 100 mΩ
  EXPECT_NEAR(r2.state, 0.108f, 0.001f);  // cell 2 = 0x006C = 108 mΩ
}

TEST(JkBmsBleJk02CellInfoTest, NullSensorsDoNotCrash) {
  TestableJkBmsBle bms;
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
}

TEST(JkBmsBleJk02CellInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_FLOAT_EQ(soc.state, 85.0f);
}

// ── Protocol version dispatch ────────────────────────────────────────────────

TEST(JkBmsBleDispatchTest, FrameType02DispatchesToCellInfo) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsBleDispatchTest, FrameType03DispatchesToDeviceInfo) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);

  bms.decode_(DEVICE_INFO_JK02_24S_V10);

  EXPECT_EQ(sw.state, "10.07");
}

TEST(JkBmsBleDispatchTest, Jk04ModeDispatches) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);

  // JK04 cell info dispatch should not crash even with a JK02 frame body
  bms.decode_(CELL_INFO_JK02_24S_V10);
}

}  // namespace esphome::jk_bms_ble::testing
