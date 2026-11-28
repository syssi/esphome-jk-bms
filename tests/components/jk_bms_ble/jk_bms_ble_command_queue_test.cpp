#include <gtest/gtest.h>
#include <algorithm>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

using Command = CommandQueue::Command;

// ── Queue contents ───────────────────────────────────────────────────────────

static std::vector<uint8_t> addresses(const CommandQueue &queue) {
  std::vector<uint8_t> result;
  for (const auto &command : queue.pending())
    result.push_back(command.address);
  return result;
}

TEST(JkBmsCommandQueueTest, DistinctRegistersKeepTheirOrder) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS));  // charging
  queue.enqueue(Command::with_value(0x1E, 0x00000000, 0x04, VERIFY_SETTINGS));  // discharging
  queue.enqueue(Command::with_value(0x1F, 0x00000001, 0x04, VERIFY_SETTINGS));  // balancer

  EXPECT_EQ(addresses(queue), (std::vector<uint8_t>{0x1D, 0x1E, 0x1F}));
}

TEST(JkBmsCommandQueueTest, NewerValueReplacesPendingWriteInPlace) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS));
  queue.enqueue(Command::with_value(0x1E, 0x00000000, 0x04, VERIFY_SETTINGS));
  queue.enqueue(Command::with_value(0x1D, 0x00000000, 0x01, VERIFY_SETTINGS | VERIFY_DEVICE_INFO));

  ASSERT_EQ(addresses(queue), (std::vector<uint8_t>{0x1D, 0x1E}));
  EXPECT_EQ(queue.pending()[0].payload, Command::with_value(0x1D, 0x00000000, 0x01, VERIFY_NONE).payload);
  EXPECT_EQ(queue.pending()[0].length, 0x01);
  EXPECT_EQ(queue.pending()[0].verify, VERIFY_SETTINGS | VERIFY_DEVICE_INFO);
}

// A number slider or a looping automation must not grow the queue
TEST(JkBmsCommandQueueTest, RepeatedWritesToOneRegisterStayASingleEntry) {
  CommandQueue queue;
  for (uint32_t value = 0; value < 1000; value++)
    queue.enqueue(Command::with_value(0x0B, value, 0x04, VERIFY_SETTINGS));  // power off voltage

  ASSERT_EQ(queue.pending().size(), 1u);
  EXPECT_EQ(queue.pending()[0].payload, Command::with_value(0x0B, 999, 0x04, VERIFY_NONE).payload);
}

TEST(JkBmsCommandQueueTest, QueueIsBoundedByTheNumberOfRegisters) {
  CommandQueue queue;
  for (int round = 0; round < 3; round++) {
    for (int address = 0; address <= 0xFF; address++)
      queue.enqueue(Command::with_value((uint8_t) address, (uint32_t) round, 0x04, VERIFY_SETTINGS));
  }

  EXPECT_EQ(queue.pending().size(), 256u);
}

// ── Sequence ─────────────────────────────────────────────────────────────────

static constexpr uint32_t T0 = 1000;

TEST(JkBmsCommandQueueTest, FirstWriteIsSentRightAway) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS));

  EXPECT_EQ(queue.next_deadline(T0), 0u);
  auto command = queue.next(T0);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->address, 0x1D);
  EXPECT_EQ(queue.awaited_command(), 0x1D);
}

TEST(JkBmsCommandQueueTest, NextWriteWaitsForTheAcknowledgementAndTheSettleTime) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_NONE));
  queue.enqueue(Command::with_value(0x1E, 0x00000001, 0x04, VERIFY_NONE));
  ASSERT_TRUE(queue.next(T0).has_value());

  EXPECT_FALSE(queue.next(T0 + 50).has_value());
  EXPECT_EQ(queue.on_acknowledgement(T0 + 80), 80u);

  // Settling: nothing is sent until SETTLE_MS after the acknowledgement
  EXPECT_EQ(queue.next_deadline(T0 + 80), CommandQueue::SETTLE_MS);
  EXPECT_FALSE(queue.expire(T0 + 80 + CommandQueue::SETTLE_MS - 1).has_value());
  EXPECT_FALSE(queue.next(T0 + 80 + CommandQueue::SETTLE_MS - 1).has_value());

  EXPECT_FALSE(queue.expire(T0 + 80 + CommandQueue::SETTLE_MS).has_value());
  auto command = queue.next(T0 + 80 + CommandQueue::SETTLE_MS);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->address, 0x1E);
}

