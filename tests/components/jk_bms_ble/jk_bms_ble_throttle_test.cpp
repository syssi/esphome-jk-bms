#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk04.h"
#include "frames_jk02_32s_v19.h"

namespace esphome::jk_bms_ble::testing {

// ── Throttle vs. command queue ────────────────────────────────────────────────
//
// The cell-info throttle (CONF_THROTTLE) only rate-limits how often cell info
// sensors get republished. It must never delay status_notification_received_
// (which gates whether update() re-queues COMMAND_CELL_INFO requests) or the
// retiring of a command whose response frame arrived — both are driven by
// "a frame arrived", not by "the frame's payload got published".

// The first cell info frame is never throttled, so each test decodes one first and only
// the following frames fall within the throttle interval.
static const uint32_t THROTTLE_FOREVER_MS = 0xFFFFFFFF;

TEST(JkBmsBleThrottleTest, Jk02StatusNotificationReceivedSetsEvenWhenThrottled) {
  TestableJkBmsBle bms;
  bms.set_throttle(THROTTLE_FOREVER_MS);
  bms.decode_(CELL_INFO_JK02_24S_V10);
  bms.clear_status_notification_received();

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(bms.status_notification_received());
}

TEST(JkBmsBleThrottleTest, Jk04StatusNotificationReceivedSetsEvenWhenThrottled) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  bms.set_throttle(THROTTLE_FOREVER_MS);
  bms.decode_(CELL_INFO_JK04);
  bms.clear_status_notification_received();

  bms.decode_(CELL_INFO_JK04);

  EXPECT_TRUE(bms.status_notification_received());
}

TEST(JkBmsBleThrottleTest, ThrottledSettingsFrameStillRetiresItsRequest) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
  bms.set_throttle(THROTTLE_FOREVER_MS);
  bms.queue_command_(0x96, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.decode_(SETTINGS_JK02_32S_V19);

  EXPECT_EQ(bms.queue_size(), 0);
  EXPECT_FALSE(bms.command_pending());
}

// A throttled (or unthrottled) cell info frame is unsolicited streaming data and must
// leave the in-flight command alone.
TEST(JkBmsBleThrottleTest, ThrottledCellInfoDoesNotRetireSettingsRequest) {
  TestableJkBmsBle bms;
  bms.set_throttle(THROTTLE_FOREVER_MS);
  bms.decode_(CELL_INFO_JK02_24S_V10);
  bms.queue_command_(0x96, 0x00000000, 0x00);
  bms.mark_command_pending();

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_TRUE(bms.command_pending());
}

}  // namespace esphome::jk_bms_ble::testing
