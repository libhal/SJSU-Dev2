#pragma once

#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"

namespace sjsu
{
namespace lpc17xx
{
class SystemController final : public sjsu::SystemController
{
 public:
  // LPC17xx Peripheral Power On Values:
  // The kDeviceId of each peripheral corresponds to the peripheral's power on
  // bit position within the LPC17xx System Controller's PCONP register.
  class Peripherals
  {
   public:
    static constexpr auto kLcd               = AddPeripheralID<0>();
    static constexpr auto kTimer0            = AddPeripheralID<1>();
    static constexpr auto kTimer1            = AddPeripheralID<2>();
    static constexpr auto kUart0             = AddPeripheralID<3>();
    static constexpr auto kUart1             = AddPeripheralID<4>();
    static constexpr auto kPwm0              = AddPeripheralID<5>();
    static constexpr auto kPwm1              = AddPeripheralID<6>();
    static constexpr auto kI2c0              = AddPeripheralID<7>();
    static constexpr auto kUart4             = AddPeripheralID<8>();
    static constexpr auto kRtc               = AddPeripheralID<9>();
    static constexpr auto kSsp1              = AddPeripheralID<10>();
    static constexpr auto kEmc               = AddPeripheralID<11>();
    static constexpr auto kAdc               = AddPeripheralID<12>();
    static constexpr auto kCan1              = AddPeripheralID<13>();
    static constexpr auto kCan2              = AddPeripheralID<14>();
    static constexpr auto kGpio              = AddPeripheralID<15>();
    static constexpr auto kSpifi             = AddPeripheralID<16>();
    static constexpr auto kMotorControlPwm   = AddPeripheralID<17>();
    static constexpr auto kQuadratureEncoder = AddPeripheralID<18>();
    static constexpr auto kI2c1              = AddPeripheralID<19>();
    static constexpr auto kSsp2              = AddPeripheralID<20>();
    static constexpr auto kSsp0              = AddPeripheralID<21>();
    static constexpr auto kTimer2            = AddPeripheralID<22>();
    static constexpr auto kTimer3            = AddPeripheralID<23>();
    static constexpr auto kUart2             = AddPeripheralID<24>();
    static constexpr auto kUart3             = AddPeripheralID<25>();
    static constexpr auto kI2c2              = AddPeripheralID<26>();
    static constexpr auto kI2s               = AddPeripheralID<27>();
    static constexpr auto kSdCard            = AddPeripheralID<28>();
    static constexpr auto kGpdma             = AddPeripheralID<29>();
    static constexpr auto kEthernet          = AddPeripheralID<30>();
    static constexpr auto kUsb               = AddPeripheralID<31>();
  };

  static constexpr uint32_t kDefaultIRCFrequency    = 4'000'000;
  static constexpr uint32_t kDefaultTimeout         = 1'000;  // ms

  inline static LPC_SC_TypeDef * system_controller = LPC_SC;

  uint32_t SetClockFrequency(uint8_t /* frequency_in_mhz */) const override
  {
    return 0;
  }

  void SetPeripheralClockDivider(
      uint8_t /* peripheral_divider */) const override
  {
    system_controller->PCLKSEL0 = 0x5555'5555;
    system_controller->PCLKSEL1 = 0x5555'5555;
  }

  uint32_t GetPeripheralClockDivider() const override
  {
    return 1;
  }

  uint32_t GetSystemFrequency() const override
  {
    return speed_in_hertz;
  }

  uint32_t GetPeripheralFrequency() const override
  {
    return speed_in_hertz;
  }
  /// Check if a peripheral is powered up by checking the power connection
  /// register. Should typically only be used for unit testing code and
  /// debugging.
  bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const override
  {
    bool peripheral_is_powered_on =
        system_controller->PCONP & (1 << peripheral_select.device_id);

    return peripheral_is_powered_on;
  }
  void PowerUpPeripheral(
      const PeripheralID & peripheral_select) const override
  {
    auto power_connection_with_enabled_peripheral =
        system_controller->PCONP | (1 << peripheral_select.device_id);

    system_controller->PCONP = power_connection_with_enabled_peripheral;
  }
  void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const override
  {
    auto power_connection_without_enabled_peripheral =
        system_controller->PCONP & ~(1 << peripheral_select.device_id);

    system_controller->PCONP = power_connection_without_enabled_peripheral;
  }

 private:
  inline static uint32_t speed_in_hertz = kDefaultIRCFrequency;
};

}  // namespace lpc17xx
}  // namespace sjsu