// Devices may lose or not send an acknowledgement: the timeout releases the queue
TEST(JkBmsCommandQueueTest, MissingAcknowledgementTimesOut) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0xB3, 0x00000005, 0x01, VERIFY_NONE));
  queue.enqueue(Command::with_value(0xB7, 0x00000001, 0x01, VERIFY_NONE));
  ASSERT_TRUE(queue.next(T0).has_value());

  EXPECT_EQ(queue.next_deadline(T0), CommandQueue::ACKNOWLEDGEMENT_TIMEOUT_MS);
  EXPECT_FALSE(queue.expire(T0 + CommandQueue::ACKNOWLEDGEMENT_TIMEOUT_MS - 1).has_value());

  EXPECT_EQ(queue.expire(T0 + CommandQueue::ACKNOWLEDGEMENT_TIMEOUT_MS), 0xB3);
  auto command = queue.next(T0 + CommandQueue::ACKNOWLEDGEMENT_TIMEOUT_MS);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->address, 0xB7);
}

TEST(JkBmsCommandQueueTest, UnexpectedAcknowledgementIsIgnored) {
  CommandQueue queue;
  EXPECT_FALSE(queue.on_acknowledgement(T0).has_value());

  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_NONE));
  ASSERT_TRUE(queue.next(T0).has_value());
  ASSERT_TRUE(queue.on_acknowledgement(T0 + 80).has_value());
  // A second acknowledgement must not shorten the settle time
  EXPECT_FALSE(queue.on_acknowledgement(T0 + 90).has_value());
  EXPECT_EQ(queue.next_deadline(T0 + 90), CommandQueue::SETTLE_MS - 10);
}

TEST(JkBmsCommandQueueTest, IdleQueueHasNoDeadline) {
  CommandQueue queue;
  EXPECT_FALSE(queue.next_deadline(T0).has_value());

  queue.enqueue(Command::with_value(0x96, 0x00000000, 0x00, VERIFY_NONE));
  ASSERT_TRUE(queue.next(T0).has_value());
  ASSERT_TRUE(queue.on_acknowledgement(T0 + 300).has_value());
  queue.expire(T0 + 300 + CommandQueue::SETTLE_MS);
  EXPECT_FALSE(queue.next_deadline(T0 + 300 + CommandQueue::SETTLE_MS).has_value());
}

// The millis() clock wraps after 49.7 days
TEST(JkBmsCommandQueueTest, DeadlinesSurviveTheClockWrapping) {
  CommandQueue queue;
  const uint32_t before_wrap = 0xFFFFFFFFu - 20;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_NONE));
  ASSERT_TRUE(queue.next(before_wrap).has_value());

  const uint32_t after_wrap = before_wrap + CommandQueue::ACKNOWLEDGEMENT_TIMEOUT_MS;  // wraps around
  EXPECT_FALSE(queue.expire(after_wrap - 1).has_value());
  EXPECT_EQ(queue.expire(after_wrap), 0x1D);
}

// ── Verification ─────────────────────────────────────────────────────────────

TEST(JkBmsCommandQueueTest, VerificationIsDueAfterTheDelay) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x01, 3500, 0x04, VERIFY_SETTINGS));  // smart sleep voltage
  ASSERT_TRUE(queue.next(T0).has_value());

  EXPECT_EQ(queue.take_due_verifications(T0 + CommandQueue::VERIFICATION_DELAY_MS - 1), VERIFY_NONE);
  EXPECT_EQ(queue.take_due_verifications(T0 + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_SETTINGS);
  // Each verification is returned once
  EXPECT_EQ(queue.take_due_verifications(T0 + CommandQueue::VERIFICATION_DELAY_MS + 1), VERIFY_NONE);
}

