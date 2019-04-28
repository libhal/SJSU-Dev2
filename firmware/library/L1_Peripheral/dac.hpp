#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Dac
{
 public:
  virtual Status Initialize() const             = 0;
  virtual void Write(uint32_t dac_output) const = 0;
  virtual void SetVoltage(float voltage) const  = 0;
};
}  // namespace sjsu
