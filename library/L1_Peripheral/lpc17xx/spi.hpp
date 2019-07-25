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

struct SpiBus  // NOLINT
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

 public:
  inline static const lpc40xx::Spi::Bus_t kSpi0 = {
    .registers       = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP0),
    .power_on_bit    = SystemController::Peripherals::kSsp0,
    .mosi            = kMosi0,
    .miso            = kMiso0,
    .sck             = kSck0,
    .pin_function_id = 0b10,
  };
  inline static const lpc40xx::Spi::Bus_t kSpi1 = {
    .registers       = reinterpret_cast<lpc40xx::LPC_SSP_TypeDef *>(LPC_SSP1),
    .power_on_bit    = SystemController::Peripherals::kSsp1,
    .mosi            = kMosi1,
    .miso            = kMiso1,
    .sck             = kSck1,
    .pin_function_id = 0b10,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu
