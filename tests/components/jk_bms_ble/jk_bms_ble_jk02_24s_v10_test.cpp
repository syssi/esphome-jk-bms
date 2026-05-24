#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_24s_v10.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsV10DeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);
  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);
  EXPECT_EQ(hw.state, "10.XG");
}

TEST(JkBmsV10DeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);
  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);
  EXPECT_EQ(sw.state, "10.07");
}

TEST(JkBmsV10CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  sensor::Sensor cells[24];
  for (int i = 0; i < 24; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_NEAR(cells[0].state, 3.284f, 0.001f);   // cell 1
  EXPECT_NEAR(cells[1].state, 3.280f, 0.001f);   // cell 2
  EXPECT_NEAR(cells[2].state, 3.279f, 0.001f);   // cell 3
  EXPECT_NEAR(cells[22].state, 3.285f, 0.001f);  // cell 23 (max)
}

TEST(JkBmsV10CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_NEAR(total.state, 78.735f, 0.001f);
}

TEST(JkBmsV10CellInfoTest, ZeroCurrent) {
  TestableJkBmsBle bms;
  sensor::Sensor current;
  bms.set_current_sensor(&current);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_FLOAT_EQ(current.state, 0.0f);
}

TEST(JkBmsV10CellInfoTest, Temperatures) {
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

TEST(JkBmsV10CellInfoTest, StateOfCharge) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_FLOAT_EQ(soc.state, 85.0f);
}

TEST(JkBmsV10CellInfoTest, Capacity) {
  TestableJkBmsBle bms;
  sensor::Sensor remaining, nominal;
  bms.set_capacity_remaining_sensor(&remaining);
  bms.set_full_charge_capacity_sensor(&nominal);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_NEAR(remaining.state, 42.804f, 0.001f);
  EXPECT_NEAR(nominal.state, 50.0f, 0.001f);
}

TEST(JkBmsV10CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_FLOAT_EQ(runtime.state, 269340.0f);
}

TEST(JkBmsV10CellInfoTest, MosfetState) {
  TestableJkBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(JkBmsV10CellInfoTest, NegativeTemperatures) {
  TestableJkBmsBle bms;
  sensor::Sensor t1, t2;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10_NEG_TEMPS);
  EXPECT_NEAR(t1.state, -3.1f, 0.1f);
  EXPECT_NEAR(t2.state, -2.9f, 0.1f);
}

TEST(JkBmsV10CellInfoTest, NegTempsStateOfCharge) {
  TestableJkBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_24S_V10_NEG_TEMPS);
  EXPECT_FLOAT_EQ(soc.state, 47.0f);
}

}  // namespace esphome::jk_bms_ble::testing
