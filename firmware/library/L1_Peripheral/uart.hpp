#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Uart
{
  virtual Status Initialize(uint32_t baud_rate) const = 0;
  virtual bool SetBaudRate(uint32_t baud_rate) const  = 0;
  virtual void Send(uint8_t out) const                = 0;
  // TODO(#442): Add an IsAvailable function to check if a byte has been receive
  virtual uint8_t Receive(uint32_t timeout) const = 0;
};
}  // namespace sjsu
