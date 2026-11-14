#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// Time sync: register 0x6C, length 0x04, value = local wall-clock seconds since 2020-01-01 00:00:00.
//
// Captures from docs/btsnoops/heating-start-stop-setting/btsnoop_hci.log (JK app, phone set to UTC+8):
//
//   2025-11-29 15:11:59 (UTC+8): AA 55 90 EB 6C 04 3F 2C 1F 0B 41 E1 B6 E4 1A 88 2E BC E2 A9
//   2025-11-29 15:12:41 (UTC+8): AA 55 90 EB 6C 04 69 2C 1F 0B 39 6B B3 83 99 44 43 B0 FB 4E
//
// Bytes 10-18 are stale BLE-TX-buffer data (see jk_bms_ble_write_command_test.cpp), so only
// register, length and value are compared.
//
// Both captures were taken in UTC+8, so they can't tell "local wall clock" apart from
// "UTC with a UTC+8 epoch". The BMS has no time zone setting, so wall clock is assumed.

static const time_t CAPTURE_1_UTC = 1764400319;  // 2025-11-29 07:11:59 UTC
static const time_t CAPTURE_2_UTC = 1764400361;  // 2025-11-29 07:12:41 UTC
static const int32_t UTC_PLUS_8 = 8 * 3600;

TEST(JkBmsSyncTimeTest, EncodesCapturedValues) {
  EXPECT_EQ(JkBmsBle::encode_time(CAPTURE_1_UTC, UTC_PLUS_8), 0x0B1F2C3Fu);
  EXPECT_EQ(JkBmsBle::encode_time(CAPTURE_2_UTC, UTC_PLUS_8), 0x0B1F2C69u);
}

TEST(JkBmsSyncTimeTest, FrameMatchesCapture) {
  auto f = JkBmsBle::build_frame(0x6C, JkBmsBle::encode_time(CAPTURE_1_UTC, UTC_PLUS_8), 0x04);
  const uint8_t expected[] = {0xAA, 0x55, 0x90, 0xEB, 0x6C, 0x04, 0x3F, 0x2C, 0x1F, 0x0B};
  for (size_t i = 0; i < sizeof(expected); i++)
    EXPECT_EQ(f[i], expected[i]) << "byte [" << i << "]";
}

TEST(JkBmsSyncTimeTest, UsesLocalWallClock) {
  // Same instant, Europe/Berlin (UTC+1): the BMS must show 08:11:59, i.e. 7 h less than in UTC+8
  EXPECT_EQ(JkBmsBle::encode_time(CAPTURE_1_UTC, 3600), 0x0B1F2C3Fu - 7 * 3600);
  // UTC: 2020-01-01 00:00:00 UTC is the epoch itself
  EXPECT_EQ(JkBmsBle::encode_time(1577836800, 0), 0u);
}

TEST(JkBmsSyncTimeTest, ClampsBeforeEpoch) { EXPECT_EQ(JkBmsBle::encode_time(1577836799, 0), 0u); }

}  // namespace esphome::jk_bms_ble::testing
