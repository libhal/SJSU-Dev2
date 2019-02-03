/// SSP provides the ability for serial communication over SPI, SSI, or
/// Microwire on the LPC407x chipset. NOTE: The SSP2 peripheral is a
/// selectable option in this driver, it is not currently available on
/// the SJTwo board. Only one set of pins are available for either SSP0
/// or SSP1 as follows:
///      Peripheral  |   MISO    |   MOSI    |   SCK
///          SSP0    |   P0.17   |   P0.18   |   P0.15
///          SSP1    |   P0.8    |   P0.9    |   P0.7
/// Order of function calls should be as follows:
///
///     1. Constructor (create object)
///     2. SetClock(...)
///     3. SetPeripheralMode(...)
///     4. Initialize()
///
/// Note that all register modifications must be made before the SSP
/// is enabled in the CR1 register (see page 612 of user manual UM10562)
/// If changes are desired after the Initialize function is called, the
/// peripheral must be disabled, and then re-enabled after changes are made.

#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"

class SspInterface
{
 public:
  // SSP frame formats
  enum FrameMode
  {
    kSpi   = 0,
    kTi    = 1,
    kMicro = 2
  };

  // SSP Master/slave modes
  enum MasterSlaveMode
  {
    kMaster = 0,
    kSlave  = 1
  };

  // SSP data size for frame packets
  enum DataSize
  {
    kFour     = 0b0011,  // 4-bit  transfer
    kFive     = 0b0100,  // 5-bit  transfer
    kSix      = 0b0101,  // 6-bit  transfer
    kSeven    = 0b0110,  // 7-bit  transfer
    kEight    = 0b0111,  // 8-bit  transfer
    kNine     = 0b1000,  // 9-bit  transfer
    kTen      = 0b1001,  // 10-bit transfer
    kEleven   = 0b1010,  // 11-bit transfer
    kTwelve   = 0b1011,  // 12-bit transfer
    kThirteen = 0b1100,  // 13-bit transfer
    kFourteen = 0b1101,  // 14-bit transfer
    kFifteen  = 0b1110,  // 15-bit transfer
    kSixteen  = 0b1111,  // 16-bit transfer
  };

  virtual void Initialize()                     = 0;
  virtual bool IsTransferRegBusy()              = 0;
  virtual uint16_t Transfer(uint16_t data)      = 0;
  virtual void SetSpiMasterDefault()            = 0;
  virtual void SetPeripheralMode(MasterSlaveMode mode, FrameMode frame,
                                 DataSize size) = 0;
  virtual uint16_t GetPeripheralMode()          = 0;

  // NOTE: "divider" is the number of prescaler-output clocks per bit.
  // See page 611 of UM10562 for SCR-Serial Clock Rate for more info.
  virtual void SetClock(bool polarity, bool phase, uint8_t prescaler,
                        uint8_t divider) = 0;
  virtual uint32_t GetClock()            = 0;
};

class Ssp final : public SspInterface, protected Lpc40xxSystemController
{
 public:
  /// SSP register lookup table
  inline static LPC_SSP_TypeDef * ssp_registers[]  = { LPC_SSP0, LPC_SSP1,
                                                      LPC_SSP2 };
  inline static LPC_SC_TypeDef * sysclock_register = LPC_SC;
  /// SSP peripherals
  enum class Peripheral
  {
    kSsp0 = 0,
    kSsp1 = 1,
    kSsp2 = 2,
    kNumberOfPeripherals
  };

  enum RegisterBitPositions : uint8_t
  {
    kDataBit         = 0,
    kFrameBit        = 4,
    kPolarityBit     = 6,
    kPhaseBit        = 7,
    kDividerBit      = 8,
    kMasterModeBit   = 2,
    kDataLineIdleBit = 4,
    kPrescalerBit    = 0,
    kSspEnable       = 1
  };

  enum MatrixLookup
  {
    kMosi = 0,
    kMiso = 1,
    kSck  = 2,
  };

  static constexpr uint8_t kPinSelect[] = { 0b010, 0b010, 0b100 };
  static constexpr size_t kNumberOfPeripherals =
      util::Value(Peripheral::kNumberOfPeripherals);
  static constexpr Pin kSspPinMatrix[kNumberOfPeripherals][3] = {
    // SSP0
    { [MatrixLookup::kMosi] = Pin::CreatePin<0, 18>(),
      [MatrixLookup::kMiso] = Pin::CreatePin<0, 17>(),
      [MatrixLookup::kSck]  = Pin::CreatePin<0, 15>() },
    // SSP1
    { [MatrixLookup::kMosi] = Pin::CreatePin<0, 9>(),
      [MatrixLookup::kMiso] = Pin::CreatePin<0, 8>(),
      [MatrixLookup::kSck]  = Pin::CreatePin<0, 7>() },
    // SSP2
    { [MatrixLookup::kMosi] = Pin::CreatePin<1, 1>(),
      [MatrixLookup::kMiso] = Pin::CreatePin<1, 4>(),
      [MatrixLookup::kSck]  = Pin::CreatePin<1, 0>() }
  };
  static constexpr Lpc40xxSystemController::PeripheralID kPowerBit[] = {
    Lpc40xxSystemController::Peripherals::kSsp0,
    Lpc40xxSystemController::Peripherals::kSsp1,
    Lpc40xxSystemController::Peripherals::kSsp2,
  };

  /// Default constructor sets up SSP0 peripheral as SPI master, should
  /// be followed by SetSpiMasterDefault() function.
  constexpr Ssp()
      : mosi_(&mosi_pin_),
        miso_(&miso_pin_),
        sck_(&sck_pin_),
        mosi_pin_(kSspPinMatrix[0][MatrixLookup::kMosi]),
        miso_pin_(kSspPinMatrix[0][MatrixLookup::kMiso]),
        sck_pin_(kSspPinMatrix[0][MatrixLookup::kSck]),
        pssp_(Peripheral::kSsp0)
  {
  }

  /// User modified constructor. Must be followed by Set functions.
  explicit constexpr Ssp(Peripheral set_pssp)
      : mosi_(&mosi_pin_),
        miso_(&miso_pin_),
        sck_(&sck_pin_),
        mosi_pin_(kSspPinMatrix[util::Value(set_pssp)][MatrixLookup::kMosi]),
        miso_pin_(kSspPinMatrix[util::Value(set_pssp)][MatrixLookup::kMiso]),
        sck_pin_(kSspPinMatrix[util::Value(set_pssp)][MatrixLookup::kSck]),
        pssp_(set_pssp)
  {
  }

  /// Constructor to pass in your own pins. Must be followed by Set functions.
  constexpr Ssp(Peripheral pssp, PinInterface * mosi_pin,
                PinInterface * miso_pin, PinInterface * sck_pin)
      : mosi_(mosi_pin),
        miso_(miso_pin),
        sck_(sck_pin),
        mosi_pin_(Pin::CreateInactivePin()),
        miso_pin_(Pin::CreateInactivePin()),
        sck_pin_(Pin::CreateInactivePin()),
        pssp_(pssp)
  {
  }

  /// Powers on the peripheral, sets the peripheral clock, format, and data
  /// size, and enables the SSP pins for communication see and more.
  /// See page 601 of user manual UM10562 LPC408x/407x for more details.
  void Initialize() override
  {
    uint32_t pssp = util::Value(pssp_);
    // Power up peripheral
    PowerUpPeripheral(kPowerBit[pssp]);
    // Enable SSP pins
    mosi_->SetPinFunction(kPinSelect[pssp]);
    miso_->SetPinFunction(kPinSelect[pssp]);
    sck_->SetPinFunction(kPinSelect[pssp]);
    // Enable SSP
    ssp_registers[pssp]->CR1 = bit::Set(ssp_registers[pssp]->CR1, kSspEnable);
  }

  /// Checks if the SSP controller is idle.
  /// @returns true if the controller is sending or receiving a data frame and
  /// false if it is idle.
  bool IsTransferRegBusy() override
  {
    return (bit::Read(ssp_registers[util::Value(pssp_)]->SR, kDataLineIdleBit));
  }

  /// Transfers a data frame to an external device using the SSP
  /// data register. This functions for both transmitting and
  /// receiving data. It is recommended this region be protected
  /// by a mutex.
  /// @param data - information to be placed in data register
  /// @return - received data from external device
  uint16_t Transfer(uint16_t data) override
  {
    uint32_t pssp = util::Value(pssp_);

    ssp_registers[pssp]->DR = data;
    while (IsTransferRegBusy())
    {
      continue;
    }
    return static_cast<uint16_t>(ssp_registers[pssp]->DR);
  }

  /// Sets up SSP peripheral as SPI master
  void SetSpiMasterDefault() override
  {
    constexpr bool kHighPolarity  = 1;
    constexpr bool kPhase0        = 0;
    constexpr uint8_t kScrDivider = 0;
    constexpr uint8_t kPrescaler  = SJ2_SYSTEM_CLOCK_RATE_MHZ;

    SetPeripheralMode(kMaster, kSpi, kEight);
    SetClock(kHighPolarity, kPhase0, kScrDivider, kPrescaler);
  }

