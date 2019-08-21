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
    constexpr auto kTestFrequency      = 100_Hz;
    constexpr float kTestMaxPulseWidth = (1_MHz / kTestFrequency).to<float>();
    constexpr auto kTestPulseWidth     = 200us;
    constexpr float kTestDutyCycle =
        static_cast<float>(kTestPulseWidth.count() / kTestMaxPulseWidth);

    test_servo.SetFrequency(kTestFrequency);
    test_servo.SetPulseWidthInMicroseconds(kTestPulseWidth);

    Verify(Method(mock_pwm, SetFrequency).Using(kTestFrequency)).Once();
    Verify(Method(mock_pwm, SetDutyCycle).Using(kTestDutyCycle)).Once();
  }

  SECTION("Write Angle")
  {
    constexpr auto kTestFrequency     = 400_Hz;
    constexpr auto kTestAngle         = 90_deg;
    constexpr auto kTestMinAngle      = 20_deg;
    constexpr auto kTestMaxAngle      = 140_deg;
    constexpr auto kTestPulseWidthMin = 1000us;
    constexpr auto kTestPulseWidthMax = 2300us;

    constexpr std::chrono::microseconds kTestMaxPulseWidth =
        std::chrono::microseconds((1_MHz / kTestFrequency).to<uint32_t>());

    constexpr float kTestPulseWidth =
        sjsu::Map(kTestAngle.to<float>(),
                  kTestMinAngle.to<float>(),
                  kTestMaxAngle.to<float>(),
                  static_cast<float>(kTestPulseWidthMin.count()),
                  static_cast<float>(kTestPulseWidthMax.count()));
    constexpr float kExpectedDutyCycle =
        kTestPulseWidth / kTestMaxPulseWidth.count();

    test_servo.SetFrequency(kTestFrequency);
    test_servo.SetAngleBounds(kTestMinAngle, kTestMaxAngle);
    test_servo.SetPulseBounds(kTestPulseWidthMin, kTestPulseWidthMax);
    test_servo.SetAngle(kTestAngle);

    Verify(
        Method(mock_pwm, SetDutyCycle).Matching([](float duty_cycle) -> bool {
          float error = duty_cycle - kExpectedDutyCycle;
          return (-0.01f <= error && error <= 0.01f);
        }))
        .Once();
  }
}
}  // namespace sjsu
