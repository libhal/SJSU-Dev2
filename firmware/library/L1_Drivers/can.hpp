#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"
#include "L2_Utilities/macros.hpp"


class CanInterface
{
 public:
    // This struct represents a CAN message based on the BOSCH CAN spec 2.0B.
    // It is HW mapped to 32-bit registers TFI, TID, TDx (pg. 568).
    SJ2_PACKED(struct) CanMessage_t
    {
        union
        {
            // TFI - Transmit Frame Information Register - Message frame info
            uint32_t frame;
            struct
            {
                // User definable priority level for a message (0-255)
                uint8_t tx_priority       : 8;
                // Reserved
                uint8_t                   : 8;
                // Data payload length (0-7 bytes)
                uint8_t data_length       : 4;
                // Reserved
                uint16_t                  : 10;
                // Request a data frame from a node
                uint8_t remote_tx_request : 1;
                // 11-bit or 29-bit identifier format
                uint8_t frame_format      : 1;
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

    virtual bool Initialize()                             = 0;
    virtual bool Send(const CanMessage_t &message)        = 0;
    virtual bool Receive(CanMessage_t * message)          = 0;
    virtual bool SelfTestBus(const TestType kType)        = 0;
    virtual bool IsBusOff()                               = 0;
    virtual void EnableBus()                              = 0;
};

class Can : public CanInterface
{
 public:
    static LPC_CAN_TypeDef * can_registers[2];
    static LPC_CANCR_TypeDef * can_central_register;
    static LPC_CANAF_TypeDef * can_acceptance_filter_register;
    static LPC_SC_TypeDef * system_control_register;

    enum Controllers : uint8_t
    {
        kCan1 = 0,
        kCan2,
        kCanControllerNumMax
    };

    enum Power : uint8_t
    {
        kCan1PowerEnableBit = 13,
        kCan2PowerEnableBit = 14
    };

    enum Ports : uint8_t
    {
        // based on schematic rev1.D
        kCan1Port = 0,
        kCan2Port = 2
    };

    enum Pins : uint8_t
    {
        // based on schematic rev1.D
        kRd1PinNumber = 0,
        kTd1PinNumber = 1,
        kRd2PinNumber = 7,
        kTd2PinNumber = 8,
    };

    enum PinFunctions : uint8_t
    {
        kRd1FunctionBit = 1,
        kTd1FunctionBit = 1,
        kRd2FunctionBit = 1,
        kTd2FunctionBit = 1,
    };

    enum Interrupts : uint8_t
    {
        kRxBufferIntEnableBit = 0,
        kTxBuffer1IntEnableBit = 1,
        kTxBuffer2IntEnableBit = 9,
        kTxBuffer3IntEnableBit = 10,
    };

    enum class BaudRates : uint8_t
    {
        kBaud100Kbps = 100,
    };

    constexpr Can(Controllers controller, BaudRates baud_rate) :
      controller_(controller),
      baud_rate_(baud_rate),
      rd_(rd_pin_),
      td_(td_pin_),
      rd_pin_((controller == kCan1 ? kCan1Port : kCan2Port),
              (controller == kCan1 ? kRd1PinNumber : kRd2PinNumber)),
      td_pin_((controller == kCan1 ? kCan1Port : kCan2Port),
              (controller == kCan1 ? kTd1PinNumber : kTd2PinNumber))
    {
    }

    constexpr Can():
      controller_(kCan1),
      baud_rate_(BaudRates::kBaud100Kbps),
      rd_(rd_pin_),
      td_(td_pin_),
      rd_pin_(kCan1Port, kRd1PinNumber),
      td_pin_(kCan1Port, kTd1PinNumber)
    {
    }

    bool Initialize() override
    {
        /*Initialization Sequence
          * Power on the CAN controller
          * Enable CAN Controller Clock (enabled in startup.cpp)
          * Configure I/O pins for CAN use
          * Enable CAN Interrupts
        */
        bool success = 1;
        if (controller_ > kCanControllerNumMax)
        {
            success = 0;
        }
        else
        {
            EnablePower();
            ConfigurePins();
            EnableInterrupts();
            SetBaudRate(baud_rate_);
        }
        return success;
    }

 private:
    Controllers controller_;
    BaudRates baud_rate_;

    PinInterface & rd_;
    PinInterface & td_;

    Pin rd_pin_;
    Pin td_pin_;


    // NOTE: The interrupt line is shared between CAN 1 & CAN 2
    // This will hook into the actual CanIsrHandler in startup.cpp
    static void ProcessIrq(void){}
    static void TransmitBuffer1to3Isr(){}
    static void ReceiveBuffer1Isr(){}
    static void WriteMessageToBuffer(){}
    static void ReadMessageFromBuffer(){}
    static void BusErrorIsr(){}
    // void SetControllerMode(const CanMode mode)

    void EnablePower()
    {
        // PCONP - Power Control Peripherals
        if (controller_ == kCan1)
        {
            system_control_register->PCONP |= (1 << kCan1PowerEnableBit);
        }
        else
        {
            system_control_register->PCONP |= (1 << kCan2PowerEnableBit);
        }
    }

    void ConfigurePins()
    {
     // Configure internal pin MUX to map board I/O pins to controller
     //                       _
     //                      / |<--x-->[GPIOx]
     //                     /  |<--x-->[SPIx]
     // [Board I/O Pin]<-->|Mux|<----->[CANx]
     //                     \  |<--x-->[UARTx]
     //                      \_|<--x-->[I2Cx]
     //
        if (controller_ == kCan1)
        {
            rd_pin_.SetPinFunction(kRd1FunctionBit);
            td_pin_.SetPinFunction(kTd1FunctionBit);
        }
        else
        {
            rd_pin_.SetPinFunction(kRd2FunctionBit);
            td_pin_.SetPinFunction(kTd2FunctionBit);
        }
    }
    void EnableInterrupts()
    {
        // IER - Interrupt Enable Register
        can_registers[controller_]->IER |= (1 << kRxBufferIntEnableBit);
        can_registers[controller_]->IER |= (1 << kTxBuffer1IntEnableBit);
        can_registers[controller_]->IER |= (1 << kTxBuffer2IntEnableBit);
        can_registers[controller_]->IER |= (1 << kTxBuffer3IntEnableBit);
    }
    void SetBaudRate(BaudRates baud_rate){(void)baud_rate;}
};
