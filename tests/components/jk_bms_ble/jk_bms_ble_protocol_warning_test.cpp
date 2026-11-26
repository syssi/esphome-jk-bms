#include <gtest/gtest.h>
#include "common.h"

namespace esphome::jk_bms_ble::testing {

// Software versions taken from the supported devices list in the README and the captured frames
TEST(JkBmsBleProtocolWarningTest, OlderSoftwareUsesJk02_24s) {
  for (const auto *version : {"8.1.2H", "8.20G", "9.01M3", "10.02", "10.07", "10.07D30", "10.10"}) {
    SCOPED_TRACE(version);
    EXPECT_EQ(JkBmsBle::jk02_protocol_version_for_software(version), PROTOCOL_VERSION_JK02_24S);
  }
}

TEST(JkBmsBleProtocolWarningTest, NewerSoftwareUsesJk02_32s) {
  for (const auto *version : {"11.17", "11.21h", "11.284", "11.288H", "11.55", "14.20", "15.37", "15.38", "18.00",
                              "19.05", "19.07", "19.25", "19.27"}) {
    SCOPED_TRACE(version);
    EXPECT_EQ(JkBmsBle::jk02_protocol_version_for_software(version), PROTOCOL_VERSION_JK02_32S);
  }
}

// JK04 firmware shares the 8.x range with JK02_24S (e.g. JK-B5A24S sw 8.0.3M), so the helper can't tell them
// apart. decode_device_info_() skips the check if JK04 is configured.
TEST(JkBmsBleProtocolWarningTest, Jk04SoftwareIsNotDistinguished) {
  EXPECT_EQ(JkBmsBle::jk02_protocol_version_for_software("8.0.3M"), PROTOCOL_VERSION_JK02_24S);
}

TEST(JkBmsBleProtocolWarningTest, UnknownSoftwareHasNoExpectation) {
  for (const auto *version : {"", "unknown", "V11.2", "0.1"}) {
    SCOPED_TRACE(version);
    EXPECT_FALSE(JkBmsBle::jk02_protocol_version_for_software(version).has_value());
  }
}

}  // namespace esphome::jk_bms_ble::testing
