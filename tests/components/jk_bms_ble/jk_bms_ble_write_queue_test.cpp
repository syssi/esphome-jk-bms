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

// ── Acknowledgement ──────────────────────────────────────────────────────────

// Captured from a JK-B1A8S10P (hw 19H, sw 19.25) and the JK app btsnoop logs in docs/
static const std::vector<uint8_t> ACKNOWLEDGEMENT = {0xAA, 0x55, 0x90, 0xEB, 0xC8, 0x01, 0x01, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
static const std::vector<uint8_t> AT_LINE = {0x41, 0x54, 0x0D, 0x0A};

static bool contains_acknowledgement(const std::vector<uint8_t> &data) {
  return JkBmsBle::contains_acknowledgement(data.data(), data.size());
}

TEST(JkBmsAcknowledgementTest, StandaloneAcknowledgementIsDetected) {
  EXPECT_TRUE(contains_acknowledgement(ACKNOWLEDGEMENT));
}

static std::vector<uint8_t> concat(std::initializer_list<std::vector<uint8_t>> parts) {
  std::vector<uint8_t> result;
  for (const auto &part : parts)
    result.insert(result.end(), part.begin(), part.end());
  return result;
}

// The JK app btsnoop logs contain "AT\r\n" + acknowledgement + "AT\r\n" and acknowledgement + "AT\r\n" notifications
TEST(JkBmsAcknowledgementTest, AcknowledgementSharingANotificationIsDetected) {
  EXPECT_TRUE(contains_acknowledgement(concat({AT_LINE, ACKNOWLEDGEMENT})));
  EXPECT_TRUE(contains_acknowledgement(concat({AT_LINE, ACKNOWLEDGEMENT, AT_LINE})));
  EXPECT_TRUE(contains_acknowledgement(concat({ACKNOWLEDGEMENT, AT_LINE})));
}

// The acknowledgement of a request may follow the end of the response frame
TEST(JkBmsAcknowledgementTest, AcknowledgementBehindAFrameIsDetected) {
  std::vector<uint8_t> tail(CELL_INFO_JK02_24S_V10.end() - 50, CELL_INFO_JK02_24S_V10.end());
  EXPECT_TRUE(contains_acknowledgement(concat({tail, ACKNOWLEDGEMENT})));
}

TEST(JkBmsAcknowledgementTest, AtLineAloneIsNoAcknowledgement) {
  EXPECT_FALSE(contains_acknowledgement(AT_LINE));
  EXPECT_FALSE(contains_acknowledgement({}));
}

TEST(JkBmsAcknowledgementTest, TruncatedAcknowledgementIsRejected) {
  std::vector<uint8_t> data(ACKNOWLEDGEMENT.begin(), ACKNOWLEDGEMENT.end() - 1);
  EXPECT_FALSE(contains_acknowledgement(data));
}

TEST(JkBmsAcknowledgementTest, AcknowledgementWithWrongChecksumIsRejected) {
  std::vector<uint8_t> data = ACKNOWLEDGEMENT;
  data[19] = 0x45;
  EXPECT_FALSE(contains_acknowledgement(data));
}

// A command echo or a response frame must not release the queue
TEST(JkBmsAcknowledgementTest, OtherFramesAreNoAcknowledgement) {
  auto request = JkBmsBle::build_frame(0x96, 0x00000000, 0x00);
  EXPECT_FALSE(JkBmsBle::contains_acknowledgement(request.data(), request.size()));
  EXPECT_FALSE(contains_acknowledgement(CELL_INFO_JK02_24S_V10));
}

}  // namespace esphome::jk_bms_ble::testing
