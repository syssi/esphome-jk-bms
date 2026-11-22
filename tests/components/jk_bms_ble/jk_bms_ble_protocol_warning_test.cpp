#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include "common.h"
#include "frames_jk02_32s_v11.h"
#include "esphome/components/logger/logger.h"

namespace esphome::jk_bms_ble::testing {

class JkBmsBleProtocolWarningTest : public ::testing::Test {
 protected:
  std::string decode_(const std::string &version, ProtocolVersion protocol) {
    auto frame = DEVICE_INFO_JK02_24S_V10;
    std::fill(frame.begin() + 22, frame.begin() + 30, 0);
    std::copy_n(version.begin(), std::min(version.size(), size_t{8}), frame.begin() + 22);
    return decode_frame_(frame, protocol);
  }

  std::string decode_frame_(const std::vector<uint8_t> &frame, ProtocolVersion protocol) {
    logger::Logger logger{115200};
    auto *previous = logger::global_logger;
    logger.pre_setup();
    ::testing::internal::CaptureStdout();
    TestableJkBmsBle bms;
    bms.set_protocol_version(protocol);
    bms.decode_device_info_(frame);
    logger::global_logger = previous;
    return ::testing::internal::GetCapturedStdout();
  }
};

TEST_F(JkBmsBleProtocolWarningTest, WarnsForNewHardwareUsingOldFrameFormat) {
  for (const auto *version : {"11A", "18H", "11.XW", "V17H"}) {
    SCOPED_TRACE(version);
    EXPECT_NE(decode_(version, PROTOCOL_VERSION_JK02_24S).find("protocol mismatch"), std::string::npos);
  }
}

TEST_F(JkBmsBleProtocolWarningTest, CapturedHardwareInfoWarnsOnlyForMismatchedConfiguration) {
  EXPECT_NE(decode_frame_(DEVICE_INFO_JK02_32S_V11, PROTOCOL_VERSION_JK02_24S).find("protocol mismatch"),
            std::string::npos);
  EXPECT_EQ(decode_frame_(DEVICE_INFO_JK02_32S_V11, PROTOCOL_VERSION_JK02_32S).find("protocol mismatch"),
            std::string::npos);
}

TEST_F(JkBmsBleProtocolWarningTest, DoesNotWarnForMatchingOrUnknownHardware) {
  for (const auto *version : {"", "unknown", "10.XG", "9.0", "V", "X11", "0"}) {
    SCOPED_TRACE(version);
    EXPECT_EQ(decode_(version, PROTOCOL_VERSION_JK02_24S).find("protocol mismatch"), std::string::npos);
  }
  EXPECT_EQ(decode_("11A", PROTOCOL_VERSION_JK02_32S).find("protocol mismatch"), std::string::npos);
  EXPECT_EQ(decode_("18H", PROTOCOL_VERSION_JK04).find("protocol mismatch"), std::string::npos);
}

}  // namespace esphome::jk_bms_ble::testing
