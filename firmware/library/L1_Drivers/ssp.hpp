// SSP provides the ability for serial communication over SPI, SSI, or
// Microwire on the LPC407x chipset. NOTE: The SSP2 peripheral is not
// available on the SJTwo board, as such this driver does not support
// it. Only one set of pins are available for either SSP0 or SSP1 as
// follows:
//      Peripheral  |   MISO    |   MOSI    |   SCK
//          SSP0    |   P0.17   |   P0.18   |   P0.15
//          SSP1    |   P0.8    |   P0.9    |   P0.7
#pragma once

#include <cstdint>

class SspInterface
{
 public:
     // SSP peripherals
    enum class Peripheral
    {
        kSsp0 = 0,
        kSsp1 = 1
    };

    enum PowerOn
    {
        kPconp0 = 21,
        kPconp1 = 10
    };

    // SSP frame formats
    enum FrameMode
    {
        kSpi     = 0,
        kTi      = 1,
        kMicro   = 2
    };

    // SSP Master/slave modes
    enum MasterSlaveMode
    {
        kMaster  = 0,
        kSlave   = 1
    };

    // SSP data size for frame packets
    enum DataSize
    {
        kFour        = 0b0011,  // 4-bit  transfer
        kFive        = 0b0100,  // 5-bit  transfer
        kSix         = 0b0101,  // 6-bit  transfer
        kSeven       = 0b0110,  // 7-bit  transfer
        kEight       = 0b0111,  // 8-bit  transfer
        kNine        = 0b1000,  // 9-bit  transfer
        kTen         = 0b1001,  // 10-bit transfer
        kEleven      = 0b1010,  // 11-bit transfer
        kTwelve      = 0b1011,  // 12-bit transfer
        kThirteen    = 0b1100,  // 13-bit transfer
        kFourteen    = 0b1101,  // 14-bit transfer
        kFifteen     = 0b1110,  // 15-bit transfer
        kSixteen     = 0b1111,  // 16-bit transfer
    };

    virtual bool Initialize()                                       = 0;
    virtual bool GetTransferStatus()                                = 0;
    virtual uint16_t Transfer(uint16_t data)                        = 0;
    virtual void SetPeripheralPower()                               = 0;
    virtual void SetPeripheralMode(
            MasterSlaveMode mode, FrameMode frame, DataSize size)   = 0;
    // NOTE: "divider" is the number of prescaler-output clocks per bit.
    // See page 611 of UM10562 for SCR-Serial Clock Rate for more info.
    virtual void SetClock(bool polarity, bool phase,
            uint8_t prescaler, uint8_t divider)                     = 0;
};
