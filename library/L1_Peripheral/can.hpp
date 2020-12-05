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
    ///
    bool is_valid = true;
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

  /// @param baud - the baud rate to set the CANBUS communication frequency to.
  virtual void ConfigureBaudRate(
      units::frequency::hertz_t baud = kStandardBaudRate) = 0;

  /// Filter out CANBUS messages based on their ID. Without such a filter, every
  /// single message on the CANBUS will be received, even those not intended or
  /// useful for this target device. Without a filter, software will need to
  /// perform work to throw away CAN messages that are irrelevant. This method
  /// will utilize the CAN peripheral hardware to perform message filtering.
  ///
  /// NOTE: ConfigureAcceptanceFilter(true) must be called for the filter to be
  /// taken into effect.
  ///
  /// @param id - the ID to accept
  /// @param mask - a mask to be applied to the ID. This works by checking the
  ///        received ID against this equation: (received_id & mask == id &
  ///        mask). This allows for a greater range of IDs to be accepted from
  ///        the bus. For example, to accept IDs 0x140, 0x141, 0x142, 0x143, ID
  ///        will be 0x140 and mask = 0x14C. This works because the last 2 bits
  ///        of the mask are zeros, and thus will be ignored when performing the
  ///        mask check operation.
  /// @param is_extended - default false, specifies if the filter is meant for
  ///        extended IDs.
  /// @throw sjsu::Exception with std::errc::not_supported if filtering is not
  ///        supported for this canbus peripheral.
  /// @return true - if the filter was able to be installed successfully
  /// @return false - if the filter was NOT able to be installed successfully.
  ///         This typically occurres when the number of hardware filters runs
  ///         out.
  virtual bool ConfigureFilter(uint32_t id,
                               uint32_t mask,
                               bool is_extended = false) = 0;

  /// Enables and disables the acceptance filter. When enabled, the filters
  ///
  /// @param enable - set to true to enable enable filter, false to disable.
  /// @throw sjsu::Exception with std::errc::not_supported if filtering is not
  ///        supported for this canbus peripheral.
  virtual void ConfigureAcceptanceFilter(bool enable) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param message - Message containing the CANBUS contents.
  virtual void Send(const Message_t & message) = 0;

  /// Receive a CANBUS message from the queue. If an non-zero ID is supplied, a
  /// message with that ID will be returned. Typically this is done using a
  /// queue and has a O(n) time complexity. Some implementations like LPC40xx
  /// and LPC17xx utilize a special RAM lookup table, giving a time complexity
  /// of O(1).
  ///
  /// @param id - retreive a message from the hardware queue that matches this
  ///        ID. ID zero means retrieve any message from the queue.
  /// @return Message_t - messages
  virtual Message_t Receive(uint32_t id = 0) = 0;

  /// Checks if there is a message available for this channel.
  ///
  /// @returns true a message was received.
  virtual bool HasData(uint32_t id = 0) = 0;

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
