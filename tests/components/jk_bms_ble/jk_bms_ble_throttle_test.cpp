#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk04.h"
#include "frames_jk02_24s_v10.h"

namespace esphome::jk_bms_ble::testing {

// The cell info throttle only rate-limits how often the cell info sensors get republished. It must
// not delay status_notification_received_, which stops update() from requesting cell info again.

TEST(JkBmsBleThrottleTest, Jk02StatusNotificationReceivedSetsEvenWhenThrottled) {
  TestableJkBmsBle bms;
  // millis() starts near 0 and last_cell_info_ defaults to 0, so every frame is throttled
  bms.set_throttle(1000000);

  EXPECT_FALSE(bms.status_notification_received());

  bms.decode_(CELL_INFO_JK02_24S_V10);

  EXPECT_TRUE(bms.status_notification_received());
}

TEST(JkBmsBleThrottleTest, Jk04StatusNotificationReceivedSetsEvenWhenThrottled) {
  TestableJkBmsBle bms;
  bms.set_protocol_version(PROTOCOL_VERSION_JK04);
  bms.set_throttle(1000000);

  EXPECT_FALSE(bms.status_notification_received());

  bms.decode_(CELL_INFO_JK04);

  EXPECT_TRUE(bms.status_notification_received());
}

}  // namespace esphome::jk_bms_ble::testing
