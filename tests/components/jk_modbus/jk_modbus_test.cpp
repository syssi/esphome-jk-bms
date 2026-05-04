#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_modbus::testing {

// ── Valid frame dispatch ───────────────────────────────────────────────────────

TEST(JkModbusParseTest, ValidFrameDispatchesToDevice) {
  TestableJkModbus bus;
  MockDevice dev;
  dev.set_address(0x4E);  // dispatch address = raw[0] = 0x4E (header byte)
  bus.register_device(&dev);

  auto frame = make_frame(0x06, {0xAA, 0xBB, 0xCC});
  for (uint8_t b : frame)
    bus.parse_byte(b);

  if (!dev.captured.has_value()) {
    ADD_FAILURE() << "no frame captured";
    return;
  }
  EXPECT_EQ(dev.captured->function, 0x06);
  EXPECT_EQ(dev.captured->data[0], 0xAA);
  EXPECT_EQ(dev.captured->data[1], 0xBB);
  EXPECT_EQ(dev.captured->data[2], 0xCC);
}

// ── Header validation ─────────────────────────────────────────────────────────

TEST(JkModbusParseTest, InvalidFirstByteReturnsFalse) {
  TestableJkModbus bus;
  EXPECT_FALSE(bus.parse_byte(0xAA));  // not 0x4E
}

TEST(JkModbusParseTest, ValidFirstByteReturnsTrue) {
  TestableJkModbus bus;
  EXPECT_TRUE(bus.parse_byte(0x4E));
}

TEST(JkModbusParseTest, InvalidSecondByteReturnsFalse) {
  TestableJkModbus bus;
  bus.parse_byte(0x4E);
  EXPECT_FALSE(bus.parse_byte(0x58));  // not 0x57
}

// ── CRC validation ────────────────────────────────────────────────────────────

TEST(JkModbusParseTest, CrcMismatchFrameRejected) {
  TestableJkModbus bus;
  MockDevice dev;
  dev.set_address(0x4E);
  bus.register_device(&dev);

  auto frame = make_frame(0x06);
  frame.back() ^= 0xFF;  // corrupt CRC low byte
  for (uint8_t b : frame)
    bus.parse_byte(b);

  EXPECT_FALSE(dev.captured.has_value());
}

// ── Partial frame accumulation ────────────────────────────────────────────────

TEST(JkModbusParseTest, PartialFrameNoDispatchUntilComplete) {
  TestableJkModbus bus;
  MockDevice dev;
  dev.set_address(0x4E);
  bus.register_device(&dev);

  auto frame = make_frame(0x06);
  for (size_t i = 0; i < frame.size() - 1; i++)
    bus.parse_byte(frame[i]);
  EXPECT_FALSE(dev.captured.has_value());

  bus.parse_byte(frame.back());  // last CRC byte → triggers dispatch
  EXPECT_TRUE(dev.captured.has_value());
}

// ── No device registered — no crash ──────────────────────────────────────────

TEST(JkModbusParseTest, NoDeviceRegisteredDoesNotCrash) {
  TestableJkModbus bus;
  auto frame = make_frame(0x06);
  for (uint8_t b : frame)
    bus.parse_byte(b);
}

}  // namespace esphome::jk_modbus::testing
