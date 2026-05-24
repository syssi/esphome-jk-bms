#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_v11.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsV11DeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V11);
  EXPECT_EQ(hw.state, "14.XA");
}

TEST(JkBmsV11DeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V11);
  EXPECT_EQ(sw.state, "14.20");
}

TEST(JkBmsV11CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor cells[16];
  for (int i = 0; i < 16; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_NEAR(cells[0].state, 3.246f, 0.001f);   // cell 1 (max)
  EXPECT_NEAR(cells[1].state, 3.230f, 0.001f);   // cell 2
  EXPECT_NEAR(cells[10].state, 3.216f, 0.001f);  // cell 11 (min)
  EXPECT_NEAR(cells[15].state, 3.242f, 0.001f);  // cell 16
}

TEST(JkBmsV11CellInfoTest, CellVoltageStats) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_NEAR(min_v.state, 3.216f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.246f, 0.001f);
  EXPECT_NEAR(min_cell.state, 11.0f, 0.1f);
  EXPECT_NEAR(max_cell.state, 1.0f, 0.1f);
  EXPECT_NEAR(delta.state, 0.030f, 0.001f);
}

TEST(JkBmsV11CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_NEAR(total.state, 51.689f, 0.001f);
}

TEST(JkBmsV11CellInfoTest, ZeroCurrent) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor current;
  bms.set_current_sensor(&current);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_FLOAT_EQ(current.state, 0.0f);
}

TEST(JkBmsV11CellInfoTest, Temperatures) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor t1, t2, tube;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_mosfet_temperature_sensor(&tube);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_NEAR(t1.state, 17.7f, 0.1f);
  EXPECT_NEAR(t2.state, 17.7f, 0.1f);
  EXPECT_NEAR(tube.state, 17.3f, 0.1f);
}

TEST(JkBmsV11CellInfoTest, StateOfCharge) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_FLOAT_EQ(soc.state, 52.0f);
}

TEST(JkBmsV11CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_FLOAT_EQ(runtime.state, 107416.0f);
}

TEST(JkBmsV11CellInfoTest, MosfetState) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(JkBmsV11CellInfoTest, NoHeating) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor heating;
  bms.set_heating_binary_sensor(&heating);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V11);
  EXPECT_FALSE(heating.state);
}

TEST(JkBmsV11CellInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_(CELL_INFO_JK02_32S_V11);
  EXPECT_NEAR(total.state, 51.689f, 0.001f);
}

}  // namespace esphome::jk_bms_ble::testing
