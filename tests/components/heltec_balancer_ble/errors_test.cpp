#include <gtest/gtest.h>
#include <cmath>
#include <iterator>
#include <string>
#include <utility>
#include "common.h"
#include "frames_ek-b24s8e200a.h"
#include "frames_gw-24s4eb.h"

namespace esphome::heltec_balancer_ble::testing {

// Mirrors DEFAULT_ERRORS in heltec_balancer_ble/__init__.py, which to_code() emits
// as a static constexpr array and hands to the hub via set_errors_table().
static constexpr const char *const DEFAULT_ERRORS[] = {
    "Battery detection failed",   // bit 0
    "Overvoltage",                // bit 1
    "Undervoltage",               // bit 2
    "Polarity error",             // bit 3
    "Excessive line resistance",  // bit 4
    "System overheating",         // bit 5
    "Charging fault",             // bit 6
    "Discharge fault",            // bit 7
};

// Deliberately shorter than the 8 bits the decoder scans, so the bounds check of
// LookupTable::get() is exercised.
static constexpr const char *const SHORT_ERRORS[] = {
    "Battery detection failed",  // bit 0
    "Overvoltage",               // bit 1
    "Undervoltage",              // bit 2
};

// The default table with bit 1 overridden - as if the user had configured
// `error_overrides: {1: "Cell overvoltage"}`.
static constexpr const char *const OVERRIDDEN_ERRORS[] = {
    "Battery detection failed",   // bit 0
    "Cell overvoltage",           // bit 1 (overridden)
    "Undervoltage",               // bit 2
    "Polarity error",             // bit 3
    "Excessive line resistance",  // bit 4
    "System overheating",         // bit 5
    "Charging fault",             // bit 6
    "Discharge fault",            // bit 7
};

struct ErrorInputs {
  bool detection_failed{false};
  bool overvoltage{false};
  bool undervoltage{false};
  bool polarity_error{false};
  bool excessive_line_resistance{false};
  bool system_overheating{false};
  bool charging_fault{false};
  bool discharge_fault{false};
};

// The error block is five 24-bit per-cell bitmasks followed by three fault bytes.
// It starts at offset 229 in a V1 cell info frame and at 266 in a V2 one.
static void set_error_fields(std::vector<uint8_t> &frame, size_t base, const ErrorInputs &in) {
  auto set_cell_mask = [&](size_t offset, bool value) {
    frame[offset + 0] = value ? 0x01 : 0x00;  // one affected cell is enough
    frame[offset + 1] = 0x00;
    frame[offset + 2] = 0x00;
  };

  set_cell_mask(base + 0, in.detection_failed);
  set_cell_mask(base + 3, in.overvoltage);
  set_cell_mask(base + 6, in.undervoltage);
  set_cell_mask(base + 9, in.polarity_error);
  set_cell_mask(base + 12, in.excessive_line_resistance);
  frame[base + 15] = in.system_overheating ? 0x01 : 0x00;
  frame[base + 16] = in.charging_fault ? 0x01 : 0x00;
  frame[base + 17] = in.discharge_fault ? 0x01 : 0x00;
}

// Feeds a V1 cell info frame carrying `in` into the decoder and returns the published
// (errors bitmask, errors) pair. Passing a nullptr table leaves errors_table_ at its
// default, i.e. a hub whose set_errors_table() call the codegen never emitted.
static std::pair<float, std::string> decode_errors(const ErrorInputs &in, const char *const *table, size_t count) {
  TestableHeltecBalancerBle bms;
  if (table != nullptr)
    bms.set_errors_table(table, count);

  sensor::Sensor bitmask;
  text_sensor::TextSensor text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&text);

  auto frame = gw::CELL_INFO_FRAME;
  set_error_fields(frame, 229, in);
  bms.decode_cell_info_(frame);

  return {bitmask.state, text.state};
}

static std::pair<float, std::string> decode_errors(const ErrorInputs &in) {
  return decode_errors(in, DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));
}

TEST(HeltecBalancerErrorsTest, NoErrorsPublishesZeroAndEmptyString) {
  auto [bitmask, text] = decode_errors({});

  EXPECT_FLOAT_EQ(bitmask, 0.0f);
  EXPECT_EQ(text, "");
}

