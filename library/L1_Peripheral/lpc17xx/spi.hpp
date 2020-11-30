#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Spi;

/// Structure used as a namespace for predefined Bus definitions
struct SpiBus  // NOLINT
{
 private:
  // SSP0 pins
  inline static lpc17xx::Pin mosi0 = lpc17xx::Pin(0, 18);
  inline static lpc17xx::Pin miso0 = lpc17xx::Pin(0, 17);
  inline static lpc17xx::Pin sck0  = lpc17xx::Pin(0, 15);
  // SSP1 pins
  inline static lpc17xx::Pin mosi1 = lpc17xx::Pin(0, 9);
  inline static lpc17xx::Pin miso1 = lpc17xx::Pin(0, 8);
  inline static lpc17xx::Pin sck1  = lpc17xx::Pin(0, 7);

 public:
  /// Definition for SPI bus 0 for LPC17xx
  inline static const lpc40xx::Spi::Bus_t kSpi0 = {
    .registers    = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP0),
    .power_on_bit = SystemController::Peripherals::kSsp0,
    .mosi         = mosi0,
    .miso         = miso0,
    .sck          = sck0,
    .pin_function = 0b10,
  };
  /// Definition for SPI bus 1 for LPC17xx
  inline static const lpc40xx::Spi::Bus_t kSpi1 = {
    .registers    = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP1),
    .power_on_bit = SystemController::Peripherals::kSsp1,
    .mosi         = mosi1,
    .miso         = miso1,
    .sck          = sck1,
    .pin_function = 0b10,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu
