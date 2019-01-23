#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

#include "config.hpp"

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"
#include "utility/time.hpp"

class I2cInterface
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
    uint64_t timeout         = I2cInterface::kI2cTimeout;
    size_t out_length        = 0;
    size_t in_length         = 0;
    size_t position          = 0;
    const uint8_t * data_out = nullptr;
    uint8_t * data_in        = nullptr;
    Status status            = Status::kSuccess;
    Operation operation      = Operation::kWrite;
    uint8_t address          = 0xFF;
    bool repeated            = false;
    bool busy                = false;
  };

  virtual void Initialize()                                           = 0;
  virtual Status Read(uint8_t address, uint8_t * destination, size_t length,
                      uint32_t timeout = kI2cTimeout)                 = 0;
  virtual Status Write(uint8_t address, const uint8_t * destination,
                       size_t length, uint32_t timeout = kI2cTimeout) = 0;
  virtual Status WriteThenRead(uint8_t address, const uint8_t * transmit,
                               size_t out_length, uint8_t * recieve,
                               size_t recieve_length,
                               uint32_t timeout = kI2cTimeout)        = 0;
  Status WriteThenRead(uint8_t address, std::initializer_list<uint8_t> transmit,
                       uint8_t * recieve, size_t recieve_length,
                       uint32_t timeout = kI2cTimeout)
  {
    return WriteThenRead(address, transmit.begin(), transmit.size(), recieve,
                         recieve_length, timeout);
  }
  Status Write(uint8_t address, std::initializer_list<uint8_t> data,
               uint32_t timeout = kI2cTimeout)
  {
    return Write(address, data.begin(), data.size(), timeout);
  }
};

class I2c final : public I2cInterface, protected Lpc40xxSystemController
{
 public:
  // Bringing in I2cInterface's Write and WriteThenRead methods that use
  // std::initializer_list.
  using I2cInterface::Write;
  using I2cInterface::WriteThenRead;

  enum class Port : uint8_t
  {
    kI2c0 = 0,
    kI2c1,
    kI2c2,
    kNumberOfPorts
  };

  enum Control : uint32_t
  {
    kAssertAcknowledge = 1 << 2,  // AA
    kInterrupt         = 1 << 3,  // SI
    kStop              = 1 << 4,  // STO
    kStart             = 1 << 5,  // STA
    kInterfaceEnable   = 1 << 6   // I2EN
  };

  enum class MasterState : uint32_t
  {
    kBusError                          = 0x00,
    kStartCondition                    = 0x08,
    kRepeatedStart                     = 0x10,
    kSlaveAddressWriteSentRecievedAck  = 0x18,
    kSlaveAddressWriteSentRecievedNack = 0x20,
    kTransmittedDataRecievedAck        = 0x28,
    kTransmittedDataRecievedNack       = 0x30,
    kArbitrationLost                   = 0x38,
    kSlaveAddressReadSentRecievedAck   = 0x40,
    kSlaveAddressReadSentRecievedNack  = 0x48,
    kRecievedDataRecievedAck           = 0x50,
    kRecievedDataRecievedNack          = 0x58,
    kOwnAddressReceived                = 0xA0,
    kDoNothing                         = 0xF8
  };

  // UM10562: Chapter 7: LPC408x/407x I/O configuration page 133
  static constexpr uint8_t kI2cPort2Function = 0b010;

  static constexpr uint8_t kNumberOfPorts = util::Value(Port::kNumberOfPorts);
  static constexpr uint8_t kI2c0          = util::Value(Port::kI2c0);
  static constexpr uint8_t kI2c1          = util::Value(Port::kI2c1);
  static constexpr uint8_t kI2c2          = util::Value(Port::kI2c2);

  inline static constexpr uint8_t kPconp[kNumberOfPorts] = { [kI2c0] = 7,
                                                             [kI2c1] = 19,
                                                             [kI2c2] = 26 };

  inline static constexpr IRQn_Type kIrq[kNumberOfPorts] = {
    [kI2c0] = I2C0_IRQn, [kI2c1] = I2C1_IRQn, [kI2c2] = I2C2_IRQn
  };

  inline static LPC_I2C_TypeDef * i2c[kNumberOfPorts] = { [kI2c0] = LPC_I2C0,
                                                          [kI2c1] = LPC_I2C1,
                                                          [kI2c2] = LPC_I2C2 };

