#pragma once

#include <cstdint>
#include <initializer_list>

#include "config.hpp"

#include "L0_Platform/lpc40xx/interrupt.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class I2c
{
 public:
  enum Operation : uint8_t
  {
    kWrite = 0,
    kRead  = 1,
  };

  static constexpr uint32_t kI2cTimeout = 1000;  // units milliseconds

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

  // Interface Methods
  virtual Status Initialize() const                           = 0;
  virtual Status Transaction(Transaction_t transaction) const = 0;
  // Utility Methods
  Status Read(uint8_t address,
              uint8_t * data,
              size_t length,
              uint32_t timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kRead,
        .address    = address,
        .data_out   = nullptr,
        .out_length = 0,
        .data_in    = data,
        .in_length  = length,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }

  Status Write(uint8_t address,
               const uint8_t * data,
               size_t length,
               uint32_t timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kWrite,
        .address    = address,
        .data_out   = data,
        .out_length = length,
        .data_in    = nullptr,
        .in_length  = 0,
        .position   = 0,
        .repeated   = false,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }

  Status Write(uint8_t address,
               std::initializer_list<uint8_t> transmit,
               uint32_t timeout = kI2cTimeout) const
  {
    return Write(address, transmit.begin(), transmit.size(), timeout);
  }

  Status WriteThenRead(uint8_t address,
                       const uint8_t * transmit,
                       size_t out_length,
                       uint8_t * recieve,
                       size_t recieve_length,
                       uint32_t timeout = kI2cTimeout) const
  {
    return Transaction({
        .operation  = Operation::kWrite,
        .address    = address,
        .data_out   = transmit,
        .out_length = out_length,
        .data_in    = recieve,
        .in_length  = recieve_length,
        .position   = 0,
        .repeated   = true,
        .busy       = true,
        .timeout    = timeout,
        .status     = Status::kSuccess,
    });
  }
  Status WriteThenRead(uint8_t address,
                       std::initializer_list<uint8_t> transmit,
                       uint8_t * recieve,
                       size_t recieve_length,
                       uint32_t timeout = kI2cTimeout) const
  {
    return WriteThenRead(address,
                         transmit.begin(),
                         transmit.size(),
                         recieve,
                         recieve_length,
                         timeout);
  }
};
}  // namespace sjsu
