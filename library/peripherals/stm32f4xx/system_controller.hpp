#pragma once

#include <cstdint>

#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "peripherals/system_controller.hpp"
#include "utility/math/bit.hpp"
#include "utility/macros.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace stm32f4xx
{
/// System controller for stm32f4xx that controls clock sources, clock speed,
/// clock outputs control, and peripheral enabling
///
/// @see 6 Reset and clock control for STM32F42xxx and STM32F43xxx (RCC)
///      https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=150
/// @see Figure 16. Clock tree
///      https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf#page=152
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
    static constexpr auto kGpioA       = ResourceID::Define<kAHB1 + 0>();
    static constexpr auto kGpioB       = ResourceID::Define<kAHB1 + 1>();
    static constexpr auto kGpioC       = ResourceID::Define<kAHB1 + 2>();
    static constexpr auto kGpioD       = ResourceID::Define<kAHB1 + 3>();
    static constexpr auto kGpioE       = ResourceID::Define<kAHB1 + 4>();
    static constexpr auto kGpioF       = ResourceID::Define<kAHB1 + 5>();
    static constexpr auto kGpioG       = ResourceID::Define<kAHB1 + 6>();
    static constexpr auto kGpioH       = ResourceID::Define<kAHB1 + 7>();
    static constexpr auto kGpioI       = ResourceID::Define<kAHB1 + 8>();
    static constexpr auto kCrc         = ResourceID::Define<kAHB1 + 12>();
    static constexpr auto kBackupSRAM  = ResourceID::Define<kAHB1 + 18>();
    static constexpr auto kCcmRam      = ResourceID::Define<kAHB1 + 20>();
    static constexpr auto kDma1        = ResourceID::Define<kAHB1 + 21>();
    static constexpr auto kDma2        = ResourceID::Define<kAHB1 + 22>();
    static constexpr auto kEthernetMac = ResourceID::Define<kAHB1 + 25>();
    static constexpr auto kEthernetTx  = ResourceID::Define<kAHB1 + 26>();
    static constexpr auto kEthernetRx  = ResourceID::Define<kAHB1 + 27>();
    static constexpr auto kEthernetPTP = ResourceID::Define<kAHB1 + 28>();
    static constexpr auto kUsbOtg      = ResourceID::Define<kAHB1 + 29>();
    static constexpr auto kUsbOtgHS    = ResourceID::Define<kAHB1 + 30>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAHB2 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kCamera   = ResourceID::Define<kAHB2 + 0>();
    static constexpr auto kCrypto   = ResourceID::Define<kAHB2 + 4>();
    static constexpr auto kHash     = ResourceID::Define<kAHB2 + 5>();
    static constexpr auto kRandom   = ResourceID::Define<kAHB2 + 6>();
    static constexpr auto kUsbOtgFs = ResourceID::Define<kAHB2 + 7>();
    //! @endcond

    /// Bit position of AHB3
    static constexpr uint32_t kAHB3 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kFlexStaticMemory = ResourceID::Define<kAHB3 + 0>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 3;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer2         = ResourceID::Define<kAPB1 + 0>();
    static constexpr auto kTimer3         = ResourceID::Define<kAPB1 + 1>();
    static constexpr auto kTimer4         = ResourceID::Define<kAPB1 + 2>();
    static constexpr auto kTimer5         = ResourceID::Define<kAPB1 + 3>();
    static constexpr auto kTimer6         = ResourceID::Define<kAPB1 + 4>();
    static constexpr auto kTimer7         = ResourceID::Define<kAPB1 + 5>();
    static constexpr auto kTimer12        = ResourceID::Define<kAPB1 + 6>();
    static constexpr auto kTimer13        = ResourceID::Define<kAPB1 + 7>();
    static constexpr auto kTimer14        = ResourceID::Define<kAPB1 + 8>();
    static constexpr auto kWindowWatchdog = ResourceID::Define<kAPB1 + 11>();
    static constexpr auto kSpi2           = ResourceID::Define<kAPB1 + 14>();
    static constexpr auto kSpi3           = ResourceID::Define<kAPB1 + 15>();
    static constexpr auto kUsart2         = ResourceID::Define<kAPB1 + 17>();
    static constexpr auto kUsart3         = ResourceID::Define<kAPB1 + 18>();
    static constexpr auto kUart4          = ResourceID::Define<kAPB1 + 19>();
    static constexpr auto kUart5          = ResourceID::Define<kAPB1 + 20>();
    static constexpr auto kI2c1           = ResourceID::Define<kAPB1 + 21>();
    static constexpr auto kI2c2           = ResourceID::Define<kAPB1 + 22>();
    static constexpr auto kI2c3           = ResourceID::Define<kAPB1 + 23>();
    static constexpr auto kCan1           = ResourceID::Define<kAPB1 + 25>();
    static constexpr auto kCan2           = ResourceID::Define<kAPB1 + 26>();
    static constexpr auto kPower          = ResourceID::Define<kAPB1 + 28>();
    static constexpr auto kDac            = ResourceID::Define<kAPB1 + 29>();
    //! @endcond

    /// Bit position of APB2
    static constexpr uint32_t kAPB2 = kBits * 4;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer1  = ResourceID::Define<kAPB2 + 0>();
    static constexpr auto kTimer8  = ResourceID::Define<kAPB2 + 1>();
    static constexpr auto kUsart1  = ResourceID::Define<kAPB2 + 4>();
    static constexpr auto kUsart6  = ResourceID::Define<kAPB2 + 6>();
    static constexpr auto kAdc1    = ResourceID::Define<kAPB2 + 8>();
    static constexpr auto kAdc2    = ResourceID::Define<kAPB2 + 9>();
    static constexpr auto kAdc3    = ResourceID::Define<kAPB2 + 10>();
    static constexpr auto kSdIO    = ResourceID::Define<kAPB2 + 11>();
    static constexpr auto kSpi1    = ResourceID::Define<kAPB2 + 12>();
    static constexpr auto kSysCfg  = ResourceID::Define<kAPB2 + 14>();
    static constexpr auto kTimer9  = ResourceID::Define<kAPB2 + 16>();
    static constexpr auto kTimer10 = ResourceID::Define<kAPB2 + 17>();
    static constexpr auto kTimer11 = ResourceID::Define<kAPB2 + 18>();
    //! @endcond

    /// Bit position of systems outside of any bus
    static constexpr uint32_t kBeyond = kBits * 5;

    //! @cond Doxygen_Suppress
    static constexpr auto kCpu         = ResourceID::Define<kBeyond + 0>();
    static constexpr auto kSystemTimer = ResourceID::Define<kBeyond + 1>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHB1ENR, &RCC->AHB2ENR, &RCC->AHB3ENR, &RCC->APB1ENR, &RCC->APB2ENR,
  };

  void Initialize() override
  {
    return;
  }

  void * GetClockConfiguration() override
  {
    SJ2_ASSERT_FATAL(
        false,
        "GetClockConfiguration() will not return a valid clock configuration");
    return nullptr;
  }

  units::frequency::hertz_t GetClockRate(ResourceID) const override
  {
    return 16_MHz;
  }

  bool IsPeripheralPoweredUp(ResourceID id) const override
  {
    return bit::Read(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerUpPeripheral(ResourceID id) const override
  {
    *EnableRegister(id) = bit::Set(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerDownPeripheral(ResourceID id) const override
  {
    *EnableRegister(id) =
        bit::Clear(*EnableRegister(id), EnableBitPosition(id));
  }

 private:
  volatile uint32_t * EnableRegister(ResourceID id) const
  {
    return enable[id.device_id / kBits];
  }

  uint32_t EnableBitPosition(ResourceID id) const
  {
    return id.device_id % kBits;
  }
};
}  // namespace stm32f4xx
}  // namespace sjsu
