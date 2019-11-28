#include <cmath>

#include "config.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Pwm);

TEST_CASE("Testing lpc40xx PWM instantiation", "[lpc40xx-pwm]")
{
  // Creating local instances of register structures
  LPC_PWM_TypeDef local_pwm;
  // Setting local register structures to all zeros
  memset(&local_pwm, 0, sizeof(local_pwm));

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kPeriperhalClockFrequency = 12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kPeriperhalClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);

  // Creating mock of Pin class
  Mock<sjsu::Pin> mock_pwm_pin;
  // Make sure mock Pin doesn't call real SetPinFunction() method
  Fake(Method(mock_pwm_pin, SetPinFunction));

  // Creating mock peripheral configuration
  Pwm::Peripheral_t mock_peripheral = {
    .registers = &local_pwm,
    .id        = sjsu::lpc40xx::SystemController::Peripherals::kPwm0,
  };

  // Creating mock channel configuration
  Pwm::Channel_t mock_channel = {
    .peripheral      = mock_peripheral,
    .pin             = mock_pwm_pin.get(),
    .channel         = 5,
    .pin_function_code = 0b101,
  };

  Pwm test_pwm(mock_channel, mock_system_controller.get());

  SECTION("Initialization values")
  {
    // Setup
    constexpr uint32_t kCounterEnable = 0;
    constexpr uint8_t kCounterReset   = 1;
    constexpr uint8_t kPwmEnable      = 3;

    // Exercise
    test_pwm.Initialize(2_kHz);

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kPwm0
                            .device_id == id.device_id;
               }));

    // Verify: PWM Count Control Register should be all zeros
    CHECK(0 == local_pwm.CTCR);
    // Verify: Check that Match Control Register reset for PWM0 has been set
    CHECK(1 == bit::Read(local_pwm.MCR, 1));
    // Verify: that the appropriate flags in the timer register have been set
    CHECK(1 == bit::Read(local_pwm.TCR, kCounterEnable));
    CHECK(0 == bit::Read(local_pwm.TCR, kCounterReset));
    CHECK(1 == bit::Read(local_pwm.TCR, kPwmEnable));
    // Verify: verify that the pwm control enable has been set
    CHECK(1 == bit::Extract(local_pwm.PCR, mock_channel.channel + 8));
    Verify(Method(mock_pwm_pin, SetPinFunction)
               .Using(mock_channel.pin_function_code))
        .Once();
    // Verify: that the default frequency was set.
    CHECK(2000_Hz == test_pwm.GetFrequency());
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
    CHECK(fabs(kDutyCycle - test_pwm.GetDutyCycle()) < 0.001f);
    CHECK(bit::Read(local_pwm.LER, mock_channel.channel));
  }

  SECTION("Setting and Getting Frequency")
  {
    // Setup
    const auto kFrequency  = 2000_Hz;
    const float kDutyCycle = 0.5f;
    // Setup: Expected values
    const auto kExpectedDivider = kPeriperhalClockFrequency / kFrequency;
    const auto kExpectedMR0     = kExpectedDivider.to<uint32_t>();
    const auto kExpectedMR5 = static_cast<uint32_t>(kExpectedMR0 * kDutyCycle);
    // Setup: Old condition of the PWM peripheral to be changed in the
    // "Exercise" phase of the test.
    local_pwm.MR0 = 100;
    local_pwm.MR5 = static_cast<uint32_t>(local_pwm.MR0 * kDutyCycle);

    // Exercise
    test_pwm.SetDutyCycle(kDutyCycle);
    test_pwm.SetFrequency(kFrequency);

    // Verify
    CHECK(kExpectedMR0 == local_pwm.MR0);
    CHECK(kExpectedMR5 == local_pwm.MR5);
    CHECK(test_pwm.GetFrequency() == 2000_Hz);
  }
}
}  // namespace sjsu::lpc40xx
