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

class SpiInterface
{
 public:
  // SSP Master/slave modes
  enum class MasterSlaveMode
  {
    kMaster = 0,
    kSlave  = 1
  };
  // SSP data size for frame packets
  enum class DataSize : uint8_t
  {
    kFour = 0,  // The smallest standard frame sized allowed for SJSU-Dev2
    kFive,
    kSix,
    kSeven,
    kEight,
    kNine,
    kTen,
    kEleven,
    kTwelve,
    kThirteen,
    kFourteen,
    kFifteen,
    kSixteen,  // The largest standard frame sized allowed for SJSU-Dev2
  };

  virtual void Initialize()                                           = 0;
  virtual uint16_t Transfer(uint16_t data)                            = 0;
  virtual void SetPeripheralMode(MasterSlaveMode mode, DataSize size) = 0;
  // NOTE: "divider" is the number of prescaler-output clocks per bit.
  // See page 611 of UM10562 for SCR-Serial Clock Rate for more info.
  virtual void SetClock(bool positive_polarity, bool phase, uint8_t prescaler,
                        uint8_t divider) = 0;
  virtual uint32_t GetClock()            = 0;
};

class Spi final : public SpiInterface, protected Lpc40xxSystemController
{
 public:
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
    kSpiEnable       = 1
  };

  // SSP data size for frame packets
  static constexpr uint8_t kDataSizeLUT[] = {
    0b0011,  // 4-bit  transfer
    0b0100,  // 5-bit  transfer
    0b0101,  // 6-bit  transfer
    0b0110,  // 7-bit  transfer
    0b0111,  // 8-bit  transfer
    0b1000,  // 9-bit  transfer
    0b1001,  // 10-bit transfer
    0b1010,  // 11-bit transfer
    0b1011,  // 12-bit transfer
    0b1100,  // 13-bit transfer
    0b1101,  // 14-bit transfer
    0b1110,  // 15-bit transfer
    0b1111,  // 16-bit transfer
  };

  struct Bus_t
  {
    LPC_SSP_TypeDef * registers;
    PeripheralID power_on_bit;
    const Pin & mosi;
    const Pin & miso;
    const Pin & sck;
    uint8_t pin_function_id;
  };

  struct Bus
  {
   private:
    // SSP0 pins
    inline static const Pin kMosi0 = Pin::CreatePin<0, 18>();
    inline static const Pin kMiso0 = Pin::CreatePin<0, 17>();
    inline static const Pin kSck0  = Pin::CreatePin<0, 15>();
    // SSP1 pins
    inline static const Pin kMosi1 = Pin::CreatePin<0, 9>();
    inline static const Pin kMiso1 = Pin::CreatePin<0, 8>();
    inline static const Pin kSck1  = Pin::CreatePin<0, 7>();
    // SSP2 pins
    inline static const Pin kMosi2 = Pin::CreatePin<1, 1>();
    inline static const Pin kMiso2 = Pin::CreatePin<1, 4>();
    inline static const Pin kSck2  = Pin::CreatePin<1, 0>();

   public:
    inline static const Bus_t kSpi0 = {
      .registers       = LPC_SSP0,
      .power_on_bit    = Peripherals::kSsp0,
      .mosi            = kMosi0,
      .miso            = kMiso0,
      .sck             = kSck0,
      .pin_function_id = 0b010,
    };
    inline static const Bus_t kSpi1 = {
      .registers       = LPC_SSP1,
      .power_on_bit    = Peripherals::kSsp1,
      .mosi            = kMosi1,
      .miso            = kMiso1,
      .sck             = kSck1,
      .pin_function_id = 0b010,
    };
    inline static const Bus_t kSpi2 = {
      .registers       = LPC_SSP2,
      .power_on_bit    = Peripherals::kSsp2,
      .mosi            = kMosi2,
      .miso            = kMiso2,
      .sck             = kSck2,
      .pin_function_id = 0b100,
    };
  };

  constexpr Spi(const Bus_t & bus) : bus_(bus) {}

  /// This METHOD MUST BE EXECUTED before any other method can be called.
  /// Powers on the peripheral, activates the SSP pins and enables the SSP
  /// peripheral.
  /// See page 601 of user manual UM10562 LPC408x/407x for more details.
  void Initialize() override
  {
    // Power up peripheral
    PowerUpPeripheral(bus_.power_on_bit);
    // Enable SSP pins
    bus_.mosi.SetPinFunction(bus_.pin_function_id);
    bus_.miso.SetPinFunction(bus_.pin_function_id);
    bus_.sck.SetPinFunction(bus_.pin_function_id);
    // Enable SSP
    bus_.registers->CR1 = bit::Set(bus_.registers->CR1, kSpiEnable);
  }

  /// An easy way to sets up an SPI peripheral as SPI master with default clock
  /// rate at 1Mhz.
  void SetSpiMasterDefault()
  {
    constexpr bool kHighPolarity  = 1;
    constexpr bool kPhase0        = 0;
    constexpr uint8_t kScrDivider = 0;
    constexpr uint8_t kPrescaler  = SJ2_SYSTEM_CLOCK_RATE_MHZ;

    SetPeripheralMode(MasterSlaveMode::kMaster, DataSize::kEight);
    SetClock(kHighPolarity, kPhase0, kScrDivider, kPrescaler);
  }

  /// Checks if the SSP controller is idle.
  /// @returns true if the controller is sending or receiving a data frame and
  /// false if it is idle.
  bool IsBusBusy()
  {
    return bit::Read(bus_.registers->SR, kDataLineIdleBit);
  }

  /// Transfers a data frame to an external device using the SSP
  /// data register. This functions for both transmitting and
  /// receiving data. It is recommended this region be protected
  /// by a mutex.
  /// @param data - information to be placed in data register
  /// @return - received data from external device
  uint16_t Transfer(uint16_t data) override
  {
    bus_.registers->DR = data;
    while (IsBusBusy())
    {
      continue;
    }
    return static_cast<uint16_t>(bus_.registers->DR);
  }

  /// Sets the various modes for the Peripheral
  /// @param mode - master or slave mode
  /// @param frame - format for Peripheral data to use
  /// @param size - number of bits per frame
  void SetPeripheralMode(MasterSlaveMode mode, DataSize size) override
  {
    bus_.registers->CR0 =
        bit::Insert(bus_.registers->CR0, util::Value(size), kDataBit, 4);
    bus_.registers->CR0 =
        bit::Insert(bus_.registers->CR0, 0, kFrameBit, 2);
    bus_.registers->CR1 =
        bit::Insert(bus_.registers->CR1, util::Value(mode), kMasterModeBit, 1);
  }

  /// Sets the clock rate for the Peripheral
  /// @param polarity - maintain bus on clock 0=low or 1=high between frames
  /// @param phase - capture serial data on 0=first or 1=second clock cycle
  /// @param divider - see notes in SSP_Interface above
  /// @param prescaler - divides the PCLK, must be even value between 2-254
  void SetClock(bool polarity, bool phase, uint8_t divider,
                uint8_t prescaler) override
  {
    // TODO(#371): Find a better alternative to powering up the peripheral for
    // every method.
    PowerUpPeripheral(bus_.power_on_bit);
    // first clear the appropriate registers
    bus_.registers->CR0 =
        bit::Insert(bus_.registers->CR0, polarity, kPolarityBit, 1);
    bus_.registers->CR0 = bit::Insert(bus_.registers->CR0, phase, kPhaseBit, 1);
    bus_.registers->CR0 =
        bit::Insert(bus_.registers->CR0, divider, kDividerBit, 8);
    bus_.registers->CPSR = prescaler;
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
    uint32_t return_val;
    return_val = (bit::Extract(bus_.registers->CPSR, kPrescalerBit, 8)) +
                 ((bit::Extract(bus_.registers->CR0, kDividerBit, 8)) << 8) +
                 ((bit::Read(bus_.registers->CR0, kPhaseBit)) << 16) +
                 ((bit::Read(bus_.registers->CR0, kPolarityBit)) << 18);

    return return_val;
  }

 private:
  const Bus_t & bus_;
};
