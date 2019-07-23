#pragma once

#include <cstdint>
#include <initializer_list>

#include "config.hpp"

#include "utility/status.hpp"

namespace sjsu
{
class I2c
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================
  enum Operation : uint8_t
  {
    kWrite = 0,
    kRead  = 1,
  };

  static constexpr uint32_t kI2cTimeout = 100;  // units milliseconds

  struct Transaction_t
  {
    // Returns an 8-bit I2C address with the 0th bit set if the i2c operation
    // is kRead.
    constexpr uint8_t GetProperAddress()
    {
      uint8_t address_8bit = static_cast<uint8_t>(address << 1);
      if (operation == Operation::kRead)
      {
        address_8bit |= 1;
      }
      return address_8bit;
    }
    Operation operation      = Operation::kWrite;
    uint8_t address          = 0xFF;
    const uint8_t * data_out = nullptr;
    size_t out_length        = 0;
    uint8_t * data_in        = nullptr;
    size_t in_length         = 0;
    size_t position          = 0;
    bool repeated            = false;
    bool busy                = false;
    uint64_t timeout         = kI2cTimeout;
    Status status            = Status::kSuccess;
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
              uint32_t timeout = kI2cTimeout) const
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
               uint32_t timeout = kI2cTimeout) const
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
  ///    i2c.Write(0x29, {0x01, 0x2a, 0x10});
  ///
  /// @param address - device address
  /// @param transmit - array literal to send to device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  Status Write(uint8_t address,
               std::initializer_list<uint8_t> transmit,
               uint32_t timeout = kI2cTimeout) const
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
                       uint32_t timeout = kI2cTimeout) const
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
                       uint32_t timeout = kI2cTimeout) const
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
