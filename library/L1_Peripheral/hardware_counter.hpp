#pragma once

#include <cstdint>
#include "L1_Peripheral/gpio.hpp"

namespace sjsu
{
/// An abstract interface for controlling a hardware counter, internal or
/// external.
class HardwareCounter
{
 public:
  /// When called, this initializes the peripheral hardware. You must ensure
  /// that the counter is set to zero after this call. NOTE: This must not
  /// enable the counter. In order to start counting based on an external input,
  /// an explicit invocation of Enable() must occur.
  virtual void Initialize() = 0;
  /// Set counter to a specific value .There is no guarantee that counts will
  /// not be missed during the invocation of this call
  ///
  /// @param new_count_value - the value to set the counter to.
  virtual void Set(uint32_t new_count_value) = 0;
  /// Starts counting from clock input. Must be called after Initialize() to
  /// begin counting.
  virtual void Enable() = 0;
  /// Stops counting from clock input. Current count is retained.
  virtual void Disable() = 0;
  /// Get the current count from hardware timer.
  virtual uint32_t GetCount() = 0;
  /// Default virtual destructor
  virtual ~HardwareCounter() = default;
};

/// A class that utilizes a generic sjsu::Gpio implementation to implement
/// a hardware
class GpioCounter : public HardwareCounter
{
 public:
  /// @param gpio - Reference to a gpio object
  /// @param edge - The digital signal edge to count up on.
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
  void Set(uint32_t new_count_value) override
  {
    count_ = new_count_value;
  }
  void Enable() override
  {
    gpio_.AttachInterrupt([this] { count_++; }, edge_);
  }
  /// Stops counting from clock input. Current count is retained.
  void Disable() override
  {
    gpio_.DetachInterrupt();
  }
  /// Get the current count from hardware timer.
  uint32_t GetCount() override
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
  uint32_t count_ = 0;
};
}  // namespace sjsu