TEST(HeltecBalancerErrorsTest, SingleCellConditionSetsItsBit) {
  auto [bitmask, text] = decode_errors({.overvoltage = true});

  EXPECT_FLOAT_EQ(bitmask, 2.0f);  // bit 1
  EXPECT_EQ(text, "Overvoltage");
}

TEST(HeltecBalancerErrorsTest, SingleFaultByteSetsItsBit) {
  auto [bitmask, text] = decode_errors({.discharge_fault = true});

  EXPECT_FLOAT_EQ(bitmask, 128.0f);  // bit 7
  EXPECT_EQ(text, "Discharge fault");
}

TEST(HeltecBalancerErrorsTest, MultipleConditionsJoinedInAscendingOrder) {
  auto [bitmask, text] = decode_errors({.overvoltage = true, .charging_fault = true});

  EXPECT_FLOAT_EQ(bitmask, 66.0f);  // bit 1 + bit 6
  EXPECT_EQ(text, "Overvoltage;Charging fault");
}

TEST(HeltecBalancerErrorsTest, AllConditionsProduceFullMask) {
  auto [bitmask, text] = decode_errors({true, true, true, true, true, true, true, true});

  EXPECT_FLOAT_EQ(bitmask, 255.0f);
  EXPECT_EQ(text, "Battery detection failed;Overvoltage;Undervoltage;Polarity error;"
                  "Excessive line resistance;System overheating;Charging fault;Discharge fault");
}

// The decoder scans 8 bits regardless of how many entries the configured table holds.
// Bits past the end must be dropped instead of read out of bounds.
TEST(HeltecBalancerErrorsTest, BitBeyondTableEndIsIgnored) {
  auto [bitmask, text] =
      decode_errors({.detection_failed = true, .discharge_fault = true}, SHORT_ERRORS, std::size(SHORT_ERRORS));

  EXPECT_FLOAT_EQ(bitmask, 129.0f);  // bit 0 + bit 7, table holds 3 entries
  EXPECT_EQ(text, "Battery detection failed");
}

// A hub without a table (entries == nullptr, count == 0) must not dereference it,
// no matter which bits are set - the raw bitmask is still published.
TEST(HeltecBalancerErrorsTest, NoTableConfiguredPublishesBitmaskOnly) {
  auto [bitmask, text] = decode_errors({true, true, true, true, true, true, true, true}, nullptr, 0);

  EXPECT_FLOAT_EQ(bitmask, 255.0f);
  EXPECT_EQ(text, "");
}

// Validates the mechanism error_overrides relies on: swapping in a different table
// changes the decoded label for a bit without any change to heltec_balancer_ble.cpp.
TEST(HeltecBalancerErrorsTest, OverriddenLabelIsUsedInsteadOfDefault) {
  auto [bitmask, text] = decode_errors({.overvoltage = true}, OVERRIDDEN_ERRORS, std::size(OVERRIDDEN_ERRORS));

  EXPECT_FLOAT_EQ(bitmask, 2.0f);
  EXPECT_EQ(text, "Cell overvoltage");
}

// The V2 frame carries the same error block 37 bytes further in.
TEST(HeltecBalancerErrorsTest, ProtocolV2UsesSameTable) {
  TestableHeltecBalancerBle bms;
  bms.set_errors_table(DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));

  sensor::Sensor bitmask;
  text_sensor::TextSensor text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&text);

  auto frame = ek200a::CELL_INFO_FRAME;
  set_error_fields(frame, 266, {.undervoltage = true, .system_overheating = true});
  bms.decode_cell_info_v2_(frame);

  EXPECT_FLOAT_EQ(bitmask.state, 36.0f);  // bit 2 + bit 5
  EXPECT_EQ(text.state, "Undervoltage;System overheating");
}

// Going offline clears the bitmask and marks the text sensor, like the other
// components do in their device unavailable path.
TEST(HeltecBalancerErrorsTest, DeviceUnavailablePublishesOffline) {
  TestableHeltecBalancerBle bms;
  bms.set_errors_table(DEFAULT_ERRORS, std::size(DEFAULT_ERRORS));

  sensor::Sensor bitmask;
  text_sensor::TextSensor text;
  bms.set_errors_bitmask_sensor(&bitmask);
  bms.set_errors_text_sensor(&text);

  bms.publish_device_unavailable_();

  EXPECT_TRUE(std::isnan(bitmask.state));
  EXPECT_EQ(text.state, "Offline");
}

}  // namespace esphome::heltec_balancer_ble::testing
