#include "peripherals/stm32f10x/pwm.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
TEST_CASE("Testing stm32f10x PWM")
{
  static constexpr units::frequency::hertz_t kDummyClockRate = 8_MHz;
  Mock<SystemController> mock_controller;
  Mock<sjsu::Pin> mock_pwm_pin;
  Fake(Method(mock_pwm_pin, Pin::ModuleInitialize));

  Fake(Method(mock_controller, SystemController::PowerUpPeripheral));
  When(Method(mock_controller, SystemController::GetClockRate))
      .AlwaysReturn(kDummyClockRate);
  SystemController::SetPlatformController(&mock_controller.get());

  TIM_TypeDef local_timer;

  testing::ClearStructure(&local_timer);

  Pwm::Channel_t mock_pwm = { .pin       = mock_pwm_pin.get(),
                              .registers = &local_timer,
                              .channel   = 1,
                              .id = SystemController::Peripherals::kTimer1 };

  Pwm test_subject(mock_pwm);

  SECTION("Initialize")
  {
    // Setup
    units::frequency::hertz_t frequency;

    SUBCASE("Frequency 1")
    {
      frequency = 1_Hz;
    }

    SUBCASE("Frequency 2")
    {
      frequency = 100_Hz;
    }

    SUBCASE("Frequency 3")
    {
      frequency = 1_kHz;
    }

    SUBCASE("Frequency 4")
    {
      frequency = 50_Hz;
    }

    SUBCASE("Frequency 5")
    {
      frequency = 100_kHz;
    }

    SUBCASE("Frequency 6")
    {
      frequency = 1_MHz;
    }

    SUBCASE("Frequency 7")
    {
      frequency = 5_MHz;
    }

    INFO("Failure for frequency " << frequency);

    // Calculate prescalar and divider
    auto & system                   = SystemController::GetPlatformController();
    const auto kPeripheralFrequency = system.GetClockRate(mock_pwm.id);
    auto period                     = 1 / frequency;

    uint32_t product      = period * kPeripheralFrequency;
    uint16_t k_max16_bits = ~0;

    uint16_t prescalar = 0;
    uint32_t divider;

    do
    {
      prescalar++;
      divider = product / prescalar;
    } while (divider > k_max16_bits);

    prescalar--;

    // Exercise
    test_subject.settings.frequency = frequency;
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_controller, PowerUpPeripheral).Using(mock_pwm.id))
        .Once();
    CHECK(prescalar == local_timer.PSC);
    CHECK(divider == local_timer.ARR);
    Verify(Method(mock_pwm_pin, Pin::ModuleInitialize)).Once();
  }

  SECTION("SetDutyCycle()")
  {
    float duty_cycle;
    mock_pwm.registers->ARR = 1400;

    SUBCASE("-10%")
    {
      duty_cycle = -0.1f;
    }

    SUBCASE("1%")
    {
      duty_cycle = 0.01f;
    }

    SUBCASE("20%")
    {
      duty_cycle = 0.2f;
    }

    SUBCASE("15%")
    {
      duty_cycle = 0.5f;
    }

    SUBCASE("87%")
    {
      duty_cycle = 0.87f;
    }

    SUBCASE("100%")
    {
      duty_cycle = 1.0f;
    }

    SUBCASE("150%")
    {
      duty_cycle = 1.5f;
    }

    float k_clamped_duty_cycle = std::clamp(duty_cycle, 0.0f, 1.0f);
    uint16_t result =
        static_cast<uint16_t>(k_clamped_duty_cycle * mock_pwm.registers->ARR);

    // Exercise
    test_subject.SetDutyCycle(duty_cycle);

    // Verify
    CHECK(result == mock_pwm.registers->CCR1);
  }
}
}  // namespace sjsu::stm32f10x
