#pragma once

#include <cstddef>
#include <cstdint>

namespace esphome::jk_bms_ble {

// Command scheduling towards the BMS. Every class in this header takes the current millis()
// value as a parameter instead of reading the clock itself, so the whole file stays free of
// ESPHome dependencies and unit-testable on the host.

// A wrap-around safe one-shot deadline on the millis() clock.
class Deadline {
 public:
  void start(uint32_t now, uint32_t duration_ms) {
    armed_ = true;
    start_ = now;
    duration_ms_ = duration_ms;
  }
  void cancel() { armed_ = false; }
  bool armed() const { return armed_; }
  bool expired(uint32_t now) const { return armed_ && (now - start_) >= duration_ms_; }

 private:
  bool armed_{false};
  uint32_t start_{0};
  uint32_t duration_ms_{0};
};

// Serializes commands towards the BMS. The head command is put on the wire (mark_pending) and
// retired either by the response frame it asked for (on_frame) or by its deadline (tick).
// Register writes ask for no frame at all (expected_frame == NO_RESPONSE); their deadline is a
// short settle time that spaces out consecutive writes.
class CommandQueue {
 public:
  static constexpr size_t LENGTH = 16;
  static constexpr uint8_t NO_RESPONSE = 0x00;

  struct Command {
    uint8_t address;
    uint32_t value;
    uint8_t length;
    uint8_t expected_frame;
  };

  // What tick() retired: nothing, or a command that timed out waiting for its response frame.
  // Settle-time expiry of a register write is the normal way those retire and is not reported.
  struct Timeout {
    bool timed_out{false};
    uint8_t address{0x00};
  };

  bool enqueue(uint8_t address, uint32_t value, uint8_t length, uint8_t expected_frame) {
    uint8_t next = (tail_ + 1) % LENGTH;
    if (next == head_)
      return false;
    commands_[tail_] = {address, value, length, expected_frame};
    tail_ = next;
    return true;
  }

  // Only valid while !empty().
  const Command &front() const { return commands_[head_]; }
  bool empty() const { return head_ == tail_; }
  uint8_t size() const { return (tail_ + LENGTH - head_) % LENGTH; }

  bool pending() const { return pending_; }
  // True while the in-flight command is still waiting for a response frame from the BMS.
  bool awaits_response() const { return pending_ && !empty() && front().expected_frame != NO_RESPONSE; }

  // The head command went on the wire; arm its deadline. A response command gets the response
  // timeout, a register write only the settle time.
  void mark_pending(uint32_t now) {
    if (empty())
      return;
    pending_ = true;
    deadline_.start(now, front().expected_frame == NO_RESPONSE ? write_settle_ms_ : response_timeout_ms_);
  }

  void advance() {
    if (empty())
      return;
    head_ = (head_ + 1) % LENGTH;
    pending_ = false;
    deadline_.cancel();
  }

  void reset() {
    head_ = tail_ = 0;
    pending_ = false;
    deadline_.cancel();
  }

  // Retires the in-flight command if frame_type is the response it asked for. Unsolicited frames
  // (the BMS auto-streams cell info) and frames arriving while nothing is on the wire leave the
  // queue untouched. Returns true if a command was retired.
  bool on_frame(uint8_t frame_type) {
    if (!pending_ || empty())
      return false;
    if (front().expected_frame == NO_RESPONSE || front().expected_frame != frame_type)
      return false;
    advance();
    return true;
  }

  // Retires the in-flight command once its deadline has passed, freeing the queue for the next
  // one. See Timeout for what gets reported back.
  Timeout tick(uint32_t now) {
    Timeout result;
    if (!pending_ || empty() || !deadline_.expired(now))
      return result;
    // pending_ and !empty() just held, so this is exactly what awaits_response() would report.
    if (front().expected_frame != NO_RESPONSE) {
      result.timed_out = true;
      result.address = front().address;
    }
    advance();
    return result;
  }

  void set_response_timeout(uint32_t ms) { response_timeout_ms_ = ms; }
  void set_write_settle(uint32_t ms) { write_settle_ms_ = ms; }

 private:
  Command commands_[LENGTH];
  uint8_t head_{0};
  uint8_t tail_{0};
  bool pending_{false};
  Deadline deadline_;
  // A BLE round trip on a busy stack can easily exceed one second.
  uint32_t response_timeout_ms_{2000};
  // Spacing between back-to-back register writes, which the BMS never acknowledges.
  uint32_t write_settle_ms_{100};
};

// The BMS never acknowledges a settings write. Reading the settings back and letting the
// entities publish what the BMS actually stored is the only user feedback: an accepted value is
// confirmed, a rejected one visibly snaps back.
//
// The read is deferred instead of queued immediately: every schedule() pushes the deadline back,
// so a batch of writes coalesces into a single read once the BMS had time to commit the last one.
class SettingsVerification {
 public:
  static constexpr uint32_t DEFAULT_DELAY_MS = 500;

  void schedule(uint32_t now) { deadline_.start(now, delay_ms_); }
  void cancel() { deadline_.cancel(); }
  bool pending() const { return deadline_.armed(); }

  // True exactly once per schedule(): when the delay has elapsed, disarm and tell the caller to
  // issue the settings read now.
  bool take_if_due(uint32_t now) {
    if (!deadline_.expired(now))
      return false;
    deadline_.cancel();
    return true;
  }

  void set_delay(uint32_t ms) { delay_ms_ = ms; }

 private:
  Deadline deadline_;
  uint32_t delay_ms_{DEFAULT_DELAY_MS};
};

}  // namespace esphome::jk_bms_ble
