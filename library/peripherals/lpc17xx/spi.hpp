#pragma once

#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc17xx/system_controller.hpp"
#include "peripherals/lpc40xx/spi.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Spi;

template <int port>
inline Spi & GetSpi()
{
  if constexpr (port == 0)
  {
    // SSP0 pins
    static lpc17xx::Pin & mosi0 = lpc17xx::GetPin<0, 18>();
    static lpc17xx::Pin & miso0 = lpc17xx::GetPin<0, 17>();
    static lpc17xx::Pin & sck0  = lpc17xx::GetPin<0, 15>();

    /// Definition for SPI bus 0 for LPC17xx
    static const lpc40xx::Spi::Bus_t kSpi0 = {
      .registers    = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP0),
      .power_on_bit = SystemController::Peripherals::kSsp0,
      .mosi         = mosi0,
      .miso         = miso0,
      .sck          = sck0,
      .pin_function = 0b10,
    };

    static Spi spi0(kSpi0);
    return spi0;
  }
  else if constexpr (port == 1)
  {
    // SSP1 pins
    static lpc17xx::Pin & mosi1 = lpc17xx::GetPin<0, 9>();
    static lpc17xx::Pin & miso1 = lpc17xx::GetPin<0, 8>();
    static lpc17xx::Pin & sck1  = lpc17xx::GetPin<0, 7>();

    /// Definition for SPI bus 1 for LPC17xx
    static const lpc40xx::Spi::Bus_t kSpi1 = {
      .registers    = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP1),
      .power_on_bit = SystemController::Peripherals::kSsp1,
      .mosi         = mosi1,
      .miso         = miso1,
      .sck          = sck1,
      .pin_function = 0b10,
    };

    static Spi spi1(kSpi1);
    return spi1;
  }
  else
  {
    static_assert(InvalidOption<port>,
                  SJ2_ERROR_MESSAGE_DECORATOR(
                      "LPC40xx only supports SPI0 and SPI1."));
    return GetSpi<0>();
  }
}
}  // namespace lpc17xx
}  // namespace sjsu
