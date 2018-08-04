#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"
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
        constexpr Transaction_t(uint8_t set_address,
                                uint8_t * set_transmitter,
                                size_t set_transmit_length,
                                uint8_t * set_receiver,
                                size_t set_receive_length,
                                size_t set_position,
                                bool set_repeated,
                                bool set_busy,
                                Status set_status,
                                Operation set_operation,
                                uint64_t set_timeout)
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

    static constexpr uint8_t kNumberOfPorts =
        static_cast<uint8_t>(Port::kNumberOfPorts);

    static const uint8_t kPconp[kNumberOfPorts];
    static const IRQn_Type kIrq[kNumberOfPorts];
    static LPC_I2C_TypeDef * i2c[kNumberOfPorts];

    template <Port port>
    static void I2cHandler()
    {
        constexpr uint8_t kPort = static_cast<uint8_t>(port);
        MasterState state      = MasterState(i2c[kPort]->STAT);
        switch (state)
        {
            case MasterState::kBusError:  // 0x00
            {
                transaction[kPort].status = I2cInterface::Status::kBusError;
                i2c[kPort]->CONSET =
                    Control::kAssertAcknowledge | Control::kStop;
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
                i2c[kPort]->CONCLR = Control::kStart;
                if (transaction[kPort].transmit_length == 0)
                {
                    transaction[kPort].busy = false;
                    i2c[kPort]->CONSET      = Control::kStop;
                }
                else
                {
                    size_t position = transaction[kPort].position++;
                    i2c[kPort]->DAT  = transaction[kPort].transmitter[position];
                }
                break;
            }
            case MasterState::kSlaveAddressWriteSentRecievedNack:  // 0x20
            {
                i2c[kPort]->CONCLR      = Control::kStart;
                transaction[kPort].busy = false;
                transaction[kPort].status =
                    I2cInterface::Status::kDeviceNotFound;
                i2c[kPort]->CONSET = Control::kStop;
                break;
            }
            case MasterState::kTransmittedDataRecievedAck:  // 0x28
            {
                if (transaction[kPort].position >=
                    transaction[kPort].transmit_length)
                {
                    if (transaction[kPort].repeated)
                    {
                        // OR with 1 to set address as READ for the next
                        // transaction
                        transaction[kPort].address |= 0b1;
                        transaction[kPort].position = 0;
                        i2c[kPort]->CONSET          = Control::kStart;
                    }
                    else
                    {
                        transaction[kPort].busy = false;
                        i2c[kPort]->CONSET      = Control::kStop;
                    }
                }
                else
                {
                    size_t position = transaction[kPort].position++;
                    i2c[kPort]->DAT  = transaction[kPort].transmitter[position];
                }
                break;
            }
            case MasterState::kTransmittedDataRecievedNack:  // 0x30
            {
                transaction[kPort].busy = false;
                i2c[kPort]->CONSET      = Control::kStop;
                break;
            }
            case MasterState::kArbitrationLost:  // 0x38
            {
                i2c[kPort]->CONSET = Control::kStart;
                break;
            }
            case MasterState::kSlaveAddressReadSentRecievedAck:  // 0x40
            {
                i2c[kPort]->CONCLR = Control::kStart;
                if (transaction[kPort].receive_length == 0)
                {
                    i2c[kPort]->CONCLR = Control::kAssertAcknowledge;
                }
                else
                {
                    i2c[kPort]->CONSET = Control::kAssertAcknowledge;
                }
                break;
            }
            case MasterState::kSlaveAddressReadSentRecievedNack:  // 0x48
            {
                i2c[kPort]->CONCLR = Control::kStart;
                transaction[kPort].status =
                    I2cInterface::Status::kDeviceNotFound;
                transaction[kPort].busy = false;
                i2c[kPort]->CONSET      = Control::kStop;
                break;
            }
            case MasterState::kRecievedDataRecievedAck:  // 0x50
            {
                size_t position = transaction[kPort].position++;
                transaction[kPort].receiver[position] =
                    static_cast<uint8_t>(i2c[kPort]->DAT);
                if (transaction[kPort].position >=
                    transaction[kPort].receive_length - 1)
                {
                    transaction[kPort].busy = false;
                    i2c[kPort]->CONCLR      = Control::kAssertAcknowledge;
                }
                else
                {
                    i2c[kPort]->CONSET = Control::kAssertAcknowledge;
                }
                break;
            }
            case MasterState::kRecievedDataRecievedNack:  // 0x58
            {
                transaction[kPort].busy = false;
                if (transaction[kPort].receive_length != 0)
                {
                    size_t position = transaction[kPort].position++;
                    transaction[kPort].receiver[position] =
                        static_cast<uint8_t>(i2c[kPort]->DAT);
                }
                i2c[kPort]->CONSET = Control::kStop;
                break;
            }
            case MasterState::kDoNothing:  // 0xF8
            {
                break;
            }
            default:
            {
                SJ2_ASSERT_FATAL(false, "Invalid I2C State Reached!!");
                i2c[kPort]->CONCLR = Control::kStop;
                break;
            }
        }
        // Clear I2C Interrupt flag
        i2c[kPort]->CONCLR = Control::kInterrupt;
    }

    constexpr explicit I2c(Port port)
        : sda_(sda_pin_),
          scl_(scl_pin_),
          sda_pin_(0, 10),
          scl_pin_(0, 11),
          port_(static_cast<uint8_t>(port)),
          initialized_(false)
    {
    }
    void Initialize() override
    {
        // UM10562: Chapter 7: LPC408x/407x I/O configuration page 133
        constexpr uint8_t kI2cPort2Function = 0b010;
        SJ2_ASSERT_FATAL(Port(port_) != Port::kNumberOfPorts,
                         "I2C.ort::kNumberOfPorts is not a Invalid I2C Port!");
        sda_.SetPinFunction(kI2cPort2Function);
        scl_.SetPinFunction(kI2cPort2Function);
        sda_.SetAsOpenDrain();
        scl_.SetAsOpenDrain();
        sda_.SetPinMode(PinConfigureInterface::kInactive);
        scl_.SetPinMode(PinConfigureInterface::kInactive);
        // TODO(#6): Use a constexpr map to map out which duty cycle values are
        // used for this
        i2c[port_]->SCLL   = 60;
        i2c[port_]->SCLH   = 60;
        i2c[port_]->CONCLR = Control::kAssertAcknowledge | Control::kStart |
                             Control::kStop | Control::kInterrupt;
        i2c[port_]->CONSET = Control::kInterfaceEnable;
        RegisterIsr(kIrq[port_], handlers[port_]);
        NVIC_EnableIRQ(kIrq[port_]);
        NVIC_SetPriority(kIrq[port_], kIrq[port_]);
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
        transaction[port_] = Transaction_t(address,           // address
                                           data,              // transmitter
                                           length,            // transmit_length
                                           nullptr,           // receiver
                                           0,                 // receive_length
                                           0,                 // position
                                           false,             // repeated
                                           true,              // busy
                                           Status::kSuccess,  // status
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
        transaction[port_] = Transaction_t(address,           // address
                                           transmit,          // transmitter
                                           transmit_length,   // transmit_length
                                           recieve,           // receiver
                                           recieve_length,    // receive_length
                                           0,                 // position
                                           true,              // repeated
                                           true,              // busy
                                           Status::kSuccess,  // status
                                           Operation::kWrite,  // operation
                                           timeout);           // timeout
        i2c[port_]->CONSET = Control::kStart;
        return BlockUntilFinished();
    }

 protected:
    static Transaction_t transaction[kNumberOfPorts];
    static IsrPointer handlers[kNumberOfPorts];

    virtual Status BlockUntilFinished()
    {
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
                false,
                "I2C.%u took too long to process and timed out! If the "
                "transaction needs more time, you may want to increase the "
                "timeout time using the timeout parameter in the Read() or "
                "Write() methods. ",
                port_);
            transaction[port_].status = Status::kTimedOut;
        }
        // Ensure that start is cleared before leaving this function
        i2c[port_]->CONCLR = Control::kStart;
        return transaction[port_].status;
    }
 private:
    PinConfigureInterface & sda_;
    PinConfigureInterface & scl_;
    PinConfigure sda_pin_;
    PinConfigure scl_pin_;
    uint8_t port_;
    bool initialized_;
};
