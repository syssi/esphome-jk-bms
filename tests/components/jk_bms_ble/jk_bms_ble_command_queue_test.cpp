#include <gtest/gtest.h>
#include "esphome/components/jk_bms_ble/command_queue.h"

// Direct unit tests for the standalone classes in command_queue.h. They take every timestamp as
// a parameter, so these tests control the clock explicitly and never touch millis().

namespace esphome::jk_bms_ble::testing {

// Register addresses used as stand-ins.
static const uint8_t REG_WRITE = 0x31;        // a settings write, never acknowledged
static const uint8_t CMD_DEVICE_INFO = 0x97;  // answered by frame 0x03
static const uint8_t FRAME_SETTINGS = 0x01;
static const uint8_t FRAME_CELL_INFO = 0x02;
static const uint8_t FRAME_DEVICE_INFO = 0x03;

static void enqueue_write(CommandQueue &queue, uint32_t value = 0x00000001) {
  ASSERT_TRUE(queue.enqueue(REG_WRITE, value, 0x04, CommandQueue::NO_RESPONSE));
}

static void enqueue_read(CommandQueue &queue) {
  ASSERT_TRUE(queue.enqueue(CMD_DEVICE_INFO, 0x00000000, 0x00, FRAME_DEVICE_INFO));
}

// ── Deadline ─────────────────────────────────────────────────────────────────

TEST(DeadlineTest, DisarmedByDefault) {
  Deadline deadline;
  EXPECT_FALSE(deadline.armed());
  EXPECT_FALSE(deadline.expired(0));
  EXPECT_FALSE(deadline.expired(0xFFFFFFFF));
}

TEST(DeadlineTest, ExpiresOnceTheDurationElapsed) {
  Deadline deadline;
  deadline.start(1000, 500);

  EXPECT_FALSE(deadline.expired(1000));
  EXPECT_FALSE(deadline.expired(1499));
  EXPECT_TRUE(deadline.expired(1500));
  EXPECT_TRUE(deadline.expired(90000));
}

TEST(DeadlineTest, ZeroDurationExpiresImmediately) {
  Deadline deadline;
  deadline.start(1000, 0);
  EXPECT_TRUE(deadline.expired(1000));
}

TEST(DeadlineTest, CancelDisarms) {
  Deadline deadline;
  deadline.start(1000, 500);
  deadline.cancel();

  EXPECT_FALSE(deadline.armed());
  EXPECT_FALSE(deadline.expired(2000));
}

TEST(DeadlineTest, RestartPushesTheDeadlineBack) {
  Deadline deadline;
  deadline.start(1000, 500);
  deadline.start(1400, 500);

  EXPECT_FALSE(deadline.expired(1500));
  EXPECT_TRUE(deadline.expired(1900));
}

// millis() wraps after ~49.7 days; a deadline armed shortly before the wrap must still expire.
TEST(DeadlineTest, SurvivesMillisWraparound) {
  Deadline deadline;
  deadline.start(0xFFFFFF00, 0x200);

  EXPECT_FALSE(deadline.expired(0xFFFFFFF0));
  EXPECT_FALSE(deadline.expired(0x000000FF));
  EXPECT_TRUE(deadline.expired(0x00000100));
}

// ── CommandQueue: ring buffer ────────────────────────────────────────────────

TEST(CommandQueueTest, StartsEmpty) {
  CommandQueue queue;
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
  EXPECT_FALSE(queue.pending());
}

TEST(CommandQueueTest, EnqueueGrowsAndFrontReturnsOldest) {
  CommandQueue queue;
  enqueue_write(queue, 0x11);
  enqueue_write(queue, 0x22);

  EXPECT_EQ(queue.size(), 2);
  EXPECT_EQ(queue.front().value, 0x11u);
}

TEST(CommandQueueTest, RejectsCommandsWhenFull) {
  CommandQueue queue;
  for (size_t i = 0; i < CommandQueue::LENGTH - 1; i++)
    enqueue_write(queue, i);

  EXPECT_FALSE(queue.enqueue(REG_WRITE, 0xAA, 0x04, CommandQueue::NO_RESPONSE));
  EXPECT_EQ(queue.size(), CommandQueue::LENGTH - 1);
}

TEST(CommandQueueTest, WrapsAround) {
  CommandQueue queue;
  for (uint8_t i = 0; i < 9; i++)
    enqueue_write(queue, i);
  for (int i = 0; i < 5; i++)
    queue.advance();

  EXPECT_EQ(queue.size(), 4);

  for (uint8_t i = 0; i < 5; i++)
    enqueue_write(queue, 0x10 + i);

  EXPECT_EQ(queue.size(), 9);
  EXPECT_EQ(queue.front().value, 0x05u);
}

TEST(CommandQueueTest, AdvanceRemovesHeadAndClearsPending) {
  CommandQueue queue;
  enqueue_write(queue);
  enqueue_write(queue);
  queue.mark_pending(1000);

  queue.advance();

  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.pending());
}

TEST(CommandQueueTest, AdvanceOnEmptyQueueIsANoop) {
  CommandQueue queue;
  queue.advance();
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.pending());
}

TEST(CommandQueueTest, ResetClearsEverything) {
  CommandQueue queue;
  enqueue_write(queue);
  enqueue_write(queue);
  queue.mark_pending(1000);

  queue.reset();

  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.pending());
  // Nothing pending anymore, so nothing can time out either.
  EXPECT_FALSE(queue.tick(90000).timed_out);
}

TEST(CommandQueueTest, MarkPendingOnEmptyQueueIsANoop) {
  CommandQueue queue;
  queue.mark_pending(1000);
  EXPECT_FALSE(queue.pending());
}

