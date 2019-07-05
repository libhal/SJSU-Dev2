#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L2_HAL/actuators/servo/servo.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Servo);

TEST_CASE("Testing Servo", "[Servo]")
{
  Mock<Pwm> mock_pwm;

  Fake(Method(mock_pwm, Initialize));
  Fake(Method(mock_pwm, SetFrequency));
  Fake(Method(mock_pwm, SetDutyCycle));

  // Inject test_gpio into button object
  Servo test_servo(mock_pwm.get());

  SECTION("Initialize")
  {
    test_servo.Initialize();
    Verify(Method(mock_pwm, Initialize).Using(Servo::kDefaultFrequency)).Once();
  }

  SECTION("Write Microseconds")
  {
    constexpr uint32_t kTestFrequency = 100;
    constexpr float kTestMaxPulseLength =
        (1 / static_cast<float>(kTestFrequency)) * 1'000'000;
    constexpr float kTestPulseLength = 200;
    constexpr float kTestDutyCycle   = kTestPulseLength / kTestMaxPulseLength;

    test_servo.SetFrequency(kTestFrequency);
    test_servo.SetPulseWidthInMicroseconds(kTestPulseLength);

    Verify(Method(mock_pwm, SetFrequency).Using(kTestFrequency)).Once();
    Verify(Method(mock_pwm, SetDutyCycle).Using(kTestDutyCycle)).Once();
  }

  SECTION("Write Angle")
  {
    constexpr uint32_t kTestFrequency = 400;
    constexpr float kTestMaxPulseLength =
        (1 / static_cast<float>(kTestFrequency)) * 1'000'000;
    constexpr float kTestAngle          = 90;
    constexpr float kTestMinAngle       = 20;
    constexpr float kTestMaxAngle       = 140;
    constexpr float kTestPulseLengthMin = 1000;
    constexpr float kTestPulseLengthMax = 2300;
    constexpr float kTestPulseLength    = sjsu::Map(kTestAngle,
                                                 kTestMinAngle,
                                                 kTestMaxAngle,
                                                 kTestPulseLengthMin,
                                                 kTestPulseLengthMax);
    constexpr float kTestDutyCycle = kTestPulseLength / kTestMaxPulseLength;

    test_servo.SetFrequency(kTestFrequency);
    test_servo.SetAngleBounds(kTestMinAngle, kTestMaxAngle);
    test_servo.SetPulseBounds(kTestPulseLengthMin, kTestPulseLengthMax);
    test_servo.SetAngle(kTestAngle);

    Verify(Method(mock_pwm, SetDutyCycle).Using(kTestDutyCycle)).Once();
  }
}
}  // namespace sjsu
