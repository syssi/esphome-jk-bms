#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk04.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsJk04DeviceInfoTest, HardwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor hw;
  bms.set_hardware_version_text_sensor(&hw);
  bms.decode_device_info_(DEVICE_INFO_JK04);
  EXPECT_EQ(hw.state, "3.0");
}

TEST(JkBmsJk04DeviceInfoTest, SoftwareVersion) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor sw;
  bms.set_software_version_text_sensor(&sw);
  bms.decode_device_info_(DEVICE_INFO_JK04);
  EXPECT_EQ(sw.state, "3.3.0");
}

TEST(JkBmsJk04DeviceInfoTest, DeviceModel) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor device_model;
  bms.set_device_model_text_sensor(&device_model);
  bms.decode_device_info_(DEVICE_INFO_JK04);
  EXPECT_EQ(device_model.state, "JK-B2A16S");
}

TEST(JkBmsJk04DeviceInfoTest, ManufacturingDateAndSerialNumberAreEmpty) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor manufacturing_date;
  text_sensor::TextSensor serial_number;
  bms.set_manufacturing_date_text_sensor(&manufacturing_date);
  bms.set_serial_number_text_sensor(&serial_number);
  bms.decode_device_info_(DEVICE_INFO_JK04);
  EXPECT_EQ(manufacturing_date.state, "");
  EXPECT_EQ(serial_number.state, "");
}

TEST(JkBmsJk04DeviceInfoTest, PowerOnCount) {
  TestableJkBmsBle bms;
  sensor::Sensor power_on_count;
  bms.set_power_on_count_sensor(&power_on_count);
  bms.decode_device_info_(DEVICE_INFO_JK04);
  EXPECT_EQ(power_on_count.state, 19.0f);
}

TEST(JkBmsJk04CellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor cells[16];
  for (int i = 0; i < 16; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04);
  EXPECT_NEAR(cells[0].state, 3.353f, 0.001f);  // cell 1 (min)
  EXPECT_NEAR(cells[1].state, 3.359f, 0.001f);  // cell 2 (max)
}

TEST(JkBmsJk04CellInfoTest, TotalVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04);
  EXPECT_NEAR(total.state, 53.691f, 0.001f);
}

TEST(JkBmsJk04CellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04);
  EXPECT_FLOAT_EQ(runtime.state, 1873811.0f);
}

TEST(JkBmsJk04CellInfoTest, Balancing) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  binary_sensor::BinarySensor balancing;
  bms.set_balancing_binary_sensor(&balancing);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04);
  EXPECT_TRUE(balancing.state);
}

TEST(JkBmsJk04CellInfoTest, DispatchedViaFrameType) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_(CELL_INFO_JK04);
  EXPECT_NEAR(total.state, 53.691f, 0.001f);
}

}  // namespace esphome::jk_bms_ble::testing
