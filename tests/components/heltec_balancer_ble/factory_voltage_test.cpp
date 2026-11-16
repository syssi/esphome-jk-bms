#include <gtest/gtest.h>
#include <algorithm>
#include "common.h"
#include "frames_ek-24s4eb.h"
#include "esphome/components/logger/logger.h"

namespace esphome::heltec_balancer_ble::testing {
class FactoryDefaultsBalancer : public TestableHeltecBalancerBle {
 public:
  using HeltecBalancerBle::decode_factory_defaults_;
};

TEST(HeltecFactoryVoltageTest, LogsSecondStandardVoltageFromItsPayloadField) {
  logger::Logger logger{115200};
  auto *previous = logger::global_logger;
  logger.pre_setup();
  ::testing::internal::CaptureStdout();
  std::vector<uint8_t> frame(100, 0);
  frame[2] = 0x11;
  frame[3] = 1;
  frame[4] = 3;
  frame[6] = 100;
  frame[14] = 0xC0;
  frame[15] = 0x3F;  // 1.5 V at offset 12
  const std::string production_date = "20220531";
  std::copy(production_date.begin(), production_date.end(), frame.begin() + 72);
  FactoryDefaultsBalancer bms;
  bms.decode_factory_defaults_(frame);
  logger::global_logger = previous;
  const auto output = ::testing::internal::GetCapturedStdout();
  EXPECT_NE(output.find("Standard voltage 2: 1.500 V"), std::string::npos);
  EXPECT_NE(output.find("Production date: 20220531"), std::string::npos);
}
TEST(HeltecPowerOnCounterTest, PreservesTheUpperTwoBytes) {
  for (const uint32_t count : {65535U, 65536U, 70000U, 131071U}) {
    TestableHeltecBalancerBle bms;
    sensor::Sensor counter;
    bms.set_power_on_count_sensor(&counter);
    auto frame = ek::DEVICE_INFO_FRAME;
    for (size_t i = 0; i < 4; i++)
      frame[56 + i] = count >> (i * 8);
    bms.decode_device_info_(frame);
    EXPECT_FLOAT_EQ(counter.state, static_cast<float>(count));
  }
}
}  // namespace esphome::heltec_balancer_ble::testing
