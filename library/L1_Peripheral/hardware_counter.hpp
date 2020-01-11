#pragma once

#include <atomic>
#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
/// An abstract interface for controlling a hardware counter, internal or
/// external.
/// @ingroup l1_peripheral
class HardwareCounter
{
 public:
  /// Definitions of the directions that a counter to count in.
  enum class Direction : int8_t
  {
    kDown = -1,
    kUp   = 1,
  };

  /// When called, this initializes the peripheral hardware. You must ensure
  /// that the counter is set to zero after this call. NOTE: This must not
  /// enable the counter. In order to start counting based on an external input,
  /// an explicit invocation of Enable() must occur.
  virtual void Initialize() = 0;

  /// Set counter to a specific value. There is no guarantee that a count will
  /// not be missed during the invocation of this call.
  ///
  /// @param new_count_value - the value to set the counter to.
  virtual void Set(int32_t new_count_value) = 0;

  /// Set the direction of the counter to either up or down. Doing so will cause
  /// the counter to begin counting in that direction as events occur.
  ///
  /// @param direction - the direction (either up or down) to count.
  virtual void SetDirection(Direction direction) = 0;

  /// Starts counting from clock input. Must be called after Initialize() to
  /// begin counting.
  virtual void Enable() = 0;

  /// Stops counting from clock input. Current count is retained.
  virtual void Disable() = 0;

  /// Get the current count from hardware timer.
  virtual int32_t GetCount() = 0;

  /// Default virtual destructor
  virtual ~HardwareCounter() = default;
};

/// A class that utilizes a generic sjsu::Gpio implementation to implement
/// a hardware
/// @ingroup l1_peripheral
class GpioCounter : public HardwareCounter
{
 public:
  /// @param gpio - Reference to a gpio object
  /// @param edge - The digital signal edge to count on.
  /// @param pull - Set the pull resistor for the pin, when the pin is not
  ///        connected.
  GpioCounter(sjsu::Gpio & gpio,
              sjsu::Gpio::Edge edge,
              sjsu::Pin::Resistor pull = sjsu::Pin::Resistor::kPullUp)
      : gpio_(gpio), edge_(edge), pull_(pull)
  {
  }

  void Initialize() override
  {
    gpio_.GetPin().SetPull(pull_);
    gpio_.SetAsInput();
  }
  void Set(int32_t new_count_value) override
  {
    count_ = new_count_value;
  }

  void SetDirection(HardwareCounter::Direction direction) override
  {
    direction_ = direction;
  }

  void Enable() override
  {
    gpio_.AttachInterrupt([this] { count_ += Value(direction_.load()); },
                          edge_);
  }

  void Disable() override
  {
    gpio_.DetachInterrupt();
  }

  int32_t GetCount() override
  {
    return count_;
  }

  /// Destructor of this object will detach the interrupt from the GPIO. This is
  /// to keep the gpio interrupt service routine from calling this objects
  /// callback when this object has been destroyed.
  ~GpioCounter()
  {
    gpio_.DetachInterrupt();
  }

 private:
  sjsu::Gpio & gpio_;
  sjsu::Gpio::Edge edge_;
  sjsu::Pin::Resistor pull_;
  std::atomic<int32_t> count_       = 0;
  std::atomic<Direction> direction_ = Direction::kUp;
};
}  // namespace sjsu
