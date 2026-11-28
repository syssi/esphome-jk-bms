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

TEST(JkBmsV19DeviceInfoTest, PowerOnCount) {
  TestableJkBmsBle bms;
  sensor::Sensor power_on_count;
  bms.set_power_on_count_sensor(&power_on_count);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(power_on_count.state, 16.0f);
}

TEST(JkBmsV19DeviceInfoTest, DeviceName) {
  TestableJkBmsBle bms;
  text_sensor::TextSensor device_name;
  bms.set_device_name_text_sensor(&device_name);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(device_name.state, "506177F49000674");
}

TEST(JkBmsV19DeviceInfoTest, BluetoothName) {
  // 15 characters: longer than a 0x9B write can set, but the text shows the whole stored name
  TestableJkBmsBle bms;
  TestText bluetooth_name;
  bms.set_bluetooth_name_text(&bluetooth_name);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(bluetooth_name.state, "506177F49000674");
}

TEST(JkBmsV19DeviceInfoTest, UserData) {
  TestableJkBmsBle bms;
  TestText user_data;
  bms.set_user_data_text(&user_data);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(user_data.state, "JK-BMS");
}

TEST(JkBmsV19DeviceInfoTest, UserData2) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestText user_data_2;
  bms.set_user_data_2_text(&user_data_2);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(user_data_2.state, "JK-BMS");
}

TEST(JkBmsV19DeviceInfoTest, UserData2WithoutNullTerminator) {
  // A text using all 16 bytes has no terminating zero: it must not run into the field at offset 150
  std::vector<uint8_t> data = DEVICE_INFO_JK02_32S_V19;
  for (size_t i = 0; i < 16; i++) {
    data[134 + i] = 'a' + i;
  }

  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestText user_data_2;
  bms.set_user_data_2_text(&user_data_2);
  bms.decode_device_info_(data);
  EXPECT_EQ(user_data_2.state, "abcdefghijklmnop");
}

TEST(JkBmsV19DeviceInfoTest, SupportedCommandsBitmask) {
  // Bit N is command N: 0x00-0x3B are all supported, 0xB4 (bit 180) isn't
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  text_sensor::TextSensor supported_commands;
  bms.set_supported_commands_bitmask_hex_text_sensor(&supported_commands);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(supported_commands.state, "0x000000000000000000EFFEF7D8C0000000001F90000000000F8DE9AFFFFFFFFF");
}

TEST(JkBmsV19DeviceInfoTest, DevicePassword) {
  TestableJkBmsBle bms;
  TestText device_password;
  bms.set_device_password_text(&device_password);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(device_password.state, "1234");
}

TEST(JkBmsV19DeviceInfoTest, DevicePasswordWithoutNullTerminator) {
  // A password using all 16 bytes has no terminating zero: it must not run into the manufacturing date at offset 78
  std::vector<uint8_t> data = DEVICE_INFO_JK02_32S_V19;
  for (size_t i = 0; i < 16; i++) {
    data[62 + i] = 'a' + i;
  }

  TestableJkBmsBle bms;
  TestText device_password;
  bms.set_device_password_text(&device_password);
  bms.decode_device_info_(data);
  EXPECT_EQ(device_password.state, "abcdefghijklmnop");
}

TEST(JkBmsV19DeviceInfoTest, SettingsPassword) {
  TestableJkBmsBle bms;
  TestText settings_password;
  bms.set_settings_password_text(&settings_password);
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_EQ(settings_password.state, "12345");
}

TEST(JkBmsV19DeviceInfoTest, SettingsPasswordWithoutNullTerminator) {
  // A password using all 16 bytes has no terminating zero: it must not run into the field at offset 134
  std::vector<uint8_t> data = DEVICE_INFO_JK02_32S_V19;
  for (size_t i = 0; i < 16; i++) {
    data[118 + i] = 'a' + i;
  }

  TestableJkBmsBle bms;
  TestText settings_password;
  bms.set_settings_password_text(&settings_password);
  bms.decode_device_info_(data);
  EXPECT_EQ(settings_password.state, "abcdefghijklmnop");
}

