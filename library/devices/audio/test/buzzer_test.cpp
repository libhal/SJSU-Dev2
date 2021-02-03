#include "devices/audio/buzzer.hpp"

#include "peripherals/lpc40xx/pwm.hpp"
#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing buzzer")
{
  // Create mock for PWM class
  Mock<Pwm> mock_pwm_pin;

  Fake(Method(mock_pwm_pin, ModuleInitialize),
       Method(mock_pwm_pin, SetDutyCycle));

  Pwm & pwm = mock_pwm_pin.get();

  // Instantiate buzzer test objects
  Buzzer test_subject(pwm);

  SECTION("Initialize()")
  {
    // Exercise
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_pwm_pin, ModuleInitialize)).Once();
    Verify(Method(mock_pwm_pin, SetDutyCycle).Using(0.0f));
  }

  SECTION("Beep()")
  {
    // Setup
    constexpr auto kFrequency = 500_Hz;
    constexpr float kVolume   = 0.5f;

    // Exercise
    test_subject.Beep(kFrequency, kVolume);

    // Verify
    // NOTE: Since the PWM is at its loudest at 50% duty cycle, the maximum PWM
    // is divided by 2.
    Verify(Method(mock_pwm_pin, ModuleInitialize)).Once();
    Verify(Method(mock_pwm_pin, SetDutyCycle).Using(kVolume / 2));
    CHECK(mock_pwm_pin.get().CurrentSettings().frequency == kFrequency);
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
