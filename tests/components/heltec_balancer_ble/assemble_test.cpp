#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include "common.h"
#include "frames_ek-24s4eb.h"
#include "frames_gw-24s4eb.h"

namespace esphome::heltec_balancer_ble::testing {

// ── Single fragment (MTU large enough for the whole frame) ────────────────────

TEST(AssembleTest, SingleFragmentDecodesCellInfo) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());

  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

TEST(AssembleTest, CorruptedCrcIsRejected) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> corrupted = ek::CELL_INFO_FRAME;
  corrupted[corrupted.size() - 2] ^= 0xFF;  // flip the CRC byte

  bms.assemble(corrupted.data(), corrupted.size());

  EXPECT_TRUE(std::isnan(total.state));  // never decoded, sensor keeps its unset state
}

// ── Small-MTU fragmentation ─────────────────────────────────────────────────
//
// Regression coverage for https://github.com/syssi/esphome-jk-bms/issues/1031: a poor BLE
// connection can force the peripheral down to a tiny MTU, so a single 300-byte cell info
// frame arrives as many short notifications. The old assemble() treated a frame as complete
// as soon as the accumulated buffer happened to end on 0xFF (END_OF_FRAME) -- but 0xFF also
// occurs as an ordinary data byte (byte 137 of this real EK-24S4EB capture sits inside a cell
// resistance float), so a fragment boundary landing there produced a bogus "CRC check failed"
// and silently dropped the whole frame.

TEST(AssembleTest, StrayFFByteInsidePayloadDoesNotTerminateFrameEarly) {
  ASSERT_EQ(ek::CELL_INFO_FRAME[137], 0xFF);  // the byte that used to trigger the bug

  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  // Deliver byte-by-byte: the buffer's last byte equals 0xFF the moment byte 137 arrives
  // (buffer size 138), long before the real 300-byte frame is complete.
  for (uint8_t b : ek::CELL_INFO_FRAME) {
    bms.assemble(&b, 1);
  }

  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

TEST(AssembleTest, IncompleteFrameDoesNotDecodeYet) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  bms.assemble(ek::CELL_INFO_FRAME.data(), 138);  // header + the stray 0xFF byte, nothing more
  EXPECT_TRUE(std::isnan(total.state));           // still waiting for the remaining 162 bytes

  bms.assemble(ek::CELL_INFO_FRAME.data() + 138, ek::CELL_INFO_FRAME.size() - 138);
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

TEST(AssembleTest, RealisticSmallMtuFragmentsReassembleEkCellInfo) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor cell1, total, temp1;
  bms.set_cell_voltage_sensor(0, &cell1);
  bms.set_total_voltage_sensor(&total);
  bms.set_temperature_sensor_1_sensor(&temp1);

  const size_t mtu_payload = 20;  // matches the real GW-24S4EB btsnoop capture chunk size
  const auto &frame = ek::CELL_INFO_FRAME;
  for (size_t offset = 0; offset < frame.size(); offset += mtu_payload) {
    size_t chunk = std::min(mtu_payload, frame.size() - offset);
    bms.assemble(&frame[offset], chunk);
  }

  EXPECT_NEAR(cell1.state, 3.325f, 0.001f);
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
  EXPECT_NEAR(temp1.state, 25.71f, 0.01f);
}

TEST(AssembleTest, RealisticSmallMtuFragmentsReassembleGwCellInfo) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  const size_t mtu_payload = 20;
  const auto &frame = gw::CELL_INFO_FRAME;
  for (size_t offset = 0; offset < frame.size(); offset += mtu_payload) {
    size_t chunk = std::min(mtu_payload, frame.size() - offset);
    bms.assemble(&frame[offset], chunk);
  }

  EXPECT_NEAR(total.state, 53.16f, 0.01f);
}

TEST(AssembleTest, OddSizedFragmentsReassembleCellInfo) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  // Fragment sizes that don't evenly divide the frame and don't align with byte 137.
  const size_t mtu_payload = 23;
  const auto &frame = ek::CELL_INFO_FRAME;
  for (size_t offset = 0; offset < frame.size(); offset += mtu_payload) {
    size_t chunk = std::min(mtu_payload, frame.size() - offset);
    bms.assemble(&frame[offset], chunk);
  }

  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Invalid length field ──────────────────────────────────────────────────────

TEST(AssembleTest, LengthFieldBelowMinimumIsDropped) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> broken = ek::CELL_INFO_FRAME;
  broken[6] = 0x05;
  broken[7] = 0x00;

  bms.assemble(broken.data(), broken.size());
  EXPECT_TRUE(std::isnan(total.state));

  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

TEST(AssembleTest, LengthFieldAboveMaximumIsDropped) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> broken = ek::CELL_INFO_FRAME;
  broken[6] = 0x00;
  broken[7] = 0x04;

  bms.assemble(broken.data(), broken.size());
  EXPECT_TRUE(std::isnan(total.state));

  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Missing end of frame marker ───────────────────────────────────────────────

TEST(AssembleTest, MissingEndOfFrameMarkerIsDropped) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> broken = ek::CELL_INFO_FRAME;
  broken.back() = 0x00;

  bms.assemble(broken.data(), broken.size());
  EXPECT_TRUE(std::isnan(total.state));

  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Trailing bytes of the next frame are retained ─────────────────────────────

TEST(AssembleTest, FragmentSpanningTwoFramesDecodesBoth) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime, total;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_voltage_sensor(&total);

  std::vector<uint8_t> stream = ek::DEVICE_INFO_FRAME;
  stream.insert(stream.end(), ek::CELL_INFO_FRAME.begin(), ek::CELL_INFO_FRAME.end());

  const size_t mtu_payload = 64;
  for (size_t offset = 0; offset < stream.size(); offset += mtu_payload) {
    size_t chunk = std::min(mtu_payload, stream.size() - offset);
    bms.assemble(&stream[offset], chunk);
  }

  EXPECT_FLOAT_EQ(runtime.state, 127854.0f);
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Resync after a lost frame ─────────────────────────────────────────────────

TEST(AssembleTest, FreshPreambleResyncsAfterAbandonedFrame) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor total;
  bms.set_total_voltage_sensor(&total);

  // Half of a cell info frame is lost mid-transmission (e.g. a BLE disconnect glitch)...
  bms.assemble(ek::CELL_INFO_FRAME.data(), 100);
  // ...then a new, unrelated frame starts. The stale half must be discarded, not prepended.
  bms.assemble(ek::DEVICE_INFO_FRAME.data(), ek::DEVICE_INFO_FRAME.size());
  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());

  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

// ── Multiple frames in sequence (as dispatched on ESP32) ──────────────────────

TEST(AssembleTest, DeviceInfoThenCellInfoBothDecode) {
  TestableHeltecBalancerBle bms;
  sensor::Sensor runtime, total;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_voltage_sensor(&total);

  bms.assemble(ek::DEVICE_INFO_FRAME.data(), ek::DEVICE_INFO_FRAME.size());
  bms.assemble(ek::CELL_INFO_FRAME.data(), ek::CELL_INFO_FRAME.size());

  EXPECT_FLOAT_EQ(runtime.state, 127854.0f);
  EXPECT_NEAR(total.state, 53.22f, 0.01f);
}

}  // namespace esphome::heltec_balancer_ble::testing
