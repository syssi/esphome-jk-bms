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
  for (const auto *version : {"11.17", "11.21h", "11.284", "14.20", "15.38", "18.00", "19.27"}) {
    SCOPED_TRACE(version);
    EXPECT_EQ(JkBmsBle::jk02_protocol_version_for_software(version), PROTOCOL_VERSION_JK02_32S);
  }
}

TEST(JkBmsBleProtocolWarningTest, UnknownSoftwareHasNoExpectation) {
  for (const auto *version : {"", "unknown", "V11.2", "0.1"}) {
    SCOPED_TRACE(version);
    EXPECT_FALSE(JkBmsBle::jk02_protocol_version_for_software(version).has_value());
  }
}

}  // namespace esphome::jk_bms_ble::testing
