#pragma once

#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class Spi
{
 public:
  // SSP Master/slave modes
  enum class MasterSlaveMode
  {
    kMaster = 0,
    kSlave  = 1
  };
  // SSP data size for frame packets
  enum class DataSize : uint8_t
  {
    kFour = 0,  // The smallest standard frame sized allowed for SJSU-Dev2
    kFive,
    kSix,
    kSeven,
    kEight,
    kNine,
    kTen,
    kEleven,
    kTwelve,
    kThirteen,
    kFourteen,
    kFifteen,
    kSixteen,  // The largest standard frame sized allowed for SJSU-Dev2
  };

  virtual Status Initialize() const                             = 0;
  virtual uint16_t Transfer(uint16_t data) const                = 0;
  virtual void SetDataSize(DataSize size) const                 = 0;
  virtual void SetClock(uint32_t frequency,
                        bool positive_clock_on_idle = false,
                        bool read_miso_on_rising    = false) const = 0;
};
}  // namespace sjsu
