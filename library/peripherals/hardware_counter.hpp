#pragma once

#include <atomic>
#include <cstdint>

#include "peripherals/gpio.hpp"
#include "module.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
/// An abstract interface for controlling a hardware counter, internal or
/// external.
///
/// @ingroup l1_peripheral
class HardwareCounter : public Module<>
{
 public:
  /// Definitions of the directions that a counter to count in.
  enum class Direction : int8_t
  {
    kDown = -1,
    kUp   = 1,
  };

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

  /// Get the current count from hardware timer.
  virtual int32_t GetCount() = 0;
};

/// A class that utilizes a generic sjsu::Gpio implementation to implement
/// a hardware
///
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
              sjsu::PinSettings_t::Resistor pull =
                  sjsu::PinSettings_t::Resistor::kPullUp)
      : gpio_(gpio), edge_(edge), pull_(pull)
  {
  }

  void ModuleInitialize() override
  {
    gpio_.GetPin().settings.resistor = pull_;
    gpio_.Initialize();
    gpio_.SetAsInput();

    gpio_.AttachInterrupt([this] { count_ += Value(direction_.load()); },
                          edge_);
  }

  void ModulePowerDown() override
  {
    gpio_.DetachInterrupt();
  }

  void Set(int32_t new_count_value) override
  {
    count_ = new_count_value;
  }

  void SetDirection(HardwareCounter::Direction direction) override
  {
    direction_ = direction;
  }

  int32_t GetCount() override
  {
    return count_;
  }

  ~GpioCounter()
  {
    gpio_.DetachInterrupt();
  }

 private:
  sjsu::Gpio & gpio_;
  sjsu::Gpio::Edge edge_;
  sjsu::PinSettings_t::Resistor pull_;
  std::atomic<int32_t> count_       = 0;
  std::atomic<Direction> direction_ = Direction::kUp;
};
}  // namespace sjsu
