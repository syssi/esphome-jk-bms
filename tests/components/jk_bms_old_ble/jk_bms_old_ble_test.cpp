#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include "common.h"

namespace esphome::jk_bms_old_ble::testing {

// ── decode_basic_info_ ─────────────────────────────────────────────────────

TEST(JkBmsOldBasicInfoTest, TotalVoltage) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_NEAR(total.state, 58.88f, 0.01f);
}

TEST(JkBmsOldBasicInfoTest, ZeroCurrentAndPower) {
  TestableJkBmsOldBle bms;
  sensor::Sensor current, power, charging_power, discharging_power;
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_FLOAT_EQ(current.state, 0.0f);
  EXPECT_FLOAT_EQ(power.state, 0.0f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_FLOAT_EQ(discharging_power.state, 0.0f);
}

TEST(JkBmsOldBasicInfoTest, Capacity) {
  TestableJkBmsOldBle bms;
  sensor::Sensor remaining, nominal;
  bms.set_capacity_remaining_sensor(&remaining);
  bms.set_full_charge_capacity_sensor(&nominal);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_NEAR(remaining.state, 7.20f, 0.01f);
  EXPECT_NEAR(nominal.state, 10.00f, 0.01f);
}

TEST(JkBmsOldBasicInfoTest, ChargingCyclesAndCellCount) {
  TestableJkBmsOldBle bms;
  sensor::Sensor cycles, cell_count;
  bms.set_charging_cycles_sensor(&cycles);
  bms.set_cell_count_sensor(&cell_count);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_FLOAT_EQ(cycles.state, 0.0f);
  EXPECT_FLOAT_EQ(cell_count.state, 15.0f);
}

TEST(JkBmsOldBasicInfoTest, StateOfCharge) {
  TestableJkBmsOldBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_FLOAT_EQ(soc.state, 72.0f);
}

TEST(JkBmsOldBasicInfoTest, Temperatures) {
  TestableJkBmsOldBle bms;
  sensor::Sensor t1, t2;
  bms.set_temperature_sensor_1_sensor(&t1);
  bms.set_temperature_sensor_2_sensor(&t2);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_NEAR(t1.state, 20.25f, 0.01f);
  EXPECT_NEAR(t2.state, 21.45f, 0.01f);
}

TEST(JkBmsOldBasicInfoTest, BothMosfetsOn) {
  TestableJkBmsOldBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD);
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(JkBmsOldBasicInfoTest, DischargingMosfetOnly) {
  TestableJkBmsOldBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_basic_info_(BASIC_INFO_JK_OLD_DISCHARGING_ONLY);
  EXPECT_FALSE(charging.state);
  EXPECT_TRUE(discharging.state);
}

// ── assemble ─────────────────────────────────────────────────────────────

TEST(JkBmsOldAssembleTest, SingleFragmentDecodesBasicInfo) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.assemble(BASIC_INFO_JK_OLD.data(), BASIC_INFO_JK_OLD.size());

  EXPECT_NEAR(total.state, 58.88f, 0.01f);
}

TEST(JkBmsOldAssembleTest, CorruptedChecksumIsRejected) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> corrupted = BASIC_INFO_JK_OLD;
  corrupted[corrupted.size() - 2] ^= 0xFF;  // flip a checksum byte

  bms.assemble(corrupted.data(), corrupted.size());

  EXPECT_TRUE(std::isnan(total.state));  // never decoded, sensor keeps its unset state
}

TEST(JkBmsOldAssembleTest, MissingEndOfFrameMarkerIsDropped) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> broken = BASIC_INFO_JK_OLD;
  broken.back() = 0x00;

  bms.assemble(broken.data(), broken.size());
  EXPECT_TRUE(std::isnan(total.state));

  bms.assemble(BASIC_INFO_JK_OLD.data(), BASIC_INFO_JK_OLD.size());
  EXPECT_NEAR(total.state, 58.88f, 0.01f);
}

TEST(JkBmsOldAssembleTest, IncompleteFrameDoesNotDecodeYet) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.assemble(BASIC_INFO_JK_OLD.data(), 10);  // header + a few payload bytes only
  EXPECT_TRUE(std::isnan(total.state));

  bms.assemble(BASIC_INFO_JK_OLD.data() + 10, BASIC_INFO_JK_OLD.size() - 10);
  EXPECT_NEAR(total.state, 58.88f, 0.01f);
}

TEST(JkBmsOldAssembleTest, RealisticSmallMtuFragmentsReassemble) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total, soc;
  bms.set_total_voltage_sensor(&total);
  bms.set_state_of_charge_sensor(&soc);

  const size_t mtu_payload = 20;  // typical small-MTU BLE notification chunk size
  const auto &frame = BASIC_INFO_JK_OLD;
  for (size_t offset = 0; offset < frame.size(); offset += mtu_payload) {
    size_t chunk = std::min(mtu_payload, frame.size() - offset);
    bms.assemble(&frame[offset], chunk);
  }

  EXPECT_NEAR(total.state, 58.88f, 0.01f);
  EXPECT_FLOAT_EQ(soc.state, 72.0f);
}

TEST(JkBmsOldAssembleTest, FreshPreambleResyncsAfterAbandonedFrame) {
  TestableJkBmsOldBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  // Half of a frame is lost mid-transmission (e.g. a BLE disconnect glitch)...
  bms.assemble(BASIC_INFO_JK_OLD.data(), 10);
  // ...then a new frame starts. The stale half must be discarded, not prepended.
  bms.assemble(BASIC_INFO_JK_OLD.data(), BASIC_INFO_JK_OLD.size());

  EXPECT_NEAR(total.state, 58.88f, 0.01f);
}

}  // namespace esphome::jk_bms_old_ble::testing
