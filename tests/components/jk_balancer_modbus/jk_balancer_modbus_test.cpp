#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_balancer_modbus::testing {

// ── Valid frame dispatch ───────────────────────────────────────────────────────

TEST(JkBalancerModbusParseTest, ValidFrameDispatchesToDevice) {
  TestableJkBalancerModbus bus;
  MockDevice dev;
  dev.set_address(0x01);
  bus.register_device(&dev);

  auto frame = make_frame(0x01, 0xFF, {0xAA, 0xBB, 0xCC});
  for (uint8_t b : frame)
    bus.parse_byte(b);

  if (!dev.captured.has_value()) {
    ADD_FAILURE() << "no frame captured";
    return;
  }
  EXPECT_EQ(dev.captured->function, 0xFF);
  EXPECT_EQ(dev.captured->data.size(), 74u);  // full frame dispatched
  EXPECT_EQ(dev.captured->data[0], 0xEB);
  EXPECT_EQ(dev.captured->data[4], 0xAA);  // payload byte 0
}

// ── Header validation ─────────────────────────────────────────────────────────

TEST(JkBalancerModbusParseTest, InvalidFirstByteReturnsFalse) {
  TestableJkBalancerModbus bus;
  EXPECT_FALSE(bus.parse_byte(0xAA));  // not 0xEB
}

TEST(JkBalancerModbusParseTest, ValidFirstByteReturnsTrue) {
  TestableJkBalancerModbus bus;
  EXPECT_TRUE(bus.parse_byte(0xEB));
}

TEST(JkBalancerModbusParseTest, InvalidSecondByteReturnsFalse) {
  TestableJkBalancerModbus bus;
  bus.parse_byte(0xEB);
  EXPECT_FALSE(bus.parse_byte(0x91));  // not 0x90
}

// ── CRC validation ────────────────────────────────────────────────────────────

TEST(JkBalancerModbusParseTest, CrcMismatchFrameRejected) {
  TestableJkBalancerModbus bus;
  MockDevice dev;
  dev.set_address(0x01);
  bus.register_device(&dev);

  auto frame = make_frame(0x01, 0xFF);
  frame[73] ^= 0xFF;  // corrupt checksum
  for (uint8_t b : frame)
    bus.parse_byte(b);

  EXPECT_FALSE(dev.captured.has_value());
}

// ── Unknown address ───────────────────────────────────────────────────────────

TEST(JkBalancerModbusParseTest, UnknownAddressNoDispatch) {
  TestableJkBalancerModbus bus;
  MockDevice dev;
  dev.set_address(0x02);  // registered for 0x02
  bus.register_device(&dev);

  auto frame = make_frame(0x01, 0xFF);  // sent from address 0x01
  for (uint8_t b : frame)
    bus.parse_byte(b);

  EXPECT_FALSE(dev.captured.has_value());
}

// ── Partial frame accumulation ────────────────────────────────────────────────

TEST(JkBalancerModbusParseTest, PartialFrameNoDispatchUntilComplete) {
  TestableJkBalancerModbus bus;
  MockDevice dev;
  dev.set_address(0x01);
  bus.register_device(&dev);

  auto frame = make_frame(0x01, 0xFF);
  for (size_t i = 0; i < 73; i++)
    bus.parse_byte(frame[i]);
  EXPECT_FALSE(dev.captured.has_value());

  bus.parse_byte(frame[73]);  // checksum byte → triggers dispatch
  EXPECT_TRUE(dev.captured.has_value());
}

// ── No device registered — no crash ──────────────────────────────────────────

TEST(JkBalancerModbusParseTest, NoDeviceRegisteredDoesNotCrash) {
  TestableJkBalancerModbus bus;
  auto frame = make_frame(0x01, 0xFF);
  for (uint8_t b : frame)
    bus.parse_byte(b);
}

}  // namespace esphome::jk_balancer_modbus::testing
