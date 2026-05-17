#include <gtest/gtest.h>
#include <cstring>
#include "common.h"

namespace esphome::heltec_balancer_ble::testing {

// Frame layout:
//   [0]     SOF1        0xAA
//   [1]     SOF2        0x55
//   [2]     device addr 0x11
//   [3]     function    0x00=write / 0x01=read
//   [4]     command
//   [5]     register
//   [6]     length low  0x14
//   [7]     length high 0x00
//   [8-11]  value (little-endian uint32)
//   [12-17] padding     0x00
//   [18]    CRC         sum(frame[0..17]) & 0xFF
//   [19]    EOF         0xFF

// ── Frame header / trailer ────────────────────────────────────────────────────

TEST(WriteCommandTest, HeaderAndTrailer) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x01, 0x04, 0x00, 0x00000000);

  EXPECT_EQ(f[0], 0xAA);   // SOF1
  EXPECT_EQ(f[1], 0x55);   // SOF2
  EXPECT_EQ(f[2], 0x11);   // device address
  EXPECT_EQ(f[6], 0x14);   // data length low
  EXPECT_EQ(f[7], 0x00);   // data length high
  EXPECT_EQ(f[19], 0xFF);  // EOF
}

TEST(WriteCommandTest, PaddingBytesAreZero) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x00, 0x05, 0x01, 0x12345678);

  for (int i = 12; i <= 17; i++)
    EXPECT_EQ(f[i], 0x00) << "padding byte [" << i << "] != 0";
}

// ── CRC ───────────────────────────────────────────────────────────────────────

TEST(WriteCommandTest, CrcIsCorrect) {
  TestableHeltecBalancerBle bms;

  // Read settings: 0xAA+0x55+0x11+0x01+0x04+0x14 = 0x29
  auto f = bms.build_command_frame_(0x01, 0x04, 0x00, 0x00000000);
  EXPECT_EQ(f[18], 0x29);

  // Write integer (cell count=16, reg=0x00): sum includes 0x10 payload byte
  auto g = bms.build_command_frame_(0x00, 0x05, 0x00, 0x00000010);
  EXPECT_EQ(g[18], 0x39);
}

TEST(WriteCommandTest, CrcMatchesByteSum) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x00, 0x05, 0x02, 0x40000000);  // 2.0f

  uint8_t expected_crc = 0;
  for (int i = 0; i < 18; i++)
    expected_crc += f[i];

  EXPECT_EQ(f[18], expected_crc);
}

// ── Integer payload ───────────────────────────────────────────────────────────

TEST(WriteCommandTest, IntegerPayloadLittleEndian) {
  TestableHeltecBalancerBle bms;
  // cell count = 16 (0x00000010)
  auto f = bms.build_command_frame_(0x00, 0x05, 0x00, 0x00000010);

  EXPECT_EQ(f[3], 0x00);  // function write
  EXPECT_EQ(f[4], 0x05);  // command
  EXPECT_EQ(f[5], 0x00);  // register
  EXPECT_EQ(f[8], 0x10);  // value byte 0
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[10], 0x00);
  EXPECT_EQ(f[11], 0x00);
}

TEST(WriteCommandTest, IntegerPayloadFullFrame) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x00, 0x05, 0x00, 0x00000010);

  // clang-format off
  const std::array<uint8_t, 20> expected = {
      0xAA, 0x55, 0x11, 0x00, 0x05, 0x00, 0x14, 0x00,
      0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x39, 0xFF,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

// ── Float payload (IEEE 754 little-endian) ────────────────────────────────────

TEST(WriteCommandTest, FloatPayloadIeee754) {
  TestableHeltecBalancerBle bms;

  // 0.004f → IEEE 754 = 0x3B83126F → bytes: 0x6F 0x12 0x83 0x3B
  uint32_t bits;
  float v = 0.004f;
  std::memcpy(&bits, &v, 4);
  auto f = bms.build_command_frame_(0x00, 0x05, 0x01, bits);

  EXPECT_EQ(f[5], 0x01);   // register
  EXPECT_EQ(f[8], 0x6F);   // IEEE 754 byte 0
  EXPECT_EQ(f[9], 0x12);   // IEEE 754 byte 1
  EXPECT_EQ(f[10], 0x83);  // IEEE 754 byte 2
  EXPECT_EQ(f[11], 0x3B);  // IEEE 754 byte 3
  EXPECT_EQ(f[18], 0x69);  // CRC
}

TEST(WriteCommandTest, FloatPayload2V) {
  TestableHeltecBalancerBle bms;

  // 2.0f → IEEE 754 = 0x40000000 → bytes: 0x00 0x00 0x00 0x40
  uint32_t bits;
  float v = 2.0f;
  std::memcpy(&bits, &v, 4);
  auto f = bms.build_command_frame_(0x00, 0x05, 0x02, bits);

  EXPECT_EQ(f[8], 0x00);
  EXPECT_EQ(f[9], 0x00);
  EXPECT_EQ(f[10], 0x00);
  EXPECT_EQ(f[11], 0x40);
  EXPECT_EQ(f[18], 0x6B);  // CRC
}

// ── Function byte ─────────────────────────────────────────────────────────────

TEST(WriteCommandTest, FunctionByteWrite) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x00, 0x05, 0x00, 0x00000000);
  EXPECT_EQ(f[3], 0x00);
}

TEST(WriteCommandTest, FunctionByteRead) {
  TestableHeltecBalancerBle bms;
  auto f = bms.build_command_frame_(0x01, 0x04, 0x00, 0x00000000);
  EXPECT_EQ(f[3], 0x01);
}

}  // namespace esphome::heltec_balancer_ble::testing
