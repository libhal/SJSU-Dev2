#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Adc
{
 public:
  virtual Status Initialize() const          = 0;
  virtual void Conversion() const            = 0;
  virtual uint16_t Read() const              = 0;
  virtual bool HasConversionFinished() const = 0;
};
}  // namespace sjsu
