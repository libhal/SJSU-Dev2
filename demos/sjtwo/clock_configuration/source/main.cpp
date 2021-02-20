#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "platforms/utility/startup.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "utility/math/bit.hpp"
#include "utility/log.hpp"

int main()
{
  auto & system = sjsu::SystemController::GetPlatformController();
  auto & config = system.GetClockConfiguration<
      sjsu::lpc40xx::SystemController::ClockConfiguration>();

  // Set the function of the pin to output the clock signal
  // This is NOT needed to change the clock rate, but is used to inspect and
  // verify that the clock rate change did work.
  sjsu::lpc40xx::Pin & clock_pin      = sjsu::lpc40xx::GetPin<1, 25>();
  constexpr uint8_t kClockOutFunction = 0b101;

  clock_pin.settings.function   = kClockOutFunction;
  clock_pin.settings.open_drain = false;
  clock_pin.settings.Floating();

  clock_pin.Initialize();

  clock_pin.EnableHysteresis(false);
  clock_pin.SetAsActiveLow(false);
  clock_pin.EnableFastMode(false);
  sjsu::LogInfo("Connect a probe to pin P1[25] to measure the clock rate");

  constexpr sjsu::bit::Mask kClockSelect = sjsu::bit::MaskFromRange(0, 3);
  constexpr sjsu::bit::Mask kDivide      = sjsu::bit::MaskFromRange(4, 7);
  constexpr sjsu::bit::Mask kEnable      = sjsu::bit::MaskFromRange(8);

  // The following register configurations do the following:
  //
  // 1. Enable              [8]   = 1
  // 2. Divide output by 16 [4:7] = 0xF
  // 3. CPU as clock output [0:3] = 0
  //
  // NOTE: Dividing the output by 16 brings the output signal frequency low
  // enough for most digital analyzers and oscilloscopes to sample the signal
  // without aliasing.
  uint32_t clock_out_reg = sjsu::bit::Value(0)
                               .Set(kEnable)
                               .Insert(0xF, kDivide)
                               .Insert(0, kClockSelect);
  sjsu::lpc40xx::LPC_SC->CLKOUTCFG = clock_out_reg;

  while (true)
  {
    using sjsu::lpc40xx::SystemController;
    units::frequency::hertz_t speed;

    // Make sure PLL0 is enabled
    config.pll[0].enabled = true;
    // IRC (12_MHz) * 10 => 120 MHz (maximum cpu clock rate)
    config.pll[0].multiply = 10;

    // Set clock source to PLL0
    config.cpu.clock   = SystemController::CpuClockSelect::kPll0;
    config.cpu.divider = 1;

    // Initialize the platform with the current configuration settings.
    sjsu::InitializePlatform();

    speed = system.GetClockRate(SystemController::Peripherals::kCpu);
    sjsu::LogInfo("CPU clock rate from PLL0 is %" PRIu32 " Hz",
                  speed.to<uint32_t>());
    sjsu::LogInfo("Waiting 5s before switching the CPU frequency\n");
    sjsu::Delay(5000ms);

    // Change function back to GPIO and pull the signal low to show a transition
    // between clock signals.
    clock_pin.settings.function = 0;
    clock_pin.settings.PullDown();
    clock_pin.Initialize();

    sjsu::Delay(1000ms);

    // Change the signal back to the CLKOUT signal.
    clock_pin.settings.function = kClockOutFunction;
    clock_pin.settings.Floating();
    clock_pin.Initialize();

    config.cpu.clock = SystemController::CpuClockSelect::kSystemClock;
    sjsu::InitializePlatform();
    speed = system.GetClockRate(SystemController::Peripherals::kCpu);
    sjsu::LogInfo("CPU clock rate from System Clock %" PRIu32 " Hz",
                  speed.to<uint32_t>());
    sjsu::LogInfo("Waiting 5s before switching the CPU frequency\n");
    sjsu::Delay(5000ms);
  }

  return 0;
}
