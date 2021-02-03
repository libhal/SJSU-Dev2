#pragma once

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <span>

#include "peripherals/inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
/// Generic settings for a standard UART peripheral
struct UartSettings_t : public MemoryEqualOperator_t<UartSettings_t>
{
  /// Defines the available frame sizes for UART payloads
  enum class FrameSize : uint8_t
  {
    kFiveBits,
    kSixBits,
    kSevenBits,
    kEightBits,
    kNineBits,
  };

  /// Defines the available stop bits options
  enum class StopBits : uint8_t
  {
    kSingle,
    kDouble,
  };

  /// Defines the parity bit options
  enum class Parity : uint8_t
  {
    /// Disable parity checks
    kNone = 0,
    /// Enable parity and set HIGH when the number of bits is odd
    kOdd,
    /// Enable parity and set HIGH when the number of bits is even
    kEven,
  };

  /// The operating baud rate (speed) of the UART signals.
  uint32_t baud_rate = 9600;

  /// The number of bits for the UART packet payload.
  FrameSize frame_size = FrameSize::kEightBits;

  /// The number of stop bits at the end of the.
  StopBits stop = StopBits::kSingle;

  /// The parity bit settings for UART.
  Parity parity = Parity::kNone;
};

/// An abstract interface for hardware that implements the Universal
/// Asynchronous Receiver Transmitter (UART) hardware communication Protocol.
/// @ingroup l1_peripheral
class Uart : public Module<UartSettings_t>
{
 public:
  /// Checks if there is data available for this port.
  ///
  /// @returns true if the UART port has received some data.
  virtual bool HasData() = 0;

  /// Sends bytes via UART port via the TX line
  ///
  /// @param data - buffer to data to write to the uart serial port
  virtual void Write(std::span<const uint8_t> data) = 0;

  /// Retrieve bytes from the RX line. If no data is available, this method
  /// should return early.
  ///
  /// @param data - buffer to load bytes from the uart RX line.
  ///
  /// @returns number of bytes read into the data buffer. Will be less
  /// than or equal to the number of bytes passed by size. More bytes can exist
  /// in the uart's buffer if the returned size is equal to the passed size.
  virtual size_t Read(std::span<uint8_t> data) = 0;

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// The default implementation reads out all of the bytes by checking
  /// HasData() repeatedly and use Read() to read each. Some implementations may
  /// have more efficient methods of clearing their buffers.
  virtual void Flush()
  {
    PollingFlush();
  }

  // ===========================================================================
  // Helper Functions
  // ===========================================================================

  /// Transmit just 1 byte
  /// @param byte - Write a single byte to the UART line.
  void Write(uint8_t byte)
  {
    std::array<uint8_t, 1> payload = { byte };
    Write(payload);
  }

  /// Transmit bytes using an array literal
  /// Usage:
  ///
  ///    uart.Write({ 0x01, 0xAA, 0x33, 0x55 });
  ///
  /// @param data - initializer list of bytes to send.
  void Write(std::initializer_list<uint8_t> data)
  {
    Write(std::span(data.begin(), data.end()));
  }

  /// Transmit bytes using an array literal
  ///
  /// @param str - string_view list of bytes to send.
  void Write(std::string_view str)
  {
    std::span span(reinterpret_cast<const uint8_t *>(str.data()), str.size());
    Write(span);
  }

  /// Transmit std::bytes to write
  ///
  /// @param data - initializer list of bytes to send.
  void Write(std::span<const std::byte> data)
  {
    auto start = reinterpret_cast<const uint8_t *>(data.data());
    Write(std::span(start, data.size()));
  }

  /// @return Retrieves a single byte from UART RX line. Users must ensure that
  /// HasData() is true before reading using this method. Otherwise contents of
  /// read data will not be correct and the returned byte will be 0xFF.
  uint8_t Read()
  {
    // If read doesn't have any data, the contents of the byte should not be
    // touched, and thus 0xFF will be returned.
    std::array<uint8_t, 1> byte = { 0xFF };

    // Read a single byte from UART
    Read(byte);

    // Return the byte
    return byte[0];
  }

  /// Receive std::bytes into byte buffer
  ///
  /// @param data - initializer list of bytes to send.
  size_t Read(std::span<std::byte> data)
  {
    uint8_t * uint_ptr = reinterpret_cast<uint8_t *>(data.data());
    return Read(std::span(uint_ptr, data.size()));
  }

  /// Will flush all bytes currently head with the UART peripherals buffers.
  ///
  /// by checking HasData() repeatedly and use Read() to read each. Some
  /// implementations may have more efficient methods of clearing their buffers.
  void PollingFlush()
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
  /// @param timeout - duration to wait for incoming data before timeout.
  /// @return std::errc::timed_out if bytes could not be read before before
  ///         timeout.
  size_t Read(std::span<uint8_t> data, std::chrono::nanoseconds timeout)
  {
    size_t position = 0;

    Wait(timeout, [this, data, &position]() -> bool {
      position += Read(data.subspan(position));
      if (position >= data.size())
      {
        return true;
      }
      return false;
    });

    return position;
  }
};

/// Template specialization that generates an inactive sjsu::Uart.
template <>
inline sjsu::Uart & GetInactive<sjsu::Uart>()
{
  class InactiveUart : public sjsu::Uart
  {
   public:
    void ModuleInitialize() override {}
    void Write(std::span<const uint8_t>) override {}
    size_t Read(std::span<uint8_t>) override
    {
      return 0;
    }
    bool HasData() override
    {
      return false;
    }
  };

  static InactiveUart inactive;
  return inactive;
}
}  // namespace sjsu
