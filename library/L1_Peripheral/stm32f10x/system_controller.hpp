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
class SystemController : public sjsu::SystemController
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
    static constexpr auto kDma1  = PeripheralID::Define<kAHB + 0>();
    static constexpr auto kDma2  = PeripheralID::Define<kAHB + 1>();
    static constexpr auto kSram  = PeripheralID::Define<kAHB + 2>();
    static constexpr auto kFlitf = PeripheralID::Define<kAHB + 4>();
    static constexpr auto kCrc   = PeripheralID::Define<kAHB + 6>();
    static constexpr auto kFsmc  = PeripheralID::Define<kAHB + 8>();
    static constexpr auto kSdio  = PeripheralID::Define<kAHB + 10>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer2         = PeripheralID::Define<kAPB1 + 0>();
    static constexpr auto kTimer3         = PeripheralID::Define<kAPB1 + 1>();
    static constexpr auto kTimer4         = PeripheralID::Define<kAPB1 + 2>();
    static constexpr auto kTimer5         = PeripheralID::Define<kAPB1 + 3>();
    static constexpr auto kTimer6         = PeripheralID::Define<kAPB1 + 4>();
    static constexpr auto kTimer7         = PeripheralID::Define<kAPB1 + 5>();
    static constexpr auto kTimer12        = PeripheralID::Define<kAPB1 + 6>();
    static constexpr auto kTimer13        = PeripheralID::Define<kAPB1 + 7>();
    static constexpr auto kTimer14        = PeripheralID::Define<kAPB1 + 8>();
    static constexpr auto kWindowWatchdog = PeripheralID::Define<kAPB1 + 11>();
    static constexpr auto kSpi2           = PeripheralID::Define<kAPB1 + 14>();
    static constexpr auto kSpi3           = PeripheralID::Define<kAPB1 + 15>();
    static constexpr auto kUsart2         = PeripheralID::Define<kAPB1 + 17>();
    static constexpr auto kUsart3         = PeripheralID::Define<kAPB1 + 18>();
    static constexpr auto kUart4          = PeripheralID::Define<kAPB1 + 19>();
    static constexpr auto kUart5          = PeripheralID::Define<kAPB1 + 20>();
    static constexpr auto kI2c1           = PeripheralID::Define<kAPB1 + 21>();
    static constexpr auto kI2c2           = PeripheralID::Define<kAPB1 + 22>();
    static constexpr auto kUsb            = PeripheralID::Define<kAPB1 + 23>();
    static constexpr auto kCan1           = PeripheralID::Define<kAPB1 + 25>();
    static constexpr auto kBackupClock    = PeripheralID::Define<kAPB1 + 27>();
    static constexpr auto kPower          = PeripheralID::Define<kAPB1 + 28>();
    static constexpr auto kDac            = PeripheralID::Define<kAPB1 + 29>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAPB2 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kAFIO    = PeripheralID::Define<kAPB2 + 0>();
    static constexpr auto kGpioA   = PeripheralID::Define<kAPB2 + 2>();
    static constexpr auto kGpioB   = PeripheralID::Define<kAPB2 + 3>();
    static constexpr auto kGpioC   = PeripheralID::Define<kAPB2 + 4>();
    static constexpr auto kGpioD   = PeripheralID::Define<kAPB2 + 5>();
    static constexpr auto kGpioE   = PeripheralID::Define<kAPB2 + 6>();
    static constexpr auto kGpioF   = PeripheralID::Define<kAPB2 + 7>();
    static constexpr auto kGpioG   = PeripheralID::Define<kAPB2 + 8>();
    static constexpr auto kAdc1    = PeripheralID::Define<kAPB2 + 9>();
    static constexpr auto kAdc2    = PeripheralID::Define<kAPB2 + 10>();
    static constexpr auto kTimer1  = PeripheralID::Define<kAPB2 + 11>();
    static constexpr auto kSpi1    = PeripheralID::Define<kAPB2 + 12>();
    static constexpr auto kTimer8  = PeripheralID::Define<kAPB2 + 13>();
    static constexpr auto kUsart1  = PeripheralID::Define<kAPB2 + 14>();
    static constexpr auto kAdc3    = PeripheralID::Define<kAPB2 + 15>();
    static constexpr auto kTimer9  = PeripheralID::Define<kAPB2 + 19>();
    static constexpr auto kTimer10 = PeripheralID::Define<kAPB2 + 20>();
    static constexpr auto kTimer11 = PeripheralID::Define<kAPB2 + 21>();
    //! @endcond

    /// Bit position of systems outside of any bus
    static constexpr uint32_t kBeyond = kBits * 3;

    //! @cond Doxygen_Suppress
    static constexpr auto kCpu         = PeripheralID::Define<kBeyond + 0>();
    static constexpr auto kSystemTimer = PeripheralID::Define<kBeyond + 1>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHBENR,
    &RCC->APB1ENR,
    &RCC->APB2ENR,
  };

  void Initialize() override
  {
    return;
  }

  /// @return the a pointer to the clock configuration object used to configure
  /// this system controller.
  void * GetClockConfiguration() override
  {
    return nullptr;
  }

  /// @return the clock rate frequency of a peripheral
  units::frequency::hertz_t GetClockRate(PeripheralID) const override
  {
    return 8_MHz;
  }

  bool IsPeripheralPoweredUp(PeripheralID id) const override
  {
    return bit::Read(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerUpPeripheral(PeripheralID id) const override
  {
    *EnableRegister(id) = bit::Set(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerDownPeripheral(PeripheralID id) const override
  {
    *EnableRegister(id) =
        bit::Clear(*EnableRegister(id), EnableBitPosition(id));
  }

 private:
  volatile uint32_t * EnableRegister(PeripheralID id) const
  {
    return enable[id.device_id / kBits];
  }

  uint32_t EnableBitPosition(PeripheralID id) const
  {
    return id.device_id % kBits;
  }
};
}  // namespace stm32f10x
}  // namespace sjsu
