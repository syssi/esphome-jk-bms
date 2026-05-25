#include <gtest/gtest.h>
#include <cstring>
#include "common.h"
#include "frames_jk04_b5a24s.h"

namespace esphome::jk_bms_ble::testing {

// ── Settings frame ────────────────────────────────────────────────────────────

TEST(JkBmsJk04B5A24SSettingsTest, CellCount) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber cell_count;
  bms.set_cell_count_number(&cell_count);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(JkBmsJk04B5A24SSettingsTest, PowerOffVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber power_off;
  bms.set_power_off_voltage_number(&power_off);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_NEAR(power_off.state, 3.2f, 0.001f);
}

TEST(JkBmsJk04B5A24SSettingsTest, BalanceTriggerVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber trigger;
  bms.set_balance_trigger_voltage_number(&trigger);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_NEAR(trigger.state, 0.008f, 0.0001f);
}

TEST(JkBmsJk04B5A24SSettingsTest, MaxBalanceCurrent) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber max_cur;
  bms.set_max_balance_current_number(&max_cur);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_NEAR(max_cur.state, 4.5f, 0.001f);
}

TEST(JkBmsJk04B5A24SSettingsTest, VoltageCalibration) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber vcal;
  bms.set_voltage_calibration_number(&vcal);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_NEAR(vcal.state, 3.033f, 0.001f);
}

TEST(JkBmsJk04B5A24SSettingsTest, BalancingStartVoltage) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestNumber start_v;
  bms.set_balancing_start_voltage_number(&start_v);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_NEAR(start_v.state, 4.25f, 0.001f);
}

TEST(JkBmsJk04B5A24SSettingsTest, BalancerSwitch) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);
  bms.decode_jk04_settings_(SETTINGS_JK04_B5A24S);
  EXPECT_TRUE(balancer.state);
}

// ── Cell info frame ───────────────────────────────────────────────────────────

TEST(JkBmsJk04B5A24SCellInfoTest, TotalRuntime) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor runtime;
  bms.set_total_runtime_sensor(&runtime);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04_B5A24S);
  EXPECT_FLOAT_EQ(runtime.state, 591338.0f);
}

TEST(JkBmsJk04B5A24SCellInfoTest, BalancingCurrent) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  sensor::Sensor bal_cur;
  bms.set_balancing_current_sensor(&bal_cur);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04_B5A24S);
  EXPECT_NEAR(bal_cur.state, -2.6f, 0.05f);
}

TEST(JkBmsJk04B5A24SCellInfoTest, Balancing) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  binary_sensor::BinarySensor balancing;
  bms.set_balancing_binary_sensor(&balancing);
  bms.decode_jk04_cell_info_(CELL_INFO_JK04_B5A24S);
  EXPECT_TRUE(balancing.state);
}

// ── Write frame builder ───────────────────────────────────────────────────────

