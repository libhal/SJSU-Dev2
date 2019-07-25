#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include "utility/status.hpp"

namespace sjsu
{
class Can
{
 public:
  union Data_t {
    uint64_t qword;
    uint32_t dword[2];
    uint16_t word[4];
    uint8_t bytes[8];
  };

  // This struct represents a transmit message based on the BOSCH CAN spec 2.0B.
  // It is HW mapped to 32-bit registers TFI, TID, TDx (pg. 568).
  struct [[gnu::packed]] TxMessage_t
  {
    union {
      // TFI - Transmit Frame Information Register - Message frame info
      uint32_t TFI;
      struct
      {
        // User definable priority level for a message (0-255)
        uint8_t tx_priority : 8;
        uint8_t reserved_1 : 8;
        // Data payload length (0-7 bytes)
        uint8_t data_length : 4;
        // Reserved
        uint16_t reserved_2 : 10;
        // Request a data frame from a node
        uint8_t remote_tx_request : 1;
        // 11-bit or 29-bit identifier format
        uint8_t frame_format : 1;
      } frame_data;
    };
    // TID - Transmit Identifier Register
    uint32_t id;  // CAN message ID
    // TDx - Transmit Data Registers A/B
    Data_t data;  // CAN message data payload
  };
  // This struct represents a receive message based on the BOSCH CAN spec 2.0B.
  // It is HW mapped to 32-bit registers RFS, RID, RDx (pg. 565).
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
    // RID - Receive Identifier Register
    uint32_t id;  // CAN message ID
    // RDx - Receive Data Registers A/B
    Data_t data;  // CAN message data payload
  };

  virtual Status Initialize() const                                      = 0;
  virtual bool Send(TxMessage_t * const kMessage, uint32_t id,
                    const uint8_t * const kPayload, size_t length) const = 0;
  virtual bool Receive(RxMessage_t * const kMessage) const               = 0;
  virtual bool SelfTestBus(uint32_t id) const                            = 0;
  virtual bool IsBusOff() const                                          = 0;
  virtual bool GetFrameErrorLocation(const char * &error_message) const  = 0;
  virtual void EnableBus() const                                         = 0;

  bool Send(TxMessage_t * const kMessage, uint32_t id,
            std::initializer_list<uint8_t> payload) const
  {
    return Send(kMessage, id, payload.begin(), payload.size());
  }
};
}  // namespace sjsu