  template <Port port>
  static void I2cHandler()
  {
    static constexpr uint8_t kPort = util::Value(port);
    MasterState state              = MasterState(i2c[kPort]->STAT);
    uint32_t clear_mask            = 0;
    uint32_t set_mask              = 0;
    switch (state)
    {
      case MasterState::kBusError:  // 0x00
      {
        transaction[kPort].status = Status::kBusError;
        set_mask = Control::kAssertAcknowledge | Control::kStop;
        break;
      }
      case MasterState::kStartCondition:  // 0x08
      {
        i2c[kPort]->DAT = transaction[kPort].GetProperAddress();
        break;
      }
      case MasterState::kRepeatedStart:  // 0x10
      {
        transaction[kPort].operation = Operation::kRead;
        i2c[kPort]->DAT              = transaction[kPort].GetProperAddress();
        break;
      }
      case MasterState::kSlaveAddressWriteSentRecievedAck:  // 0x18
      {
        clear_mask = Control::kStart;
        if (transaction[kPort].out_length == 0)
        {
          transaction[kPort].busy   = false;
          transaction[kPort].status = Status::kSuccess;
          set_mask                  = Control::kStop;
        }
        else
        {
          size_t position = transaction[kPort].position++;
          i2c[kPort]->DAT = transaction[kPort].data_out[position];
        }
        break;
      }
      case MasterState::kSlaveAddressWriteSentRecievedNack:  // 0x20
      {
        clear_mask                = Control::kStart;
        transaction[kPort].busy   = false;
        transaction[kPort].status = Status::kDeviceNotFound;
        set_mask                  = Control::kStop;
        break;
      }
      case MasterState::kTransmittedDataRecievedAck:  // 0x28
      {
        if (transaction[kPort].position >= transaction[kPort].out_length)
        {
          if (transaction[kPort].repeated)
          {
            // OR with 1 to set address as READ for the next transaction
            transaction[kPort].operation = Operation::kRead;
            transaction[kPort].position  = 0;
            set_mask                     = Control::kStart;
          }
          else
          {
            transaction[kPort].busy = false;
            set_mask                = Control::kStop;
          }
        }
        else
        {
          size_t position = transaction[kPort].position++;
          i2c[kPort]->DAT = transaction[kPort].data_out[position];
        }
        break;
      }
      case MasterState::kTransmittedDataRecievedNack:  // 0x30
      {
        transaction[kPort].busy = false;
        set_mask                = Control::kStop;
        break;
      }
      case MasterState::kArbitrationLost:  // 0x38
      {
        set_mask = Control::kStart;
        break;
      }
      case MasterState::kSlaveAddressReadSentRecievedAck:  // 0x40
      {
        clear_mask = Control::kStart;
        if (transaction[kPort].in_length == 0)
        {
          set_mask = Control::kStop;
        }
        // If we only want 1 byte, make sure to nack that byte
        else if (transaction[kPort].in_length == 1)
        {
          clear_mask |= Control::kAssertAcknowledge;
        }
        // If we want more then 1 byte, make sure to ack the first byte
        else
        {
          set_mask = Control::kAssertAcknowledge;
        }
        break;
      }
      case MasterState::kSlaveAddressReadSentRecievedNack:  // 0x48
      {
        clear_mask                = Control::kStart;
        transaction[kPort].status = Status::kDeviceNotFound;
        transaction[kPort].busy   = false;
        set_mask                  = Control::kStop;
        break;
      }
      case MasterState::kRecievedDataRecievedAck:  // 0x50
      {
        const size_t kBufferEnd = transaction[kPort].in_length;
        if (transaction[kPort].position < kBufferEnd)
        {
          const size_t kPosition = transaction[kPort].position;
          transaction[kPort].data_in[kPosition] =
              static_cast<uint8_t>(i2c[kPort]->DAT);
          transaction[kPort].position++;
        }
        // Check if the position has been pushed past the buffer length
        if (transaction[kPort].position + 1 >= kBufferEnd)
        {
          clear_mask              = Control::kAssertAcknowledge;
          transaction[kPort].busy = false;
        }
        else
        {
          set_mask = Control::kAssertAcknowledge;
        }
        break;
      }
      case MasterState::kRecievedDataRecievedNack:  // 0x58
      {
        transaction[kPort].busy = false;
        if (transaction[kPort].in_length != 0)
        {
          size_t position = transaction[kPort].position++;
          transaction[kPort].data_in[position] =
              static_cast<uint8_t>(i2c[kPort]->DAT);
        }
        set_mask = Control::kStop;
        break;
      }
      case MasterState::kDoNothing:  // 0xF8
      {
        break;
      }
      default:
      {
        clear_mask = Control::kStop;
        SJ2_ASSERT_FATAL(false, "Invalid I2C State Reached!!");
        break;
      }
    }
    // Clear I2C Interrupt flag
    clear_mask |= Control::kInterrupt;
    // Set register controls
    i2c[kPort]->CONSET = set_mask;
    i2c[kPort]->CONCLR = clear_mask;
  }

  static Transaction_t & GetTransactionInfo(Port port)
  {
    return transaction[util::Value(port)];
  }

