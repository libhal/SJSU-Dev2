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
  virtual Returns<void> Initialize(uint32_t baud_rate) const = 0;

  /// Set UART baud rate
  ///
  /// @param baud_rate the speed of the UART transmit and receive.
  virtual Returns<void> SetBaudRate(uint32_t baud_rate) const = 0;

  /// Sends bytes via UART port via the TX line
  ///
  /// @param data - buffer to data to write to the uart serial port
  /// @param size - the number of bytes to write to the uart serial port
  virtual Returns<void> Write(const void * data, size_t size) const = 0;

  /// Retrieve bytes from the RX line. If no data is available, this method
  /// should return early.
  ///
  /// @param data - pointer to a buffer to load bytes from the uart RX line.
  /// @param size - the number of bytes to read from the uart serial port
  ///
  /// @returns number of bytes actually read into the data buffer. Will be less
  /// than or equal to the number of bytes passed by size. More bytes can exist
  /// in the uart's buffer if the returned size is equal to the passed size.
  virtual Returns<size_t> Read(void * data, size_t size) const = 0;

  /// Checks if there is data available for this port.
  /// @returns true if the UART port has received some data.
  virtual Returns<bool> HasData() const = 0;

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// The default implementation reads out all of the bytes by checking
  /// HasData() repeatedly and use Read() to read each. Some implementations may
  /// have more efficient methods of clearing their buffers.
  virtual Returns<void> Flush() const
  {
    PollingFlush();
    return {};
  }

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Transmit just 1 byte
  /// @param byte - Write a single byte to the UART line.
  Returns<void> Write(uint8_t byte) const
  {
    return Write(&byte, 1);
  }

  /// Transmit bytes using an array literal
  /// Usage:
  ///
  ///    uart.Write({ 0x01, 0xAA, 0x33, 0x55 });
  ///
  /// @param data - initializer list of bytes to send.
  Returns<void> Write(std::initializer_list<uint8_t> data) const
  {
    return Write(data.begin(), data.size());
  }

  /// @return Retrieves a single byte from UART RX line. Users must ensure that
  /// HasData() is true before reading using this method. Otherwise contents of
  /// read data will not be correct and the returned byte will be 0xFF.
  Returns<uint8_t> Read() const
  {
    uint8_t byte;
    if (SJ2_RETURN_ON_ERROR(HasData()))
    {
      SJ2_RETURN_ON_ERROR(Read(&byte, sizeof(byte)));
      return byte;
    }
    return Error(std::errc::resource_unavailable_try_again,
                 "This UART port does not have any data.");
  }

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// by checking HasData() repeatedly and use Read() to read each. Some
  /// implementations may have more efficient methods of clearing their buffers.
  Returns<void> PollingFlush() const
  {
    while (SJ2_RETURN_ON_ERROR(HasData()))
    {
      SJ2_RETURN_ON_ERROR(Read());
    }
    return {};
  }

  /// This method will handle waiting for bytes to be received via UART for the
  /// duration of the timeout.
  ///
  /// @param data - buffer to read bytes into.
  /// @param size - number of bytes to read before timeout.
  /// @param timeout - duration to wait for incoming data before timeout.
  /// @return std::errc::timed_out if bytes could not be read before before
  ///         timeout.
  Returns<size_t> Read(void * data,
                       size_t size,
                       std::chrono::nanoseconds timeout) const
  {
    size_t position       = 0;
    uint8_t * data_buffer = reinterpret_cast<uint8_t *>(data);

    SJ2_RETURN_ON_ERROR(
        Wait(timeout, [this, &data_buffer, size, &position]() -> Returns<bool> {
          // Read byte from UART port (HadData() check occurs within the byte
          // Read())
          auto read_result = Read();

          // If we get a TRY-AGAIN error, then we will return false and try
          // again.
          if (std::errc::resource_unavailable_try_again == read_result)
          {
            return false;
          }

          // If read_result contains some other error, then we should probagate
          // that up.
          else if (!read_result)
          {
            return tl::unexpected(read_result.error());
          }

          // Now we know that read_result has a value, thus we can write it to
          // the buffer.
          else
          {
            data_buffer[position++] = read_result.value();
          }

          // If position is above the buffer size supplied, then we are finished
          // and its time to return true.
          if (position >= size)
          {
            return true;
          }

          // We are not done yet, return false.
          return false;
        }));

    return position;
  }
};
}  // namespace sjsu
