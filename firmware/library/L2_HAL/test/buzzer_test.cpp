#include "L2_HAL/buzzer.hpp"
#include "L1_Drivers/pwm.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Buzzer);

TEST_CASE("Testing buzzer", "[buzzer]")
{
  // Create mock for PWM class
  Mock<PwmInterface> mock_pwm_pin;

  Fake(Method(mock_pwm_pin, Initialize), Method(mock_pwm_pin, SetDutyCycle),
       Method(mock_pwm_pin, SetFrequency), Method(mock_pwm_pin, GetDutyCycle),
       Method(mock_pwm_pin, GetFrequency));

  PwmInterface & pwm = mock_pwm_pin.get();

  // Instantiate buzzer test objects
  Buzzer test1(&pwm);

  SECTION("Check Initialize")
  {
    constexpr uint32_t kFrequency = 500;
    constexpr float kVolume       = 0.0f;
    test1.Initialize();
    Verify(Method(mock_pwm_pin, Initialize).Using(500),
           Method(mock_pwm_pin, SetDutyCycle).Using(kVolume)),
        Method(mock_pwm_pin, SetFrequency).Using(kFrequency);
  }

  SECTION("Check Beep")
  {
    constexpr uint32_t kFrequency = 0;
    constexpr float kVolume       = 0.0f;
    test1.Beep(kFrequency, kVolume);

    float vol_error = kVolume - test1.GetVolume();
    CHECK((-0.1f <= vol_error && vol_error <= 0.1f));

    uint32_t freq_error = kFrequency - test1.GetFrequency();
    CHECK(freq_error == 0);
  }

  SECTION("Check Stop")
  {
    constexpr float kVolume = 0.0f;
    test1.Stop();
    float vol_error = kVolume - test1.GetVolume();
    CHECK((-0.1f <= vol_error && vol_error <= 0.1f));
  }
}
