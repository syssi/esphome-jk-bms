#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace esphome::jk_bms_ble {

// Frames requested shortly after a write to verify it, so the entity publishes what the BMS actually stored. Flags, set
// per writable entity in its Python table.
enum Verify : uint8_t {
  VERIFY_NONE = 0,
  VERIFY_SETTINGS = 1 << 0,     // 0x96 request, answered with a settings frame
  VERIFY_DEVICE_INFO = 1 << 1,  // 0x97 request
};

// Orders the commands sent to the BMS. Free of ESPHome dependencies: every method takes the current millis() value, so
// the whole sequence is unit-testable on the host. The owner sends what next() returns and arms a single timer for
// next_deadline().
//
// The BMS drops a command that arrives while it is still processing the previous one, e.g. a 0x96 request sent while
// the device info frame of a 0x97 request is still being transmitted. It acknowledges every processed command
// (64-250 ms after a write, up to 454 ms after a 0x96 request; seen on JK02_32S hw 19, JK02_24S hw 7.x, 8.x, 10.x and
// JK04 hw 3.0), so the next command is sent once the acknowledgement arrived and the settle time passed. The timeout
// releases the queue if an acknowledgement gets lost or a device doesn't send one.
//
// The acknowledgement only confirms that a command was processed, not which value was stored. Each write therefore
// (re)arms the readback of the frames named by its verify flags: a batch of writes results in a single read per frame,
// requested once the queue is idle again.
class CommandQueue {
 public:
  static constexpr uint32_t ACKNOWLEDGEMENT_TIMEOUT_MS = 1000;
  // A command sent within a few milliseconds of the acknowledgement is still dropped now and then; 25 ms were enough in
  // every test (JK-B1A8S10P, sw 19.25)
  static constexpr uint32_t SETTLE_MS = 100;
  static constexpr uint32_t VERIFICATION_DELAY_MS = 500;

  struct Command {
    uint8_t address;
    uint32_t value;
    uint8_t length;
    uint8_t verify;
  };

  // The last value wins: a pending command for the same register is updated in place and keeps its position. The queue
  // holds at most one entry per register.
  void enqueue(const Command &command) {
    for (auto &pending : this->pending_) {
      if (pending.address == command.address) {
        pending = command;
        return;
      }
    }
    this->pending_.push_back(command);
  }

  // Ends a wait that is over: the settle time after an acknowledgement, or the acknowledgement timeout. Returns the
  // command that was never acknowledged.
  std::optional<uint8_t> expire(uint32_t now) {
    if (this->state_ == State::AWAITING_ACKNOWLEDGEMENT && now - this->since_ >= ACKNOWLEDGEMENT_TIMEOUT_MS) {
      this->state_ = State::IDLE;
      return this->awaited_command_;
    }
    if (this->state_ == State::SETTLING && now - this->since_ >= SETTLE_MS)
      this->state_ = State::IDLE;
    return std::nullopt;
  }

  // The verify flags whose readback is due, each returned once
  uint8_t take_due_verifications(uint32_t now) {
    uint8_t due = VERIFY_NONE;
    for (auto &verification : this->verifications_) {
      if (verification.armed && now - verification.since >= VERIFICATION_DELAY_MS) {
        verification.armed = false;
        due |= verification.flag;
      }
    }
    return due;
  }

  // The command to send now, if the previous one is done. The queue then waits for its acknowledgement.
  std::optional<Command> next(uint32_t now) {
    if (this->state_ != State::IDLE || this->pending_.empty())
      return std::nullopt;

    const Command command = this->pending_.front();
    this->pending_.erase(this->pending_.begin());
    this->state_ = State::AWAITING_ACKNOWLEDGEMENT;
    this->since_ = now;
    this->awaited_command_ = command.address;
    for (auto &verification : this->verifications_) {
      if (command.verify & verification.flag) {
        verification.armed = true;
        verification.since = now;
      }
    }
    return command;
  }

  // Returns the milliseconds the awaited command took, or nothing if no acknowledgement was awaited
  std::optional<uint32_t> on_acknowledgement(uint32_t now) {
    if (this->state_ != State::AWAITING_ACKNOWLEDGEMENT)
      return std::nullopt;
    const uint32_t latency = now - this->since_;
    this->state_ = State::SETTLING;
    this->since_ = now;
    return latency;
  }

  // Milliseconds until expire(), take_due_verifications() or next() has something to do, or nothing if the queue is
  // idle and no verification is armed
  std::optional<uint32_t> next_deadline(uint32_t now) const {
    std::optional<uint32_t> deadline;
    auto consider = [&](uint32_t since, uint32_t duration) {
      const uint32_t elapsed = now - since;
      const uint32_t remaining = elapsed >= duration ? 0 : duration - elapsed;
      if (!deadline || remaining < *deadline)
        deadline = remaining;
    };

    if (this->state_ == State::AWAITING_ACKNOWLEDGEMENT)
      consider(this->since_, ACKNOWLEDGEMENT_TIMEOUT_MS);
    if (this->state_ == State::SETTLING)
      consider(this->since_, SETTLE_MS);
    if (this->state_ == State::IDLE && !this->pending_.empty())
      deadline = 0;
    for (const auto &verification : this->verifications_) {
      if (verification.armed)
        consider(verification.since, VERIFICATION_DELAY_MS);
    }
    return deadline;
  }

  uint8_t awaited_command() const { return this->awaited_command_; }
  const std::vector<Command> &pending() const { return this->pending_; }

  // Pending commands and readbacks belong to a lost connection and must not be sent after a reconnect
  void reset() {
    this->pending_.clear();
    this->state_ = State::IDLE;
    for (auto &verification : this->verifications_)
      verification.armed = false;
  }

 protected:
  enum class State : uint8_t { IDLE, AWAITING_ACKNOWLEDGEMENT, SETTLING };
  struct Verification {
    uint8_t flag;
    bool armed;
    uint32_t since;
  };

  // Oldest first
  std::vector<Command> pending_;
  State state_{State::IDLE};
  // Start of the current wait: when the awaited command was sent, or when it was acknowledged
  uint32_t since_{0};
  uint8_t awaited_command_{0};
  Verification verifications_[2]{{VERIFY_SETTINGS, false, 0}, {VERIFY_DEVICE_INFO, false, 0}};
};

}  // namespace esphome::jk_bms_ble
