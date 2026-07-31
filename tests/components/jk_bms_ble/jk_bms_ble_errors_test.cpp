#include <gtest/gtest.h>
#include <iterator>
#include <string>
#include <utility>
#include "common.h"
#include "frames_jk02_32s_v11.h"

namespace esphome::jk_bms_ble::testing {

// A lookup table mirroring what the Python codegen in jk_bms_ble/__init__.py
// (DEFAULT_ERRORS_JK02, optionally patched by `error_overrides`) emits as a
// static constexpr array and hands to the hub via set_errors_jk02_table().
// Only the first few bits are covered here - the table is deliberately shorter
// than the 32 bits the decoder scans so the bounds check is exercised too.
static constexpr const char *const DEFAULT_ERRORS[] = {
    "Wire resistance",                      // bit 0
    "MOSFET overtemperature",               // bit 1
    "Cell count is not equal to settings",  // bit 2
    "",                                     // bit 3
    "Battery is fully charged",             // bit 4
    "Battery pack overvoltage",             // bit 5
};

// Same table, but with bit 4 overridden - as if the user had configured
// `error_overrides: {4: "Cell overvoltage"}` for older firmware.
static constexpr const char *const OVERRIDDEN_ERRORS[] = {
    "Wire resistance",                      // bit 0
    "MOSFET overtemperature",               // bit 1
    "Cell count is not equal to settings",  // bit 2
    "",                                     // bit 3
    "Cell overvoltage",                     // bit 4 (overridden)
    "Battery pack overvoltage",             // bit 5
};

static void set_jk02_32s_error_mask(std::vector<uint8_t> &frame, uint32_t mask) {
  // Errors bitmask lives at byte offset 166-169 (little-endian) for JK02_32S frames:
  // decode_jk02_cell_info_ doubles its 16-byte JK02_32S offset (134 + 16*2 = 166)
  // once it reaches the extra cells/temperature fields past the shared 24S layout.
  frame[166] = uint8_t(mask >> 0);
  frame[167] = uint8_t(mask >> 8);
  frame[168] = uint8_t(mask >> 16);
  frame[169] = uint8_t(mask >> 24);
}

static void set_jk02_24s_error_mask(std::vector<uint8_t> &frame, uint16_t mask) {
  // Errors bitmask lives at byte offset 136-137 (little-endian) for JK02_24S frames.
  frame[136] = uint8_t(mask >> 0);
  frame[137] = uint8_t(mask >> 8);
}

// Feeds a JK02_32S cell info frame carrying `mask` into the decoder and returns the
// published (errors bitmask hex, errors) pair. Passing a nullptr table leaves
// errors_jk02_table_ at its default, i.e. a hub whose set_errors_jk02_table() call
// the codegen never emitted.
static std::pair<std::string, std::string> decode_32s_errors(uint32_t mask, const char *const *table, size_t count) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  if (table != nullptr)
    bms.set_errors_jk02_table(table, count);

  text_sensor::TextSensor hex, text;
  bms.set_errors_bitmask_hex_text_sensor(&hex);
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, mask);
  bms.decode_jk02_cell_info_(frame);

  return {hex.state, text.state};
}

static std::pair<std::string, std::string> decode_32s_errors(uint32_t mask) {
  return decode_32s_errors(mask, DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));
}

TEST(JkBmsBleErrorsTest, SingleKnownBit) {
  auto [hex, text] = decode_32s_errors(0x00000001);  // bit 0

  EXPECT_EQ(hex, "0x00000001");
  EXPECT_EQ(text, "Wire resistance");
}

TEST(JkBmsBleErrorsTest, MaskZeroPublishesEmptyString) {
  auto [hex, text] = decode_32s_errors(0x00000000);

  EXPECT_EQ(hex, "0x00000000");
  EXPECT_EQ(text, "");
}

TEST(JkBmsBleErrorsTest, MultipleBitsJoinedInAscendingOrder) {
  auto [hex, text] = decode_32s_errors(0x00000011);  // bit 0 + bit 4

  EXPECT_EQ(hex, "0x00000011");
  EXPECT_EQ(text, "Wire resistance;Battery is fully charged");
}

TEST(JkBmsBleErrorsTest, EmptyPlaceholderBitIsSkippedButStillInHex) {
  auto [hex, text] = decode_32s_errors(0x00000009);  // bit 0 + bit 3 (placeholder, "")

  EXPECT_EQ(hex, "0x00000009");
  EXPECT_EQ(text, "Wire resistance");
}

// The decoder scans 32 bits regardless of how many entries the configured table
// holds. Bits past the end must be dropped instead of read out of bounds.
TEST(JkBmsBleErrorsTest, BitBeyondTableEndIsIgnored) {
  auto [hex, text] = decode_32s_errors(0x80000041);  // bit 0 + bit 6 + bit 31, table holds 6 entries

  EXPECT_EQ(hex, "0x80000041");
  EXPECT_EQ(text, "Wire resistance");
}

// A hub without a table (entries == nullptr, count == 0) must not dereference it,
// no matter which bits are set - the raw bitmask is still published.
TEST(JkBmsBleErrorsTest, NoTableConfiguredPublishesBitmaskOnly) {
  auto [hex, text] = decode_32s_errors(0xFFFFFFFF, nullptr, 0);

  EXPECT_EQ(hex, "0xFFFFFFFF");
  EXPECT_EQ(text, "");
}

// Validates the mechanism error_overrides relies on: swapping in a different table
// changes the decoded label for a bit without any change to jk_bms_ble.cpp.
TEST(JkBmsBleErrorsTest, OverriddenLabelIsUsedInsteadOfDefault) {
  auto [hex, text] = decode_32s_errors(0x00000010, OVERRIDDEN_ERRORS, std::size(OVERRIDDEN_ERRORS));  // bit 4

  EXPECT_EQ(hex, "0x00000010");
  EXPECT_EQ(text, "Cell overvoltage");
}

TEST(JkBmsBleErrorsTest, Protocol24SUsesSameTableWith16BitMask) {
  TestableJkBmsBle bms;  // default protocol version is JK02_24S
  bms.set_errors_jk02_table(DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));
  text_sensor::TextSensor hex, text;
  bms.set_errors_bitmask_hex_text_sensor(&hex);
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_24S_V10;
  set_jk02_24s_error_mask(frame, 0x0021);  // bit 0 + bit 5

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(hex.state, "0x00000021");
  EXPECT_EQ(text.state, "Wire resistance;Battery pack overvoltage");
}

}  // namespace esphome::jk_bms_ble::testing
