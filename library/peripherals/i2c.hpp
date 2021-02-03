#pragma once

#include <cstdint>
#include <initializer_list>
#include <span>

#include "config.hpp"
#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Generic settings for a standard I2C peripheral
struct I2cSettings_t : public MemoryEqualOperator_t<I2cSettings_t>
{
  /// Operating frequency
  units::frequency::hertz_t frequency = 100'000_Hz;
  /// Clock duty cycle
  float duty_cycle = 0.5;
};

/// An abstract interface for hardware that implements the Inter-integrated
/// Circuit (I2C) or Two Wire Interface (TWI) hardware communication Protocol.
/// @ingroup l1_peripheral
class I2c : public Module<I2cSettings_t>
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

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

  /// Namespace of common I2C transaction errors
  class CommonErrors
  {
   public:
    /// This exception occurs when the overall transaction takes longer than the
    /// transaction timeout time given. This includes the whole transaction from
    /// address transmission to the last byte retrieved from the device.
    static inline const auto kTimeout =
        Exception(std::errc::timed_out,
                  "I2C took too long to process and timed out! Consider "
                  "increasing the timeout time.");

    /// This exception occurs when there is an anomaly on the I2C bus. These
    /// sort of errors can be caused by EMI or another device talking on the bus
    /// at an incorrect time. A bus error is typically considered a situation in
    /// which, the I2C controller device reads the state of the line and it does
    /// not match what it expects. For example, if the I2C bus means to put a 1
    /// or HIGH voltage on the SDA line, then samples the SDA line to make sure
    /// the expected HIGH voltage is on the line, BUT sees that the line is held
    /// LOW, it knows that something is wrong with the line and will stop the
    /// transaction. Keep in mind that, since I2C is open drain with a pull up
    /// resistor, the only way for the I2C controller to see an error is if the
    /// controller wants to let the pin go HIGH by releasing control of the pin,
    /// but something else on the bus is pulling it LOW.
    static inline const auto kBusError =
        Exception(std::errc::io_error, "I2C bus error occurred.");

    /// This exception occurs at the start of an I2C transaction when the
    /// device's I2C address is written to the bus and does NOT acknowledge.
    static inline const auto kDeviceNotFound =
        Exception(std::errc::no_such_device_or_address,
                  "I2C address not found/acknowledged by device.");
  };

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
    std::errc status = static_cast<std::errc>(0);
  };

  /// Perform a I2C transaction using the information contained in the
  /// transaction parameter.
  ///
  /// @throw sjsu::Exception - with error codes, std::errc::timed_out,
  /// std::errc::io_error, or std::errc::no_such_device_or_address, depending on
  /// the circumstances of the error that occurred during the transaction.
  virtual void Transaction(Transaction_t transaction) = 0;

  // ===========================================================================
  // Helper Functions
  // ===========================================================================

  /// Read from a device on the I2C bus
  ///
  /// @param address - device address
  /// @param receive_buffer - pointer to a byte buffer to read information into
  /// @param receive_buffer_length - number of bytes to be written into the
  ///        data buffer
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void Read(uint8_t address,
            uint8_t * receive_buffer,
            size_t receive_buffer_length,
            std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return Transaction({
        .operation  = Operation::kRead,
        .address    = address,
        .data_out   = nullptr,
        .out_length = 0,
        .data_in    = receive_buffer,
        .in_length  = receive_buffer_length,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
    });
  }

  /// Read from a device on the I2C bus
  ///
  /// @param address - device address
  /// @param receive - byte span to read information into
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void Read(uint8_t address,
            std::span<uint8_t> receive,
            std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return Read(address, receive.data(), receive.size(), timeout);
  }

  /// Write to a device on the I2C bus
  ///
  /// @param address - device address
  /// @param transmit_buffer - pointer to a byte buffer to send to the device
  /// @param transmit_buffer_length - number of bytes to be written to the
  ///        device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void Write(uint8_t address,
             const uint8_t * transmit_buffer,
             size_t transmit_buffer_length,
             std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return Transaction({
        .operation  = Operation::kWrite,
        .address    = address,
        .data_out   = transmit_buffer,
        .out_length = transmit_buffer_length,
        .data_in    = nullptr,
        .in_length  = 0,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
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
  void Write(uint8_t address,
             std::initializer_list<uint8_t> transmit,
             std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return Write(address, transmit.begin(), transmit.size(), timeout);
  }

  /// Write to a device on the I2C bus
  ///
  /// Usage:
  ///
  ///     std::array<uint8_t> payload = {0x01, 0x2a, 0x10};
  ///     i2c.Write(0x29, payload);
  ///
  /// @param address - device address
  /// @param transmit - array or span to send to device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void Write(uint8_t address,
             std::span<const uint8_t> transmit,
             std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return Write(address, transmit.data(), transmit.size(), timeout);
  }

  /// Write to a device on the I2C bus, then read from that device.
  ///
  /// This is very common for most I2C devices, where the microcontroller must
  /// send a register address byte, then perform a read transaction in order to
  /// read the data from the device.
  ///
  /// @param address - device address
  /// @param transmit_buffer - pointer to a byte buffer to write information
  ///        into
  /// @param transmit_buffer_length - number of bytes to be written into the
  ///        data buffer
  /// @param receive_buffer - pointer to a byte buffer to read from the device
  /// @param receive_buffer_length - number of bytes to be read from the device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void WriteThenRead(uint8_t address,
                     const uint8_t * transmit_buffer,
                     size_t transmit_buffer_length,
                     uint8_t * receive_buffer,
                     size_t receive_buffer_length,
                     std::chrono::milliseconds timeout = kI2cTimeout)
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
    });
  }

  /// Write to a device on the I2C bus, then read from that device.
  ///
  /// Usage:
  ///
  ///    std::array<uint8_t, 10> buffer;
  ///    i2c.Write(0x29, {0x01}, buffer.data(), buffer.size());
  ///
  /// @param address - device address
  /// @param transmit - array literal to send to device
  /// @param receive_buffer - pointer to a byte buffer to read from the device
  /// @param receive_buffer_length - number of bytes to be read from the device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void WriteThenRead(uint8_t address,
                     std::initializer_list<uint8_t> transmit,
                     uint8_t * receive_buffer,
                     size_t receive_buffer_length,
                     std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return WriteThenRead(address,
                         transmit.begin(),
                         transmit.size(),
                         receive_buffer,
                         receive_buffer_length,
                         timeout);
  }

  /// Write to a device on the I2C bus, then read from that device.
  ///
  /// Usage:
  ///
  ///    std::array<uint8_t, 10> buffer;
  ///    i2c.Write(0x29, {0x01}, buffer.data(), buffer.size());
  ///
  /// @param address - device address
  /// @param transmit - span referencing bytes to transmit to device
  /// @param receive - span to byte buffer to received data from device
  /// @param timeout - Amount of time to wait for a response by device before
  ///        bailing out.
  void WriteThenRead(uint8_t address,
                     std::span<const uint8_t> transmit,
                     std::span<uint8_t> receive,
                     std::chrono::milliseconds timeout = kI2cTimeout)
  {
    return WriteThenRead(address,
                         transmit.data(),
                         transmit.size(),
                         receive.data(),
                         receive.size(),
                         timeout);
  }
};

/// Template specialization that generates an inactive sjsu::I2c.
template <>
inline sjsu::I2c & GetInactive<sjsu::I2c>()
{
  class InactiveI2c : public sjsu::I2c
  {
   public:
    void ModuleInitialize() override {}
    void Transaction(Transaction_t) override {}
  };

  static InactiveI2c inactive;
  return inactive;
}
}  // namespace sjsu
