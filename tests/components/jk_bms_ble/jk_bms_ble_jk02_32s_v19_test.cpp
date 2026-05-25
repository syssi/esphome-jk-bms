#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_v19.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsV19DeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(hw.state, "19A");
}

TEST(JkBmsV19DeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(sw.state, "19.27");
}

TEST(JkBmsV19CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor cells[16];
  for (int i = 0; i < 16; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_NEAR(cells[0].state, 3.381f, 0.001f);   // cell 1 (max)
  EXPECT_NEAR(cells[15].state, 3.377f, 0.001f);  // cell 16
}

TEST(JkBmsV19CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_NEAR(total.state, 54.028f, 0.001f);
}

TEST(JkBmsV19CellInfoTest, NegativeCurrent) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor current;
  bms.set_current_sensor(&current);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_NEAR(current.state, -0.727f, 0.001f);
}

TEST(JkBmsV19CellInfoTest, Temperatures) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor t1, t2, tube;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_mosfet_temperature_sensor(&tube);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_NEAR(t1.state, 11.9f, 0.1f);
  EXPECT_NEAR(t2.state, 12.7f, 0.1f);
  EXPECT_NEAR(tube.state, 12.8f, 0.1f);
}

TEST(JkBmsV19CellInfoTest, StateOfCharge) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_FLOAT_EQ(soc.state, 100.0f);
}

TEST(JkBmsV19CellInfoTest, Capacity) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor remaining, nominal;
  bms.set_capacity_remaining_sensor(&remaining);
  bms.set_full_charge_capacity_sensor(&nominal);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_NEAR(remaining.state, 312.898f, 0.001f);
  EXPECT_NEAR(nominal.state, 314.000f, 0.001f);
}

TEST(JkBmsV19CellInfoTest, CycleCount) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor cycles;
  bms.set_charging_cycles_sensor(&cycles);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_FLOAT_EQ(cycles.state, 5.0f);
}

TEST(JkBmsV19CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_FLOAT_EQ(runtime.state, 3609276.0f);
}

TEST(JkBmsV19CellInfoTest, MosfetState) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(JkBmsV19CellInfoTest, DetailLogCount) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor detail_log_entry_count;
  bms.set_detail_log_entry_count_sensor(&detail_log_entry_count);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_FLOAT_EQ(detail_log_entry_count.state, 62.0f);
}

TEST(JkBmsV19CellInfoTest, BatteryTypeCode) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor battery_type_id;
  bms.set_battery_type_id_sensor(&battery_type_id);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_FLOAT_EQ(battery_type_id.state, 0.0f);  // LFP
}

TEST(JkBmsV19CellInfoTest, BatteryType) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  text_sensor::TextSensor battery_type;
  bms.set_battery_type_text_sensor(&battery_type);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_EQ(battery_type.state, "LFP");
}

}  // namespace esphome::jk_bms_ble::testing
