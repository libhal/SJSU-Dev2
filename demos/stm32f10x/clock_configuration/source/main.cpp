#include <array>
#include <cstdio>

#include "L0_Platform/startup.hpp"
#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "utility/time.hpp"
#include "utility/log.hpp"
#include "L2_HAL/switches/button.hpp"

int main()
{
  sjsu::LogInfo("Starting Clock Configuration Application...");

  using sjsu::stm32f10x::SystemController;
  auto & config =
      sjsu::SystemController::GetPlatformController()
          .GetClockConfiguration<SystemController::ClockConfiguration>();

  // Change to true to measure the settings when using the internal oscillator
  constexpr bool kUseInternalOscillator = false;

  // Configuration for maximum frequency using internal oscillator
  if constexpr (kUseInternalOscillator)
  {
    // Set the source of the PLL's oscillator to 4MHz.
    // See page 93 in RM0008 to see that the internal high speed oscillator,
    // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
    config.pll.source = SystemController::PllSource::kHighSpeedInternal;
    // Enable PLL to increase the frequency of the system
    config.pll.enable = true;
    // Multiply the 4MHz * 16 => 64 MHz
    config.pll.multiply = SystemController::PllMultiply::kMultiplyBy16;
    // Set the system clock to the PLL
    config.system_clock = SystemController::SystemClockSelect::kPll;
    // APB1's maximum frequency is 36 MHz, so we divide 64 MHz / 2 => 32 MHz.
    config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
    // Keep APB1's clock undivided as it can handle up to 72 MHz.
    config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
    // Maximum frequency for ADC is 14 MHz, thus we divide 64 MHz / 6 to get
    // 10.66 MHz which is below 14 MHz.
    config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;
  }
  else
  {
    // Set the speed of the high speed external oscillator.
    // NOTE: Change this if its different for your board.
    config.high_speed_external = 8_MHz;
    // Set the source of the PLL's oscillator to 4MHz.
    // See page 93 in RM0008 to see that the internal high speed oscillator,
    // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
    config.pll.source = SystemController::PllSource::kHighSpeedExternal;
    // Enable PLL to increase the frequency of the system
    config.pll.enable = true;
    // Multiply the 8MHz * 9 => 72 MHz
    // NOTE: 72 MHz is the maximum. There have been successful attempts to
    // overclock up to 128 MHz, but these maybe unstable.
    config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
    // Set the system clock to the PLL
    config.system_clock = SystemController::SystemClockSelect::kPll;
    // APB1's maximum frequency is 36 MHz, so we divide 72 MHz / 2 => 36 MHz.
    config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
    // Keep APB1's clock undivided as it can handle up to 72 MHz.
    config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
    // Maximum frequency for ADC is 12 MHz, thus we divide 72 MHz / 6 to get
    // 12 MHz.
    config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;
  }

  // Its best to run the whole Platform Initialization sequence over again as
  // many of the clocks will have changed and will need to be updated in order
  // for proper operation of the system.
  sjsu::InitializePlatform();

  // Set pin to its alternative mode which is MCU CLOCK OUTPUT (MCO) mode.
  sjsu::LogInfo("Clock output on PA[8]");
  sjsu::stm32f10x::Gpio mco_pin('A', 8);
  mco_pin.GetPin().ConfigureFunction(1);

  struct McoOptions_t
  {
    const char * name;
    uint8_t code;
  };

  std::array<McoOptions_t, 3> mco_clocks = {
    McoOptions_t{ "High Speed Internal Clock", 0b101 },
    McoOptions_t{ "High Speed External Clock", 0b110 },
    McoOptions_t{ "PLL Clock Divided by 2", 0b111 },
  };

  // Bitmask of the Clock out bit position in the RCC_CFGR register.
  // See page. 101 of RM0008.
  static constexpr auto kMCO = sjsu::bit::MaskFromRange(24, 26);

  // Grabbing a pointer to the clock configuration register to reduce the
  // verbosity of the code.
  volatile uint32_t * clock_config = &sjsu::stm32f10x::RCC->CFGR;

  while (true)
  {
    for (const auto & settings : mco_clocks)
    {
      *clock_config = sjsu::bit::Insert(*clock_config, 0, kMCO);
      // Logging is enough of a delay to see differences on an oscilloscope or a
      // digital analyzer.
      sjsu::LogInfo("Current Clock Output: %s", settings.name);
      *clock_config = sjsu::bit::Insert(*clock_config, settings.code, kMCO);
      sjsu::Delay(5s);
    }
    sjsu::LogInfo("Restarting Clock Output");
  }

  return 0;
}
