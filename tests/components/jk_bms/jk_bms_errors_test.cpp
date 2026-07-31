#include <gtest/gtest.h>
#include <iterator>
#include <string>
#include <utility>
#include "common.h"

namespace esphome::jk_bms::testing {

static constexpr uint8_t FUNCTION_READ_ALL = 0x06;

// A lookup table mirroring what the Python codegen in jk_bms/__init__.py
// (DEFAULT_ERRORS, optionally patched by `error_overrides`) emits as a static
// constexpr array and hands to the hub via set_errors_table(). Only the first
// few bits are covered here - the table is deliberately shorter than the 16 bits
// the decoder scans so the bounds check is exercised too.
static constexpr const char *const DEFAULT_ERRORS[] = {
    "Low capacity",             // bit 0
    "MOSFET overtemperature",   // bit 1
    "Charge overvoltage",       // bit 2
    "Discharge undervoltage",   // bit 3
    "Battery overtemperature",  // bit 4
    "",                         // bit 5 (reserved placeholder for this fixture)
};

// Same table, but with bit 0 overridden - as if the user had configured
// `error_overrides: {0: "Low SoC"}`.
static constexpr const char *const OVERRIDDEN_ERRORS[] = {
    "Low SoC",                  // bit 0 (overridden)
    "MOSFET overtemperature",   // bit 1
    "Charge overvoltage",       // bit 2
    "Discharge undervoltage",   // bit 3
    "Battery overtemperature",  // bit 4
    "",                         // bit 5
};

static void set_error_mask(std::vector<uint8_t> &frame, uint16_t mask) {
  // on_status_data_ reads the errors bitmask via jk_get_16bit(offset + 6 + 3 * 8)
  // with offset = frame[1] + 3 = 0x2A + 3 = 45, so index 75. jk_get_16bit is
  // big-endian: high byte first.
  frame[75] = uint8_t(mask >> 8);
  frame[76] = uint8_t(mask >> 0);
}

// Feeds a status frame carrying `mask` into the decoder and returns the published
// (errors bitmask, errors) pair. Passing a nullptr table leaves errors_table_ at
// its default, i.e. a hub whose set_errors_table() call the codegen never emitted.
static std::pair<float, std::string> decode_errors(uint16_t mask, const char *const *table, size_t count) {
  TestableJkBms bms;
  if (table != nullptr)
    bms.set_errors_table(table, count);

  sensor::Sensor bitmask;
  text_sensor::TextSensor text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&text);

  auto frame = STATUS_FRAME_14S;
  set_error_mask(frame, mask);
  bms.on_jk_modbus_data(FUNCTION_READ_ALL, frame);

  return {bitmask.state, text.state};
}

static std::pair<float, std::string> decode_errors(uint16_t mask) {
  return decode_errors(mask, DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));
}

TEST(JkBmsErrorsTest, SingleKnownBit) {
  auto [bitmask, text] = decode_errors(0x0001);  // bit 0

  EXPECT_FLOAT_EQ(bitmask, 1.0f);
  EXPECT_EQ(text, "Low capacity");
}

TEST(JkBmsErrorsTest, MaskZeroPublishesEmptyString) {
  auto [bitmask, text] = decode_errors(0x0000);

  EXPECT_FLOAT_EQ(bitmask, 0.0f);
  EXPECT_EQ(text, "");
}

TEST(JkBmsErrorsTest, MultipleBitsJoinedInAscendingOrder) {
  auto [bitmask, text] = decode_errors(0x0003);  // bit 0 + bit 1

  EXPECT_FLOAT_EQ(bitmask, 3.0f);
  EXPECT_EQ(text, "Low capacity;MOSFET overtemperature");
}

TEST(JkBmsErrorsTest, EmptyPlaceholderBitIsSkippedButStillInBitmask) {
  auto [bitmask, text] = decode_errors(0x0021);  // bit 0 + bit 5 (placeholder, "")

  EXPECT_FLOAT_EQ(bitmask, 33.0f);
  EXPECT_EQ(text, "Low capacity");
}

// The decoder scans 16 bits regardless of how many entries the configured table
// holds. Bits past the end must be dropped instead of read out of bounds.
TEST(JkBmsErrorsTest, BitBeyondTableEndIsIgnored) {
  auto [bitmask, text] = decode_errors(0x8041);  // bit 0 + bit 6 + bit 15, table holds 6 entries

  EXPECT_FLOAT_EQ(bitmask, 32833.0f);
  EXPECT_EQ(text, "Low capacity");
}

// A hub without a table (entries == nullptr, count == 0) must not dereference it,
// no matter which bits are set - the raw bitmask is still published.
TEST(JkBmsErrorsTest, NoTableConfiguredPublishesBitmaskOnly) {
  auto [bitmask, text] = decode_errors(0xFFFF, nullptr, 0);

  EXPECT_FLOAT_EQ(bitmask, 65535.0f);
  EXPECT_EQ(text, "");
}

// Validates the mechanism error_overrides relies on: swapping in a different table
// changes the decoded label for a bit without any change to jk_bms.cpp.
TEST(JkBmsErrorsTest, OverriddenLabelIsUsedInsteadOfDefault) {
  auto [bitmask, text] = decode_errors(0x0001, OVERRIDDEN_ERRORS, std::size(OVERRIDDEN_ERRORS));  // bit 0

  EXPECT_FLOAT_EQ(bitmask, 1.0f);
  EXPECT_EQ(text, "Low SoC");
}

}  // namespace esphome::jk_bms::testing