// ── CommandQueue: retirement by response frame ───────────────────────────────

TEST(CommandQueueTest, OnFrameRetiresTheMatchingResponse) {
  CommandQueue queue;
  enqueue_read(queue);
  queue.mark_pending(1000);

  EXPECT_TRUE(queue.on_frame(FRAME_DEVICE_INFO));
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.pending());
}

TEST(CommandQueueTest, OnFrameIgnoresUnsolicitedFrames) {
  CommandQueue queue;
  enqueue_read(queue);
  queue.mark_pending(1000);

  EXPECT_FALSE(queue.on_frame(FRAME_CELL_INFO));
  EXPECT_FALSE(queue.on_frame(FRAME_SETTINGS));
  EXPECT_EQ(queue.size(), 1);
  EXPECT_TRUE(queue.pending());
}

// A command that was never put on the wire cannot have been answered.
TEST(CommandQueueTest, OnFrameIgnoresUnsentCommands) {
  CommandQueue queue;
  enqueue_read(queue);

  EXPECT_FALSE(queue.on_frame(FRAME_DEVICE_INFO));
  EXPECT_EQ(queue.size(), 1);
}

// NO_RESPONSE is 0x00; even a (hypothetical) frame type 0x00 must not retire a register write.
TEST(CommandQueueTest, NoFrameEverRetiresARegisterWrite) {
  CommandQueue queue;
  enqueue_write(queue);
  queue.mark_pending(1000);

  for (int frame_type : {0x00, 0x01, 0x02, 0x03, 0x05})
    EXPECT_FALSE(queue.on_frame((uint8_t) frame_type)) << "frame 0x" << std::hex << frame_type;

  EXPECT_EQ(queue.size(), 1);
  EXPECT_TRUE(queue.pending());
}

// ── CommandQueue: retirement by deadline ─────────────────────────────────────

TEST(CommandQueueTest, TickIsIdleBeforeTheDeadline) {
  CommandQueue queue;
  queue.set_response_timeout(2000);
  enqueue_read(queue);
  queue.mark_pending(1000);

  EXPECT_FALSE(queue.tick(2999).timed_out);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_TRUE(queue.pending());
}

TEST(CommandQueueTest, TickReportsAResponseTimeout) {
  CommandQueue queue;
  queue.set_response_timeout(2000);
  enqueue_read(queue);
  queue.mark_pending(1000);

  const auto retired = queue.tick(3000);

  EXPECT_TRUE(retired.timed_out);
  EXPECT_EQ(retired.address, CMD_DEVICE_INFO);
  EXPECT_TRUE(queue.empty());
  EXPECT_FALSE(queue.pending());
}

// Settle-time expiry is how register writes normally retire; it is not a timeout.
TEST(CommandQueueTest, TickRetiresASettledWriteSilently) {
  CommandQueue queue;
  queue.set_write_settle(100);
  enqueue_write(queue);
  queue.mark_pending(1000);

  const auto retired = queue.tick(1100);

  EXPECT_FALSE(retired.timed_out);
  EXPECT_TRUE(queue.empty());
}

TEST(CommandQueueTest, WriteSettleDoesNotApplyToResponseCommands) {
  CommandQueue queue;
  queue.set_write_settle(0);
  queue.set_response_timeout(60000);
  enqueue_read(queue);
  queue.mark_pending(1000);

  EXPECT_FALSE(queue.tick(1000).timed_out);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_TRUE(queue.pending());
}

TEST(CommandQueueTest, TickIgnoresUnsentCommands) {
  CommandQueue queue;
  queue.set_write_settle(0);
  enqueue_write(queue);

  EXPECT_FALSE(queue.tick(90000).timed_out);
  EXPECT_EQ(queue.size(), 1);
}

// ── SettingsVerification ─────────────────────────────────────────────────────

TEST(SettingsVerificationTest, IdleByDefault) {
  SettingsVerification verification;
  EXPECT_FALSE(verification.pending());
  EXPECT_FALSE(verification.take_if_due(0xFFFFFFFF));
}

TEST(SettingsVerificationTest, ScheduleArms) {
  SettingsVerification verification;
  verification.schedule(1000);
  EXPECT_TRUE(verification.pending());
}

TEST(SettingsVerificationTest, NotDueBeforeTheDelayElapsed) {
  SettingsVerification verification;
  verification.set_delay(500);
  verification.schedule(1000);

  EXPECT_FALSE(verification.take_if_due(1499));
  EXPECT_TRUE(verification.pending());
}

// take_if_due() fires exactly once per schedule(): the verification read must not retrigger.
TEST(SettingsVerificationTest, FiresExactlyOnce) {
  SettingsVerification verification;
  verification.set_delay(500);
  verification.schedule(1000);

  EXPECT_TRUE(verification.take_if_due(1500));
  EXPECT_FALSE(verification.pending());
  EXPECT_FALSE(verification.take_if_due(1500));
  EXPECT_FALSE(verification.take_if_due(90000));
}

// Every schedule() pushes the deadline back, so a batch of writes ends in a single read.
TEST(SettingsVerificationTest, ReschedulingCoalesces) {
  SettingsVerification verification;
  verification.set_delay(500);
  verification.schedule(1000);
  verification.schedule(1400);

  EXPECT_FALSE(verification.take_if_due(1500));
  EXPECT_TRUE(verification.take_if_due(1900));
}

TEST(SettingsVerificationTest, CancelDisarms) {
  SettingsVerification verification;
  verification.schedule(1000);
  verification.cancel();

  EXPECT_FALSE(verification.pending());
  EXPECT_FALSE(verification.take_if_due(90000));
}

}  // namespace esphome::jk_bms_ble::testing
