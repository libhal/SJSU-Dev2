#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>

#include "module.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// The common interface for the CANBUS peripherals.
/// @ingroup l1_peripheral
class Can : public Module
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

    void SetPayload(std::span<const uint8_t> data)
    {
      std::copy_n(
          data.begin(), std::min(payload.size(), data.size()), payload.begin());
    }
  };

  /// Standard baud rate for most CANBUS networks
  static constexpr units::frequency::hertz_t kStandardBaudRate = 100'000_Hz;

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// @param baud - baud rate to configure the CANBUS to
  virtual void ConfigureBaudRate(
      units::frequency::hertz_t baud = kStandardBaudRate) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param message - Message containing the CANBUS contents.
  virtual void Send(const Message_t & message) = 0;

  /// Receive data via CANBUS
  ///
  /// @return retrieved can message. Will return with length field = 0 if no
  /// messages exist.
  virtual Message_t Receive() = 0;

  /// Checks if there is a message available for this channel.
  ///
  /// @returns true a message was received.
  virtual bool HasData() = 0;

  /// Determine if you can communicate over the bus.
  ///
  /// @param id - device you want to ping to determine if you can communicate on
  ///             the bus.
  /// @return true - on success
  /// @return false - on failure
  virtual bool SelfTest(uint32_t id) = 0;

  /// @return true - if the device is "bus Off"
  /// @return false - if the device is NOT "bus off"
  virtual bool IsBusOff() = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param id - ID to send the data to.
  /// @param payload - array literal payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  void Send(uint32_t id, std::initializer_list<uint8_t> payload)
  {
    Message_t message;

    message.id     = id;
    message.length = static_cast<uint8_t>(payload.size());
    message.SetPayload(payload);

    return Send(message);
  }

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param id - ID to send the data to.
  /// @param payload - uint8_t span payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  void Send(uint32_t id, std::span<const uint8_t> payload)
  {
    Message_t message;

    message.id     = id;
    message.length = static_cast<uint8_t>(payload.size());
    message.SetPayload(payload);

    return Send(message);
  }
};
}  // namespace sjsu
