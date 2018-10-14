#include "config.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L1_Drivers/pwm.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing PWM instantiation", "[pwm]")
{
  using fakeit::Fake;
  using fakeit::Mock;
  LPC_PWM_TypeDef local_pwm;
  LPC_SC_TypeDef local_sc;
  LPC_IOCON_TypeDef local_iocon;
  // TODO(delwin-lei): Remove later with an interface Mock
  memset(&local_pwm, 0, sizeof(local_pwm));
  memset(&local_sc, 0, sizeof(local_sc));
  memset(&local_iocon, 0, sizeof(local_iocon));
  Pwm::pwm1     = &local_pwm;
  Pwm::sc       = &local_sc;
  Pwm::match[0] = &local_pwm.MR0;
  Pwm::match[1] = &local_pwm.MR1;
  Pin::pin_map  = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  Mock<PinInterface> mock_pwm_pin;

  Fake(Method(mock_pwm_pin, SetPinFunction));
  Pwm test2_0(1);

  constexpr uint32_t kDefaultFrequency = 1'000;
  constexpr uint8_t kActivePwm         = (1 << 6);
  constexpr uint8_t kResetMr0          = (1 << 1);
  constexpr uint8_t kCounterEnable     = (1 << 0);
  constexpr uint8_t kTimerMode         = (0b11 << 0);
  constexpr uint8_t kPwmEnable         = (1 << 3);
  constexpr uint32_t kChannelEnable    = (1 << 9);

  test2_0.Initialize();
  SECTION("Initialization values")
  {
    CHECK((local_sc.PCONP & 0b111'1111) == (kActivePwm & 0b111'1111));
    CHECK((local_pwm.MCR & 0b11) == (kResetMr0 & 0b11));
    CHECK((local_pwm.TCR & 0b1111) == ((kCounterEnable | kPwmEnable) & 0b1111));
    CHECK((local_pwm.CTCR & 0b11) == (~kTimerMode & 0b11));
    CHECK((local_pwm.PCR & 0b11'1111'1111) ==
          (kChannelEnable & 0b11'1111'1111));

    CHECK(test2_0.GetFrequency() == kDefaultFrequency);
    test2_0.Initialize(5000);
    CHECK(test2_0.GetFrequency() == 5000);
  }

  SECTION("Match Register 0")
  {
    CHECK(test2_0.GetMatchRegister0() == local_pwm.MR0);
  }

  SECTION("Enabling PWM Channels")
  {
    CHECK(test2_0.PwmOutputEnable(1) == kChannelEnable);
  }

  SECTION("Calculate the Duty Cycle")
  {
    test2_0.SetDutyCycle(.27f);
    float error = static_cast<float>(test2_0.CalculateDutyCycle(.27f)) -
                  (.27f * static_cast<float>(test2_0.GetMatchRegister0()));
    CHECK((-.1f <= error && error <= .1f) == true);
  }

  SECTION("Setting and Getting Frequency")
  {
    test2_0.SetFrequency(2000);
    CHECK(config::kSystemClockRate / local_pwm.MR0 == 2000);
    CHECK(test2_0.GetFrequency() == 2000);
  }

  SECTION("Set Duty Cycle")
  {
    test2_0.SetDutyCycle(.50f);
    float error = (static_cast<float>(local_pwm.MR1) /
                   static_cast<float>(local_pwm.MR0)) -
                  .50f;
    CHECK(local_pwm.MR0 == test2_0.GetMatchRegister0());
    CHECK(local_pwm.MR1 == test2_0.CalculateDutyCycle(.50f));
    CHECK((-0.1f <= error && error <= 0.1f) == true);
    CHECK(local_pwm.LER == 0b10);
  }

  SECTION("Get Duty Cycle")
  {
    float duty_cycle = 0.10f;
    test2_0.SetDutyCycle(duty_cycle);
    float error = duty_cycle - test2_0.GetDutyCycle();
    CHECK((-0.01f <= error && error <= 0.01f) == true);
  }
}
