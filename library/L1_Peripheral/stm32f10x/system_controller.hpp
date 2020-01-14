#pragma once

#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"

#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// System controller for stm32f10x that controls clock sources, clock speed,
/// clock outputs control, and peripheral enabling
class SystemController final : public sjsu::SystemController
{
 public:
  /// Number of bits between each enable register
  static constexpr uint32_t kBits = 32;

  /// Namespace containing the set of all peripherals id on the stm32f4xx mcu
  /// family.
  class Peripherals
  {
   public:
    /// Bit position of AHB
    static constexpr uint32_t kAHB = kBits * 0;

    //! @cond Doxygen_Suppress
    static constexpr auto kDma1  = AddPeripheralID<kAHB + 0>();
    static constexpr auto kDma2  = AddPeripheralID<kAHB + 1>();
    static constexpr auto kSram  = AddPeripheralID<kAHB + 2>();
    static constexpr auto kFlitf = AddPeripheralID<kAHB + 4>();
    static constexpr auto kCrc   = AddPeripheralID<kAHB + 6>();
    static constexpr auto kFsmc  = AddPeripheralID<kAHB + 8>();
    static constexpr auto kSdio  = AddPeripheralID<kAHB + 10>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer2         = AddPeripheralID<kAPB1 + 0>();
    static constexpr auto kTimer3         = AddPeripheralID<kAPB1 + 1>();
    static constexpr auto kTimer4         = AddPeripheralID<kAPB1 + 2>();
    static constexpr auto kTimer5         = AddPeripheralID<kAPB1 + 3>();
    static constexpr auto kTimer6         = AddPeripheralID<kAPB1 + 4>();
    static constexpr auto kTimer7         = AddPeripheralID<kAPB1 + 5>();
    static constexpr auto kTimer12        = AddPeripheralID<kAPB1 + 6>();
    static constexpr auto kTimer13        = AddPeripheralID<kAPB1 + 7>();
    static constexpr auto kTimer14        = AddPeripheralID<kAPB1 + 8>();
    static constexpr auto kWindowWatchdog = AddPeripheralID<kAPB1 + 11>();
    static constexpr auto kSpi2           = AddPeripheralID<kAPB1 + 14>();
    static constexpr auto kSpi3           = AddPeripheralID<kAPB1 + 15>();
    static constexpr auto kUsart2         = AddPeripheralID<kAPB1 + 17>();
    static constexpr auto kUsart3         = AddPeripheralID<kAPB1 + 18>();
    static constexpr auto kUart4          = AddPeripheralID<kAPB1 + 19>();
    static constexpr auto kUart5          = AddPeripheralID<kAPB1 + 20>();
    static constexpr auto kI2c1           = AddPeripheralID<kAPB1 + 21>();
    static constexpr auto kI2c2           = AddPeripheralID<kAPB1 + 22>();
    static constexpr auto kUsb            = AddPeripheralID<kAPB1 + 23>();
    static constexpr auto kCan1           = AddPeripheralID<kAPB1 + 25>();
    static constexpr auto kBackupClock    = AddPeripheralID<kAPB1 + 27>();
    static constexpr auto kPower          = AddPeripheralID<kAPB1 + 28>();
    static constexpr auto kDac            = AddPeripheralID<kAPB1 + 29>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAPB2 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kAFIO    = AddPeripheralID<kAPB2 + 0>();
    static constexpr auto kGpioA   = AddPeripheralID<kAPB2 + 2>();
    static constexpr auto kGpioB   = AddPeripheralID<kAPB2 + 3>();
    static constexpr auto kGpioC   = AddPeripheralID<kAPB2 + 4>();
    static constexpr auto kGpioD   = AddPeripheralID<kAPB2 + 5>();
    static constexpr auto kGpioE   = AddPeripheralID<kAPB2 + 6>();
    static constexpr auto kGpioF   = AddPeripheralID<kAPB2 + 7>();
    static constexpr auto kGpioG   = AddPeripheralID<kAPB2 + 8>();
    static constexpr auto kAdc1    = AddPeripheralID<kAPB2 + 9>();
    static constexpr auto kAdc2    = AddPeripheralID<kAPB2 + 10>();
    static constexpr auto kTimer1  = AddPeripheralID<kAPB2 + 11>();
    static constexpr auto kSpi1    = AddPeripheralID<kAPB2 + 12>();
    static constexpr auto kTimer8  = AddPeripheralID<kAPB2 + 13>();
    static constexpr auto kUsart1  = AddPeripheralID<kAPB2 + 14>();
    static constexpr auto kAdc3    = AddPeripheralID<kAPB2 + 15>();
    static constexpr auto kTimer9  = AddPeripheralID<kAPB2 + 19>();
    static constexpr auto kTimer10 = AddPeripheralID<kAPB2 + 20>();
    static constexpr auto kTimer11 = AddPeripheralID<kAPB2 + 21>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHBENR,
    &RCC->APB1ENR,
    &RCC->APB2ENR,
  };

  void SetSystemClockFrequency(units::frequency::megahertz_t) const override {}

  void SetPeripheralClockDivider(const PeripheralID &, uint8_t) const override
  {
  }

  uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
  {
    return 1;
  }

  units::frequency::hertz_t GetSystemFrequency() const override
  {
    return 8_MHz;
  }

  bool IsPeripheralPoweredUp(const PeripheralID & id) const override
  {
    return bit::Read(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerUpPeripheral(const PeripheralID & id) const override
  {
    *EnableRegister(id) = bit::Set(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerDownPeripheral(const PeripheralID & id) const override
  {
    *EnableRegister(id) =
        bit::Clear(*EnableRegister(id), EnableBitPosition(id));
  }

 private:
  volatile uint32_t * EnableRegister(const PeripheralID & id) const
  {
    return enable[id.device_id / kBits];
  }
  uint32_t EnableBitPosition(const PeripheralID & id) const
  {
    return id.device_id % kBits;
  }
};
}  // namespace stm32f10x
}  // namespace sjsu
