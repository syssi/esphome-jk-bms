#include <gtest/gtest.h>
#include "common.h"
#include "frames_jk02_32s_v19.h"

namespace esphome::jk_bms_ble::testing {

// The BMS sends "AT\r\n" several times per second and acknowledges every command with a 20 byte frame. The BLE module
// packs whatever it has buffered into one notification, so the JK app btsnoop logs in docs/ contain notifications with
// "AT\r\n" in front of a settings frame's preamble, "AT\r\n" + acknowledgement + "AT\r\n" and acknowledgement +
// "AT\r\n". The notifications below are built from a captured device info frame in the same shapes.

static const std::vector<uint8_t> AT_LINE = {0x41, 0x54, 0x0D, 0x0A};
static const std::vector<uint8_t> ACKNOWLEDGEMENT = {0xAA, 0x55, 0x90, 0xEB, 0xC8, 0x01, 0x01, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44};

static std::vector<uint8_t> concat(std::initializer_list<std::vector<uint8_t>> parts) {
  std::vector<uint8_t> result;
  for (const auto &part : parts)
    result.insert(result.end(), part.begin(), part.end());
  return result;
}

static std::vector<uint8_t> slice(const std::vector<uint8_t> &data, size_t begin, size_t end) {
  return {data.begin() + begin, data.begin() + std::min(end, data.size())};
}

static void assemble(TestableJkBmsBle &bms, const std::vector<uint8_t> &notification) {
  bms.assemble(notification.data(), notification.size());
}

// Feeds the frame in notifications of `chunk` bytes, as the BLE module sends it
static void assemble_frame(TestableJkBmsBle &bms, const std::vector<uint8_t> &frame, size_t chunk) {
  for (size_t i = 0; i < frame.size(); i += chunk)
    assemble(bms, slice(frame, i, i + chunk));
}

class JkBmsAssembleTest : public ::testing::Test {
 protected:
  void SetUp() override { bms.set_software_version_text_sensor(&software_version); }

  // True if a device info frame was decoded since the last call
  bool decoded() {
    bool result = software_version.state == "19.27";
    software_version.state = "";
    return result;
  }

  TestableJkBmsBle bms;
  text_sensor::TextSensor software_version;
};

TEST_F(JkBmsAssembleTest, FrameSplitIntoNotificationsIsDecoded) {
  assemble_frame(bms, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded());
  EXPECT_TRUE(bms.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, AtLinesAreDropped) {
  for (int i = 0; i < 100; i++)
    assemble(bms, AT_LINE);
  EXPECT_TRUE(bms.frame_buffer_.empty());
}

// 75 "AT\r\n" lines used to add up to 300 bytes and fail the CRC check of a frame that never existed
TEST_F(JkBmsAssembleTest, AtLinesDontBreakTheNextFrame) {
  for (int i = 0; i < 74; i++)
    assemble(bms, AT_LINE);
  assemble_frame(bms, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded());
}

TEST_F(JkBmsAssembleTest, AcknowledgementsAreDropped) {
  assemble(bms, ACKNOWLEDGEMENT);
  assemble(bms, concat({AT_LINE, ACKNOWLEDGEMENT, AT_LINE}));
  assemble(bms, concat({ACKNOWLEDGEMENT, AT_LINE}));
  EXPECT_TRUE(bms.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, AtLineInFrontOfThePreambleIsDropped) {
  const auto &frame = DEVICE_INFO_JK02_32S_V19;
  assemble(bms, concat({AT_LINE, slice(frame, 0, 124)}));
  assemble(bms, slice(frame, 124, 252));
  assemble(bms, slice(frame, 252, 300));
  EXPECT_TRUE(decoded());
}

// Old devices were said to send frames of up to 320 bytes: the frame followed by its acknowledgement
TEST_F(JkBmsAssembleTest, AcknowledgementBehindTheFrameIsDropped) {
  assemble_frame(bms, concat({DEVICE_INFO_JK02_32S_V19, ACKNOWLEDGEMENT}), 128);
  EXPECT_TRUE(decoded());
  EXPECT_TRUE(bms.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, BytesBetweenFramesAreDropped) {
  assemble_frame(bms, DEVICE_INFO_JK02_32S_V19, 150);
  EXPECT_TRUE(decoded());

  assemble(bms, concat({ACKNOWLEDGEMENT, AT_LINE}));
  assemble(bms, AT_LINE);
  EXPECT_TRUE(bms.frame_buffer_.empty());

  assemble_frame(bms, DEVICE_INFO_JK02_32S_V19, 150);
  EXPECT_TRUE(decoded());
}

// A frame cut short (e.g. a lost notification) is discarded by the next preamble
TEST_F(JkBmsAssembleTest, IncompleteFrameIsReplacedByTheNextOne) {
  assemble(bms, slice(DEVICE_INFO_JK02_32S_V19, 0, 128));
  assemble_frame(bms, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded());
}

}  // namespace esphome::jk_bms_ble::testing
