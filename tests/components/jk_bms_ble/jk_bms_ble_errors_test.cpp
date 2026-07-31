#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_v11.h"

namespace esphome::jk_bms_ble::testing {

// A lookup table mirroring what the Python codegen in jk_bms_ble/__init__.py
// (DEFAULT_ERRORS_JK02, optionally patched by `error_overrides`) emits as a
// static constexpr array and hands to the hub via set_errors_jk02_table().
static constexpr const char *const kTestErrorsJk02[] = {
    "Wire resistance",                      // bit 0
    "MOSFET overtemperature",               // bit 1
    "Cell count is not equal to settings",  // bit 2
    "",                                     // bit 3
    "Battery is fully charged",             // bit 4
    "Battery pack overvoltage",             // bit 5
};
static constexpr size_t kTestErrorsJk02Count = 6;

// Same table, but with bit 4 overridden - as if the user had configured
// `error_overrides: {4: "Cell overvoltage"}` for older firmware.
static constexpr const char *const kOverriddenErrorsJk02[] = {
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

TEST(JkBmsBleErrorsTest, NoTableConfiguredMaskZeroIsSafe) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  text_sensor::TextSensor hex, text;
  bms.set_errors_bitmask_hex_text_sensor(&hex);
  bms.set_errors_text_sensor(&text);

  // errors_jk02_table_ is left at its default (entries=nullptr, count=0), just
  // like a freshly constructed hub before to_code() runs the generated
  // set_errors_jk02_table() call. A zero mask must never dereference the table.
  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, 0x00000000);

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(hex.state, "0x00000000");
  EXPECT_EQ(text.state, "");
}

TEST(JkBmsBleErrorsTest, SingleKnownBit) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(kTestErrorsJk02, kTestErrorsJk02Count);
  text_sensor::TextSensor hex, text;
  bms.set_errors_bitmask_hex_text_sensor(&hex);
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, 0x00000001);  // bit 0

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(hex.state, "0x00000001");
  EXPECT_EQ(text.state, "Wire resistance");
}

TEST(JkBmsBleErrorsTest, MultipleBitsJoinedInAscendingOrder) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(kTestErrorsJk02, kTestErrorsJk02Count);
  text_sensor::TextSensor text;
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, 0x00000011);  // bit 0 + bit 4

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(text.state, "Wire resistance;Battery is fully charged");
}

TEST(JkBmsBleErrorsTest, EmptyPlaceholderBitIsSkippedButStillInHex) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(kTestErrorsJk02, kTestErrorsJk02Count);
  text_sensor::TextSensor hex, text;
  bms.set_errors_bitmask_hex_text_sensor(&hex);
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, 0x00000009);  // bit 0 + bit 3 (placeholder, "")

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(hex.state, "0x00000009");
  EXPECT_EQ(text.state, "Wire resistance");
}

// Validates the generic table-override mechanism that error_overrides relies
// on: swapping in a different table changes the decoded label for a bit
// without any change to jk_bms_ble.cpp.
TEST(JkBmsBleErrorsTest, OverriddenLabelIsUsedInsteadOfDefault) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_errors_jk02_table(kOverriddenErrorsJk02, kTestErrorsJk02Count);
  text_sensor::TextSensor text;
  bms.set_errors_text_sensor(&text);

  auto frame = CELL_INFO_JK02_32S_V11;
  set_jk02_32s_error_mask(frame, 0x00000010);  // bit 4

  bms.decode_jk02_cell_info_(frame);

  EXPECT_EQ(text.state, "Cell overvoltage");
}

TEST(JkBmsBleErrorsTest, Jk02_24SUsesSameTableWith16BitMask) {
  TestableJkBmsBle bms;  // default protocol version is JK02_24S
  bms.set_errors_jk02_table(kTestErrorsJk02, kTestErrorsJk02Count);
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
