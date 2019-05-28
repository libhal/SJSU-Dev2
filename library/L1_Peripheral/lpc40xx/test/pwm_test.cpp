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
  LPC_SC_TypeDef local_sc;

  // Setting local register structures to all zeros
  memset(&local_pwm, 0, sizeof(local_pwm));
  memset(&local_sc, 0, sizeof(local_sc));

  // Set system controller pointer to local register
  sjsu::lpc40xx::SystemController::system_controller = &local_sc;

  // Creating mock of Pin class
  Mock<sjsu::Pin> mock_pwm_pin;
  // Make sure mock Pin doesn't call real SetPinFunction() method
  Fake(Method(mock_pwm_pin, SetPinFunction));

  // Creating mock peripheral configuration
  Pwm::Peripheral_t mock_peripheral = {
    .registers       = &local_pwm,
    .power_on_id     = sjsu::lpc40xx::SystemController::Peripherals::kPwm0,
    .pin_function_id = 0b101,
  };

  // Creating mock channel configuration
  Pwm::Channel_t mock_channel = {
    .peripheral = mock_peripheral,
    .pin        = mock_pwm_pin.get(),
    .channel    = 1,
  };

  Pwm test_pwm(mock_channel);

  constexpr uint8_t kResetMr0                  = (1 << 1);
  constexpr uint8_t kCounterEnable             = (1 << 0);
  constexpr uint8_t kTimerMode                 = (0b11 << 0);
  constexpr uint8_t kPwmEnable                 = (1 << 3);
  constexpr uint32_t kChannelEnable            = 1;
  constexpr uint32_t kChannelEnableBitPosition = 10;

  SECTION("Initialization values")
  {
    test_pwm.Initialize();

    sjsu::lpc40xx::SystemController system_controller;
    CHECK(system_controller.IsPeripheralPoweredUp(
        sjsu::lpc40xx::SystemController::Peripherals::kPwm0));
    CHECK((local_pwm.MCR & 0b11) == (kResetMr0 & 0b11));
    CHECK((local_pwm.TCR & 0b1111) == ((kCounterEnable | kPwmEnable) & 0b1111));
    CHECK((local_pwm.CTCR & 0b11) == (~kTimerMode & 0b11));
    CHECK(((local_pwm.PCR >> kChannelEnableBitPosition) & 0b11'1111) ==
          kChannelEnable);

    Verify(Method(mock_pwm_pin, SetPinFunction)
               .Using(mock_channel.peripheral.pin_function_id))
        .Once();

    CHECK(Pwm::kDefaultFrequency == test_pwm.GetFrequency());
    test_pwm.Initialize(5000);
    CHECK(5000 == test_pwm.GetFrequency());
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
    test_pwm.SetFrequency(2000);
    CHECK(config::kSystemClockRate / local_pwm.MR0 == 2000);
    CHECK(test_pwm.GetFrequency() == 2000);
  }

  SECTION("Set Duty Cycle")
  {
    test_pwm.SetFrequency(1000);
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
    test_pwm.SetFrequency(1000);
    test_pwm.SetDutyCycle(duty_cycle);
    float error = duty_cycle - test_pwm.GetDutyCycle();
    CHECK((-0.01f <= error && error <= 0.01f) == true);
  }
  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx
