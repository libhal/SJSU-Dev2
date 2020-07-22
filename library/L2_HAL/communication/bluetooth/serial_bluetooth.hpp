#pragma once

#include <chrono>
#include <initializer_list>

namespace sjsu
{
namespace bluetooth
{
/// The interface should be implemented by bluetooth peripherals that utilizes
/// serial communication such as SPI or UART for the transferring of data.
class SerialBluetooth
{
 public:
  /// @returns True if the device currently has a connection with a another
  ///          device.
  virtual bool GetState() const = 0;

  /// Disconnect from the current connection.
  virtual void Disconnect() const = 0;

  virtual void SendCommand(const char * command,
                           const char * parameter           = nullptr,
                           std::chrono::nanoseconds timeout = 10ms) const = 0;

  /// Sends a command to the device.
  ///
  /// @param command The command to send.
  /// @param parameters Parameters to go with command if applicable.
  virtual void SendCommand(const char * command,
                           const std::initializer_list<uint8_t> parameters = {},
                           std::chrono::nanoseconds timeout = 10ms) const = 0;
};
}  // namespace bluetooth
}  // namespace sjsu
