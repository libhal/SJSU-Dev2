#include "peripherals/lpc40xx/pwm.hpp"

#include <cmath>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/lpc40xx/pin.hpp"
#include "testing/testing_frameworks.hpp"
#include "config.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx PWM instantiation")
{
  // Creating local instances of register structures
  LPC_PWM_TypeDef local_pwm;
  // Setting local register structures to all zeros
  testing::ClearStructure(&local_pwm);

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kPeriperhalClockFrequency = 12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kPeriperhalClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Creating mock of Pin class
  Mock<sjsu::Pin> mock_pwm_pin;
  // Make sure mock Pin doesn't call real ConfigureFunction() method
  Fake(Method(mock_pwm_pin, Pin::ModuleInitialize));

  // Creating mock peripheral configuration
  Pwm::Peripheral_t mock_peripheral = {
    .registers = &local_pwm,
    .id        = sjsu::lpc40xx::SystemController::Peripherals::kPwm0,
  };

  // Creating mock channel configuration
  Pwm::Channel_t mock_channel = {
    .peripheral        = mock_peripheral,
    .pin               = mock_pwm_pin.get(),
    .channel           = 5,
    .pin_function_code = 0b101,
  };

  Pwm test_pwm(mock_channel);

  SECTION("Initialize()")
  {
    // Setup
    constexpr int kCounterEnable = 0;
    constexpr int kCounterReset  = 1;
    constexpr int kPwmEnable     = 3;

    SECTION("10 kHz")
    {
      test_pwm.settings.frequency = 10_kHz;
    }

    SECTION("20 kHz")
    {
      test_pwm.settings.frequency = 20_kHz;
    }

    SECTION("5 Hz")
    {
      test_pwm.settings.frequency = 5_Hz;
    }

    SECTION("100 kHz")
    {
      test_pwm.settings.frequency = 100_kHz;
    }

    // Setup
    const float kDutyCycle = 0.5f;
    // Setup: Expected values
    const auto kExpectedDivider =
        kPeriperhalClockFrequency / test_pwm.settings.frequency;
    const auto kExpectedMR0 = kExpectedDivider.to<uint32_t>();
    const auto kExpectedMR5 = static_cast<uint32_t>(kExpectedMR0 * kDutyCycle);

    // Setup: Old condition of the PWM peripheral to be changed in the
    // "Exercise" phase of the test.
    local_pwm.MR0 = 100;
    local_pwm.MR5 = static_cast<uint32_t>(local_pwm.MR0 * kDutyCycle);

    // Exercise
    test_pwm.Initialize();

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::ResourceID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kPwm0
                            .device_id == id.device_id;
               }));

    // Verify: PWM Count Control Register should be all zeros
    CHECK(0 == local_pwm.CTCR);
    // Verify: Clear Prescalar
    CHECK(0 == local_pwm.PR);
    // Verify: Clear counter
    CHECK(0 == local_pwm.PC);
    // Verify: Check that Match Control Register reset for PWM0 has been set
    CHECK(1 == bit::Read(local_pwm.MCR, 1));
    // Verify: verify that the pwm control enable has been set
    CHECK(1 == bit::Extract(local_pwm.PCR, mock_channel.channel + 8));
    CHECK(mock_pwm_pin.get().CurrentSettings().function ==
          mock_channel.pin_function_code);

    // Verify: that the appropriate flags in the timer register have been set
    CHECK(1 == bit::Read(local_pwm.TCR, kCounterEnable));
    CHECK(0 == bit::Read(local_pwm.TCR, kCounterReset));
    CHECK(1 == bit::Read(local_pwm.TCR, kPwmEnable));

    // Verify
    CHECK(kExpectedMR0 == local_pwm.MR0);
    CHECK(kExpectedMR5 == local_pwm.MR5);
  }

  SECTION("PowerDown()")
  {
    // Setup
    constexpr int kPwmEnable = 3;

    // Exercise
    test_pwm.PowerDown();

    // Verify
    CHECK(!bit::Read(local_pwm.TCR, kPwmEnable));
  }

  SECTION("Set & Get Duty Cycle")
  {
    // Setup
    local_pwm.MR0           = 100;
    local_pwm.MR5           = 0;
    const auto kDutyCycle   = 0.27f;
    const auto kExpectedMR5 = static_cast<uint32_t>(local_pwm.MR0 * kDutyCycle);

    // Exercise
    test_pwm.SetDutyCycle(kDutyCycle);

    // Verify
    CHECK(kExpectedMR5 == local_pwm.MR5);
    CHECK(test_pwm.GetDutyCycle() ==
          doctest::Approx(kDutyCycle).epsilon(0.001f));
    CHECK(bit::Read(local_pwm.LER, mock_channel.channel));
  }
}
}  // namespace sjsu::lpc40xx
