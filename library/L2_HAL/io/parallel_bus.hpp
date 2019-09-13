#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "utility/log.hpp"

namespace sjsu
{
class ParallelBus
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================
  virtual void Initialize()                                  = 0;
  virtual void Write(uint32_t data)                          = 0;
  virtual uint32_t Read()                                    = 0;
  virtual size_t BusWidth() const                            = 0;
  virtual void SetDirection(sjsu::Gpio::Direction direction) = 0;
  virtual void SetAsOpenDrain([[maybe_unused]] bool set_as_open_drain = true)
  {
    SJ2_ASSERT_FATAL(false,
                     "SetAsOpenDrain() is not available for this parallel bus "
                     "implementation.");
  }
  // ===========================================================================
  // Utility Methods
  // ===========================================================================
  void SetAsOutput()
  {
    SetDirection(sjsu::Gpio::Direction::kOutput);
  }
  void SetAsInput()
  {
    SetDirection(sjsu::Gpio::Direction::kInput);
  }
};
}  // namespace sjsu
