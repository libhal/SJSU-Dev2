#pragma once

#include <cstdint>

#include "L0_Platform/interrupt.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class Timer
{
 public:
  // Modes for each match register that when match register matches timer
  // system can trigger an Interrupt, Stop Counter, or Restart and in any
  // mixture of those commands.
  enum TimerIsrCondition : uint8_t
  {
    kInterrupt            = 0b001,
    kRestart              = 0b010,
    kStop                 = 0b100,
    kInterruptRestart     = 0b011,
    kInterruptStop        = 0b101,
    kRestartStop          = 0b110,
    kInterruptRestartStop = 0b111
  };

  virtual Status Initialize(uint32_t us_per_tick, IsrPointer isr = nullptr,
                            int32_t priority = -1) const = 0;
  virtual void SetTimer(uint32_t time, TimerIsrCondition mode,
                        uint8_t reg = 0) const           = 0;
  virtual uint32_t GetTimer() const                      = 0;
};
}  // namespace sjsu
