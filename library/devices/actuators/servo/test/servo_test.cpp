#include "devices/actuators/servo/servo.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing Servo")
{
  Mock<Pwm> mock_pwm;

  Fake(Method(mock_pwm, ModuleInitialize));
  Fake(Method(mock_pwm, SetDutyCycle));

  // Inject test_gpio into button object
  Servo test_servo(mock_pwm.get());

  SECTION("ModuleInitialize")
  {
    test_servo.ModuleInitialize();
    Verify(Method(mock_pwm, ModuleInitialize)).Once();
    CHECK(mock_pwm.get().CurrentSettings().frequency == 50_Hz);
  }

  SECTION("Write Microseconds")
  {
    constexpr auto kTestFrequency      = 100_Hz;
    constexpr float kTestMaxPulseWidth = (1_MHz / kTestFrequency).to<float>();
    constexpr auto kTestPulseWidth     = 200us;
    constexpr float kTestDutyCycle =
        static_cast<float>(kTestPulseWidth.count() / kTestMaxPulseWidth);

    test_servo.settings.frequency = (kTestFrequency);
    test_servo.Initialize();
    test_servo.SetPulseWidthInMicroseconds(kTestPulseWidth);

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

    test_servo.settings.frequency = kTestFrequency;
    test_servo.settings.min_angle = kTestMinAngle;
    test_servo.settings.max_angle = kTestMaxAngle;
    test_servo.settings.min_pulse = kTestPulseWidthMin;
    test_servo.settings.max_pulse = kTestPulseWidthMax;
    test_servo.Initialize();
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
