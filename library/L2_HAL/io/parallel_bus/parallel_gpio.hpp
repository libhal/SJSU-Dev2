#pragma once

#include <initializer_list>
#include <span>

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
  /// @param gpio_array - an array of pointers to mutable sjsu::Gpio objects.
  ///        The const is a measure to make sure that this class does not
  ///        attempt to change pointer values of the array.
  constexpr explicit ParallelGpio(std::span<sjsu::Gpio *> gpio_array)
      : io_(gpio_array)
  {
  }

  void ModuleInitialize() override
  {
    for (auto gpio : io_)
    {
      if (gpio == nullptr)
      {
        throw sjsu::Exception(
            std::errc::invalid_argument,
            "The gpios in the parallel bus cannot be nullptr.");
      }
      // Initialize the GPIO
      gpio->Initialize();
    }
  }

  void ModuleEnable(bool enable = true) override
  {
    for (auto gpio : io_)
    {
      gpio->Enable(enable);
    }
  }

  void ConfigureAsOpenDrain(bool set_as_open_drain = true) override
  {
    for (auto gpio : io_)
    {
      gpio->GetPin().ConfigureAsOpenDrain(set_as_open_drain);
    }
  }

  void Write(uint32_t data) override
  {
    SetDirection(sjsu::Gpio::Direction::kOutput);

    for (int i = 0; auto gpio : io_)
    {
      gpio->Set(static_cast<sjsu::Gpio::State>(bit::Read(data, i++)));
    }
  }

  uint32_t Read() override
  {
    SetDirection(sjsu::Gpio::Direction::kInput);

    uint32_t read_value = 0;
    for (int i = 0; auto gpio : io_)
    {
      read_value |= gpio->Read() << (i++);
    }

    return read_value;
  }

  size_t BusWidth() override
  {
    return io_.size();
  }

  void SetDirection(sjsu::Gpio::Direction direction) override
  {
    for (auto gpio : io_)
    {
      gpio->SetDirection(direction);
    }
  }

 private:
  std::span<sjsu::Gpio *> io_;
};
}  // namespace sjsu
