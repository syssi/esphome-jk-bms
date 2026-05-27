#include <gtest/gtest.h>
#include "common.h"
#include "frames_logbook.h"

namespace esphome::jk_bms_ble::testing {

TEST(JkBmsLogbookTest, FrameSize) { EXPECT_EQ(LOGBOOK_JK02_32S.size(), 300u); }

TEST(JkBmsLogbookTest, DecodeDoesNotCrash) {
  TestableJkBmsBle bms;
  bms.decode_logbook_(LOGBOOK_JK02_32S);
}

TEST(JkBmsLogbookTest, LogCount) {
  // log_count is at bytes 6-9 (LE): 0x04 0x00 0x00 0x00 = 4
  uint32_t log_count = uint32_t(LOGBOOK_JK02_32S[6]) | (uint32_t(LOGBOOK_JK02_32S[7]) << 8) |
                       (uint32_t(LOGBOOK_JK02_32S[8]) << 16) | (uint32_t(LOGBOOK_JK02_32S[9]) << 24);
  EXPECT_EQ(log_count, 4u);
}

TEST(JkBmsLogbookTest, Entry0Boot) {
  // Entry 0: bytes 11-15 → ts=0, code=0x01 (Boot)
  uint32_t ts = uint32_t(LOGBOOK_JK02_32S[11]) | (uint32_t(LOGBOOK_JK02_32S[12]) << 8) |
                (uint32_t(LOGBOOK_JK02_32S[13]) << 16) | (uint32_t(LOGBOOK_JK02_32S[14]) << 24);
  uint8_t code = LOGBOOK_JK02_32S[15];
  EXPECT_EQ(ts, 0u);
  EXPECT_EQ(code, 0x01u);
}

TEST(JkBmsLogbookTest, Entry1FactorySettingLFP) {
  // Entry 1: bytes 16-20 → ts=238, code=0x44 (Factory setting LFP)
  uint32_t ts = uint32_t(LOGBOOK_JK02_32S[16]) | (uint32_t(LOGBOOK_JK02_32S[17]) << 8) |
                (uint32_t(LOGBOOK_JK02_32S[18]) << 16) | (uint32_t(LOGBOOK_JK02_32S[19]) << 24);
  uint8_t code = LOGBOOK_JK02_32S[20];
  EXPECT_EQ(ts, 238u);
  EXPECT_EQ(code, 0x44u);
}

TEST(JkBmsLogbookTest, Entry2ChargeLowTempProtection) {
  // Entry 2: bytes 21-25 → ts=2514095, code=0x1B
  uint32_t ts = uint32_t(LOGBOOK_JK02_32S[21]) | (uint32_t(LOGBOOK_JK02_32S[22]) << 8) |
                (uint32_t(LOGBOOK_JK02_32S[23]) << 16) | (uint32_t(LOGBOOK_JK02_32S[24]) << 24);
  uint8_t code = LOGBOOK_JK02_32S[25];
  EXPECT_EQ(ts, 2514095u);
  EXPECT_EQ(code, 0x1Bu);
}

TEST(JkBmsLogbookTest, Entry3ChargeLowTempProtectionReleased) {
  // Entry 3: bytes 26-30 → ts=2514136, code=0x1C
  uint32_t ts = uint32_t(LOGBOOK_JK02_32S[26]) | (uint32_t(LOGBOOK_JK02_32S[27]) << 8) |
                (uint32_t(LOGBOOK_JK02_32S[28]) << 16) | (uint32_t(LOGBOOK_JK02_32S[29]) << 24);
  uint8_t code = LOGBOOK_JK02_32S[30];
  EXPECT_EQ(ts, 2514136u);
  EXPECT_EQ(code, 0x1Cu);
}

}  // namespace esphome::jk_bms_ble::testing
