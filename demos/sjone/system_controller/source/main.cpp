#include "platforms/utility/startup.hpp"
#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc17xx/system_controller.hpp"
#include "peripherals/lpc17xx/uart.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting LPC176x/5x SystemController example...");

  constexpr units::frequency::hertz_t kInputFrequency =
      sjsu::lpc17xx::SystemController::kIrcFrequency;

  auto & system_controller = sjsu::SystemController::GetPlatformController();
  // By default, the CPU clock divider is set to 6 to get a 48 MHz CPU clock.
  auto & clock_configuration = system_controller.GetClockConfiguration<
      sjsu::lpc17xx::SystemController::ClockConfiguration_t>();
  // Changing CPU clock divider to get a 96 MHz CPU clock.
  clock_configuration.cpu.divider = 3;

  sjsu::InitializePlatform();

  using sjsu::lpc17xx::LPC_SC_TypeDef;
  // Reading the multiplier and pre-divider values locked into the PLL0STAT
  // register to calculate the current running CPU clock speed.
  const uint32_t kMultiplier = sjsu::bit::Extract(
      sjsu::lpc17xx::LPC_SC->PLL0STAT,
      sjsu::lpc17xx::SystemController::Pll0::StatusRegister::kMultiplierMask);
  const uint32_t kPreDivider = sjsu::bit::Extract(
      sjsu::lpc17xx::LPC_SC->PLL0STAT,
      sjsu::lpc17xx::SystemController::Pll0::StatusRegister::kPreDividerMask);
  const uint32_t kCpuDivider = sjsu::bit::Extract(
      sjsu::lpc17xx::LPC_SC->CCLKCFG,
      sjsu::lpc17xx::SystemController::CpuClockRegister::kDividerMask);
  const units::frequency::hertz_t kCpuClockRate =
      ((2 * (kMultiplier + 1) * kInputFrequency) / (kPreDivider + 1)) /
      (kCpuDivider + 1);

  sjsu::LogInfo("CPU Clock Frequency: %lu", kCpuClockRate.to<uint32_t>());

  // Configure the CLKOUT pin (P1.27) to output the CPU clock
  //
  // NOTE: P1.27 may not be available for the 80-pin package option of the MCU.
  constexpr uint8_t kClockOutFunction = 0b01;
  sjsu::lpc17xx::Pin & clock_pin = sjsu::lpc17xx::GetPin<1, 27>();
  clock_pin.settings.function = kClockOutFunction;
  clock_pin.settings.Floating();
  clock_pin.settings.open_drain = false;
  clock_pin.Initialize();

  // Clockout Configuration register (CLKOUTCFG) bit masks,
  constexpr uint8_t kClockOutEnableBit = 8;
  constexpr auto kClockOutSelectMask   = sjsu::bit::MaskFromRange(0, 3);
  constexpr auto kClockOutDividerMask  = sjsu::bit::MaskFromRange(4, 7);

  /// The select code to output CPU clock.
  constexpr uint8_t kClockOutSelect = 0b000;
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG =
      sjsu::bit::Set(sjsu::lpc17xx::LPC_SC->CLKOUTCFG, kClockOutEnableBit);
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG = sjsu::bit::Insert(
      sjsu::lpc17xx::LPC_SC->CLKOUTCFG, kClockOutSelect, kClockOutSelectMask);

  // Setting CLKOUT divider to 16, the resulting frequency outputted by CLKOUT
  // should be 6 MHz.
  constexpr uint8_t kClockOutDivider = 15;
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG   = sjsu::bit::Insert(
      sjsu::lpc17xx::LPC_SC->CLKOUTCFG, kClockOutDivider, kClockOutDividerMask);

  while (1)
  {
    continue;
  }

  return 0;
}
