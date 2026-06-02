#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// Captures recorded from a JK02_32S BMS (2026-05-27):
//
//   Button Trg. Emerg.  ON:  AA 55 90 EB 31 04 01 00 00 00 60 6B 59 D5 59 E1 6C C1 B9 C9
//   DRY ARM Intermittent ON: AA 55 90 EB 32 04 01 00 00 00 55 63 42 44 71 55 0B BA 97 11
//   Discharge OCP 2     OFF: AA 55 90 EB 33 04 00 00 00 00 E6 68 67 FE 71 5B 50 76 55 4B
//
// Additional captures recorded from a JK02_32S BMS (2026-05-31):
//
//   Discharge OCP 2     OFF: AA 55 90 EB 33 04 00 00 00 00 A7 F9 96 FC 1E E0 66 2C 0E 81
//   GPS Locked Charging  ON: AA 55 90 EB 35 04 01 00 00 00 B4 B4 59 CF CE 23 3D 27 DF 78
//   GPS Locked Discharging ON:AA 55 90 EB 36 04 01 00 00 00 B5 C4 8C DD 6C 11 32 3D E2 65
//
// Note: bytes 10–18 in the captures are stale BLE-TX-buffer data from the capturing
// client and are not part of the JK protocol payload.  build_frame() zero-initialises
// those bytes, so the CRC in the captures (C9 / 11 / 4B / 81 / 78 / 65) differs from
// what build_frame() produces (B0 / B1 / B1 / B1 / B4 / B5).  Register (byte 4),
// length (byte 5), and value (bytes 6–9) match exactly.

// ── Header / trailer ─────────────────────────────────────────────────────────

TEST(JkBmsWriteCommandTest, HeaderIsFixed) {
  auto f = JkBmsBle::build_frame(0x31, 0x00000001, 0x04);

  EXPECT_EQ(f[0], 0xAA);
  EXPECT_EQ(f[1], 0x55);
  EXPECT_EQ(f[2], 0x90);
  EXPECT_EQ(f[3], 0xEB);
}

TEST(JkBmsWriteCommandTest, PaddingBytesAreZero) {
  auto f = JkBmsBle::build_frame(0x31, 0x00000001, 0x04);

  for (int i = 10; i <= 18; i++)
    EXPECT_EQ(f[i], 0x00) << "padding byte [" << i << "] != 0";
}

TEST(JkBmsWriteCommandTest, CrcMatchesByteSum) {
  auto f = JkBmsBle::build_frame(0x31, 0x00000001, 0x04);

  uint8_t expected = 0;
  for (int i = 0; i < 19; i++)
    expected += f[i];

  EXPECT_EQ(f[19], expected);
}

// ── Emergency Button Trigger (register 0x31) ─────────────────────────────────

TEST(JkBmsWriteCommandTest, EmergencyButtonTriggerOn) {
  // Captured: AA 55 90 EB 31 04 01 00 00 00 ...
  auto f = JkBmsBle::build_frame(0x31, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x31);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x01);  // value: ON (little-endian)
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB0);  // CRC = sum(AA+55+90+EB+31+04+01) & 0xFF
}

TEST(JkBmsWriteCommandTest, EmergencyButtonTriggerOff) {
  auto f = JkBmsBle::build_frame(0x31, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x31);
  EXPECT_EQ(f[5], 0x04);
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xAF);
}

TEST(JkBmsWriteCommandTest, EmergencyButtonTriggerFullFrameOn) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x31, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB0,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x31, 0x00000001, 0x04), expected);
}

// ── Dry Contact Alarm Intermittent (register 0x32) ───────────────────────────

TEST(JkBmsWriteCommandTest, DryContactAlarmIntermittentOn) {
  // Captured: AA 55 90 EB 32 04 01 00 00 00 ...
  auto f = JkBmsBle::build_frame(0x32, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x32);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x01);  // value: ON
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB1);
}

TEST(JkBmsWriteCommandTest, DryContactAlarmIntermittentOff) {
  auto f = JkBmsBle::build_frame(0x32, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x32);
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[19], 0xB0);
}

TEST(JkBmsWriteCommandTest, DryContactAlarmIntermittentFullFrameOn) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x32, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB1,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x32, 0x00000001, 0x04), expected);
}

// ── Discharge Overcurrent Protection 2 (register 0x33) ───────────────────────

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection2Off) {
  // Captured: AA 55 90 EB 33 04 00 00 00 00 ...  (two independent captures)
  auto f = JkBmsBle::build_frame(0x33, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x33);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB1);
}

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection2On) {
  auto f = JkBmsBle::build_frame(0x33, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x33);
  EXPECT_EQ(f[6], 0x01);  // value: ON
  EXPECT_EQ(f[19], 0xB2);
}

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection2FullFrameOff) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x33, 0x04, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB1,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x33, 0x00000000, 0x04), expected);
}

// ── Discharge Overcurrent Protection 3 (register 0x34) ───────────────────────

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection3Off) {
  auto f = JkBmsBle::build_frame(0x34, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x34);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB2);
}

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection3On) {
  auto f = JkBmsBle::build_frame(0x34, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x34);
  EXPECT_EQ(f[6], 0x01);  // value: ON
  EXPECT_EQ(f[19], 0xB3);
}

TEST(JkBmsWriteCommandTest, DischargeOvercurrentProtection3FullFrameOn) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x34, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB3,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x34, 0x00000001, 0x04), expected);
}

// ── GPS Locked Charging (register 0x35) ──────────────────────────────────────

TEST(JkBmsWriteCommandTest, GpsLockedChargingOn) {
  // Captured: AA 55 90 EB 35 04 01 00 00 00 B4 B4 59 CF CE 23 3D 27 DF 78
  auto f = JkBmsBle::build_frame(0x35, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x35);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x01);  // value: ON
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB4);
}

TEST(JkBmsWriteCommandTest, GpsLockedChargingOff) {
  auto f = JkBmsBle::build_frame(0x35, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x35);
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[19], 0xB3);
}

TEST(JkBmsWriteCommandTest, GpsLockedChargingFullFrameOn) {
  // Captured register/length/value match (bytes 10–18 differ, see file header):
  //   AA 55 90 EB 35 04 01 00 00 00 B4 B4 59 CF CE 23 3D 27 DF 78
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x35, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB4,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x35, 0x00000001, 0x04), expected);
}

// ── GPS Locked Discharging (register 0x36) ───────────────────────────────────

TEST(JkBmsWriteCommandTest, GpsLockedDischargingOn) {
  // Captured: AA 55 90 EB 36 04 01 00 00 00 B5 C4 8C DD 6C 11 32 3D E2 65
  auto f = JkBmsBle::build_frame(0x36, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x36);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x01);  // value: ON
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xB5);
}

TEST(JkBmsWriteCommandTest, GpsLockedDischargingOff) {
  auto f = JkBmsBle::build_frame(0x36, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x36);
  EXPECT_EQ(f[6], 0x00);  // value: OFF
  EXPECT_EQ(f[19], 0xB4);
}

TEST(JkBmsWriteCommandTest, GpsLockedDischargingFullFrameOn) {
  // Captured register/length/value match (bytes 10–18 differ, see file header):
  //   AA 55 90 EB 36 04 01 00 00 00 B5 C4 8C DD 6C 11 32 3D E2 65
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x36, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xB5,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x36, 0x00000001, 0x04), expected);
}

// ── Protocol Mode / CAN vs RS485 (register 0x2A) ─────────────────────────────
//
// Captures recorded from a JK02_32S BMS (issue #283):
//
//   Protocol Mode CAN:   aa:55:90:eb:2a:04:00:00:00:00:11:9d:1b:c4:c1:a5:85:91:a9:5a
//   Protocol Mode RS485: aa:55:90:eb:2a:04:01:00:00:00:c2:a9:65:de:02:7f:42:c3:c3:a0
//
// Length byte is 0x04 (uint32 payload).  Bytes 10–18 are stale BLE-TX-buffer data;
// build_frame() zero-initialises them, so the CRC here (A8/A9) differs from the
// captured CRC (5A/A0).  Register (byte 4), length (byte 5), and value (bytes 6–9)
// match the captures exactly.
//
// CRC = sum8(bytes[0..18]) = (AA+55+90+EB+2A+04+value) & 0xFF
//                           = (0x2A8 + value) & 0xFF → 0xA8 (CAN) / 0xA9 (RS485)

