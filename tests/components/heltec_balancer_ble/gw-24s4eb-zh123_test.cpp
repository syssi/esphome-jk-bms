#include <gtest/gtest.h>
#include <string>
#include "common.h"
#include "frames_gw-24s4eb-zh123.h"
#include "esphome/components/logger/logger.h"

namespace esphome::heltec_balancer_ble::testing {
using namespace gw_zh123;

// The factory defaults frame is only logged, so the tests assert on the log output.
class GwZh123FactoryDefaultsTest : public ::testing::Test {
 protected:
  std::string decode_and_capture_log_(void (*decode)(TestableHeltecBalancerBle &)) {
    logger::Logger logger{115200};
    auto *previous = logger::global_logger;
    logger.pre_setup();
    ::testing::internal::CaptureStdout();
    TestableHeltecBalancerBle bms;
    decode(bms);
    logger::global_logger = previous;
    return ::testing::internal::GetCapturedStdout();
  }

  std::string decode_factory_defaults_() {
    return decode_and_capture_log_(
        [](TestableHeltecBalancerBle &bms) { bms.decode_factory_defaults_(FACTORY_DEFAULTS_FRAME); });
  }
};

// ── Factory defaults frame ────────────────────────────────────────────────────

TEST_F(GwZh123FactoryDefaultsTest, StandardVoltage1) {
  EXPECT_NE(decode_factory_defaults_().find("Standard voltage 1: 1.499 V"), std::string::npos);
}

TEST_F(GwZh123FactoryDefaultsTest, StandardVoltage2) {
  EXPECT_NE(decode_factory_defaults_().find("Standard voltage 2: 1.498 V"), std::string::npos);
}

TEST_F(GwZh123FactoryDefaultsTest, PowerOnCounter) {
  EXPECT_NE(decode_factory_defaults_().find("Power on counter: 8"), std::string::npos);
}

TEST_F(GwZh123FactoryDefaultsTest, TotalRuntime) {
  EXPECT_NE(decode_factory_defaults_().find("Total runtime: 778742"), std::string::npos);
}

TEST_F(GwZh123FactoryDefaultsTest, ProductionDate) {
  EXPECT_NE(decode_factory_defaults_().find("Production date: 20220531"), std::string::npos);
}

TEST_F(GwZh123FactoryDefaultsTest, DispatchedViaFrameType) {
  const auto log = decode_and_capture_log_([](TestableHeltecBalancerBle &bms) { bms.decode_(FACTORY_DEFAULTS_FRAME); });

  EXPECT_NE(log.find("Standard voltage 2: 1.498 V"), std::string::npos);
}

}  // namespace esphome::heltec_balancer_ble::testing
