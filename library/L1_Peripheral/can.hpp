#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

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

  /// Data section of a CANBUS message
  union Data_t {
    /// Full payload
    uint64_t qword;
    /// Payload divided into 2 32-bit chunks
    uint32_t dword[2];
    /// Payload divided into 4 16-bit chunks
    uint16_t word[4];
    /// Payload divided into 8 bytes
    uint8_t bytes[8];
  };

  /// This struct represents a transmit message based on the BOSCH CAN
  /// spec 2.0B. It is HW mapped to 32-bit registers TFI, TID, TDx (pg. 568).
  struct [[gnu::packed]] TxMessage_t
  {
    union {
      /// TFI - Transmit Frame Information Register - Message frame info
      uint32_t TFI;
      struct
      {
        /// User definable priority level for a message (0-255)
        uint8_t tx_priority : 8;
        uint8_t : 8;
        /// Data payload length (0-7 bytes)
        uint8_t data_length : 4;
        uint16_t : 10;
        /// Request a data frame from a node
        uint8_t remote_tx_request : 1;
        /// 11-bit or 29-bit identifier format
        uint8_t frame_format : 1;
      } frame_data;
    };
    /// TID - Transmit Identifier Register, CAN message ID
    uint32_t id;
    /// TDx - Transmit Data Registers A/B, CAN message data payload
    Data_t data;
  };

  /// This struct represents a receive message based on the BOSCH CAN spec 2.0B.
  /// It is HW mapped to 32-bit registers RFS, RID, RDx (pg. 565).
  struct [[gnu::packed]] RxMessage_t
  {
    union {
      // RFS - Receive Frame Status Register
      uint32_t RFS;
      struct
      {
        uint16_t id_index : 10;
        uint8_t bypass_mode : 1;
        uint8_t reserved_1 : 5;
        uint8_t data_length : 4;
        uint16_t reserved_2 : 10;
        uint8_t remote_tx_request : 1;
        uint8_t frame_format : 1;
      } frame_data;
    };
    /// RID - Receive Identifier Register
    uint32_t id;  // CAN message ID
    /// RDx - Receive Data Registers A/B
    Data_t data;  // CAN message data payload
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Initialize the CANBUS peripheral. Must be called before calling anything
  /// else in the driver.
  virtual Status Initialize() const = 0;

  /// Enables CANBUS and allows communication. Must be called after Initialize()
  /// before using this driver.
  virtual void EnableBus() const = 0;

  // TODO(#1103): Refactor this such that the user does not need to supply their
  // own TxMessage_t structure, we handle that on the interface utility method
  // side.

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param kMessage - Buffer to contain CANBUS contents. This should be zero
  ///                   intialized before usage.
  /// @param id - the ID of the device to send the message to
  /// @param kPayload - the data to send to the device
  /// @param length - the number of bytes to transmit
  /// @return true - on success
  /// @return false - on failure
  virtual bool Send(TxMessage_t * const kMessage,
                    uint32_t id,
                    const uint8_t * const kPayload,
                    size_t length) const = 0;

  /// Receive data via CANBUS
  ///
  /// @param kMessage - will contain the received data
  /// @return true - on success
  /// @return false - on failure
  virtual bool Receive(RxMessage_t * const kMessage) const = 0;

  /// Determine if you can communicate over the bus.
  ///
  /// @param id - device you want to ping to determine if you can communicate on
  ///             the bus.
  /// @return true - on success
  /// @return false - on failure
  virtual bool SelfTestBus(uint32_t id) const = 0;

  /// @return true - if the device is "bus Off"
  /// @return false - if the device is NOT "bus off"
  virtual bool IsBusOff() const = 0;

  /// Returns an error message into the error_message parameter
  ///
  /// @param error_message - location to store the error message
  /// @return true - if an error did occur and the error_message field was
  ///         populated.
  virtual bool GetFrameErrorLocation(const char *& error_message) const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param kMessage - Transmit buffer to hold the CANBUS transmission data
  /// @param id - ID to send the data to.
  /// @param payload - array literal payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  bool Send(TxMessage_t * const kMessage,
            uint32_t id,
            std::initializer_list<uint8_t> payload) const
  {
    return Send(kMessage, id, payload.begin(), payload.size());
  }
};
}  // namespace sjsu
