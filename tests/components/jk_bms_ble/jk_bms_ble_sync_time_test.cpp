#include <ctime>
#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_24s_v10.h"
#include "frames_jk02_32s_b1a8s10p.h"
#include "frames_jk02_32s_v19.h"

namespace esphome::jk_bms_ble::testing {

// Time sync: register 0x6C, length 0x04, value = seconds since 2020-01-01 00:00:00 UTC+8
// (= unix timestamp - 1577808000). The value doesn't depend on the local time zone.
//
// Captures from docs/btsnoops/heating-start-stop-setting/btsnoop_hci.log:
//
//   2025-11-29 07:11:59 UTC: AA 55 90 EB 6C 04 3F 2C 1F 0B 41 E1 B6 E4 1A 88 2E BC E2 A9
//   2025-11-29 07:12:41 UTC: AA 55 90 EB 6C 04 69 2C 1F 0B 39 6B B3 83 99 44 43 B0 FB 4E
//
// Bytes 10-18 are stale BLE-TX-buffer data (see jk_bms_ble_write_command_test.cpp), so only
// register, length and value are compared.
//
// CELL_INFO_JK02_32S_B1A8S10P was captured on 2026-11-25 20:02:03 UTC (21:02:03 CET). Its clock
// (bytes 262-265) reads 217828922 = 2026-11-25 20:02:02 UTC, which confirms that the clock isn't
// shifted by the local time zone.

static const time_t CAPTURE_1_UTC = 1764400319;       // 2025-11-29 07:11:59 UTC
static const time_t CAPTURE_2_UTC = 1764400361;       // 2025-11-29 07:12:41 UTC
static const time_t B1A8S10P_CLOCK_UTC = 1795636922;  // 2026-11-25 20:02:02 UTC

TEST(JkBmsSyncTimeTest, EncodesCapturedValues) {
  EXPECT_EQ(JkBmsBle::encode_time(CAPTURE_1_UTC), 0x0B1F2C3Fu);
  EXPECT_EQ(JkBmsBle::encode_time(CAPTURE_2_UTC), 0x0B1F2C69u);
}

TEST(JkBmsSyncTimeTest, FrameMatchesCapture) {
  auto f = JkBmsBle::build_frame(0x6C, JkBmsBle::encode_time(CAPTURE_1_UTC), 0x04);
  const uint8_t expected[] = {0xAA, 0x55, 0x90, 0xEB, 0x6C, 0x04, 0x3F, 0x2C, 0x1F, 0x0B};
  for (size_t i = 0; i < sizeof(expected); i++)
    EXPECT_EQ(f[i], expected[i]) << "byte [" << i << "]";
}

TEST(JkBmsSyncTimeTest, MatchesClockOfRealDevice) {
  EXPECT_EQ(JkBmsBle::encode_time(B1A8S10P_CLOCK_UTC), 217828922u);
  EXPECT_EQ(JkBmsBle::decode_time(217828922u), B1A8S10P_CLOCK_UTC);
}

TEST(JkBmsSyncTimeTest, EpochIsMidnightUtcPlus8) {
  // 2020-01-01 00:00:00 UTC+8 = 2019-12-31 16:00:00 UTC
  EXPECT_EQ(JkBmsBle::encode_time(1577808000), 0u);
  EXPECT_EQ(JkBmsBle::decode_time(0), 1577808000);
}

TEST(JkBmsSyncTimeTest, ClampsBeforeEpoch) { EXPECT_EQ(JkBmsBle::encode_time(1577807999), 0u); }

// Without a time zone configured (as on the host), the clock is shown in UTC
TEST(JkBmsDeviceTimeTest, DecodesClockFromCellInfo) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  text_sensor::TextSensor device_time;
  bms.set_device_time_text_sensor(&device_time);
  bms.decode_jk02_cell_info_(CELL_INFO_JK02_32S_B1A8S10P);
  EXPECT_EQ(device_time.state, "2026-11-25 20:02:02");
}

TEST(JkBmsDeviceTimeTest, SkipsUnsetClock) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  text_sensor::TextSensor device_time;
  bms.set_device_time_text_sensor(&device_time);
  std::vector<uint8_t> data = CELL_INFO_JK02_32S_B1A8S10P;
  data[262] = data[263] = data[264] = data[265] = 0x00;
  bms.decode_jk02_cell_info_(data);
  EXPECT_FALSE(device_time.has_state());
}

// Device info byte 163 bit 4 reports a real-time clock. Time sync is only sent if it is set.
TEST(JkBmsDeviceTimeTest, DeviceInfoReportsClock) {
  TestableJkBmsBle bms;
  bms.decode_device_info_(DEVICE_INFO_JK02_32S_V19);
  EXPECT_TRUE(bms.has_clock());
}

TEST(JkBmsDeviceTimeTest, DeviceInfoWithoutClock) {
  TestableJkBmsBle bms;
  bms.decode_device_info_(DEVICE_INFO_JK02_24S_V10);
  EXPECT_FALSE(bms.has_clock());
}

}  // namespace esphome::jk_bms_ble::testing
