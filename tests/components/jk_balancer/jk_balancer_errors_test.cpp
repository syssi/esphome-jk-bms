#include <gtest/gtest.h>
#include <iterator>
#include <string>
#include <utility>
#include "common.h"

namespace esphome::jk_balancer::testing {

static constexpr uint8_t FUNCTION_READ_ALL = 0xFF;

// Deliberately shorter than the 8 bits the decoder scans, so the bounds check
// of LookupTable::get() is exercised.
static constexpr const char *const SHORT_ERRORS[] = {
    "Wrong cell count",     // bit 0
    "Resistance too high",  // bit 1
    "Overvoltage",          // bit 2
};

// The default table with bit 0 overridden - as if the user had configured
// `error_overrides: {0: "Cell count mismatch"}`.
static constexpr const char *const OVERRIDDEN_ERRORS[] = {
    "Cell count mismatch",  // bit 0 (overridden)
    "Resistance too high",  // bit 1
    "Overvoltage",          // bit 2
    "",                     // bit 3
    "",                     // bit 4
    "",                     // bit 5
    "",                     // bit 6
    "",                     // bit 7
};

// Feeds a status frame carrying `mask` into the decoder and returns the published
// (errors bitmask, errors) pair. Passing a nullptr table leaves errors_table_ at
// its default, i.e. a hub whose set_errors_table() call the codegen never emitted.
static std::pair<float, std::string> decode_errors(uint8_t mask, const char *const *table, size_t count) {
  TestableJkBalancer bms;
  if (table != nullptr)
    bms.set_errors_table(table, count);

  sensor::Sensor bitmask;
  text_sensor::TextSensor text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&text);

  auto frame = STATUS_FRAME_17S;
  frame[12] = mask;  // byte 12 holds the alarm status bitmask
  bms.on_jk_balancer_modbus_data(FUNCTION_READ_ALL, frame);

  return {bitmask.state, text.state};
}

static std::pair<float, std::string> decode_errors(uint8_t mask) {
  return decode_errors(mask, DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));
}

TEST(JkBalancerErrorsTest, SingleKnownBit) {
  auto [bitmask, text] = decode_errors(0x01);  // bit 0

  EXPECT_FLOAT_EQ(bitmask, 1.0f);
  EXPECT_EQ(text, "Wrong cell count");
}

TEST(JkBalancerErrorsTest, MaskZeroPublishesEmptyString) {
  auto [bitmask, text] = decode_errors(0x00);

  EXPECT_FLOAT_EQ(bitmask, 0.0f);
  EXPECT_EQ(text, "");
}

TEST(JkBalancerErrorsTest, MultipleBitsJoinedInAscendingOrder) {
  auto [bitmask, text] = decode_errors(0x05);  // bit 0 + bit 2

  EXPECT_FLOAT_EQ(bitmask, 5.0f);
  EXPECT_EQ(text, "Wrong cell count;Overvoltage");
}

TEST(JkBalancerErrorsTest, ReservedBitIsSkippedButStillInBitmask) {
  auto [bitmask, text] = decode_errors(0x09);  // bit 0 + bit 3 (reserved, "")

  EXPECT_FLOAT_EQ(bitmask, 9.0f);
  EXPECT_EQ(text, "Wrong cell count");
}

// The decoder scans 8 bits regardless of how many entries the configured table
// holds. Bits past the end must be dropped instead of read out of bounds.
TEST(JkBalancerErrorsTest, BitBeyondTableEndIsIgnored) {
  auto [bitmask, text] = decode_errors(0x89, SHORT_ERRORS, std::size(SHORT_ERRORS));  // bit 0 + 3 + 7

  EXPECT_FLOAT_EQ(bitmask, 137.0f);
  EXPECT_EQ(text, "Wrong cell count");
}

// A hub without a table (entries == nullptr, count == 0) must not dereference it,
// no matter which bits are set - the raw bitmask is still published.
TEST(JkBalancerErrorsTest, NoTableConfiguredPublishesBitmaskOnly) {
  auto [bitmask, text] = decode_errors(0xFF, nullptr, 0);

  EXPECT_FLOAT_EQ(bitmask, 255.0f);
  EXPECT_EQ(text, "");
}

// Validates the mechanism error_overrides relies on: swapping in a different table
// changes the decoded label for a bit without any change to jk_balancer.cpp.
TEST(JkBalancerErrorsTest, OverriddenLabelIsUsedInsteadOfDefault) {
  auto [bitmask, text] = decode_errors(0x01, OVERRIDDEN_ERRORS, std::size(OVERRIDDEN_ERRORS));  // bit 0

  EXPECT_FLOAT_EQ(bitmask, 1.0f);
  EXPECT_EQ(text, "Cell count mismatch");
}

}  // namespace esphome::jk_balancer::testing
