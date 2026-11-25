#include <cmath>
#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_b1a8s10p.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsB1A8S10PCellInfoTest, ConnectedTemperatureSensors) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor t1, t2, mosfet;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_mosfet_temperature_sensor(&mosfet);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  EXPECT_FLOAT_EQ(t1.state, 19.4f);
  EXPECT_FLOAT_EQ(t2.state, 19.5f);
  EXPECT_FLOAT_EQ(mosfet.state, 20.5f);
}

TEST(JkBmsB1A8S10PCellInfoTest, UnconnectedTemperatureSensorsStayUnknown) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor t3, t4;
  int t3_publishes = 0, t4_publishes = 0;
  t3.add_on_state_callback([&](float) { t3_publishes++; });
  t4.add_on_state_callback([&](float) { t4_publishes++; });
  bms.set_temperature_sensor(2, &t3);
  bms.set_temperature_sensor(3, &t4);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  EXPECT_TRUE(std::isnan(t3.state));
  EXPECT_TRUE(std::isnan(t4.state));
  EXPECT_EQ(t3_publishes, 0);
  EXPECT_EQ(t4_publishes, 0);
}

TEST(JkBmsB1A8S10PCellInfoTest, DisconnectedTemperatureSensorIsClearedOnce) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor t3;
  int publishes = 0;
  t3.add_on_state_callback([&](float) { publishes++; });
  bms.set_temperature_sensor(2, &t3);

  // Probe connected: bytes 258-259 = 0x00C8 (20.0 °C)
  std::vector<uint8_t> connected = CELL_INFO_JK02_32S_B1A8S10P;
  connected[258] = 0xC8;
  connected[259] = 0x00;
  bms.decode_jk02_cell_info_(connected);
  EXPECT_FLOAT_EQ(t3.state, 20.0f);
  EXPECT_EQ(publishes, 1);

  // Probe removed: the stale reading is cleared with a single NAN publish
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  EXPECT_TRUE(std::isnan(t3.state));
  EXPECT_EQ(publishes, 2);
}

TEST(JkBmsB1A8S10PCellInfoTest, CellVoltages) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor cell1, cell4;
  bms.set_cell_voltage_sensor(0, &cell1);
  bms.set_cell_voltage_sensor(3, &cell4);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  EXPECT_FLOAT_EQ(cell1.state, 3.324f);
  EXPECT_FLOAT_EQ(cell4.state, 3.324f);
}

}  // namespace esphome::jk_bms_ble::testing
