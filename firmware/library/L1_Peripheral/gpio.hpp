#pragma once

#include <cstdint>

#include "L0_Platform/lpc40xx/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class Gpio
{
 public:
  enum Direction : uint8_t
  {
    kInput  = 0,
    kOutput = 1
  };
  enum State : uint8_t
  {
    kLow  = 0,
    kHigh = 1
  };
  enum class Edge : uint8_t
  {
    kEdgeRising  = 0,
    kEdgeFalling = 1,
    kEdgeBoth    = 2
  };
  virtual void SetDirection(Direction direction) const               = 0;
  virtual void Set(State output) const                               = 0;
  virtual void Toggle() const                                        = 0;
  virtual State ReadState() const                                    = 0;
  virtual bool Read() const                                          = 0;
  virtual const sjsu::Pin & GetPin() const                           = 0;
  virtual void AttachInterrupt(IsrPointer function, Edge edge) const = 0;
  virtual void DetachInterrupt() const                               = 0;
};
}  // namespace sjsu
