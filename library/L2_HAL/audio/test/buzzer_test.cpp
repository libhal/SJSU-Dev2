#include "L2_HAL/audio/buzzer.hpp"

#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Buzzer);

TEST_CASE("Testing buzzer")
{
  // Create mock for PWM class
  Mock<Pwm> mock_pwm_pin;

  Fake(Method(mock_pwm_pin, ModuleInitialize),
       Method(mock_pwm_pin, ModuleEnable),
       Method(mock_pwm_pin, SetDutyCycle),
       Method(mock_pwm_pin, ConfigureFrequency));

  Pwm & pwm = mock_pwm_pin.get();

  // Instantiate buzzer test objects
  Buzzer test_subject(pwm);

  SECTION("Initialize()")
  {
    // Exercise
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_pwm_pin, ModuleInitialize)).Once();
  }

  SECTION("Enable()")
  {
    // Exercise
    pwm.SetStateToInitialized();
    test_subject.ModuleEnable();

    // Verify
    Verify(Method(mock_pwm_pin, ModuleEnable).Using(true),
           Method(mock_pwm_pin, SetDutyCycle).Using(0.0f));
  }

  SECTION("Enable(false)")
  {
    // Exercise
    pwm.SetStateToEnabled();
    test_subject.ModuleEnable(false);

    // Verify
    Verify(Method(mock_pwm_pin, ModuleEnable).Using(false));
    Verify(Method(mock_pwm_pin, SetDutyCycle).Using(0.0f)).Never();
  }

  SECTION("Beep()")
  {
    // Setup
    constexpr auto kFrequency = 500_Hz;
    constexpr float kVolume   = 0.5f;

    // Exercise
    pwm.SetStateToEnabled();
    test_subject.Beep(kFrequency, kVolume);

    // Verify
    // NOTE: Since the PWM is at its loudest at 50% duty cycle, the maximum PWM
    // is divided by 2.
    Verify(Method(mock_pwm_pin, ModuleEnable).Using(false),
           Method(mock_pwm_pin, ConfigureFrequency).Using(kFrequency),
           Method(mock_pwm_pin, ModuleEnable).Using(true),
           Method(mock_pwm_pin, SetDutyCycle).Using(kVolume / 2));
  }

  SECTION("Stop()")
  {
    // Exercise
    test_subject.Stop();

    // Verify
    Verify(Method(mock_pwm_pin, SetDutyCycle).Using(0.0f));
  }
}
}  // namespace sjsu
