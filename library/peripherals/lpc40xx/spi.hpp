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
///     3. ConfigurePullResistor(...)
///     4. Initialize()
///
/// Note that all register modifications must be made before the SSP
/// is enabled in the CR1 register (see page 612 of user manual UM10562)
/// If changes are desired after the Initialize function is called, the
/// peripheral must be disabled, and then re-enabled after changes are made.

#pragma once

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "peripherals/spi.hpp"
#include "utility/math/bit.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Implementation of the SPI peripheral for the LPC40xx family of
/// microcontrollers.
class Spi final : public sjsu::Spi
{
 public:
  /// SSPn Control Register 0
  struct ControlRegister0  // NOLINT
  {
    /// Data Size Select. This field controls the number of bits transferred in
    /// each frame. Values 0000-0010 are not supported and should not be used.
    static constexpr auto kDataBit = bit::MaskFromRange(0, 3);

    /// Frame Format bitmask.
    /// 00 = SPI, 01 = TI, 10 = Microwire, 11 = Invalid
    static constexpr auto kFrameBit = bit::MaskFromRange(4, 5);

    /// If bit is set to 0 SSP controller maintains the bus clock low between
    /// frames.
    ///
    /// If bit is set to 1 SSP controller maintains the bus clock high between
    /// frames.
    static constexpr auto kPolarityBit = bit::MaskFromRange(6);

    /// If bit is set to 0 SSP controller captures serial data on the first
    /// clock transition of the frame, that is, the transition away from the
    /// inter-frame state of the clock line.
    ///
    /// If bit is set to 1 SSP controller captures serial data on the second
    /// clock transition of the frame, that is, the transition back to the
    /// inter-frame state of the clock line.
    static constexpr auto kPhaseBit = bit::MaskFromRange(7);

    /// Bitmask for dividing the peripheral clock to set the SPI clock
    /// frequency.
    static constexpr auto kDividerBit = bit::MaskFromRange(8, 15);
  };
  /// SSPn Control Register 1
  struct ControlRegister1  // NOLINT
  {
    /// Setting this bit to 1 will enable the peripheral for communication.
    static constexpr auto kSpiEnable = bit::MaskFromRange(1);

    /// Setting this bit to 1 will enable spi slave mode.
    static constexpr auto kSlaveModeBit = bit::MaskFromRange(2);
  };
  /// SSPn Status Register
  struct StatusRegister  // NOLINT
  {
    /// This bit is 0 if the SSPn controller is idle, or 1 if it is currently
    /// sending/receiving a frame and/or the Tx FIFO is not empty.
    static constexpr auto kDataLineBusyBit = bit::MaskFromRange(4);
  };

  /// SSP data size for frame packets
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

  /// Bus_t holds all of the information for an SPI bus on the LPC40xx platform.
  struct Bus_t
  {
    /// Pointer to the LPC SSP peripheral in memory
    LPC_SSP_TypeDef * registers;

    /// ResourceID of the SSP peripheral to power on at initialization
    sjsu::SystemController::ResourceID power_on_bit;

    /// Refernce to the M.ASTER-O.UT-S.LAVE-I.N (output from microcontroller)
    /// spi pin.
    sjsu::Pin & mosi;

    /// Refernce to the M.ASTER-I.N-S.LAVE-O.UT (input to microcontroller) spi
    /// pin.
    sjsu::Pin & miso;

    /// Refernce to serial clock spi pin.
    sjsu::Pin & sck;

    /// Function code to set each pin to the appropriate SSP function.
    uint8_t pin_function;
  };

  /// Constructor for LPC40xx Spi peripheral
  ///
  /// @param bus - pass a reference to a constant lpc40xx::Spi::Bus_t
  ///        definition.
  explicit constexpr Spi(const Bus_t & bus) : bus_(bus) {}

  /// This METHOD MUST BE EXECUTED before any other method can be called.
  /// Powers on the peripheral, activates the SSP pins and enables the SSP
  /// peripheral.
  /// See page 601 of user manual UM10562 LPC408x/407x for more details.
  void ModuleInitialize() override
  {
    constexpr uint8_t kSpiFormatCode = 0b00;

    // Power up peripheral
    sjsu::SystemController::GetPlatformController().PowerUpPeripheral(
        bus_.power_on_bit);

    // Set SSP frame format to SPI
    bus_.registers->CR0 = bit::Insert(
        bus_.registers->CR0, kSpiFormatCode, ControlRegister0::kFrameBit);

    // Set SPI to master mode by clearing
    bus_.registers->CR1 =
        bit::Clear(bus_.registers->CR1, ControlRegister1::kSlaveModeBit);

    ConfigureFrequency();
    ConfigureClockMode();
    ConfigureFrameSize();

    // Initialize SSP pins
    bus_.mosi.settings.function = bus_.pin_function;
    bus_.miso.settings.function = bus_.pin_function;
    bus_.sck.settings.function  = bus_.pin_function;
    bus_.mosi.Initialize();
    bus_.miso.Initialize();
    bus_.sck.Initialize();

    // Enable SSP
    bus_.registers->CR1 =
        bit::Set(bus_.registers->CR1, ControlRegister1::kSpiEnable);
  }

  void ModulePowerDown() override
  {
    // Disable SSP
    bus_.registers->CR1 =
        bit::Clear(bus_.registers->CR1, ControlRegister1::kSpiEnable);
  }

