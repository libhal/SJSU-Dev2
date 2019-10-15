#pragma once

#include <cstddef>
#include <cstdint>

namespace sjsu
{
class Eeprom
{
 public:
  virtual void Initialize() const                           = 0;
  virtual void Write(const uint8_t * wdata,
                     uint32_t address, size_t count) const  = 0;
  virtual void Read(uint8_t * rdata, uint32_t address,
                    size_t count) const                     = 0;
};
}  // namespace sjsu
