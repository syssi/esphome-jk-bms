#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_v19.h"

// Component-level tests: how JkBmsBle wires the CommandQueue and the SettingsVerification
// together. The mechanics of the classes themselves are covered directly in
// jk_bms_ble_command_queue_test.cpp.

namespace esphome::jk_bms_ble::testing {

// Register addresses used as stand-ins for a settings write, which the BMS never acknowledges.
static const uint8_t REG_WRITE = 0x31;
static const uint8_t CMD_CELL_INFO = 0x96;    // answered by frame 0x01 (settings)
static const uint8_t CMD_DEVICE_INFO = 0x97;  // answered by frame 0x03 (device info)

// ── Enqueuing ────────────────────────────────────────────────────────────────

TEST(JkBmsBleQueueTest, QueueCommandFillsInTheExpectedResponseFrame) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_CELL_INFO, 0x00000000, 0x00);

  EXPECT_EQ(bms.queue_front().expected_frame, FRAME_TYPE_SETTINGS);
}

TEST(JkBmsBleQueueTest, QueueCommandMarksRegisterWritesAsUnacknowledged) {
  TestableJkBmsBle bms;
  bms.queue_command_(REG_WRITE, 0x00000001, 0x04);

  EXPECT_EQ(bms.queue_front().expected_frame, CommandQueue::NO_RESPONSE);
}

TEST(JkBmsBleQueueTest, QueueFullDropsCommand) {
  TestableJkBmsBle bms;
  const uint8_t max_size = CommandQueue::LENGTH - 1;
  for (uint8_t i = 0; i < max_size; i++)
    EXPECT_TRUE(bms.queue_command_(REG_WRITE, i, 0x04));

  EXPECT_FALSE(bms.queue_command_(REG_WRITE, 0xAA, 0x04));
  EXPECT_EQ(bms.queue_size(), max_size);
}

// ── Only the requested response retires a command ────────────────────────────
//
// assemble() only attempts a CRC check (and, on success, decode_()) once
// MIN_RESPONSE_SIZE bytes have accumulated. The captured frames below are full
// 300-byte frames, so a single assemble() call is enough to trigger it.

TEST(JkBmsBleQueueTest, MatchingResponseAdvancesQueue) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);
  bms.queue_command_(CMD_CELL_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.assemble(DEVICE_INFO_JK02_24S_V10.data(), DEVICE_INFO_JK02_24S_V10.size());

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_FALSE(bms.command_pending());
}

TEST(JkBmsBleQueueTest, SettingsFrameAdvancesSettingsRequest) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.queue_command_(CMD_CELL_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.assemble(SETTINGS_JK02_32S_V19.data(), SETTINGS_JK02_32S_V19.size());

  EXPECT_EQ(bms.queue_size(), 0);
}

// The BMS auto-streams cell info, so those frames arrive unsolicited and must never be
// mistaken for the acknowledgement of an in-flight command.
TEST(JkBmsBleQueueTest, StreamedCellInfoDoesNotAdvanceQueue) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.assemble(CELL_INFO_JK02_24S_V10.data(), CELL_INFO_JK02_24S_V10.size());

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_TRUE(bms.command_pending());
}

// A command that was never put on the wire cannot have been answered.
TEST(JkBmsBleQueueTest, ResponseDoesNotAdvanceUnsentCommand) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);

  bms.assemble(DEVICE_INFO_JK02_24S_V10.data(), DEVICE_INFO_JK02_24S_V10.size());

  EXPECT_EQ(bms.queue_size(), 1);
}

// Register writes are answered by no frame at all; they are retired by the settle timer.
TEST(JkBmsBleQueueTest, RegisterWriteIsNotAdvancedByAnyFrame) {
  TestableJkBmsBle bms;
  bms.queue_command_(REG_WRITE, 0x00000001, 0x04);
  bms.mark_command_pending();

  for (int frame_type : {0x00, 0x01, 0x02, 0x03, 0x05})
    EXPECT_FALSE(bms.queue_on_frame((uint8_t) frame_type)) << "frame 0x" << std::hex << frame_type;

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_TRUE(bms.command_pending());
}

TEST(JkBmsBleQueueTest, BadCrcDoesNotAdvanceQueue) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  auto bad_crc = DEVICE_INFO_JK02_24S_V10;
  bad_crc.back() ^= 0xFF;
  bms.assemble(bad_crc.data(), bad_crc.size());

  EXPECT_EQ(bms.queue_size(), 1);
}

TEST(JkBmsBleQueueTest, IncompleteFrameDoesNotAdvanceQueue) {
  TestableJkBmsBle bms;
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  // Fewer than MIN_RESPONSE_SIZE bytes: assemble() buffers them but never
  // reaches the CRC check, so decode_() (and the queue advance) never runs.
  bms.assemble(DEVICE_INFO_JK02_24S_V10.data(), 50);

  EXPECT_EQ(bms.queue_size(), 1);
}

// ── loop() choreography ──────────────────────────────────────────────────────

