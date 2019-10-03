#pragma once

#include <cstdint>
#include <initializer_list>

#include "config.hpp"

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for hardware that implements the Inter-integrated
/// Circuit (I2C) or Two Wire Interface (TWI) hardware communication Protocol.
class I2c
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================

  /// Operation defines the types of operations that can occur in an I2C
  /// transaction.
  enum Operation : uint8_t
  {
    kWrite = 0,
    kRead  = 1,
  };
  /// Default timeout time for I2C if you choose to not supply your own timeout
  /// time.
  static constexpr std::chrono::milliseconds kI2cTimeout = 100ms;
  /// A common structure for holding the information needed for I2C
  /// transactions.
  struct Transaction_t
  {
    /// Returns an 8-bit I2C address with the 0th bit set if the i2c operation
    /// is kRead.
    constexpr uint8_t GetProperAddress()
    {
      uint8_t address_8bit = static_cast<uint8_t>(address << 1);
      if (operation == Operation::kRead)
      {
        address_8bit |= 1;
      }
      return address_8bit;
    }
    /// Defines the starting operation of this transaction. The use of the word
    /// "starting", refers to the fact that, the operation can change from Read
    /// -> Write if a WriteThenRead() function was called on this structure. In
    /// that case, the transaction will start from a Write and then transition
    /// to a Read after the necessary number of bytes have been written to the
    /// bus.
    /// Read operations never transition to Write.
    Operation operation = Operation::kWrite;
    /// The 7-bit I2C address of the device to communicate with.
    uint8_t address = 0xFF;
    /// Pointer to a buffer of bytes to write to the device.
    const uint8_t * data_out = nullptr;
    /// The number of bytes to write to the device.
    size_t out_length = 0;
    /// Pointer to a buffer to store retrieved bytes into.
    uint8_t * data_in = nullptr;
    /// The number of bytes to read from the device.
    size_t in_length = 0;
    /// The current position in the out or in buffer.
    size_t position = 0;
    /// This flag determins if a "repeat start" condition should be emitted on
    /// the bus. This flag being set will transition the hardware from Write to
    /// Read mode. If this flag is true, data_out and data_in must be set to an
    /// actual buffer (cannot be nullptr).
    bool repeated = false;
    /// This flag indicates to the driver whether or not the I2C transaction is
    /// still occuring. Use this to break out of a while loop if your I2C
    /// implementation is interrupt based.
    bool busy = false;
    /// How long should the calling code wait before timing out and moving on
    /// without a result.
    std::chrono::milliseconds timeout = kI2cTimeout;
    /// The status of the transaction after it is completed, fails, or times
    /// out.
    Status status = Status::kSuccess;
  };

  // ==============================
  // Interface Methods
  // ==============================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual Status Initialize() const = 0;
  /// Perform a I2C transaction using the information contained in the
  /// transaction parameter.
  ///
  /// @return Status::kTimeout if the transaction could not be performed in the
  ///         set time.
  ///         Status::kDeviceNotFound if external device does not respond to
  ///         address on the bus
  ///         Status::kSuccess if transaction was fulfilled.
  virtual Status Transaction(Transaction_t transaction) const = 0;

  // ==============================
  // Utility Methods
  // ==============================

  /// Read from a device on the I2C bus
  ///
  /// @param address - device address
  /// @param transmit_buffer - pointer to a byte buffer to read information into
  /// @param transmit_buffer_length - number of bytes to be written into the
  ///        data buffer
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status Read(uint8_t address,
              uint8_t * transmit_buffer,
              size_t transmit_buffer_length,
              std::chrono::milliseconds timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kRead,
        .address    = address,
        .data_out   = nullptr,
        .out_length = 0,
        .data_in    = transmit_buffer,
        .in_length  = transmit_buffer_length,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }

  /// Write to a device on the I2C bus
  ///
  /// @param address - device address
  /// @param receive_buffer - pointer to a byte buffer to send to the device
  /// @param receive_buffer_length - number of bytes to be written to the device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status Write(uint8_t address,
               const uint8_t * receive_buffer,
               size_t receive_buffer_length,
               std::chrono::milliseconds timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kWrite,
        .address    = address,
        .data_out   = receive_buffer,
        .out_length = receive_buffer_length,
        .data_in    = nullptr,
        .in_length  = 0,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }
  /// Write to a device on the I2C bus
  ///
  /// Usage:
  ///
  ///     i2c.Write(0x29, {0x01, 0x2a, 0x10});
  ///
  /// @param address - device address
  /// @param transmit - array literal to send to device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status Write(uint8_t address,
               std::initializer_list<uint8_t> transmit,
               std::chrono::milliseconds timeout = kI2cTimeout) const
  {
    return Write(address, transmit.begin(), transmit.size(), timeout);
  }
  /// Write to a device on the I2C bus, then read from that device.
  ///
  /// This is very common for most I2C devices, where the microcontroller must
  /// send a register address byte, then perform a read transaction in order to
  /// read the data from the device.
  ///
  /// @param address - device address
  /// @param transmit_buffer - pointer to a byte buffer to read information into
  /// @param transmit_buffer_length - number of bytes to be written into the
  ///        data buffer
  /// @param receive_buffer - pointer to a byte buffer to send to the device
  /// @param receive_buffer_length - number of bytes to be written to the device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status WriteThenRead(uint8_t address,
                       const uint8_t * transmit_buffer,
                       size_t transmit_buffer_length,
                       uint8_t * receive_buffer,
                       size_t receive_buffer_length,
                       std::chrono::milliseconds timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kWrite,
        .address    = address,
        .data_out   = transmit_buffer,
        .out_length = transmit_buffer_length,
        .data_in    = receive_buffer,
        .in_length  = receive_buffer_length,
        .position   = 0,
        .repeated   = true,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }
  /// Write to a device on the I2C bus, then read from that device.
  ///
  /// Usage:
  ///
  ///    uint8_t buffer[10];
  ///    i2c.Write(0x29, {0x01}, buffer, sizeof(buffer));
  ///
  /// @param address - device address
  /// @param transmit - array literal to send to device
  /// @param receive_buffer - b
  /// @param receive_buffer_length -
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status WriteThenRead(uint8_t address,
                       std::initializer_list<uint8_t> transmit,
                       uint8_t * receive_buffer,
                       size_t receive_buffer_length,
                       std::chrono::milliseconds timeout = kI2cTimeout) const
  {
    return WriteThenRead(address,
                         transmit.begin(),
                         transmit.size(),
                         receive_buffer,
                         receive_buffer_length,
                         timeout);
  }
};
}  // namespace sjsu
