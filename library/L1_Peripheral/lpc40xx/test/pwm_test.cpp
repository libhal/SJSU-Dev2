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
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);

  // Creating mock of Pin class
  Mock<sjsu::Pin> mock_pwm_pin;
  // Make sure mock Pin doesn't call real SetPinFunction() method
  Fake(Method(mock_pwm_pin, SetPinFunction));

  // Creating mock peripheral configuration
  Pwm::Peripheral_t mock_peripheral = {
    .registers   = &local_pwm,
    .power_on_id = sjsu::lpc40xx::SystemController::Peripherals::kPwm0,
  };

  // Creating mock channel configuration
  Pwm::Channel_t mock_channel = {
    .peripheral      = mock_peripheral,
    .pin             = mock_pwm_pin.get(),
    .channel         = 1,
    .pin_function_id = 0b101,
  };

  Pwm test_pwm(mock_channel, mock_system_controller.get());

  SECTION("Initialization values")
  {
    test_pwm.Initialize(1_kHz);
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kPwm0
                            .device_id == id.device_id;
               }));

    // PWM Count Control Register should be all zeros
    CHECK(0 == local_pwm.CTCR);
    // Check that Match Control Register reset for PWM0 has been set
    CHECK(0b1 == bit::Read(local_pwm.MCR, 1));
    // Timer Control Register
    constexpr uint32_t kCounterEnable = 0;
    constexpr uint8_t kCounterReset   = 1;
    constexpr uint8_t kPwmEnable      = 3;
    CHECK(0b1 == bit::Read(local_pwm.TCR, kCounterEnable));
    CHECK(0b0 == bit::Read(local_pwm.TCR, kCounterReset));
    CHECK(0b1 == bit::Read(local_pwm.TCR, kPwmEnable));

    CHECK(0b1 == bit::Extract(local_pwm.PCR, mock_channel.channel + 8));

    Verify(Method(mock_pwm_pin, SetPinFunction)
               .Using(mock_channel.pin_function_id))
        .Once();

    test_pwm.Initialize(5_kHz);
    CHECK(5000_Hz == test_pwm.GetFrequency());
  }

  SECTION("Match Register 0")
  {
    CHECK(test_pwm.GetMatchRegisters()[0] == local_pwm.MR0);
  }

  SECTION("Calculate the Duty Cycle")
  {
    test_pwm.SetDutyCycle(.27f);
    float calculated_duty_cycle_precent =
        static_cast<float>(test_pwm.CalculateDutyCycle(.27f));
    float match_register_0 =
        static_cast<float>(test_pwm.GetMatchRegisters()[0]);
    float error = calculated_duty_cycle_precent - (.27f * match_register_0);
    CHECK((-.1f <= error && error <= .1f) == true);
  }

  SECTION("Setting and Getting Frequency")
  {
    test_pwm.SetDutyCycle(0.5f);
    test_pwm.SetFrequency(2000_Hz);
    CHECK(kDummySystemControllerClockFrequency / local_pwm.MR0 == 2000_Hz);
    CHECK(test_pwm.GetFrequency() == 2000_Hz);
  }

  SECTION("Set Duty Cycle")
  {
    test_pwm.SetFrequency(1000_Hz);
    test_pwm.SetDutyCycle(.50f);
    float mr0   = static_cast<float>(local_pwm.MR0);
    float mr1   = static_cast<float>(local_pwm.MR1);
    float error = (mr1 / mr0) - 0.5f;
    CHECK(local_pwm.MR0 == test_pwm.GetMatchRegisters()[0]);
    CHECK(local_pwm.MR1 == test_pwm.CalculateDutyCycle(.50f));
    CHECK((-0.1f <= error && error <= 0.1f) == true);
    CHECK(local_pwm.LER == 0b10);
  }

  SECTION("Get Duty Cycle")
  {
    float duty_cycle = 0.10f;
    test_pwm.SetFrequency(1000_Hz);
    test_pwm.SetDutyCycle(duty_cycle);
    float error = duty_cycle - test_pwm.GetDutyCycle();
    CHECK((-0.01f <= error && error <= 0.01f) == true);
  }
  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx
