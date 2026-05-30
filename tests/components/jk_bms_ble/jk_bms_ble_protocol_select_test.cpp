#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// Captures recorded from a JK02_32S BMS (2026-05-27), register A5 = UART1, A6 = CAN.
// Frame format: AA 55 90 EB [reg] 02 [idx_lo] [idx_hi] [BLE-buf artifacts...] [CRC]
//
// Length byte is 0x02 (uint16 payload), not 0x04 as used by switch commands.
// Bytes 8-18 in captures are stale BLE-TX-buffer data from the capturing client;
// build_frame() zero-initialises them — only register (byte 4), length (byte 5),
// and value (bytes 6-7) match the captures exactly.
//
//   UART1 proto 0 (4G-GPS):  AA 55 90 EB A5 02 00 00 ...
//   UART1 proto 1 (RS485):   AA 55 90 EB A5 02 01 00 ...
//   UART1 proto 2 (NIU):     AA 55 90 EB A5 02 02 00 ...
//   CAN   proto 0 (JK CAN):  AA 55 90 EB A6 02 00 00 ...
//   CAN   proto 1 (Deye):    AA 55 90 EB A6 02 01 00 ...
//   CAN   proto 2 (Pylon):   AA 55 90 EB A6 02 02 00 ...

// ── Shared structure ──────────────────────────────────────────────────────────

TEST(JkBmsProtocolSelectTest, LengthByteIsTwo) {
  EXPECT_EQ(JkBmsBle::build_frame(0xA5, 0, 0x02)[5], 0x02);
  EXPECT_EQ(JkBmsBle::build_frame(0xA6, 0, 0x02)[5], 0x02);
}

TEST(JkBmsProtocolSelectTest, PaddingBytesAreZero) {
  auto f = JkBmsBle::build_frame(0xA5, 1, 0x02);
  for (int i = 8; i <= 18; i++)
    EXPECT_EQ(f[i], 0x00) << "padding byte [" << i << "] != 0";
}

TEST(JkBmsProtocolSelectTest, CrcMatchesByteSum) {
  auto f = JkBmsBle::build_frame(0xA5, 1, 0x02);
  uint8_t expected = 0;
  for (int i = 0; i < 19; i++)
    expected += f[i];
  EXPECT_EQ(f[19], expected);
}

// ── UART1 protocol selection (register 0xA5) ──────────────────────────────────

TEST(JkBmsProtocolSelectTest, Uart1Proto0Is4gGps) {
  // Captured: AA 55 90 EB A5 02 00 00 ...
  auto f = JkBmsBle::build_frame(0xA5, 0x00000000, 0x02);

  EXPECT_EQ(f[4], 0xA5);  // register
  EXPECT_EQ(f[5], 0x02);  // length
  EXPECT_EQ(f[6], 0x00);  // protocol index 0 (little-endian)
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x21);  // CRC
}

TEST(JkBmsProtocolSelectTest, Uart1Proto1IsRs485Modbus) {
  // Captured: AA 55 90 EB A5 02 01 00 ...
  auto f = JkBmsBle::build_frame(0xA5, 0x00000001, 0x02);

  EXPECT_EQ(f[4], 0xA5);
  EXPECT_EQ(f[6], 0x01);  // protocol index 1
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x22);
}

TEST(JkBmsProtocolSelectTest, Uart1Proto2IsNiu) {
  // Captured: AA 55 90 EB A5 02 02 00 ...
  auto f = JkBmsBle::build_frame(0xA5, 0x00000002, 0x02);

  EXPECT_EQ(f[4], 0xA5);
  EXPECT_EQ(f[6], 0x02);  // protocol index 2
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x23);
}

TEST(JkBmsProtocolSelectTest, Uart1Proto0FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA5, 0x02, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x21,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA5, 0x00000000, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, Uart1Proto1FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA5, 0x02, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x22,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA5, 0x00000001, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, Uart1Proto2FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA5, 0x02, 0x02, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x23,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA5, 0x00000002, 0x02), expected);
}

// ── CAN protocol selection (register 0xA6) ────────────────────────────────────

TEST(JkBmsProtocolSelectTest, CanProto0IsJkCan) {
  // Captured: AA 55 90 EB A6 02 00 00 ...
  auto f = JkBmsBle::build_frame(0xA6, 0x00000000, 0x02);

  EXPECT_EQ(f[4], 0xA6);  // register
  EXPECT_EQ(f[5], 0x02);  // length
  EXPECT_EQ(f[6], 0x00);  // protocol index 0
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x22);
}

TEST(JkBmsProtocolSelectTest, CanProto1IsDeye) {
  // Captured: AA 55 90 EB A6 02 01 00 ...
  auto f = JkBmsBle::build_frame(0xA6, 0x00000001, 0x02);

  EXPECT_EQ(f[4], 0xA6);
  EXPECT_EQ(f[6], 0x01);  // protocol index 1
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x23);
}

TEST(JkBmsProtocolSelectTest, CanProto2IsPylon) {
  // Captured: AA 55 90 EB A6 02 02 00 ...
  auto f = JkBmsBle::build_frame(0xA6, 0x00000002, 0x02);

  EXPECT_EQ(f[4], 0xA6);
  EXPECT_EQ(f[6], 0x02);  // protocol index 2
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x24);
}

TEST(JkBmsProtocolSelectTest, CanProto0FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA6, 0x02, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x22,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA6, 0x00000000, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, CanProto1FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA6, 0x02, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x23,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA6, 0x00000001, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, CanProto2FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA6, 0x02, 0x02, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x24,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA6, 0x00000002, 0x02), expected);
}

// ── LCD Buzzer Trigger (register 0xA9) ───────────────────────────────────────
//
//   0 (Off):                 AA 55 90 EB A9 02 00 00 ...
//   1 (Low SoC):             AA 55 90 EB A9 02 01 00 ...
//   2 (Battery Over Voltage):AA 55 90 EB A9 02 02 00 ...
//   3 (Battery Under Voltage):AA 55 90 EB A9 02 03 00 ...

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger0IsOff) {
  // Captured: AA 55 90 EB A9 02 00 00 ...
  auto f = JkBmsBle::build_frame(0xA9, 0x00000000, 0x02);

  EXPECT_EQ(f[4], 0xA9);  // register
  EXPECT_EQ(f[5], 0x02);  // length
  EXPECT_EQ(f[6], 0x00);  // trigger index 0 (Off)
  EXPECT_EQ(f[7], 0x00);
  EXPECT_EQ(f[19], 0x25);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger1IsLowSoc) {
  // Captured: AA 55 90 EB A9 02 01 00 ...
  auto f = JkBmsBle::build_frame(0xA9, 0x00000001, 0x02);

  EXPECT_EQ(f[4], 0xA9);
  EXPECT_EQ(f[6], 0x01);  // trigger index 1 (Low SoC)
  EXPECT_EQ(f[19], 0x26);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger2IsBatteryOverVoltage) {
  // Captured: AA 55 90 EB A9 02 02 00 ...
  auto f = JkBmsBle::build_frame(0xA9, 0x00000002, 0x02);

  EXPECT_EQ(f[4], 0xA9);
  EXPECT_EQ(f[6], 0x02);  // trigger index 2 (Battery Over Voltage)
  EXPECT_EQ(f[19], 0x27);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger3IsBatteryUnderVoltage) {
  // Captured: AA 55 90 EB A9 02 03 00 ...
  auto f = JkBmsBle::build_frame(0xA9, 0x00000003, 0x02);

  EXPECT_EQ(f[4], 0xA9);
  EXPECT_EQ(f[6], 0x03);  // trigger index 3 (Battery Under Voltage)
  EXPECT_EQ(f[19], 0x28);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger0FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA9, 0x02, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x25,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA9, 0x00000000, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger1FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA9, 0x02, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x26,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA9, 0x00000001, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger2FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA9, 0x02, 0x02, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x27,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA9, 0x00000002, 0x02), expected);
}

TEST(JkBmsProtocolSelectTest, LcdBuzzerTrigger3FullFrame) {
  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x90, 0xEB, 0xA9, 0x02, 0x03, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x28,
  };
  // clang-format on
  EXPECT_EQ(JkBmsBle::build_frame(0xA9, 0x00000003, 0x02), expected);
}

// ── Register distinctness ─────────────────────────────────────────────────────

TEST(JkBmsProtocolSelectTest, Uart1AndCanRegistersDistinct) {
  auto uart1 = JkBmsBle::build_frame(0xA5, 0x00000001, 0x02);
  auto can = JkBmsBle::build_frame(0xA6, 0x00000001, 0x02);
  EXPECT_NE(uart1, can);
}

TEST(JkBmsProtocolSelectTest, AllThreeRegistersDistinct) {
  auto uart1 = JkBmsBle::build_frame(0xA5, 0x00000001, 0x02);
  auto can = JkBmsBle::build_frame(0xA6, 0x00000001, 0x02);
  auto buzzer = JkBmsBle::build_frame(0xA9, 0x00000001, 0x02);
  EXPECT_NE(uart1, can);
  EXPECT_NE(uart1, buzzer);
  EXPECT_NE(can, buzzer);
}

}  // namespace esphome::jk_bms_ble::testing