  // This defaults to I2C port 2
  constexpr I2c()
      : sda_(sda_pin_),
        scl_(scl_pin_),
        sda_pin_(Pin(0, 10)),
        scl_pin_(Pin(0, 11)),
        port_(util::Value(Port::kI2c2)),
        initialized_(false),
        pins_initialized_(false)
  {
  }
  /// Pins must be initialized to I2C prior to being passed into this class
  /// @param sda_pin address of an initialized sda pin object.
  /// @param scl_pin address of an initialized scl pin object.
  /// @param initialize_pins should only be used for testing.
  constexpr I2c(Port port, PinInterface * sda_pin, PinInterface * scl_pin,
                bool pins_are_initialized = true)
      : sda_(*sda_pin),
        scl_(*scl_pin),
        sda_pin_(Pin::CreateInactivePin()),
        scl_pin_(Pin::CreateInactivePin()),
        port_(util::Value(port)),
        initialized_(false),
        pins_initialized_(pins_are_initialized)
  {
  }
  void Initialize() override
  {
    SJ2_ASSERT_FATAL(Port(port_) != Port::kNumberOfPorts,
                     "I2C.ort::kNumberOfPorts is not a Invalid I2C Port!");
    if (!pins_initialized_)
    {
      sda_.SetPinFunction(kI2cPort2Function);
      scl_.SetPinFunction(kI2cPort2Function);
      sda_.SetAsOpenDrain();
      scl_.SetAsOpenDrain();
      sda_.SetMode(PinInterface::Mode::kInactive);
      scl_.SetMode(PinInterface::Mode::kInactive);
    }
    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kI2c2);
    float peripheral_frequency = static_cast<float>(GetPeripheralFrequency());
    float scll         = ((peripheral_frequency / 75'000.0f) / 2.0f) * 0.7f;
    i2c[port_]->SCLL   = static_cast<uint32_t>(scll);
    float sclh         = ((peripheral_frequency / 75'000.0f) / 2.0f) * 1.3f;
    i2c[port_]->SCLH   = static_cast<uint32_t>(sclh);
    i2c[port_]->CONCLR = Control::kAssertAcknowledge | Control::kStart |
                         Control::kStop | Control::kInterrupt;
    i2c[port_]->CONSET = Control::kInterfaceEnable;
    RegisterIsr(kIrq[port_], kHandlers[port_], true);
    initialized_ = true;
  }

  Status Read(uint8_t address, uint8_t * data, size_t length,
              uint32_t timeout = kI2cTimeout) override
  {
    transaction[port_] = { .timeout    = timeout,
                           .out_length = 0,
                           .in_length  = length,
                           .position   = 0,
                           .data_out   = nullptr,
                           .data_in    = data,
                           .status     = Status::kSuccess,
                           .operation  = Operation::kRead,
                           .address    = address,
                           .repeated   = false,
                           .busy       = true };
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }

  Status Write(uint8_t address, const uint8_t * data, size_t length,
               uint32_t timeout = kI2cTimeout) override
  {
    transaction[port_] = { .timeout    = timeout,
                           .out_length = length,
                           .in_length  = 0,
                           .position   = 0,
                           .data_out   = data,
                           .data_in    = nullptr,
                           .status     = Status::kSuccess,
                           .operation  = Operation::kWrite,
                           .address    = address,
                           .repeated   = false,
                           .busy       = true };
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }
  Status WriteThenRead(uint8_t address, const uint8_t * transmit,
                       size_t out_length, uint8_t * recieve,
                       size_t recieve_length,
                       uint32_t timeout = kI2cTimeout) override
  {
    transaction[port_] = { .timeout    = timeout,
                           .out_length = out_length,
                           .in_length  = recieve_length,
                           .position   = 0,
                           .data_out   = transmit,
                           .data_in    = recieve,
                           .status     = Status::kSuccess,
                           .operation  = Operation::kWrite,
                           .address    = address,
                           .repeated   = true,
                           .busy       = true };
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }

  inline constexpr static IsrPointer kHandlers[kNumberOfPorts] = {
    [kI2c0] = I2cHandler<Port::kI2c0>,
    [kI2c1] = I2cHandler<Port::kI2c1>,
    [kI2c2] = I2cHandler<Port::kI2c2>
  };

 protected:
  virtual Status BlockUntilFinished()
  {
#if !defined(HOST_TEST)
    SJ2_ASSERT_FATAL(initialized_,
                     "Attempted to use I2C.%u, but peripheral was not "
                     "initialized! Be sure to run the Initialize() method "
                     "of this class, before using it.",
                     port_);
    auto wait_for_i2c_transaction = [this]() -> bool {
      return !transaction[port_].busy;
    };
    Status status = Wait(transaction[port_].timeout, wait_for_i2c_transaction);

    if (status == Status::kTimedOut)
    {
      // Abort I2C communication if this point is reached!
      i2c[port_]->CONSET = Control::kAssertAcknowledge | Control::kStop;
      SJ2_ASSERT_WARNING(
          transaction[port_].out_length == 0 ||
              transaction[port_].in_length == 0,
          "I2C.%u took too long to process and timed out! If the "
          "transaction needs more time, you may want to increase the "
          "timeout time.",
          port_);
      transaction[port_].status = Status::kTimedOut;
    }
    // Ensure that start is cleared before leaving this function
    i2c[port_]->CONCLR = Control::kStart;
#endif  // !defined HOST_TEST
    return transaction[port_].status;
  }
  inline static Transaction_t transaction[kNumberOfPorts];
  PinInterface & sda_;
  PinInterface & scl_;
  Pin sda_pin_;
  Pin scl_pin_;
  uint8_t port_;
  bool initialized_;
  bool pins_initialized_;
};
