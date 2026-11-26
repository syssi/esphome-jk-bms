#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// ── Queue contents ───────────────────────────────────────────────────────────

TEST(JkBmsWriteQueueTest, DistinctRegistersKeepTheirOrder) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x04);  // charging
  bms.enqueue_write_(0x1E, 0x00000000, 0x04);  // discharging
  bms.enqueue_write_(0x1F, 0x00000001, 0x04);  // balancer

  ASSERT_EQ(bms.pending_writes_.size(), 3u);
  EXPECT_EQ(bms.pending_writes_[0].address, 0x1D);
  EXPECT_EQ(bms.pending_writes_[1].address, 0x1E);
  EXPECT_EQ(bms.pending_writes_[2].address, 0x1F);
}

TEST(JkBmsWriteQueueTest, NewerValueReplacesPendingWriteInPlace) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x04);
  bms.enqueue_write_(0x1E, 0x00000000, 0x04);
  bms.enqueue_write_(0x1D, 0x00000000, 0x04);

  ASSERT_EQ(bms.pending_writes_.size(), 2u);
  EXPECT_EQ(bms.pending_writes_[0].address, 0x1D);
  EXPECT_EQ(bms.pending_writes_[0].value, 0x00000000u);
  EXPECT_EQ(bms.pending_writes_[1].address, 0x1E);
}

TEST(JkBmsWriteQueueTest, ReplacementUpdatesTheLength) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x01);
  bms.enqueue_write_(0x1D, 0x00000001, 0x04);

  ASSERT_EQ(bms.pending_writes_.size(), 1u);
  EXPECT_EQ(bms.pending_writes_[0].length, 0x04);
}

// A number slider or a looping automation must not grow the queue
TEST(JkBmsWriteQueueTest, RepeatedWritesToOneRegisterStayASingleEntry) {
  TestableJkBmsBle bms;
  for (uint32_t value = 0; value < 1000; value++)
    bms.enqueue_write_(0x0B, value, 0x04);  // power off voltage

  ASSERT_EQ(bms.pending_writes_.size(), 1u);
  EXPECT_EQ(bms.pending_writes_[0].value, 999u);
}

TEST(JkBmsWriteQueueTest, QueueIsBoundedByTheNumberOfRegisters) {
  TestableJkBmsBle bms;
  for (int round = 0; round < 3; round++) {
    for (int address = 0; address <= 0xFF; address++)
      bms.enqueue_write_(address, round, 0x04);
  }

  EXPECT_EQ(bms.pending_writes_.size(), 256u);
}

// ── Settings verification ────────────────────────────────────────────────────

TEST(JkBmsWriteQueueTest, ReadRequestsDoNotTriggerVerification) {
  EXPECT_FALSE(TestableJkBmsBle::is_settings_register_(0x96));  // cell info / settings
  EXPECT_FALSE(TestableJkBmsBle::is_settings_register_(0x97));  // device info
  EXPECT_FALSE(TestableJkBmsBle::is_settings_register_(0xA1));  // logbook
}

TEST(JkBmsWriteQueueTest, SettingsWritesTriggerVerification) {
  EXPECT_TRUE(TestableJkBmsBle::is_settings_register_(0x1D));  // charging switch
  EXPECT_TRUE(TestableJkBmsBle::is_settings_register_(0x2B));  // display always on
  EXPECT_TRUE(TestableJkBmsBle::is_settings_register_(0x0B));  // power off voltage
}

}  // namespace esphome::jk_bms_ble::testing