TEST(JkBmsWriteCommandTest, MultiplexedPortModeCan) {
  // Captured: aa 55 90 eb 2a 04 00 00 00 00 ...
  auto f = JkBmsBle::build_frame(0x2A, 0x00000000, 0x04);

  EXPECT_EQ(f[4], 0x2A);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x00);  // value: CAN (index 0, little-endian)
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xA8);  // CRC = (AA+55+90+EB+2A+04) & FF
}

TEST(JkBmsWriteCommandTest, MultiplexedPortModeRs485) {
  // Captured: aa 55 90 eb 2a 04 01 00 00 00 ...
  auto f = JkBmsBle::build_frame(0x2A, 0x00000001, 0x04);

  EXPECT_EQ(f[4], 0x2A);  // register
  EXPECT_EQ(f[5], 0x04);  // length
  EXPECT_EQ(f[6], 0x01);  // value: RS485 (index 1)
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xA9);
}

TEST(JkBmsWriteCommandTest, MultiplexedPortModeCanFullFrame) {
  // Captured register/length/value match (bytes 10–18 differ, see file header):
  //   aa 55 90 eb 2a 04 00 00 00 00 11 9d 1b c4 c1 a5 85 91 a9 5a
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x2A, 0x04, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xA8,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x2A, 0x00000000, 0x04), expected);
}

TEST(JkBmsWriteCommandTest, MultiplexedPortModeRs485FullFrame) {
  // Captured register/length/value match (bytes 10–18 differ, see file header):
  //   aa 55 90 eb 2a 04 01 00 00 00 c2 a9 65 de 02 7f 42 c3 c3 a0
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x2A, 0x04, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xA9,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x2A, 0x00000001, 0x04), expected);
}

// ── Smart Sleep Delay (register 0x39, 1-byte write) ──────────────────────────
//
// Capture recorded from a JK02_32S BMS (issue #421), value = 23 hours:
//
//   AA.55.90.EB.39.01.17.BB.2E.15.63.0E.3C.DC.A4.3A.7A.06.20.D0
//
// Length byte is 0x01 (1-byte payload).  Bytes 7–18 are stale BLE-TX-buffer
// data; build_frame() zero-initialises them — only register (byte 4), length
// (byte 5), and value (byte 6) match the capture exactly.
//
// CRC = sum8(bytes[0..18]) = (AA+55+90+EB+39+01+value) & 0xFF
//                           = (0x274 + value) & 0xFF → 0xCB (23 h)

TEST(JkBmsWriteCommandTest, SmartSleepDelay23h) {
  // Captured: AA 55 90 EB 39 01 17 ...
  auto f = JkBmsBle::build_frame(0x39, 0x00000017, 0x01);

  EXPECT_EQ(f[4], 0x39);  // register
  EXPECT_EQ(f[5], 0x01);  // length: 1 byte
  EXPECT_EQ(f[6], 0x17);  // value: 23 hours
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0xCB);  // CRC = (AA+55+90+EB+39+01+17) & FF
}

TEST(JkBmsWriteCommandTest, SmartSleepDelay1h) {
  auto f = JkBmsBle::build_frame(0x39, 0x00000001, 0x01);

  EXPECT_EQ(f[4], 0x39);
  EXPECT_EQ(f[5], 0x01);
  EXPECT_EQ(f[6], 0x01);  // value: 1 hour (minimum)
  EXPECT_EQ(f[19], 0xB5);
}

TEST(JkBmsWriteCommandTest, SmartSleepDelay100h) {
  auto f = JkBmsBle::build_frame(0x39, 0x00000064, 0x01);

  EXPECT_EQ(f[4], 0x39);
  EXPECT_EQ(f[5], 0x01);
  EXPECT_EQ(f[6], 0x64);  // value: 100 hours (maximum)
  EXPECT_EQ(f[19], 0x18);
}

TEST(JkBmsWriteCommandTest, SmartSleepDelay23hFullFrame) {
  // Captured register/length/value match (bytes 7–18 differ, see file header):
  //   AA 55 90 EB 39 01 17 BB 2E 15 63 0E 3C DC A4 3A 7A 06 20 D0
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0x39, 0x01, 0x17, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xCB,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0x39, 0x00000017, 0x01), expected);
}

