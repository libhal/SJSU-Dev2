#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc17xx/uart.hpp"
#include "utility/log.hpp"

int main()
{
  LOG_INFO("Starting LPC176x/5x SystemController example...");

  constexpr units::frequency::hertz_t kInputFrequency =
      sjsu::lpc17xx::SystemController::kDefaultIRCFrequency;
  constexpr units::frequency::hertz_t kDesiredFrequency = 96_MHz;
  sjsu::lpc17xx::SystemController controller;
  controller.SetSystemClockFrequency(kDesiredFrequency);
  controller.SetPeripheralClockDivider(
      sjsu::lpc17xx::SystemController::Peripherals::kUart0, 2);
  sjsu::SystemController::SetPlatformController(&controller);

  // re-configuring uart0 after changing cpu speed
  sjsu::lpc17xx::Uart uart0(sjsu::lpc17xx::UartPort::kUart0);
  uart0.SetBaudRate(config::kBaudRate);

  using sjsu::lpc17xx::LPC_SC_TypeDef;
  // Reading the multiplier and pre-divider values locked into the PLL0STAT
  // register to calculate the current running CPU clock speed.
  const uint32_t kMultiplier =
      sjsu::bit::Extract(sjsu::lpc17xx::LPC_SC->PLL0STAT,
                         sjsu::lpc17xx::SystemController::MainPll::kMultiplier);
  const uint32_t kPreDivider =
      sjsu::bit::Extract(sjsu::lpc17xx::LPC_SC->PLL0STAT,
                         sjsu::lpc17xx::SystemController::MainPll::kPreDivider);
  const uint32_t kCpuDivider =
      sjsu::bit::Extract(sjsu::lpc17xx::LPC_SC->CCLKCFG,
                         sjsu::lpc17xx::SystemController::CpuClock::kDivider);
  const units::frequency::hertz_t kClockFrequency =
      ((2 * (kMultiplier + 1) * kInputFrequency) / (kPreDivider + 1)) /
      (kCpuDivider + 1);

  LOG_INFO("CPU Clock Frequency: %" PRIu32, kClockFrequency.to<uint32_t>());

  // configure the CLKOUT pin, P1.27, to output system clock
  sjsu::lpc17xx::Pin clock_pin(1, 27);
  clock_pin.SetPinFunction(0b01);
  clock_pin.SetFloating();
  clock_pin.SetAsOpenDrain(false);

  constexpr uint8_t kClockOutEnableBit = 8;
  constexpr sjsu::bit::Mask kClockOutSelectMask =
      sjsu::bit::CreateMaskFromRange(0, 3);
  constexpr sjsu::bit::Mask kClockOutDividerMask =
      sjsu::bit::CreateMaskFromRange(4, 7);
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG =
      sjsu::bit::Set(sjsu::lpc17xx::LPC_SC->CLKOUTCFG, kClockOutEnableBit);
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG = sjsu::bit::Insert(
      sjsu::lpc17xx::LPC_SC->CLKOUTCFG, 0b000, kClockOutSelectMask);
  // setting CLKOUT divider to 16, the resulting frequency outputted by CLKOUT
  // should be 6 MHz.
  sjsu::lpc17xx::LPC_SC->CLKOUTCFG = sjsu::bit::Insert(
      sjsu::lpc17xx::LPC_SC->CLKOUTCFG, 0xF, kClockOutDividerMask);

  while (1)
  {
    continue;
  }

  return 0;
}
