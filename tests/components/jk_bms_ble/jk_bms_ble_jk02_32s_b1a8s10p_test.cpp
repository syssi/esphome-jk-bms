#include <cmath>
#include <gtest/gtest.h>
#include <iterator>
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

// The labels of DEFAULT_ERRORS_JK02 up to bit 9, as the codegen hands them to the hub
static constexpr const char *const ERRORS_UP_TO_BIT_9[] = {
    "Wire resistance",                      // bit 0
    "MOSFET overtemperature",               // bit 1
    "Cell count is not equal to settings",  // bit 2
    "",                                     // bit 3
    "Battery is fully charged",             // bit 4
    "Battery pack overvoltage",             // bit 5
    "Charge overcurrent",                   // bit 6
    "Charge short circuit",                 // bit 7
    "Charge overtemperature",               // bit 8
    "Charge undertemperature",              // bit 9
};

TEST(JkBmsB1A8S10PCellInfoTest, ChargeUndertemperatureProtection) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(ERRORS_UP_TO_BIT_9, std::size(ERRORS_UP_TO_BIT_9));
  text_sensor::TextSensor errors, errors_hex;
  binary_sensor::BinarySensor charging, discharging;
  sensor::Sensor t1, t2;
  bms.set_errors_text_sensor(&errors);
  bms.set_errors_bitmask_hex_text_sensor(&errors_hex);
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P_CHARGE_UNDERTEMPERATURE);
  EXPECT_EQ(errors.state, "Charge undertemperature");
  EXPECT_EQ(errors_hex.state, "0x00000200");
  EXPECT_FALSE(charging.state);
  EXPECT_TRUE(discharging.state);
  EXPECT_FLOAT_EQ(t1.state, 2.6f);
  EXPECT_FLOAT_EQ(t2.state, 20.5f);
}

// T1 already in ice water, but the charge undertemperature protection still at -10 °C
TEST(JkBmsB1A8S10PCellInfoTest, NoErrorsBeforeTheProtectionTriggers) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(ERRORS_UP_TO_BIT_9, std::size(ERRORS_UP_TO_BIT_9));
  text_sensor::TextSensor errors, errors_hex;
  binary_sensor::BinarySensor charging;
  bms.set_errors_text_sensor(&errors);
  bms.set_errors_bitmask_hex_text_sensor(&errors_hex);
  bms.set_charging_binary_sensor(&charging);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P_ICE_WATER);
  EXPECT_EQ(errors.state, "");
  EXPECT_EQ(errors_hex.state, "0x00000000");
  EXPECT_TRUE(charging.state);
}

}  // namespace esphome::jk_bms_ble::testing