TEST(JkBmsV19DeviceInfoTest, DeviceNameWithoutNullTerminator) {
  // A name using all 16 bytes has no terminating zero: it must not run into the passcode at offset 62.
  std::vector<uint8_t> data = DEVICE_INFO_JK02_32S_V19;
  for (size_t i = 0; i < 16; i++) {
    data[46 + i] = 'A' + i;
  }

  TestableJkBmsBle bms;
  text_sensor::TextSensor device_name;
  bms.set_device_name_text_sensor(&device_name);
  bms.decode_device_info_(data);
  EXPECT_EQ(device_name.state, "ABCDEFGHIJKLMNOP");
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

TEST(JkBmsV19CellInfoTest, PrechargingOff) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor precharging;
  bms.set_precharging_binary_sensor(&precharging);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  EXPECT_TRUE(precharging.has_state());
  EXPECT_FALSE(precharging.state);
}

// The precharge flag is the low byte of the SOH/precharge word (byte 191), see
// docs/pb2a16s20p/BMS RS485 Modbus V1.1.pdf. None of the captures was taken during
// a precharge, so the flag is set on a copy of the real frame.
TEST(JkBmsV19CellInfoTest, PrechargingOn) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor precharging;
  bms.set_precharging_binary_sensor(&precharging);
  auto frame = CELL_INFO_JK02_32S_V19;
  frame[191] = 0x01;
  bms.decode_jk02_cell_info_(frame);
  EXPECT_TRUE(precharging.state);
}

// Bytes 200-201 hold the user alarm 2 word, so an alarm bit must not report precharging.
TEST(JkBmsV19CellInfoTest, UserAlarm2DoesNotSetPrecharging) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  binary_sensor::BinarySensor precharging;
  bms.set_precharging_binary_sensor(&precharging);
  auto frame = CELL_INFO_JK02_32S_V19;
  frame[200] = 0x01;
  bms.decode_jk02_cell_info_(frame);
  EXPECT_FALSE(precharging.state);
}

// Protection recovery countdowns (bytes 202-213, uint16, seconds). The captured frame
// wasn't recorded during a protection event, so all countdowns are zero.
TEST(JkBmsV19CellInfoTest, ProtectionRecoveryCountdownsIdle) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor dch_ocp, dch_scp, chg_ocp, chg_scp, cell_uvp, cell_ovp;
  bms.set_discharge_overcurrent_protection_recovery_countdown_sensor(&dch_ocp);
  bms.set_discharge_short_circuit_protection_recovery_countdown_sensor(&dch_scp);
  bms.set_charge_overcurrent_protection_recovery_countdown_sensor(&chg_ocp);
  bms.set_charge_short_circuit_protection_recovery_countdown_sensor(&chg_scp);
  bms.set_cell_undervoltage_protection_recovery_countdown_sensor(&cell_uvp);
  bms.set_cell_overvoltage_protection_recovery_countdown_sensor(&cell_ovp);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_V19);
  for (auto *sens : {&dch_ocp, &dch_scp, &chg_ocp, &chg_scp, &cell_uvp, &cell_ovp}) {
    EXPECT_TRUE(sens->has_state());
    EXPECT_EQ(sens->state, 0.0f);
  }
}

// Distinct values per field verify the mapping of each countdown to its bytes.
TEST(JkBmsV19CellInfoTest, ProtectionRecoveryCountdowns) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor dch_ocp, dch_scp, chg_ocp, chg_scp, cell_uvp, cell_ovp;
  bms.set_discharge_overcurrent_protection_recovery_countdown_sensor(&dch_ocp);
  bms.set_discharge_short_circuit_protection_recovery_countdown_sensor(&dch_scp);
  bms.set_charge_overcurrent_protection_recovery_countdown_sensor(&chg_ocp);
  bms.set_charge_short_circuit_protection_recovery_countdown_sensor(&chg_scp);
  bms.set_cell_undervoltage_protection_recovery_countdown_sensor(&cell_uvp);
  bms.set_cell_overvoltage_protection_recovery_countdown_sensor(&cell_ovp);
  auto frame = CELL_INFO_JK02_32S_V19;
  frame[202] = 0x01;  // 1 s
  frame[204] = 0x02;  // 2 s
  frame[206] = 0x03;  // 3 s
  frame[208] = 0x04;  // 4 s
  frame[210] = 0x2C;  // 300 s
  frame[211] = 0x01;
  frame[212] = 0xFF;  // 65535 s
  frame[213] = 0xFF;
  bms.decode_jk02_cell_info_(frame);
  EXPECT_EQ(dch_ocp.state, 1.0f);
  EXPECT_EQ(dch_scp.state, 2.0f);
  EXPECT_EQ(chg_ocp.state, 3.0f);
  EXPECT_EQ(chg_scp.state, 4.0f);
  EXPECT_EQ(cell_uvp.state, 300.0f);
  EXPECT_EQ(cell_ovp.state, 65535.0f);
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

