#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "config.hpp"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L2_Utilities/enum.hpp"
#include "L2_Utilities/macros.hpp"

class I2cInterface
{
 public:
  enum class Status : uint8_t
  {
    kSuccess        = 0,
    kBusError       = 1,
    kDeviceNotFound = 2,
    kTimedOut       = 3
  };
  static constexpr uint32_t kDefaultTimeout = 1000;  // units milliseconds

  virtual void Initialize()                                        = 0;
  virtual Status Read(uint8_t address, uint8_t * destination, size_t length,
                      uint32_t timeout = kDefaultTimeout)          = 0;
  virtual Status Write(uint8_t address, uint8_t * destination, size_t length,
                       uint32_t timeout = kDefaultTimeout)         = 0;
  virtual Status WriteThenRead(uint8_t address, uint8_t * transmit,
                               size_t transmit_length, uint8_t * recieve,
                               size_t recieve_length,
                               uint32_t timeout = kDefaultTimeout) = 0;
};

class I2c : public I2cInterface
{
 public:
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

  enum Operation : uint8_t
  {
    kWrite = 0,
    kRead  = 1,
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

  struct Transaction_t
  {
    constexpr Transaction_t()
        : address(0xFF),
          transmitter(nullptr),
          transmit_length(0),
          receiver(nullptr),
          receive_length(0),
          position(0),
          repeated(false),
          busy(false),
          status(Status::kSuccess),
          operation(Operation::kWrite),
          timeout(kDefaultTimeout)
    {
    }
    constexpr Transaction_t(uint8_t set_address, uint8_t * set_transmitter,
                            size_t set_transmit_length, uint8_t * set_receiver,
                            size_t set_receive_length, size_t set_position,
                            bool set_repeated, bool set_busy, Status set_status,
                            Operation set_operation, uint64_t set_timeout)
        : address(set_address),
          transmitter(set_transmitter),
          transmit_length(set_transmit_length),
          receiver(set_receiver),
          receive_length(set_receive_length),
          position(set_position),
          repeated(set_repeated),
          busy(set_busy),
          status(set_status),
          operation(set_operation),
          timeout(set_timeout)
    {
      address = static_cast<uint8_t>(set_address << 1);
      if (set_operation == Operation::kRead)
      {
        address |= 1;
      }
    }
    uint8_t address;
    uint8_t * transmitter;
    size_t transmit_length;
    uint8_t * receiver;
    size_t receive_length;
    size_t position;
    bool repeated;
    bool busy;
    Status status;
    Operation operation;
    uint64_t timeout;
  };

  // UM10562: Chapter 7: LPC408x/407x I/O configuration page 133
  static constexpr uint8_t kI2cPort2Function = 0b010;

  static constexpr uint8_t kNumberOfPorts = util::Value(Port::kNumberOfPorts);

