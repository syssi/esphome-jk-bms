#include <gtest/gtest.h>
#include <cmath>
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
  void SetUp() override {
    bms_.set_protocol_version(PROTOCOL_VERSION_JK02_32S);
    bms_.set_software_version_text_sensor(&software_version_);
    bms_.set_total_voltage_sensor(&total_voltage_);
  }

  // True if a device info frame was decoded since the last call
  bool decoded_() {
    bool result = software_version_.state == "19.27";
    software_version_.state = "";
    return result;
  }

  // True if a cell info frame was decoded since the last call
  bool cell_info_decoded_() {
    bool result = !std::isnan(total_voltage_.state);
    total_voltage_.state = NAN;
    return result;
  }

  TestableJkBmsBle bms_;
  text_sensor::TextSensor software_version_;
  sensor::Sensor total_voltage_;
};

TEST_F(JkBmsAssembleTest, FrameSplitIntoNotificationsIsDecoded) {
  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded_());
  EXPECT_TRUE(bms_.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, AtLinesAreDropped) {
  for (int i = 0; i < 100; i++)
    assemble(bms_, AT_LINE);
  EXPECT_TRUE(bms_.frame_buffer_.empty());
}

// 75 "AT\r\n" lines used to add up to 300 bytes and fail the CRC check of a frame that never existed
TEST_F(JkBmsAssembleTest, AtLinesDontBreakTheNextFrame) {
  for (int i = 0; i < 74; i++)
    assemble(bms_, AT_LINE);
  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded_());
}

TEST_F(JkBmsAssembleTest, AcknowledgementsAreDropped) {
  assemble(bms_, ACKNOWLEDGEMENT);
  assemble(bms_, concat({AT_LINE, ACKNOWLEDGEMENT, AT_LINE}));
  assemble(bms_, concat({ACKNOWLEDGEMENT, AT_LINE}));
  EXPECT_TRUE(bms_.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, AtLineInFrontOfThePreambleIsDropped) {
  const auto &frame = DEVICE_INFO_JK02_32S_V19;
  assemble(bms_, concat({AT_LINE, slice(frame, 0, 124)}));
  assemble(bms_, slice(frame, 124, 252));
  assemble(bms_, slice(frame, 252, 300));
  EXPECT_TRUE(decoded_());
}

// Old devices were said to send frames of up to 320 bytes: the frame followed by its acknowledgement
TEST_F(JkBmsAssembleTest, AcknowledgementBehindTheFrameIsDropped) {
  assemble_frame(bms_, concat({DEVICE_INFO_JK02_32S_V19, ACKNOWLEDGEMENT}), 128);
  EXPECT_TRUE(decoded_());
  EXPECT_TRUE(bms_.frame_buffer_.empty());
}

TEST_F(JkBmsAssembleTest, BytesBetweenFramesAreDropped) {
  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 150);
  EXPECT_TRUE(decoded_());

  assemble(bms_, concat({ACKNOWLEDGEMENT, AT_LINE}));
  assemble(bms_, AT_LINE);
  EXPECT_TRUE(bms_.frame_buffer_.empty());

  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 150);
  EXPECT_TRUE(decoded_());
}

// A frame cut short (e.g. a lost notification) is discarded by the next preamble
TEST_F(JkBmsAssembleTest, IncompleteFrameIsReplacedByTheNextOne) {
  assemble(bms_, slice(DEVICE_INFO_JK02_32S_V19, 0, 128));
  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded_());
}

// JK-BD6A17S6P (hw 7.X, sw 7.1.0H): a 0x97 request answered while a cell info frame is being sent. The last
// notification of the cell info frame carries its last 44 bytes followed by the start of the device info frame.
TEST_F(JkBmsAssembleTest, FrameEndAndNextPreambleInOneNotificationAreBothDecoded) {
  const auto &cell_info = CELL_INFO_JK02_32S_V19;
  const auto &device_info = DEVICE_INFO_JK02_32S_V19;
  total_voltage_.state = NAN;

  assemble(bms_, slice(cell_info, 0, 128));
  assemble(bms_, slice(cell_info, 128, 256));
  assemble(bms_, concat({slice(cell_info, 256, 300), slice(device_info, 0, 84)}));
  EXPECT_TRUE(cell_info_decoded_());

  assemble(bms_, slice(device_info, 84, 212));
  assemble(bms_, slice(device_info, 212, 300));
  EXPECT_TRUE(decoded_());
  EXPECT_TRUE(bms_.frame_buffer_.empty());
}

// The bytes in front of the preamble don't complete the frame in progress: it is dropped, the new one is kept
TEST_F(JkBmsAssembleTest, IncompleteFrameInFrontOfAPreambleIsDropped) {
  total_voltage_.state = NAN;
  assemble(bms_, slice(CELL_INFO_JK02_32S_V19, 0, 128));
  assemble(bms_, concat({slice(CELL_INFO_JK02_32S_V19, 128, 140), slice(DEVICE_INFO_JK02_32S_V19, 0, 128)}));
  assemble(bms_, slice(DEVICE_INFO_JK02_32S_V19, 128, 256));
  assemble(bms_, slice(DEVICE_INFO_JK02_32S_V19, 256, 300));
  EXPECT_FALSE(cell_info_decoded_());
  EXPECT_TRUE(decoded_());
}

// A preamble followed by an endless stream: the buffer is checked at 300 bytes and cleared, everything after it is
// dropped notification by notification. The buffer never holds more than 300 bytes after a notification.
TEST_F(JkBmsAssembleTest, EndlessFrameDoesntGrowTheBuffer) {
  total_voltage_.state = NAN;
  const std::vector<uint8_t> preamble = {0x55, 0xAA, 0xEB, 0x90};
  std::vector<uint8_t> stream = concat({preamble, std::vector<uint8_t>(60000, 0x00)});
  for (size_t i = 0; i < stream.size(); i += 244) {
    assemble(bms_, slice(stream, i, i + 244));
    ASSERT_LT(bms_.frame_buffer_.size(), 300u) << "after " << i + 244 << " bytes";
  }
  EXPECT_FALSE(decoded_());
  EXPECT_FALSE(cell_info_decoded_());
  EXPECT_TRUE(bms_.frame_buffer_.empty());

  // The next frame is decoded again
  assemble_frame(bms_, DEVICE_INFO_JK02_32S_V19, 128);
  EXPECT_TRUE(decoded_());
}

}  // namespace esphome::jk_bms_ble::testing