TEST(JkBmsJk04B5A24SWriteFrameTest, PowerOffVoltage3p1V) {
  float v = 3.100f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x20, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x20, 0x04, 0x66, 0x66, 0x46, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, PowerOffVoltage3p15V) {
  float v = 3.150f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x20, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x20, 0x04, 0x9A, 0x99, 0x49, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, PowerOffVoltage3p151V) {
  float v = 3.151f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x20, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x20, 0x04, 0xFC, 0xA9, 0x49, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, PowerOffVoltage3p2V) {
  float v = 3.2f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x20, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x20, 0x04, 0xCD, 0xCC, 0x4C, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalanceTrigger8mV) {
  float v = 0.008f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x64, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x64, 0x04, 0x6F, 0x12, 0x03, 0x3C,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalanceTrigger9mV) {
  float v = 0.009f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x64, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x64, 0x04, 0xBC, 0x74, 0x13, 0x3C,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalanceTrigger10mV) {
  float v = 0.010f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x64, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x64, 0x04, 0x0A, 0xD7, 0x23, 0x3C,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalanceTrigger11mV) {
  float v = 0.011f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x64, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x64, 0x04, 0x58, 0x39, 0x34, 0x3C,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE3};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, MaxBalanceCurrent4p4A) {
  float v = 4.400f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x68, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x68, 0x04, 0xCD, 0xCC, 0x8C, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, MaxBalanceCurrent4p45A) {
  float v = 4.450f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x68, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x68, 0x04, 0x66, 0x66, 0x8E, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, MaxBalanceCurrent4p456A) {
  float v = 4.456f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x68, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x68, 0x04, 0x8D, 0x97, 0x8E, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, MaxBalanceCurrent4p5A) {
  float v = 4.5f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x68, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x68, 0x04, 0x00, 0x00, 0x90, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB6};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, CellCount15) {
  auto frame = JkBmsBle::build_frame(0x1C, 15, 2);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x1C, 0x02, 0x0F, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA7};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, CellCount16) {
  auto frame = JkBmsBle::build_frame(0x1C, 16, 2);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x1C, 0x02, 0x10, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalancerOn) {
  auto frame = JkBmsBle::build_frame(0x6C, 0x00000001, 1);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x6C, 0x01, 0x01, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalancerOff) {
  auto frame = JkBmsBle::build_frame(0x6C, 0x00000000, 1);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x6C, 0x01, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

// ── Voltage calibration (ADC Vref) write frames ──────────────────────────────
// App displays pack voltage (52.xx V); actual value sent is ADC Vref (~3.03 V).
// HA entity uses the raw Vref, matching what decode_jk04_settings_ publishes.

TEST(JkBmsJk04B5A24SWriteFrameTest, VoltageCalibration3p031334V) {
  // display 52.91 V → Vref 3.031334 V
  float v = 3.031334f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x44, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x44, 0x04, 0x60, 0x01, 0x42, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, VoltageCalibration3p030761V) {
  // display 52.90 V → Vref 3.030761 V
  float v = 3.030761f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x44, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x44, 0x04, 0xFD, 0xF7, 0x41, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, VoltageCalibration3p030188V) {
  // display 52.89 V → Vref 3.030188 V
  float v = 3.030188f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x44, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x44, 0x04, 0x9A, 0xEE, 0x41, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

// ── Balancing start voltage write frames ─────────────────────────────────────

TEST(JkBmsJk04B5A24SWriteFrameTest, BalancingStartVoltage1p00V) {
  float v = 1.00f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x58, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x58, 0x04, 0x00, 0x00, 0x80, 0x3F,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalancingStartVoltage1p10V) {
  float v = 1.10f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x58, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x58, 0x04, 0xCD, 0xCC, 0x8C, 0x3F,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkBmsJk04B5A24SWriteFrameTest, BalancingStartVoltage1p11V) {
  float v = 1.11f;
  uint32_t bits;
  memcpy(&bits, &v, sizeof(bits));
  auto frame = JkBmsBle::build_frame(0x58, bits, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x58, 0x04, 0x7B, 0x14, 0x8E, 0x3F,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

// ── JK04 payload encoder ─────────────────────────────────────────────────────

TEST(JkNumberJk04PayloadTest, Float32PowerOffVoltage) {
  // len=4 → IEEE 754, factor ignored
  uint32_t payload = JkBmsBle::encode_jk04_payload(3.2f, 4, 1000.0f);
  auto frame = JkBmsBle::build_frame(0x20, payload, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x20, 0x04, 0xCD, 0xCC, 0x4C, 0x40,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC3};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkNumberJk04PayloadTest, Float32BalanceTrigger8mV) {
  uint32_t payload = JkBmsBle::encode_jk04_payload(0.008f, 4, 1000.0f);
  auto frame = JkBmsBle::build_frame(0x64, payload, 4);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x64, 0x04, 0x6F, 0x12, 0x03, 0x3C,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkNumberJk04PayloadTest, Uint16CellCount16) {
  // len=2 → integer encoding, factor=1.0
  uint32_t payload = JkBmsBle::encode_jk04_payload(16.0f, 2, 1.0f);
  EXPECT_EQ(payload, 16u);
  auto frame = JkBmsBle::build_frame(0x1C, payload, 2);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x1C, 0x02, 0x10, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

TEST(JkNumberJk04PayloadTest, Uint16CellCount15) {
  uint32_t payload = JkBmsBle::encode_jk04_payload(15.0f, 2, 1.0f);
  EXPECT_EQ(payload, 15u);
  auto frame = JkBmsBle::build_frame(0x1C, payload, 2);
  std::vector<uint8_t> expected = {0xAA, 0x55, 0x90, 0xEB, 0x1C, 0x02, 0x0F, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA7};
  EXPECT_EQ(std::vector<uint8_t>(frame.begin(), frame.end()), expected);
}

}  // namespace esphome::jk_bms_ble::testing