TEST(JkBmsBleQueueTest, LoopAdvancesExpiredCommand) {
  TestableJkBmsBle bms;
  bms.set_write_settle(0);
  bms.queue_command_(REG_WRITE, 0x00000001, 0x04);
  bms.queue_command_(REG_WRITE, 0x00000002, 0x04);
  bms.mark_command_pending();

  bms.loop();

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_FALSE(bms.command_pending());
}

TEST(JkBmsBleQueueTest, LoopLeavesAnUnexpiredCommandAlone) {
  TestableJkBmsBle bms;
  bms.set_response_timeout(60000);
  bms.queue_command_(CMD_DEVICE_INFO, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.loop();

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_TRUE(bms.command_pending());
}

// ── Settings verification after a write ──────────────────────────────────────
//
// The BMS never acknowledges a settings write. write_register() therefore schedules a deferred
// settings read; the entities then publish what the BMS actually stored, so the user sees
// whether the change was accepted or rejected.

TEST(JkBmsBleQueueTest, WriteRegisterSchedulesSettingsVerification) {
  TestableJkBmsBle bms;
  EXPECT_TRUE(bms.write_register(REG_WRITE, 0x00000001, 0x04));

  EXPECT_TRUE(bms.settings_verification_pending());
}

TEST(JkBmsBleQueueTest, ReadCommandsDoNotScheduleSettingsVerification) {
  TestableJkBmsBle bms;
  // The "Retrieve settings" / "Retrieve device info" buttons go through write_register() too.
  EXPECT_TRUE(bms.write_register(CMD_CELL_INFO, 0x00000000, 0x00));
  EXPECT_TRUE(bms.write_register(CMD_DEVICE_INFO, 0x00000000, 0x00));

  EXPECT_FALSE(bms.settings_verification_pending());
}

// queue_command_() is the internal transport used by update() and the connection setup; it must
// never schedule a verification behind the caller's back.
TEST(JkBmsBleQueueTest, QueueCommandDoesNotScheduleSettingsVerification) {
  TestableJkBmsBle bms;
  bms.queue_command_(REG_WRITE, 0x00000001, 0x04);

  EXPECT_FALSE(bms.settings_verification_pending());
}

// A write that never made it into the queue will never reach the BMS, so there is nothing to
// verify either.
TEST(JkBmsBleQueueTest, RejectedWriteDoesNotScheduleSettingsVerification) {
  TestableJkBmsBle bms;
  for (uint8_t i = 0; i < CommandQueue::LENGTH - 1; i++)
    bms.queue_command_(REG_WRITE, i, 0x04);

  EXPECT_FALSE(bms.write_register(REG_WRITE, 0xAA, 0x04));
  EXPECT_FALSE(bms.settings_verification_pending());
}

TEST(JkBmsBleQueueTest, SettingsVerificationIsDeferred) {
  TestableJkBmsBle bms;
  bms.schedule_settings_verification();

  EXPECT_TRUE(bms.settings_verification_pending());
  EXPECT_EQ(bms.queue_size(), 0);

  // The delay has not elapsed yet, so nothing is queued.
  bms.loop();
  EXPECT_TRUE(bms.settings_verification_pending());
  EXPECT_EQ(bms.queue_size(), 0);

  bms.expire_settings_verification();
  bms.loop();

  EXPECT_FALSE(bms.settings_verification_pending());
  EXPECT_EQ(bms.queue_size(), 1);
}

// The verification read asks for the settings frame (0x96 is answered by frame 0x01).
TEST(JkBmsBleQueueTest, VerificationQueuesASettingsRead) {
  TestableJkBmsBle bms;
  bms.expire_settings_verification();

  bms.loop();

  ASSERT_EQ(bms.queue_size(), 1);
  EXPECT_EQ(bms.queue_front().address, CMD_CELL_INFO);
  EXPECT_EQ(bms.queue_front().expected_frame, FRAME_TYPE_SETTINGS);
}

// The verification read is itself a command, so it must not schedule another verification.
TEST(JkBmsBleQueueTest, SettingsVerificationDoesNotRetriggerItself) {
  TestableJkBmsBle bms;
  bms.expire_settings_verification();

  bms.loop();
  bms.loop();

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_FALSE(bms.settings_verification_pending());
}

// A batch of writes must collapse into a single verification read.
TEST(JkBmsBleQueueTest, SettingsVerificationCoalesces) {
  TestableJkBmsBle bms;
  for (int i = 0; i < 5; i++)
    bms.schedule_settings_verification();

  bms.expire_settings_verification();
  bms.loop();
  bms.loop();

  EXPECT_EQ(bms.queue_size(), 1);
}

TEST(JkBmsBleQueueTest, SettingsVerificationWaitsForAnIdleQueue) {
  TestableJkBmsBle bms;
  bms.queue_command_(REG_WRITE, 0x00000001, 0x04);
  bms.expire_settings_verification();

  bms.loop();

  EXPECT_TRUE(bms.settings_verification_pending());
  EXPECT_EQ(bms.queue_size(), 1);
}

}  // namespace esphome::jk_bms_ble::testing
