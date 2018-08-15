#pragma once

#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/SystemFiles/system_LPC407x_8x_177x_8x.h"
#include "L1_Drivers/pin_configure.hpp"

class UartInterface
{
    virtual void SetBaudRate(uint32_t baud_rate) = 0;
    virtual bool Initialize(uint32_t baud_rate) = 0;
    virtual void Send(uint8_t out) = 0;
    virtual uint8_t Receive() = 0;
};

class Uart : public UartInterface
{
 public:
    //  UART0 pins Tx = 0,0; Rx = 0,1
    //  UART2 pins Tx = 2,8; Rx = 2,9
    //  UART3 pins Tx = 4,28; Rx = 4,29
    //  UART4 pins Tx = 1,29; Rx = 2,9
    static constexpr LPC_UART_TypeDef * UARTBaseReg[4] =
    {
        [0] = LPC_UART0,
        [1] = LPC_UART2,
        [2] = LPC_UART3,
        [3] = reinterpret_cast <LPC_UART_TypeDef *>(LPC_UART4)
    };
    enum PortPin : uint8_t
    {
        kPort0 = 0,
        kPort2 = 2,
        kPort4 = 4,
        kPort1 = 1
    };
    enum Pin : uint8_t
    {
        kPin0 = 0,
        kPin1 = 1,
        kPin8 = 8,
        kPin9 = 9,
        kPin28 = 28,
        kPin29 = 29
    };
    const uint8_t TxUartPortFunction[4] =
    {
        4,
        2,
        2,
        5
    };
    const uint8_t RxUartPortFunction[4] =
    {
        4,
        2,
        2,
        3
    };
    const uint8_t Powerbit[4] =
    {
        3,
        24,
        25,
        8
    };

    void SetBaudRate(uint32_t baud_rate) override
    {
        constexpr uint8_t StandardUart = 0b011;
        float baudrate = static_cast<float>(baud_rate);

        // Set baud rate
        UARTBaseReg[mode] -> LCR |= (1 << 7);
        uint32_t div =
            static_cast<uint32_t>(OSC_CLK / (16.0f * baudrate) + 0.5f);
        UARTBaseReg[mode] -> DLM = static_cast<uint8_t>(div >> 8);
        UARTBaseReg[mode] -> DLL = static_cast<uint8_t>(div >> 0);
        UARTBaseReg[mode] -> LCR = StandardUart;
    }

    bool Initialize(uint32_t baud_rate) override
    {
        constexpr uint8_t FIFOEnableAndReset = 0b111;
        // Powering the port
        LPC_SC -> PCONP |= (1 << Powerbit[mode]);

        // Setting the pin functions and modes
        rx->SetPinFunction(RxUartPortFunction[mode]);
        tx->SetPinFunction(TxUartPortFunction[mode]);
        tx->SetPinMode(PinConfigureInterface::PinMode::kPullUp);
        rx->SetPinMode(PinConfigureInterface::PinMode::kPullUp);

        // Baud rate setting
        Uart::SetBaudRate(baud_rate);

        UARTBaseReg[mode] -> FCR |= (FIFOEnableAndReset << 0);

        return true;
    }

    void Send(uint8_t out) override
    {
        UARTBaseReg[mode]->THR = out;
        while (!(UARTBaseReg[mode]->LSR & (1 << 5))){continue;}
    }

    uint8_t Receive() override
    {
        uint8_t receiver;
        while (!(UARTBaseReg[mode]->LSR & (1 << 0))){continue;}
        receiver = static_cast<uint8_t>(UARTBaseReg[mode]->RBR);
        return receiver;
        return 0;
    }

    // Default consturctor will set up UART2
    constexpr Uart() :
        tx(&tx_pin),
        rx(&rx_pin),
        tx_pin(kPort2, kPin8),
        rx_pin(kPort2, kPin9),
        mode(1)
    {
    }
    // explicit constexpr Uart(uint8_t mode)
    // tx(tx_pin),
    // rx(rx_pin),
    // port((mode == kPort0) ? mode : mode - 1)
    // {
    // }

    constexpr Uart(uint8_t port, uint8_t pin) :
        tx(&tx_pin),
        rx(&rx_pin),
        tx_pin(PinConfigure::CreateInactivePin()),
        rx_pin(PinConfigure::CreateInactivePin()),
        mode(1)
    {
        tx_pin = PinConfigure(port, pin);
        rx_pin = PinConfigure(port, kPin9);
    }
    ~Uart()
    {
        // Do nothing
    }
 private:
    PinConfigureInterface * tx;
    PinConfigureInterface * rx;
    PinConfigure tx_pin;
    PinConfigure rx_pin;
    uint8_t mode;
};