  /// Sets the various modes for the Peripheral
  /// @param mode - master or slave mode
  /// @param frame - format for Peripheral data to use
  /// @param size - number of bits per frame
  void SetPeripheralMode(MasterSlaveMode mode, FrameMode frame,
                         DataSize size) override
  {
    uint32_t pssp = util::Value(pssp_);
    // TODO(#371): Find a better alternative to powering up the peripheral for
    // every method.
    Ssp::PowerUpPeripheral(Ssp::kPowerBit[pssp]);
    if (frame == kMicro)
    {
      size = kEight;
    }
    ssp_registers[pssp]->CR0 =
        bit::Insert(ssp_registers[pssp]->CR0, util::Value(size), kDataBit, 4);
    ssp_registers[pssp]->CR0 =
        bit::Insert(ssp_registers[pssp]->CR0, util::Value(frame), kFrameBit, 2);
    ssp_registers[pssp]->CR1 = bit::Insert(
        ssp_registers[pssp]->CR1, util::Value(mode), kMasterModeBit, 1);
  }

  /// Gets the Peripheral modes from registers
  /// @return - returns a 16-bit value as follows: 0000_000x 0xx0_xxxx
  ///       MasterSlaveMode = 1-bit @ bit 8
  ///       FrameMode       = 2-bit @ bit 5
  ///       DataSize        = 4-bit @ bit 0
  uint16_t GetPeripheralMode() override
  {
    uint16_t return_val = 0;
    uint32_t pssp       = util::Value(pssp_);
    // TODO(#371): Find a better alternative to powering up the peripheral for
    // every method.
    Ssp::PowerUpPeripheral(Ssp::kPowerBit[pssp]);

    return_val = static_cast<uint16_t>(
        (bit::Extract(ssp_registers[pssp]->CR0, kDataBit, 4)) +
        ((bit::Extract(ssp_registers[pssp]->CR0, kFrameBit, 2)) << 5) +
        ((bit::Extract(ssp_registers[pssp]->CR1, kMasterModeBit)) << 8));
    return return_val;
  }

  /// Sets the clock rate for the Peripheral
  /// @param polarity - maintain bus on clock 0=low or 1=high between frames
  /// @param phase - capture serial data on 0=first or 1=second clock cycle
  /// @param divider - see notes in SSP_Interface above
  /// @param prescaler - divides the PCLK, must be even value between 2-254
  void SetClock(bool polarity, bool phase, uint8_t divider,
                uint8_t prescaler) override
  {
    uint32_t pssp = util::Value(pssp_);
    // TODO(#371): Find a better alternative to powering up the peripheral for
    // every method.
    Ssp::PowerUpPeripheral(Ssp::kPowerBit[pssp]);
    // first clear the appropriate registers
    ssp_registers[pssp]->CR0 =
        bit::Insert(ssp_registers[pssp]->CR0, polarity, kPolarityBit, 1);
    ssp_registers[pssp]->CR0 =
        bit::Insert(ssp_registers[pssp]->CR0, phase, kPhaseBit, 1);
    ssp_registers[pssp]->CR0 =
        bit::Insert(ssp_registers[pssp]->CR0, divider, kDividerBit, 8);
    ssp_registers[pssp]->CPSR = prescaler;
  }

  /// Gets the Peripheral clock from registers
  /// @return - returns a 32-bit value as follows:
  ///   0000_0000 0000_0x0x xxxx_xxxx xxxx_xxxx
  ///       polarity    = 1-bit @ bit 18
  ///       phase       = 1-bit @ bit 16
  ///       divider     = 8-bits @ bit 8
  ///       prescaler   = 8-bits @ bit 0
  uint32_t GetClock() override
  {
    uint32_t return_val = 0;
    uint32_t pssp       = util::Value(pssp_);
    // TODO(#371): Find a better alternative to powering up the peripheral for
    // every method.
    Ssp::PowerUpPeripheral(Ssp::kPowerBit[pssp]);

    return_val =
        (bit::Extract(ssp_registers[pssp]->CPSR, kPrescalerBit, 8)) +
        ((bit::Extract(ssp_registers[pssp]->CR0, kDividerBit, 8)) << 8) +
        ((bit::Read(ssp_registers[pssp]->CR0, kPhaseBit)) << 16) +
        ((bit::Read(ssp_registers[pssp]->CR0, kPolarityBit)) << 18);

    return return_val;
  }

 private:
  PinInterface * mosi_;
  PinInterface * miso_;
  PinInterface * sck_;

  Pin mosi_pin_;
  Pin miso_pin_;
  Pin sck_pin_;
  // SSP member variables
  Peripheral pssp_;  // SSP interfaces
};