  static const uint8_t kPconp[kNumberOfPorts];
  static const IRQn_Type kIrq[kNumberOfPorts];
  static LPC_I2C_TypeDef * i2c[kNumberOfPorts];

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
        i2c[kPort]->DAT = transaction[kPort].address;
        break;
      }
      case MasterState::kRepeatedStart:  // 0x10
      {
        i2c[kPort]->DAT = transaction[kPort].address;
        break;
      }
      case MasterState::kSlaveAddressWriteSentRecievedAck:  // 0x18
      {
        clear_mask = Control::kStart;
        if (transaction[kPort].transmit_length == 0)
        {
          SetBusyState(port, false);
          transaction[kPort].status = Status::kSuccess;
          set_mask                  = Control::kStop;
        }
        else
        {
          size_t position = transaction[kPort].position++;
          i2c[kPort]->DAT = transaction[kPort].transmitter[position];
        }
        break;
      }
      case MasterState::kSlaveAddressWriteSentRecievedNack:  // 0x20
      {
        clear_mask = Control::kStart;
        SetBusyState(port, false);
        transaction[kPort].status = Status::kDeviceNotFound;
        set_mask                  = Control::kStop;
        break;
      }
      case MasterState::kTransmittedDataRecievedAck:  // 0x28
      {
        if (transaction[kPort].position >= transaction[kPort].transmit_length)
        {
          if (transaction[kPort].repeated)
          {
            // OR with 1 to set address as READ for the next transaction
            transaction[kPort].address |= 0b1;
            transaction[kPort].position = 0;
            set_mask                    = Control::kStart;
          }
          else
          {
            SetBusyState(port, false);
            set_mask = Control::kStop;
          }
        }
        else
        {
          size_t position = transaction[kPort].position++;
          i2c[kPort]->DAT = transaction[kPort].transmitter[position];
        }
        break;
      }
      case MasterState::kTransmittedDataRecievedNack:  // 0x30
      {
        SetBusyState(port, false);
        set_mask = Control::kStop;
        break;
      }
      case MasterState::kArbitrationLost:  // 0x38
      {
        set_mask = Control::kStart;
        break;
      }
      case MasterState::kSlaveAddressReadSentRecievedAck:  // 0x40
      {
        if (transaction[kPort].receive_length == 0)
        {
          clear_mask = Control::kAssertAcknowledge | Control::kStart;
        }
        else
        {
          clear_mask = Control::kStart;
          set_mask   = Control::kAssertAcknowledge;
        }
        break;
      }
      case MasterState::kSlaveAddressReadSentRecievedNack:  // 0x48
      {
        clear_mask                = Control::kStart;
        transaction[kPort].status = Status::kDeviceNotFound;
        SetBusyState(port, false);
        set_mask = Control::kStop;
        break;
      }
      case MasterState::kRecievedDataRecievedAck:  // 0x50
      {
        const size_t kBufferEnd = transaction[kPort].receive_length;
        if (transaction[kPort].position < kBufferEnd)
        {
          const size_t kPosition = transaction[kPort].position;
          transaction[kPort].receiver[kPosition] =
              static_cast<uint8_t>(i2c[kPort]->DAT);
          transaction[kPort].position++;
        }
        // Check if the position has been pushed past the buffer length
        if (transaction[kPort].position < kBufferEnd)
        {
          set_mask = Control::kAssertAcknowledge;
        }
        else
        {
          SetBusyState(port, false);
          clear_mask = Control::kAssertAcknowledge;
        }
        break;
      }
      case MasterState::kRecievedDataRecievedNack:  // 0x58
      {
        SetBusyState(port, false);
        if (transaction[kPort].receive_length != 0)
        {
          size_t position = transaction[kPort].position++;
          transaction[kPort].receiver[position] =
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
    i2c[kPort]->CONCLR = clear_mask;
    i2c[kPort]->CONSET = set_mask;
  }

  static void SetBusyState(Port port, bool busy_state)
  {
    uint8_t port_number           = util::Value(port);
    transaction[port_number].busy = busy_state;
  }

  static Transaction_t & GetTransactionInfo(Port port)
  {
    uint8_t port_number = util::Value(port);
    return transaction[port_number];
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
  // Pins must be initialized to I2C prior to being passed into this class
  // @param initialize_pins should only be used for testing
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
    // TODO(#6): Need to include power I2C on logic.
    i2c[port_]->SCLL   = ((config::kSystemClockRate / 75'000) / 2) * 0.7;
    i2c[port_]->SCLH   = ((config::kSystemClockRate / 75'000) / 2) * 1.3;
    i2c[port_]->CONCLR = Control::kAssertAcknowledge | Control::kStart |
                         Control::kStop | Control::kInterrupt;
    i2c[port_]->CONSET = Control::kInterfaceEnable;
    RegisterIsr(kIrq[port_], handlers[port_], true);
    initialized_ = true;
  }
  Status Read(uint8_t address, uint8_t * data, size_t length,
              uint32_t timeout = kDefaultTimeout) override
  {
    transaction[port_] = Transaction_t(address,           // address
                                       nullptr,           // transmitter
                                       0,                 // transmit_length
                                       data,              // receiver
                                       length,            // receive_length
                                       0,                 // position
                                       false,             // repeated
                                       true,              // busy
                                       Status::kSuccess,  // status
                                       Operation::kRead,  // operation
                                       timeout);          // timeout
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }
  Status Write(uint8_t address, uint8_t * data, size_t length,
               uint32_t timeout = kDefaultTimeout) override
  {
    transaction[port_] = Transaction_t(address,            // address
                                       data,               // transmitter
                                       length,             // transmit_length
                                       nullptr,            // receiver
                                       0,                  // receive_length
                                       0,                  // position
                                       false,              // repeated
                                       true,               // busy
                                       Status::kSuccess,   // status
                                       Operation::kWrite,  // operation
                                       timeout);           // timeout
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }
  Status WriteThenRead(uint8_t address, uint8_t * transmit,
                       size_t transmit_length, uint8_t * recieve,
                       size_t recieve_length,
                       uint32_t timeout = kDefaultTimeout) override
  {
    transaction[port_] = Transaction_t(address,            // address
                                       transmit,           // transmitter
                                       transmit_length,    // transmit_length
                                       recieve,            // receiver
                                       recieve_length,     // receive_length
                                       0,                  // position
                                       true,               // repeated
                                       true,               // busy
                                       Status::kSuccess,   // status
                                       Operation::kWrite,  // operation
                                       timeout);           // timeout
    i2c[port_]->CONSET = Control::kStart;
    return BlockUntilFinished();
  }

  static IsrPointer handlers[kNumberOfPorts];

 protected:
  static Transaction_t transaction[kNumberOfPorts];

  virtual Status BlockUntilFinished()
  {
#if !defined(HOST_TEST)
    SJ2_ASSERT_FATAL(initialized_,
                     "Attempted to use I2C.%u, but peripheral was not "
                     "initialized! Be sure to run the Initialize() method "
                     "of this class, before using it.",
                     port_);
    uint64_t timeout_time = Milliseconds() + transaction[port_].timeout;
    uint64_t current_time = Milliseconds();
    while (transaction[port_].busy && current_time < timeout_time)
    {
      current_time = Milliseconds();
    }
    if (current_time >= timeout_time && transaction[port_].busy)
    {
      // Abort I2C communication if this point is reached!
      i2c[port_]->CONSET = Control::kAssertAcknowledge | Control::kStop;
      SJ2_ASSERT_WARNING(
          transaction[port_].transmit_length == 0 ||
              transaction[port_].receive_length == 0,
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

 private:
  PinInterface & sda_;
  PinInterface & scl_;
  Pin sda_pin_;
  Pin scl_pin_;
  uint8_t port_;
  bool initialized_;
  bool pins_initialized_;
};
