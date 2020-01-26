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
/// @ingroup l1_peripheral
class Uart
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param baud_rate - set the communication speed
  virtual Status Initialize(uint32_t baud_rate) const = 0;

  /// Set UART baud rate
  ///
  /// @param baud_rate the speed of the UART transmit and receive.
  /// @return true - if baud rate was able to be set correctly.
  /// @return false - if baud rate is not possible to be set.
  virtual bool SetBaudRate(uint32_t baud_rate) const = 0;

  /// Sends bytes via UART port via the TX line
  ///
  /// @param data - buffer to data to write to the uart serial port
  /// @param size - the number of bytes to write to the uart serial port
  virtual void Write(const void * data, size_t size) const = 0;

  /// Retrieve bytes from the RX line. If no data is available, this method
  /// should return early.
  ///
  /// @param data - pointer to a buffer to load bytes from the uart RX line.
  /// @param size - the number of bytes to read from the uart serial port
  ///
  /// @returns number of bytes actually read into the data buffer. Will be less
  /// than or equal to the number of bytes passed by size. More bytes can exist
  /// in the uart's buffer if the returned size is equal to the passed size.
  virtual size_t Read(void * data, size_t size) const = 0;

  /// Checks if there is data available for this port.
  /// @returns true if the UART port has received some data.
  virtual bool HasData() const = 0;

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// The default implementation reads out all of the bytes by checking
  /// HasData() repeatedly and use Read() to read each. Some implementations may
  /// have more efficient methods of clearing their buffers.
  virtual void Flush() const
  {
    PollingFlush();
  }
  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Transmit just 1 byte
  /// @param byte - Write a single byte to the UART line.
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

  /// @return Retrieves a single byte from UART RX line. Users must ensure that
  /// HasData() is true before reading using this method. Otherwise contents of
  /// read data will not be correct and the returned byte will be 0xFF.
  uint8_t Read() const
  {
    uint8_t byte;
    if (Read(&byte, sizeof(byte)) == 0)
    {
      byte = 0xFF;
    }
    return byte;
  }

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// by checking HasData() repeatedly and use Read() to read each. Some
  /// implementations may have more efficient methods of clearing their buffers.
  void PollingFlush() const
  {
    while (HasData())
    {
      Read();
    }
  }

  /// This method will handle waiting for bytes to be received via UART for the
  /// duration of the timeout.
  ///
  /// @param data - buffer to read bytes into.
  /// @param size - number of bytes to read before timeout.
  /// @param timeout - duration to wait for incoming data before timeout.
  /// @return sjsu::Status::kSuccess if bytes were read before timeout.
  /// @return sjsu::Status::kTimeout if bytes could not be read before before
  ///         timeout.
  sjsu::Status Read(void * data,
                    size_t size,
                    std::chrono::nanoseconds timeout) const
  {
    size_t position       = 0;
    uint8_t * data_buffer = reinterpret_cast<uint8_t *>(data);
    return Wait(timeout, [this, &data_buffer, size, &position]() -> bool {
      if (HasData())
      {
        data_buffer[position++] = Read();
        if (position >= size)
        {
          return true;
        }
      }
      return false;
    });
  }
};
}  // namespace sjsu
