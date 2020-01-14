#pragma once

#include <cstdint>

#include "L0_Platform/stm32f4xx/stm32f4xx.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace stm32f4xx
{
/// System controller for stm32f4xx that controls clock sources, clock speed,
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
    /// Bit position of AHB1
    static constexpr uint32_t kAHB1 = kBits * 0;

    //! @cond Doxygen_Suppress
    static constexpr auto kGpioA       = AddPeripheralID<kAHB1 + 0>();
    static constexpr auto kGpioB       = AddPeripheralID<kAHB1 + 1>();
    static constexpr auto kGpioC       = AddPeripheralID<kAHB1 + 2>();
    static constexpr auto kGpioD       = AddPeripheralID<kAHB1 + 3>();
    static constexpr auto kGpioE       = AddPeripheralID<kAHB1 + 4>();
    static constexpr auto kGpioF       = AddPeripheralID<kAHB1 + 5>();
    static constexpr auto kGpioG       = AddPeripheralID<kAHB1 + 6>();
    static constexpr auto kGpioH       = AddPeripheralID<kAHB1 + 7>();
    static constexpr auto kGpioI       = AddPeripheralID<kAHB1 + 8>();
    static constexpr auto kCrc         = AddPeripheralID<kAHB1 + 12>();
    static constexpr auto kBackupSRAM  = AddPeripheralID<kAHB1 + 18>();
    static constexpr auto kCcmRam      = AddPeripheralID<kAHB1 + 20>();
    static constexpr auto kDma1        = AddPeripheralID<kAHB1 + 21>();
    static constexpr auto kDma2        = AddPeripheralID<kAHB1 + 22>();
    static constexpr auto kEthernetMac = AddPeripheralID<kAHB1 + 25>();
    static constexpr auto kEthernetTx  = AddPeripheralID<kAHB1 + 26>();
    static constexpr auto kEthernetRx  = AddPeripheralID<kAHB1 + 27>();
    static constexpr auto kEthernetPTP = AddPeripheralID<kAHB1 + 28>();
    static constexpr auto kUsbOtg      = AddPeripheralID<kAHB1 + 29>();
    static constexpr auto kUsbOtgHS    = AddPeripheralID<kAHB1 + 30>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAHB2 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kCamera   = AddPeripheralID<kAHB2 + 0>();
    static constexpr auto kCrypto   = AddPeripheralID<kAHB2 + 4>();
    static constexpr auto kHash     = AddPeripheralID<kAHB2 + 5>();
    static constexpr auto kRandom   = AddPeripheralID<kAHB2 + 6>();
    static constexpr auto kUsbOtgFs = AddPeripheralID<kAHB2 + 7>();
    //! @endcond

    /// Bit position of AHB3
    static constexpr uint32_t kAHB3 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kFlexStaticMemory = AddPeripheralID<kAHB3 + 0>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 3;

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
    static constexpr auto kI2c3           = AddPeripheralID<kAPB1 + 23>();
    static constexpr auto kCan1           = AddPeripheralID<kAPB1 + 25>();
    static constexpr auto kCan2           = AddPeripheralID<kAPB1 + 26>();
    static constexpr auto kPower          = AddPeripheralID<kAPB1 + 28>();
    static constexpr auto kDac            = AddPeripheralID<kAPB1 + 29>();
    //! @endcond

    /// Bit position of APB2
    static constexpr uint32_t kAPB2 = kBits * 4;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer1  = AddPeripheralID<kAPB2 + 0>();
    static constexpr auto kTimer8  = AddPeripheralID<kAPB2 + 1>();
    static constexpr auto kUsart1  = AddPeripheralID<kAPB2 + 4>();
    static constexpr auto kUsart6  = AddPeripheralID<kAPB2 + 6>();
    static constexpr auto kAdc1    = AddPeripheralID<kAPB2 + 8>();
    static constexpr auto kAdc2    = AddPeripheralID<kAPB2 + 9>();
    static constexpr auto kAdc3    = AddPeripheralID<kAPB2 + 10>();
    static constexpr auto kSdIO    = AddPeripheralID<kAPB2 + 11>();
    static constexpr auto kSpi1    = AddPeripheralID<kAPB2 + 12>();
    static constexpr auto kSysCfg  = AddPeripheralID<kAPB2 + 14>();
    static constexpr auto kTimer9  = AddPeripheralID<kAPB2 + 16>();
    static constexpr auto kTimer10 = AddPeripheralID<kAPB2 + 17>();
    static constexpr auto kTimer11 = AddPeripheralID<kAPB2 + 18>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHB1ENR, &RCC->AHB2ENR, &RCC->AHB3ENR, &RCC->APB1ENR, &RCC->APB2ENR,
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
    return 16_MHz;
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
}  // namespace stm32f4xx
}  // namespace sjsu
