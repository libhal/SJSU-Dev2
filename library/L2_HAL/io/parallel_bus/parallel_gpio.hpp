#pragma once

#include <initializer_list>

#include "L2_HAL/io/parallel_bus.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// A parallel bus composed of sjsu::Gpio objects
class ParallelGpio : public sjsu::ParallelBus
{
 public:
  /// Construct ParallelGpio
  ///
  /// @param array - an array of pointers to mutable sjsu::Gpio objects. The
  ///        const is a measure to make sure that this class does not attempt to
  ///        change pointer values of the array.
  /// @param width - The size of the array of gpio.
  ParallelGpio(sjsu::Gpio * const array[], size_t width)
      : io_(array), kWidth(width)
  {
  }
  /// Constructor of ParallelGpio that takes a std::initializer_list of
  /// sjsu::Gpio pointers. This simplifies the usage of parallel bus and helps
  /// to eliminate mistakes where the array size is not correct.
  ///
  /// Example Usage:
  ///
  ///    ParallelGpio parallel_gpio({
  ///        gpio_led0,
  ///        gpio_led1,
  ///        gpio_led2,
  ///        gpio_led3,
  ///    });
  ///
  /// @param array - std::initializer_list of sjsu::Gpio pointers.
  explicit ParallelGpio(const std::initializer_list<sjsu::Gpio *> & array)
      : io_(array.begin()), kWidth(array.size())
  {
  }
  void Initialize() override
  {
    bool parallel_gpio_bus_initialized_successfully = true;
    for (size_t i = 0; i < kWidth; i++)
    {
      if (io_[i] == nullptr)
      {
        parallel_gpio_bus_initialized_successfully = false;
        LOG_ERROR("Gpio %zu of parallel bus is NULL.", i);
      }
    }

    SJ2_ASSERT_FATAL(parallel_gpio_bus_initialized_successfully,
                     "ParallelGpio initialization failed.");

    SetAsInput();
  }
  /// Set the pins of the parallel bus as open drain (or open collector).
  ///
  /// @param set_as_open_drain - if true, set output of parallel bus pins to
  /// open drain. Otherwise, set pin as push-pull.
  void SetAsOpenDrain(bool set_as_open_drain = true) override
  {
    for (size_t i = 0; i < kWidth; i++)
    {
      io_[i]->GetPin().SetAsOpenDrain(set_as_open_drain);
    }
  }

  void Write(uint32_t data) override
  {
    SetDirection(sjsu::Gpio::Direction::kOutput);
    for (uint32_t i = 0; i < kWidth; i++)
    {
      io_[i]->Set(static_cast<sjsu::Gpio::State>(bit::Read(data, i)));
    }
  }

  uint32_t Read() override
  {
    uint32_t read_value = 0;
    SetDirection(sjsu::Gpio::Direction::kInput);

    for (size_t i = 0; i < kWidth; i++)
    {
      read_value |= io_[i]->Read() << i;
    }

    return read_value;
  }
  size_t BusWidth() const override
  {
    return kWidth;
  }
  void SetDirection(sjsu::Gpio::Direction direction) override
  {
    for (size_t i = 0; i < kWidth; i++)
    {
      io_[i]->SetDirection(direction);
    }
  }

 private:
  sjsu::Gpio * const * io_;
  const size_t kWidth;
};
}  // namespace sjsu