// ── Settings frame ────────────────────────────────────────────────────────────

// Option order must match MULTIPLEXED_PORT_MODE_OPTIONS in select/__init__.py,
// because the port switch bit doubles as the option index.
static constexpr const char *const MULTIPLEXED_PORT_MODE_OPTS[] = {"CAN", "RS485"};

TEST(JkBmsV19SettingsTest, MultiplexedPortModeCan) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestSelect port_mode{"CAN", "RS485"};
  bms.set_multiplexed_port_mode_select(&port_mode);
  bms.set_multiplexed_port_mode_table(MULTIPLEXED_PORT_MODE_OPTS, 2);
  bms.decode_jk02_settings_(SETTINGS_JK02_32S_V19);
  ASSERT_TRUE(port_mode.has_state());
  EXPECT_EQ(port_mode.current_option().str(), "CAN");  // byte 282 bit3 clear
}

TEST(JkBmsV19SettingsTest, MultiplexedPortModeRs485) {
  // No RS485 capture available: take the captured frame and set the port switch bit.
  std::vector<uint8_t> data = SETTINGS_JK02_32S_V19;
  data[282] |= 0x08;

  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestSelect port_mode{"CAN", "RS485"};
  bms.set_multiplexed_port_mode_select(&port_mode);
  bms.set_multiplexed_port_mode_table(MULTIPLEXED_PORT_MODE_OPTS, 2);
  bms.decode_jk02_settings_(data);
  ASSERT_TRUE(port_mode.has_state());
  EXPECT_EQ(port_mode.current_option().str(), "RS485");
}

TEST(JkBmsV19SettingsTest, MultiplexedPortModeWithoutTableStaysUnset) {
  // The lookup table is only installed when the select is configured in YAML.
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestSelect port_mode{"CAN", "RS485"};
  bms.set_multiplexed_port_mode_select(&port_mode);
  bms.decode_jk02_settings_(SETTINGS_JK02_32S_V19);
  EXPECT_FALSE(port_mode.has_state());
}

TEST(JkBmsV19SettingsTest, HeatingSwitch) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestSwitch heating;
  bms.set_heating_switch(&heating);
  bms.decode_jk02_settings_(SETTINGS_JK02_32S_V19);
  EXPECT_TRUE(heating.state);  // byte 282 bit0 set
}

TEST(JkBmsV19SettingsTest, DeviceAddress) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor device_address;
  bms.set_device_address_sensor(&device_address);
  bms.decode_jk02_settings_(SETTINGS_JK02_32S_V19);
  EXPECT_EQ(device_address.state, 0.0f);  // byte 270
}

TEST(JkBmsV19SettingsTest, DeviceAddressNonZero) {
  // Captured device uses address 0: take the captured frame and set the DIP switch address.
  std::vector<uint8_t> data = SETTINGS_JK02_32S_V19;
  data[270] = 0x0F;

  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor device_address;
  bms.set_device_address_sensor(&device_address);
  bms.decode_jk02_settings_(data);
  EXPECT_EQ(device_address.state, 15.0f);
}

TEST(JkBmsV19SettingsTest, DeviceAddressIsUint32) {
  // The device address is a 32-bit field (bytes 270-273), not a single byte.
  std::vector<uint8_t> data = SETTINGS_JK02_32S_V19;
  data[270] = 0x2C;
  data[271] = 0x01;

  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  sensor::Sensor device_address;
  bms.set_device_address_sensor(&device_address);
  bms.decode_jk02_settings_(data);
  EXPECT_EQ(device_address.state, 300.0f);
}

TEST(JkBmsV19SettingsTest, DischargePrechargeTime) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestNumber precharge_time;
  bms.set_discharge_precharge_time_number(&precharge_time);
  bms.decode_jk02_settings_(SETTINGS_JK02_32S_V19);
  EXPECT_EQ(precharge_time.state, 0.0f);  // bytes 274-277
}

TEST(JkBmsV19SettingsTest, DischargePrechargeTimeAbove255Seconds) {
  // The precharge time is a 32-bit field (bytes 274-277). Values above 255 s
  // must not be truncated to the low byte (300 s = 0x012C).
  std::vector<uint8_t> data = SETTINGS_JK02_32S_V19;
  data[274] = 0x2C;
  data[275] = 0x01;

  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  TestNumber precharge_time;
  bms.set_discharge_precharge_time_number(&precharge_time);
  bms.decode_jk02_settings_(data);
  EXPECT_EQ(precharge_time.state, 300.0f);
}

}  // namespace esphome::jk_bms_ble::testing
