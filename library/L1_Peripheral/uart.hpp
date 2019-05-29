#pragma once

#include <cstdint>
#include <initializer_list>

#include "utility/status.hpp"

namespace sjsu
{
class Uart
{
 public:
  // ===========================
  // Interface Required methods
  // ===========================
  virtual Status Initialize(uint32_t baud_rate) const                      = 0;
  virtual bool SetBaudRate(uint32_t baud_rate) const                       = 0;
  virtual void Write(const uint8_t * data, size_t size) const              = 0;
  virtual Status Read(uint8_t * data, size_t size, uint32_t timeout) const = 0;
  virtual bool HasData() const                                             = 0;
  // ================
  // Utility methods
  // ================
  // Transmit just 1 byte
  void Write(uint8_t byte)
  {
    Write(&byte, 1);
  }
  // Transmit bytes using an array literal
  void Write(std::initializer_list<uint8_t> data)
  {
    Write(data.begin(), data.size());
  }
  // Overload of Read that waits forever if a timeout was not supplied.
  Status Read(uint8_t * data, size_t size)
  {
    return Read(data, size, UINT32_MAX);
  }
  // Wait to receive just 1 byte
  uint8_t Read(uint32_t timeout = UINT32_MAX)
  {
    uint8_t byte;
    // load byte with byte recieved from uart buffer.
    // If Recieve returns TIMEOUT, runs out of time, then load byte with 0xFF.
    if (Read(&byte, 1, timeout) == Status::kTimedOut)
    {
      byte = 0;
    }
    return byte;
  }
};
}  // namespace sjsu
