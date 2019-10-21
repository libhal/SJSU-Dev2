#pragma once

#include <cstddef>
#include <cstdint>

namespace sjsu
{
/// An abstract interface for communicating with EEPROM devices.
class Eeprom
{
 public:
  // ==============================
  // Interface Methods
  // ==============================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual void Initialize() const                           = 0;

  /// Writes a stream of data to the EEPROM
  ///
  /// @param wdata   - array of data to be written to EEPROM
  /// @param address - address where data will start being written to
  /// @param count   - number of bytes that have to be transferred
  virtual void Write(const uint8_t * wdata,
                     uint32_t address, size_t count) const  = 0;

  /// Reads a stream of data from the EEPROM
  ///
  /// @param rdata   - array that read data will be stored in
  /// @param address - address where data will start being read from
  /// @param count   - number of bytes that have to be read
  virtual void Read(uint8_t * rdata, uint32_t address,
                    size_t count) const                     = 0;
};
}  // namespace sjsu
