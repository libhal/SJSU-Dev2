#pragma once

#include <cstdint>
#include <initializer_list>
#include <limits>

#include "utility/status.hpp"
#include "utility/time.hpp"

namespace sjsu
{
/// An abstract interface for hardware that implements the Universal
/// Asynchronous Receiver Transmitter (UART) hardware communication Protocol.
class Uart
{
 public:
  // ===========================
  // Interface Methods
  // ===========================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param baud_rate - set the communication speed
  virtual Status Initialize(uint32_t baud_rate) const = 0;
  /// Set baud rate
  virtual bool SetBaudRate(uint32_t baud_rate) const = 0;
  /// Write data to the UART
  ///
  /// @param data - buffer to data to write to the uart serial port
  /// @param size - the number of bytes to write to the uart serial port
  virtual void Write(const uint8_t * data, size_t size) const = 0;
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param data - buffer to data to write to the uart serial port
  /// @param size - the number of bytes to write to the uart serial port
  /// @param timeout - timeout in milliseconds
  ///
  /// @returns Status::kTimeout if buffer was not filled all the way before
  ///          timeout
  /// @returns Status::kSuccess if read was successful
  virtual Status Read(uint8_t * data,
                      size_t size,
                      std::chrono::microseconds timeout =
                          std::chrono::microseconds::max()) const = 0;
  /// Checks if there is data available for this port.
  /// @returns true if the UART port has received some data.
  virtual bool HasData() const = 0;

  // ================
  // Utility Methods
  // ================

  /// Transmit just 1 byte
  /// @param byte - Write a single byte to the UART port.
  void Write(uint8_t byte) const
  {
    Write(&byte, 1);
  }
  /// Transmit bytes using an array literal
  /// Usage:
  ///
  ///    uart.Write({ 0x01, 0xAA, 0x33, 0x55 });
  ///
  /// @param data - initializer list of bytes to send.
  void Write(std::initializer_list<uint8_t> data) const
  {
    Write(data.begin(), data.size());
  }
  /// Wait to receive just 1 byte
  ///
  /// @param timeout - The maximum amount of time to wait for a byte to be
  ///        transmitted.
  uint8_t Read(std::chrono::microseconds timeout =
                   std::chrono::microseconds::max()) const
  {
    uint8_t byte;
    if (Read(&byte, 1, timeout) == Status::kTimedOut)
    {
      byte = 0xFF;
    }
    return byte;
  }
};
}  // namespace sjsu