// The readback starts when the write is sent, not when it is queued
TEST(JkBmsCommandQueueTest, VerificationStartsWhenTheWriteIsSent) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_NONE));
  queue.enqueue(Command::with_value(0xA5, 0x00000001, 0x02, VERIFY_DEVICE_INFO));  // UART1 protocol
  ASSERT_TRUE(queue.next(T0).has_value());
  ASSERT_TRUE(queue.on_acknowledgement(T0 + 80).has_value());
  queue.expire(T0 + 180);
  ASSERT_TRUE(queue.next(T0 + 180).has_value());

  EXPECT_EQ(queue.take_due_verifications(T0 + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_NONE);
  EXPECT_EQ(queue.take_due_verifications(T0 + 180 + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_DEVICE_INFO);
}

// Every write restarts the timer of its frame, so a batch of writes results in a single read per frame
TEST(JkBmsCommandQueueTest, BatchOfWritesCoalescesIntoOneReadPerFrame) {
  CommandQueue queue;
  uint32_t now = T0;
  for (uint8_t address : {0x01, 0x0B, 0xA5, 0x1D}) {
    queue.enqueue(Command::with_value(address, 0x00000001, 0x04,
                                      address == 0xA5 ? (uint8_t) VERIFY_DEVICE_INFO : (uint8_t) VERIFY_SETTINGS));
    ASSERT_TRUE(queue.next(now).has_value());
    ASSERT_TRUE(queue.on_acknowledgement(now + 80).has_value());
    now += 80 + CommandQueue::SETTLE_MS;
    queue.expire(now);
    EXPECT_EQ(queue.take_due_verifications(now), VERIFY_NONE);
  }
  // 0xA5 was the third write, 0x1D the last one
  const uint32_t a5_sent = T0 + 2 * (80 + CommandQueue::SETTLE_MS);
  const uint32_t last_sent = T0 + 3 * (80 + CommandQueue::SETTLE_MS);
  EXPECT_EQ(queue.take_due_verifications(a5_sent + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_DEVICE_INFO);
  EXPECT_EQ(queue.take_due_verifications(last_sent + CommandQueue::VERIFICATION_DELAY_MS - 1), VERIFY_NONE);
  EXPECT_EQ(queue.take_due_verifications(last_sent + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_SETTINGS);
}

TEST(JkBmsCommandQueueTest, ArmedVerificationSetsTheDeadline) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x01, 3500, 0x04, VERIFY_SETTINGS));
  ASSERT_TRUE(queue.next(T0).has_value());
  ASSERT_TRUE(queue.on_acknowledgement(T0 + 80).has_value());
  queue.expire(T0 + 180);

  EXPECT_EQ(queue.next_deadline(T0 + 180), CommandQueue::VERIFICATION_DELAY_MS - 180);
}

// Pending writes and readbacks of a lost connection must not be sent after a reconnect
TEST(JkBmsCommandQueueTest, ResetDropsEverything) {
  CommandQueue queue;
  queue.enqueue(Command::with_value(0x01, 3500, 0x04, VERIFY_SETTINGS));
  queue.enqueue(Command::with_value(0x1D, 0x00000001, 0x04, VERIFY_SETTINGS));
  ASSERT_TRUE(queue.next(T0).has_value());

  queue.reset();

  EXPECT_TRUE(queue.pending().empty());
  EXPECT_FALSE(queue.next_deadline(T0 + 1).has_value());
  EXPECT_EQ(queue.take_due_verifications(T0 + CommandQueue::VERIFICATION_DELAY_MS), VERIFY_NONE);
  queue.enqueue(Command::with_value(0x97, 0x00000000, 0x00, VERIFY_NONE));
  EXPECT_TRUE(queue.next(T0 + 2).has_value());
}

