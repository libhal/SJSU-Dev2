#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/macros.hpp"

class CanInterface
{
 public:
  // This struct represents a CAN message based on the BOSCH CAN spec 2.0B.
  // This struct is HW mapped to 32-bit registers TFI, TID, TDx (pg. 568).
  SJ2_PACKED(struct) CanMessage_t
  {
    union {
      // TFI - Transmit Frame Information Register - Message frame info
      uint32_t frame;
      struct
      {
        // User definable priority level for a message (0-255)
        uint8_t tx_priority : 8;
        // Reserved
        uint8_t : 8;
        // Data payload length (0-7 bytes)
        uint8_t data_length : 4;
        // Reserved
        uint16_t : 10;
        // Request a data frame from a node
        uint8_t remote_tx_request : 1;
        // 11-bit or 29-bit identifier format
        uint8_t frame_format : 1;
      } frame_data;
    };
    // TID - Transmit Identifier Register
    uint32_t id;  // CAN message ID
    // TDx - Transmit Data Registers A/B
    uint8_t data[8];  // CAN message data payload
  };

  enum class TestType
  {
    kLocal  = 0,
    kGlobal = 1,
  };

  virtual bool Initialize()                       = 0;
  virtual bool Send(const CanMessage_t & message) = 0;
  virtual bool Receive(CanMessage_t * message)    = 0;
  virtual bool SelfTestBus(const TestType kType)  = 0;
  virtual bool IsBusOff()                         = 0;
  virtual void EnableBus()                        = 0;
};