// ── Emergency Duration (register 0xB5, 1-byte write) ─────────────────────────
//
// Captures recorded from a JK02_32S BMS:
//
//   Emergency 31 min: AA.55.90.EB.B5.01.1F.B5.C4.8C.DD.6C.11.32.3D.E2.B4.B4.59.C0
//   Emergency 32 min: AA.55.90.EB.B5.01.20.CF.CE.23.3D.27.DF.A7.F9.96.FC.1E.E0.83
//   Emergency 33 min: AA.55.90.EB.B5.01.21.66.2C.0E.7B.8C.CA.30.42.8F.BC.9F.7B.99
//
// Length byte is 0x01 (1-byte payload).  Bytes 7–18 are stale BLE-TX-buffer
// data; build_frame() zero-initialises them — only register (byte 4), length
// (byte 5), and value (byte 6) match the captures exactly.
//
// CRC = sum8(bytes[0..18]) = (AA+55+90+EB+B5+01+value) & 0xFF
//                           = (0x330 + value) & 0xFF

TEST(JkBmsWriteCommandTest, EmergencyDuration31min) {
  // Captured: AA 55 90 EB B5 01 1F ...
  auto f = JkBmsBle::build_frame(0xB5, 0x0000001F, 0x01);

  EXPECT_EQ(f[4], 0xB5);  // register
  EXPECT_EQ(f[5], 0x01);  // length: 1 byte
  EXPECT_EQ(f[6], 0x1F);  // value: 31 minutes
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[19], 0x4F);  // CRC = (AA+55+90+EB+B5+01+1F) & FF
}

TEST(JkBmsWriteCommandTest, EmergencyDuration32min) {
  // Captured: AA 55 90 EB B5 01 20 ...
  auto f = JkBmsBle::build_frame(0xB5, 0x00000020, 0x01);

  EXPECT_EQ(f[4], 0xB5);
  EXPECT_EQ(f[5], 0x01);
  EXPECT_EQ(f[6], 0x20);  // value: 32 minutes
  EXPECT_EQ(f[19], 0x50);
}

TEST(JkBmsWriteCommandTest, EmergencyDuration33min) {
  // Captured: AA 55 90 EB B5 01 21 ...
  auto f = JkBmsBle::build_frame(0xB5, 0x00000021, 0x01);

  EXPECT_EQ(f[4], 0xB5);
  EXPECT_EQ(f[5], 0x01);
  EXPECT_EQ(f[6], 0x21);  // value: 33 minutes
  EXPECT_EQ(f[19], 0x51);
}

TEST(JkBmsWriteCommandTest, EmergencyDuration31minFullFrame) {
  // Captured register/length/value match (bytes 7–18 differ, see file header):
  //   AA 55 90 EB B5 01 1F B5 C4 8C DD 6C 11 32 3D E2 B4 B4 59 C0
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xB5, 0x01, 0x1F, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x4F,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xB5, 0x0000001F, 0x01), expected);
}

// ── Register uniqueness ───────────────────────────────────────────────────────

TEST(JkBmsWriteCommandTest, RegistersAreDistinct) {
  auto f2a = JkBmsBle::build_frame(0x2A, 0x00000001, 0x04);
  auto f31 = JkBmsBle::build_frame(0x31, 0x00000001, 0x04);
  auto f32 = JkBmsBle::build_frame(0x32, 0x00000001, 0x04);
  auto f33 = JkBmsBle::build_frame(0x33, 0x00000001, 0x04);
  auto f34 = JkBmsBle::build_frame(0x34, 0x00000001, 0x04);
  auto f35 = JkBmsBle::build_frame(0x35, 0x00000001, 0x04);
  auto f36 = JkBmsBle::build_frame(0x36, 0x00000001, 0x04);

  EXPECT_NE(f2a, f31);
  EXPECT_NE(f2a, f32);
  EXPECT_NE(f31, f32);
  EXPECT_NE(f31, f33);
  EXPECT_NE(f32, f33);
  EXPECT_NE(f33, f34);
  EXPECT_NE(f34, f35);
  EXPECT_NE(f35, f36);
}

}  // namespace esphome::jk_bms_ble::testing