// ── Payload ──────────────────────────────────────────────────────────────────

TEST(JkBmsCommandQueueTest, ValueIsStoredLittleEndianInFrontOfTheLengthByte) {
  const Command command = Command::with_value(0x0B, 0x12345678, 0x02, VERIFY_SETTINGS);
  EXPECT_EQ(command.length, 0x02);
  EXPECT_EQ(command.payload[0], 0x78);
  EXPECT_EQ(command.payload[1], 0x56);
  EXPECT_EQ(command.payload[2], 0x34);
  EXPECT_EQ(command.payload[3], 0x12);
  for (size_t i = 4; i < CommandQueue::MAX_PAYLOAD_SIZE; i++)
    EXPECT_EQ(command.payload[i], 0x00) << i;
}

TEST(JkBmsCommandQueueTest, RawPayloadSetsTheLengthByte) {
  const uint8_t text[] = {'6', '5', '4', '3', '2', '1'};
  auto command = Command::with_payload(0xA0, text, sizeof(text), VERIFY_DEVICE_INFO);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->address, 0xA0);
  EXPECT_EQ(command->length, 6);
  EXPECT_EQ(command->verify, VERIFY_DEVICE_INFO);
  EXPECT_TRUE(std::equal(std::begin(text), std::end(text), command->payload.begin()));
  for (size_t i = sizeof(text); i < CommandQueue::MAX_PAYLOAD_SIZE; i++)
    EXPECT_EQ(command->payload[i], 0x00) << i;
}

TEST(JkBmsCommandQueueTest, PayloadFillsTheFrame) {
  const uint8_t data[CommandQueue::MAX_PAYLOAD_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  auto command = Command::with_payload(0xA0, data, sizeof(data), VERIFY_NONE);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->length, 13);
  EXPECT_EQ(command->payload[12], 13);
}

TEST(JkBmsCommandQueueTest, PayloadLongerThanTheFrameIsRejected) {
  const uint8_t data[CommandQueue::MAX_PAYLOAD_SIZE + 1] = {};
  EXPECT_FALSE(Command::with_payload(0xA0, data, sizeof(data), VERIFY_NONE).has_value());
}

TEST(JkBmsCommandQueueTest, EmptyPayloadIsAllowed) {
  auto command = Command::with_payload(0x97, nullptr, 0, VERIFY_NONE);
  ASSERT_TRUE(command.has_value());
  EXPECT_EQ(command->length, 0);
}

// The last value wins for raw payloads too, including a shorter one
TEST(JkBmsCommandQueueTest, NewerPayloadReplacesPendingOne) {
  CommandQueue queue;
  const uint8_t first[] = {'6', '5', '4', '3', '2', '1', '0'};
  const uint8_t second[] = {'1', '2', '3'};
  auto first_command = Command::with_payload(0xA0, first, sizeof(first), VERIFY_DEVICE_INFO);
  auto second_command = Command::with_payload(0xA0, second, sizeof(second), VERIFY_DEVICE_INFO);
  ASSERT_TRUE(first_command.has_value());
  ASSERT_TRUE(second_command.has_value());
  queue.enqueue(*first_command);
  queue.enqueue(*second_command);

  ASSERT_EQ(queue.pending().size(), 1u);
  EXPECT_EQ(queue.pending()[0].length, 3);
  EXPECT_EQ(queue.pending()[0].payload[2], '3');
  EXPECT_EQ(queue.pending()[0].payload[3], 0x00);
}

// ── Acknowledgement ──────────────────────────────────────────────────────────

