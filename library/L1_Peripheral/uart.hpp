#pragma once

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <span>

#include "L1_Peripheral/inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/time.hpp"

namespace sjsu
{
/// An abstract interface for hardware that implements the Universal
/// Asynchronous Receiver Transmitter (UART) hardware communication Protocol.
/// @ingroup l1_peripheral
class Uart : public Module
{
 public:
  // ===========================================================================
  // Interface Definitions
  // ===========================================================================
  enum class FrameSize : uint8_t
  {
    kFiveBits,
    kSixBits,
    kSevenBits,
    kEightBits,
    kNineBits,
  };

  enum class StopBits : uint8_t
  {
    kSingle,
    kDouble,
  };

  enum class Parity : uint8_t
  {
    kNone = 0,
    kOdd,
    kEven,
    kForced1,
    kForced0
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// Set UART baud rate.
  /// Will attempt to provide a baud rate closest to the value supplied.
  ///
  /// @param baud_rate the speed of the UART transmit and receive.
  virtual void ConfigureBaudRate(uint32_t baud_rate) = 0;

  /// Set UART baud rate.
  /// Will attempt to provide a baud rate closest to the value supplied.
  ///
  /// @param size - Size of UART data frames (8 is standard).
  /// @param stop - Number of stop bits (1 stop bit is standard)
  /// @param parity - Type of parity control (none parity is standard)
  virtual void ConfigureFormat(FrameSize size = FrameSize::kEightBits,
                               StopBits stop  = StopBits::kSingle,
                               Parity parity  = Parity::kNone) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

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
  // Utility Methods
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
    void ModuleEnable(bool) override {}
    void ConfigureBaudRate(uint32_t) override {}
    void ConfigureFormat(FrameSize = FrameSize::kEightBits,
                         StopBits  = StopBits::kSingle,
                         Parity    = Parity::kNone) override
    {
    }
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
