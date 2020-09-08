#pragma once

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <scope>

#include "utility/status.hpp"

namespace sjsu
{
/// The common interface for the CANBUS peripherals.
/// @ingroup l1_peripheral
class Can
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// This struct represents a transmit message based on the BOSCH CAN
  /// spec 2.0B.
  struct Message_t
  {
    /// The format of the can message
    enum class Format
    {
      /// Use 11-bit ID message
      kStandard = 0,
      /// Use 29-bit ID message
      kExtended = 1,
      kNumberOfFormats,
    };

    /// CAN message ID
    uint32_t id;
    /// Length of the payload
    uint8_t length = 0;
    /// Container of the payload contents
    std::array<uint8_t, 8> payload;
    /// ID format
    Format format = Format::kStandard;
    /// Is this message a remote request message. If so the contents of payload
    /// are ignored. Length shall have the length of requested data to get back
    /// from the device responsible for message id.
    bool is_remote_request = false;
  };

  /// Standard baud rate for most CANBUS networks
  static constexpr units::frequency::hertz_t kStandardBaudRate = 100'000_Hz;

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Initialize the CANBUS peripheral. Must be called before calling anything
  /// else in the driver.
  virtual Returns<void> Initialize() const = 0;

  /// Enables CANBUS and allows communication. Must be called after Initialize()
  /// before using this driver.
  virtual Returns<void> Enable() const = 0;

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param message - Message containing the CANBUS contents.
  virtual Returns<void> Send(const Message_t & message) const = 0;

  /// Receive data via CANBUS
  ///
  /// @return retrieved can message. Will return with length field = 0 if no
  /// messages exist.
  virtual Returns<Message_t> Receive() const = 0;

  /// Checks if there is a message available for this channel.
  ///
  /// @returns true a message was received.
  virtual Returns<bool> HasData() const = 0;

  /// Determine if you can communicate over the bus.
  ///
  /// @param id - device you want to ping to determine if you can communicate on
  ///             the bus.
  /// @return true - on success
  /// @return false - on failure
  virtual Returns<bool> SelfTest(uint32_t id) const = 0;

  /// @return true - if the device is "bus Off"
  /// @return false - if the device is NOT "bus off"
  virtual Returns<bool> IsBusOff() const = 0;

  /// @param baud - baud rate to configure the CANBUS to
  virtual Returns<void> SetBaudRate(
      units::frequency::hertz_t baud = kStandardBaudRate) const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param id - ID to send the data to.
  /// @param payload - array literal payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  Returns<void> Send(uint32_t id, std::initializer_list<uint8_t> payload) const
  {
    Message_t message;

    message.id     = id;
    message.length = static_cast<uint8_t>(payload.size());

    std::copy_n(payload.begin(),
                std::min(payload.size(), message.payload.size()),
                message.payload.begin());

    return Send(message);
  }
};
}  // namespace sjsu