// Captured from a JK-B1A8S10P (hw 19H, sw 19.25) and the JK app btsnoop logs in docs/
static const std::vector<uint8_t> ACKNOWLEDGEMENT = {0xAA, 0x55, 0x90, 0xEB, 0xC8, 0x01, 0x01, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
static const std::vector<uint8_t> AT_LINE = {0x41, 0x54, 0x0D, 0x0A};

static bool contains_acknowledgement(const std::vector<uint8_t> &data) {
  return JkBmsBle::find_acknowledgement(data.data(), data.size()) == ACKNOWLEDGEMENT_STATUS_OK;
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
  EXPECT_FALSE(JkBmsBle::find_acknowledgement(request.data(), request.size()).has_value());
  EXPECT_FALSE(contains_acknowledgement(CELL_INFO_JK02_24S_V10));
}

// ── Acknowledgement status ───────────────────────────────────────────────────

// Recorded on a JK02_32S (hw 19) after a 0x9B write with length byte 15, about 40 ms after the write. The BMS kept the
// stored name.
static const std::vector<uint8_t> NEGATIVE_ACKNOWLEDGEMENT = {0xAA, 0x55, 0x90, 0xEB, 0xC8, 0x01, 0x00,
                                                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                              0x00, 0x00, 0x00, 0x00, 0x00, 0x43};

static optional<uint8_t> find_acknowledgement(const std::vector<uint8_t> &data) {
  return JkBmsBle::find_acknowledgement(data.data(), data.size());
}

TEST(JkBmsAcknowledgementStatusTest, AcknowledgementIsOk) {
  EXPECT_EQ(find_acknowledgement(ACKNOWLEDGEMENT), ACKNOWLEDGEMENT_STATUS_OK);
}

TEST(JkBmsAcknowledgementStatusTest, NegativeAcknowledgementIsRejected) {
  EXPECT_EQ(find_acknowledgement(NEGATIVE_ACKNOWLEDGEMENT), ACKNOWLEDGEMENT_STATUS_REJECTED);
  EXPECT_EQ(find_acknowledgement(concat({AT_LINE, NEGATIVE_ACKNOWLEDGEMENT, AT_LINE})),
            ACKNOWLEDGEMENT_STATUS_REJECTED);
}

// Any status with a valid checksum ends the wait, also one never seen so far
TEST(JkBmsAcknowledgementStatusTest, UnknownStatusIsReturned) {
  std::vector<uint8_t> data = ACKNOWLEDGEMENT;
  data[6] = 0x02;
  data[19] = 0x45;
  EXPECT_EQ(find_acknowledgement(data), 0x02);
}

TEST(JkBmsAcknowledgementStatusTest, NegativeAcknowledgementWithWrongChecksumIsIgnored) {
  std::vector<uint8_t> data = NEGATIVE_ACKNOWLEDGEMENT;
  data[19] = 0x44;
  EXPECT_FALSE(find_acknowledgement(data).has_value());
}

// The acknowledgement is found behind an earlier command frame in the same notification
TEST(JkBmsAcknowledgementStatusTest, AcknowledgementBehindACommandFrameIsFound) {
  auto request = JkBmsBle::build_frame(0x96, 0x00000000, 0x00);
  std::vector<uint8_t> data(request.begin(), request.end());
  EXPECT_EQ(find_acknowledgement(concat({data, NEGATIVE_ACKNOWLEDGEMENT})), ACKNOWLEDGEMENT_STATUS_REJECTED);
}

// A preamble inside a truncated frame must not hide the acknowledgement behind it
TEST(JkBmsAcknowledgementStatusTest, AcknowledgementBehindAStrayPreambleIsFound) {
  const std::vector<uint8_t> stray = {0xAA, 0x55, 0x90, 0xEB, 0xC8};
  EXPECT_EQ(find_acknowledgement(concat({stray, ACKNOWLEDGEMENT})), ACKNOWLEDGEMENT_STATUS_OK);
}

TEST(JkBmsAcknowledgementStatusTest, OtherContentHasNoStatus) {
  EXPECT_FALSE(find_acknowledgement(AT_LINE).has_value());
  EXPECT_FALSE(find_acknowledgement({}).has_value());
  EXPECT_FALSE(find_acknowledgement(CELL_INFO_JK02_24S_V10).has_value());
}

}  // namespace esphome::jk_bms_ble::testing