  /// Checks if the SSP controller is idle.
  /// @returns true if the controller is sending or receiving a data frame and
  /// false if it is idle.
  bool IsBusBusy() const
  {
    return bit::Read(bus_.registers->SR, StatusRegister::kDataLineBusyBit);
  }

  void Transfer(std::span<uint8_t> buffer) override
  {
    for (auto & transfer_byte : buffer)
    {
      bus_.registers->DR = transfer_byte;
      while (IsBusBusy())
      {
        continue;
      }
      transfer_byte = static_cast<uint8_t>(bus_.registers->DR);
    }
  }

  void Transfer(std::span<uint16_t> buffer) override
  {
    for (auto & transfer_byte : buffer)
    {
      bus_.registers->DR = transfer_byte;
      while (IsBusBusy())
      {
        continue;
      }
      transfer_byte = static_cast<uint16_t>(bus_.registers->DR);
    }
  }

 private:
  void ConfigureFrequency()
  {
    auto & system         = sjsu::SystemController::GetPlatformController();
    auto system_frequency = system.GetClockRate(bus_.power_on_bit);

    auto prescaler = system_frequency / settings.clock_rate;

    // Store lower half of prescalar in clock prescalar register
    bus_.registers->CPSR = prescaler.to<uint16_t>() & 0xFF;

    // Store upper 8 bit half of the prescalar in control register 0
    bus_.registers->CR0 = bit::Insert(bus_.registers->CR0,
                                      prescaler.to<uint16_t>() >> 8,
                                      ControlRegister0::kDividerBit);
  }

  void ConfigureClockMode()
  {
    bus_.registers->CR0 = bit::Insert(bus_.registers->CR0,
                                      Value(settings.polarity),
                                      ControlRegister0::kPolarityBit);

    bus_.registers->CR0 = bit::Insert(bus_.registers->CR0,
                                      Value(settings.phase),
                                      ControlRegister0::kPhaseBit);
  }

  void ConfigureFrameSize()
  {
    // NOTE: In UM10562 page 611, you will see that DSS (Data Size Select) is
    // equal to the bit transfer minus 1. So we can add 3 to our DataSize enum
    // to get the appropriate tranfer code.
    constexpr uint32_t kBitTransferCodeOffset = 3;
    const auto kSizeCode = Value(settings.frame_size) + kBitTransferCodeOffset;

    bus_.registers->CR0 = bit::Insert(bus_.registers->CR0,
                                      static_cast<uint8_t>(kSizeCode),
                                      ControlRegister0::kDataBit);
  }

  const Bus_t & bus_;
};

template <int port>
inline Spi & GetSpi()
{
  if constexpr (port == 0)
  {
    // SSP0 pins
    static sjsu::lpc40xx::Pin & mosi0 = sjsu::lpc40xx::GetPin<0, 18>();
    static sjsu::lpc40xx::Pin & miso0 = sjsu::lpc40xx::GetPin<0, 17>();
    static sjsu::lpc40xx::Pin & sck0  = sjsu::lpc40xx::GetPin<0, 15>();

    /// Definition for SPI bus 0 for LPC40xx
    static const Spi::Bus_t kSpi0 = {
      .registers    = LPC_SSP0,
      .power_on_bit = sjsu::lpc40xx::SystemController::Peripherals::kSsp0,
      .mosi         = mosi0,
      .miso         = miso0,
      .sck          = sck0,
      .pin_function = 0b010,
    };

    static Spi spi0(kSpi0);
    return spi0;
  }
  else if constexpr (port == 1)
  {
    // SSP1 pins
    static sjsu::lpc40xx::Pin & mosi1 = sjsu::lpc40xx::GetPin<0, 9>();
    static sjsu::lpc40xx::Pin & miso1 = sjsu::lpc40xx::GetPin<0, 8>();
    static sjsu::lpc40xx::Pin & sck1  = sjsu::lpc40xx::GetPin<0, 7>();

    /// Definition for SPI bus 1 for LPC40xx
    static const Spi::Bus_t kSpi1 = {
      .registers    = LPC_SSP1,
      .power_on_bit = sjsu::lpc40xx::SystemController::Peripherals::kSsp1,
      .mosi         = mosi1,
      .miso         = miso1,
      .sck          = sck1,
      .pin_function = 0b010,
    };

    static Spi spi1(kSpi1);
    return spi1;
  }
  else if constexpr (port == 2)
  {  // SSP2 pins
    static sjsu::lpc40xx::Pin & mosi2 = sjsu::lpc40xx::GetPin<1, 1>();
    static sjsu::lpc40xx::Pin & miso2 = sjsu::lpc40xx::GetPin<1, 4>();
    static sjsu::lpc40xx::Pin & sck2  = sjsu::lpc40xx::GetPin<1, 0>();

    /// Definition for SPI bus 2 for LPC40xx
    static const Spi::Bus_t kSpi2 = {
      .registers    = LPC_SSP2,
      .power_on_bit = sjsu::lpc40xx::SystemController::Peripherals::kSsp2,
      .mosi         = mosi2,
      .miso         = miso2,
      .sck          = sck2,
      .pin_function = 0b100,
    };

    static Spi spi2(kSpi2);
    return spi2;
  }
  else
  {
    static_assert(InvalidOption<port>,
                  SJ2_ERROR_MESSAGE_DECORATOR(
                      "LPC40xx only supports SPI0, SPI1, and SPI2."));
    return GetSpi<0>();
  }
}
}  // namespace lpc40xx
}  // namespace sjsu
