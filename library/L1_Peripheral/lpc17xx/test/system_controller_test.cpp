// Tests for the LPC176x/5x System Controller.
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc17xx
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("Testing LPC176x/5x System Controller", "[lpc17xx-SystemController]")
{
  // Simulate local version of LPC_SC
  LPC_SC_TypeDef local_sc;
  memset(&local_sc, 0, sizeof(local_sc));
  SystemController::system_controller = &local_sc;

  constexpr SystemController::PeripheralID kPeripherals[] = {
    SystemController::Peripherals::kTimer0,
    SystemController::Peripherals::kTimer1,
    SystemController::Peripherals::kUart0,
    SystemController::Peripherals::kUart1,
    SystemController::Peripherals::kPwm1,
    SystemController::Peripherals::kI2c0,
    SystemController::Peripherals::kSpi,
    SystemController::Peripherals::kRtc,
    SystemController::Peripherals::kSsp1,
    SystemController::Peripherals::kAdc,
    SystemController::Peripherals::kCan1,
    SystemController::Peripherals::kCan2,
    SystemController::Peripherals::kGpio,
    SystemController::Peripherals::kRit,
    SystemController::Peripherals::kMotorControlPwm,
    SystemController::Peripherals::kQuadratureEncoder,
    SystemController::Peripherals::kI2c1,
    SystemController::Peripherals::kSsp0,
    SystemController::Peripherals::kTimer2,
    SystemController::Peripherals::kTimer3,
    SystemController::Peripherals::kUart2,
    SystemController::Peripherals::kUart3,
    SystemController::Peripherals::kI2c2,
    SystemController::Peripherals::kI2s,
    SystemController::Peripherals::kGpdma,
    SystemController::Peripherals::kEthernet,
    SystemController::Peripherals::kUsb,
  };
  // In the kPeripherals array, all the peripheral clock select for peripherals
  // starting from kRit (index = 13) are in the PCLKSEL1 register while all the
  // clock selects for peripherals before kRit are in the PCLKSEL0 register.
  constexpr uint8_t kPclkSel1StartIndex = 13;

  SystemController system_controller;

  SECTION("Set CPU Clock Frequency")
  {
    constexpr units::frequency::hertz_t kDefaultIRCFrequency = 4_MHz;
    constexpr units::frequency::hertz_t kExpectedFrequency   = 48_MHz;

    system_controller.SetSystemClockFrequency(kExpectedFrequency);

    CHECK(bit::Read(local_sc.PLL0CON, SystemController::kPllEnableBit) == 0b1);
    CHECK(bit::Read(local_sc.PLL0CON, SystemController::kPllConnectBit) == 0b1);
    // Reading the multiplier, pre-divider, and cpu clock dividers from their
    // corrensponding registers to re-calculate the desired cpu frequency to
    // check at the correct values were written
    const uint32_t kMultiplier =
        bit::Extract(local_sc.PLL0CFG, SystemController::MainPll::kMultiplier) +
        1;
    const uint32_t kPreDivider =
        bit::Extract(local_sc.PLL0CFG, SystemController::MainPll::kPreDivider) +
        1;
    const uint32_t kCpuDivider =
        bit::Extract(local_sc.CCLKCFG, SystemController::CpuClock::kDivider) +
        1;
    const uint32_t kCalculatedClockFrequency =
        ((2 * kMultiplier * kDefaultIRCFrequency.to<uint32_t>()) /
         kPreDivider) /
        kCpuDivider;

    CHECK(bit::Extract(local_sc.CLKSRCSEL,
                       SystemController::Oscillator::kSelect) == 0b00);
    CHECK(kCalculatedClockFrequency == kExpectedFrequency.to<uint32_t>());
    CHECK(system_controller.GetSystemFrequency() == kExpectedFrequency);
  }

  SECTION("Set USB PLL Input Frequency")
  {
    const SystemController::UsbPllInputFrequency kInputFrequencies[] = {
      SystemController::UsbPllInputFrequency::k12MHz,
      SystemController::UsbPllInputFrequency::k16MHz,
      SystemController::UsbPllInputFrequency::k24MHz
    };
    const uint8_t kExpectedMultiplier[] = {
      Value(SystemController::UsbPllMultiplier::kMultiplyBy4),
      Value(SystemController::UsbPllMultiplier::kMultiplyBy3),
      Value(SystemController::UsbPllMultiplier::kMultiplyBy2),
    };
    constexpr uint8_t kExpectedDivider =
        Value(SystemController::UsbPllDivider::kDivideBy1);

    for (uint8_t i = 0; i > std::size(kInputFrequencies); i++)
    {
      system_controller.SetUsbPllInputFrequency(kInputFrequencies[i]);

      const uint32_t kMultiplier =
          bit::Extract(local_sc.PLL1CFG, SystemController::UsbPll::kMultiplier);
      const uint32_t kDivider =
          bit::Extract(local_sc.PLL1CFG, SystemController::UsbPll::kDivider);

      CHECK(bit::Read(local_sc.PLL1CON, SystemController::kPllEnableBit));
      CHECK(bit::Read(local_sc.PLL1CON, SystemController::kPllConnectBit));
      CHECK(kMultiplier == kExpectedMultiplier[i]);
      CHECK(kDivider == kExpectedDivider);
    }
  }

  SECTION("Set and Get Peripheral Clock Divider")
  {
    using Peripherals = SystemController::Peripherals;

    const uint8_t kPeripheralDividers[] = { 4, 1, 2, 8 };
    constexpr uint8_t kDividerBitWidth  = 2;
    volatile uint32_t * local_pclksel0  = &(local_sc.PCLKSEL0);
    volatile uint32_t * local_pclksel1  = &(local_sc.PCLKSEL1);

    constexpr auto kSystemFrequency = 48_MHz;
    system_controller.SetSystemClockFrequency(kSystemFrequency);

    // Test for peripherals in PCLKSEL0
    for (uint8_t i = 0; i < kPclkSel1StartIndex; i++)
    {
      for (uint8_t divider_select = 0; divider_select < 4; divider_select++)
      {
        // PCLKSEL0 has two CAN peripherals. For these two peripherals, when the
        // divider select is 0b11, the divider value used should be 6.
        const bool kIsCanPeripheral =
            (kPeripherals[i].device_id == Peripherals::kCan1.device_id ||
             kPeripherals[i].device_id == Peripherals::kCan2.device_id);
        uint8_t divider = kPeripheralDividers[divider_select];
        if (kIsCanPeripheral && (divider_select == 0b11))
        {
          divider = 6;
        }
        system_controller.SetPeripheralClockDivider(kPeripherals[i], divider);

        const uint32_t kDividerSelect = bit::Extract(
            *local_pclksel0, kPeripherals[i].device_id * 2, kDividerBitWidth);
        CHECK(kDividerSelect == divider_select);
        CHECK(system_controller.GetPeripheralClockDivider(kPeripherals[i]) ==
              divider);
        CHECK(system_controller.GetPeripheralFrequency(kPeripherals[i]) ==
              kSystemFrequency / divider);
      }
    }
    // Test for peripherals in PCLKSEL1
    for (uint8_t i = kPclkSel1StartIndex; i < std::size(kPeripherals); i++)
    {
      for (uint8_t divider_select = 0; divider_select < 4; divider_select++)
      {
        const uint8_t kDivider = kPeripheralDividers[divider_select];
        system_controller.SetPeripheralClockDivider(kPeripherals[i], kDivider);

        const uint32_t kDividerSelect = bit::Extract(
            *local_pclksel1, kPeripherals[i].device_id * 2, kDividerBitWidth);
        CHECK(kDividerSelect == divider_select);
        CHECK(system_controller.GetPeripheralClockDivider(kPeripherals[i]) ==
              kDivider);
        CHECK(system_controller.GetPeripheralFrequency(kPeripherals[i]) ==
              kSystemFrequency / kDivider);
      }
    }
  }

  SECTION("Peripheral Power Control")
  {
    volatile uint32_t * pconp_register = &(local_sc.PCONP);
    // set all peripherals to be initially off
    *pconp_register = 0;
    for (uint8_t i = 0; i < std::size(kPeripherals); i++)
    {
      CHECK(system_controller.IsPeripheralPoweredUp(kPeripherals[i]) == false);

      system_controller.PowerUpPeripheral(kPeripherals[i]);
      CHECK(bit::Read(*pconp_register, kPeripherals[i].device_id) == true);
      CHECK(system_controller.IsPeripheralPoweredUp(kPeripherals[i]) == true);

      system_controller.PowerDownPeripheral(kPeripherals[i]);
      CHECK(bit::Read(*pconp_register, kPeripherals[i].device_id) == false);
      CHECK(system_controller.IsPeripheralPoweredUp(kPeripherals[i]) == false);
    }

    SystemController::system_controller = LPC_SC;
  }
}
}  // namespace sjsu::lpc17xx
