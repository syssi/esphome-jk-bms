#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// ── Queue contents ───────────────────────────────────────────────────────────

TEST(JkBmsWriteQueueTest, DistinctRegistersKeepTheirOrder) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS);  // charging
  bms.enqueue_write_(0x1E, 0x00000000, 0x04, VERIFY_SETTINGS);  // discharging
  bms.enqueue_write_(0x1F, 0x00000001, 0x04, VERIFY_SETTINGS);  // balancer

  ASSERT_EQ(bms.pending_writes_.size(), 3u);
  EXPECT_EQ(bms.pending_writes_[0].address, 0x1D);
  EXPECT_EQ(bms.pending_writes_[1].address, 0x1E);
  EXPECT_EQ(bms.pending_writes_[2].address, 0x1F);
}

TEST(JkBmsWriteQueueTest, NewerValueReplacesPendingWriteInPlace) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS);
  bms.enqueue_write_(0x1E, 0x00000000, 0x04, VERIFY_SETTINGS);
  bms.enqueue_write_(0x1D, 0x00000000, 0x04, VERIFY_SETTINGS);

  ASSERT_EQ(bms.pending_writes_.size(), 2u);
  EXPECT_EQ(bms.pending_writes_[0].address, 0x1D);
  EXPECT_EQ(bms.pending_writes_[0].value, 0x00000000u);
  EXPECT_EQ(bms.pending_writes_[1].address, 0x1E);
}

TEST(JkBmsWriteQueueTest, ReplacementUpdatesTheLength) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x1D, 0x00000001, 0x01, VERIFY_SETTINGS);
  bms.enqueue_write_(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS);

  ASSERT_EQ(bms.pending_writes_.size(), 1u);
  EXPECT_EQ(bms.pending_writes_[0].length, 0x04);
}

// A number slider or a looping automation must not grow the queue
TEST(JkBmsWriteQueueTest, RepeatedWritesToOneRegisterStayASingleEntry) {
  TestableJkBmsBle bms;
  for (uint32_t value = 0; value < 1000; value++)
    bms.enqueue_write_(0x0B, value, 0x04, VERIFY_SETTINGS);  // power off voltage

  ASSERT_EQ(bms.pending_writes_.size(), 1u);
  EXPECT_EQ(bms.pending_writes_[0].value, 999u);
}

TEST(JkBmsWriteQueueTest, QueueIsBoundedByTheNumberOfRegisters) {
  TestableJkBmsBle bms;
  for (int round = 0; round < 3; round++) {
    for (int address = 0; address <= 0xFF; address++)
      bms.enqueue_write_(address, round, 0x04, VERIFY_SETTINGS);
  }

  EXPECT_EQ(bms.pending_writes_.size(), 256u);
}

// ── Verification ─────────────────────────────────────────────────────────────

TEST(JkBmsWriteQueueTest, PendingWriteKeepsItsVerifyFlags) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0xA5, 0x00000001, 0x02, VERIFY_DEVICE_INFO);  // UART1 protocol
  bms.enqueue_write_(0x6B, 0x00000001, 0x04, VERIFY_NONE);         // emergency

  ASSERT_EQ(bms.pending_writes_.size(), 2u);
  EXPECT_EQ(bms.pending_writes_[0].verify, VERIFY_DEVICE_INFO);
  EXPECT_EQ(bms.pending_writes_[1].verify, VERIFY_NONE);
}

TEST(JkBmsWriteQueueTest, ReplacementUpdatesTheVerifyFlags) {
  TestableJkBmsBle bms;
  bms.enqueue_write_(0x9D, 0x00000000, 0x00, VERIFY_SETTINGS);
  bms.enqueue_write_(0x9D, 0x00000000, 0x00, VERIFY_SETTINGS | VERIFY_DEVICE_INFO);

  ASSERT_EQ(bms.pending_writes_.size(), 1u);
  EXPECT_EQ(bms.pending_writes_[0].verify, VERIFY_SETTINGS | VERIFY_DEVICE_INFO);
}

}  // namespace esphome::jk_bms_ble::testing
