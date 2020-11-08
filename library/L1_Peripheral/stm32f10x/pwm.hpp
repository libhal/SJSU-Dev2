#pragma once

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "utility/log.hpp"

namespace sjsu::stm32f10x
{
/// An implementation of the sjsu::Pwm peripheral for the stm32f10x family of
/// microcontrollers.
class Pwm final : public sjsu::Pwm
{
 public:
  // We use pins spread out across Timers 1 - 5
  struct PwmPin_t
  {
    // Reference to the pin
    sjsu::Pin & pin;

    // Holds the address to the corresponding Timer peripheral
    TIM_TypeDef * registers;

    // Holds the channel (1 - 4) which corresponds to the pin
    uint8_t channel;

    // Peripheral ID for the timer
    sjsu::SystemController::ResourceID id;
  };

  struct Channel_t
  {
   private:
    // Note: Pins on the same timer must use the same frequency
    // Timer 1
    inline static sjsu::stm32f10x::Pin pa8  = sjsu::stm32f10x::Pin('A', 8);
    inline static sjsu::stm32f10x::Pin pa9  = sjsu::stm32f10x::Pin('A', 9);
    inline static sjsu::stm32f10x::Pin pa10 = sjsu::stm32f10x::Pin('A', 10);
    inline static sjsu::stm32f10x::Pin pa11 = sjsu::stm32f10x::Pin('A', 11);

    // Timer 2
    inline static sjsu::stm32f10x::Pin pa0 = sjsu::stm32f10x::Pin('A', 0);
    inline static sjsu::stm32f10x::Pin pa1 = sjsu::stm32f10x::Pin('A', 1);
    inline static sjsu::stm32f10x::Pin pa2 = sjsu::stm32f10x::Pin('A', 2);
    inline static sjsu::stm32f10x::Pin pa3 = sjsu::stm32f10x::Pin('A', 3);

    // Timer 3
    inline static sjsu::stm32f10x::Pin pa6 = sjsu::stm32f10x::Pin('A', 6);
    inline static sjsu::stm32f10x::Pin pa7 = sjsu::stm32f10x::Pin('A', 7);
    inline static sjsu::stm32f10x::Pin pb0 = sjsu::stm32f10x::Pin('B', 0);
    inline static sjsu::stm32f10x::Pin pb1 = sjsu::stm32f10x::Pin('B', 1);

    // Timer 4
    inline static sjsu::stm32f10x::Pin pb6 = sjsu::stm32f10x::Pin('B', 6);
    inline static sjsu::stm32f10x::Pin pb7 = sjsu::stm32f10x::Pin('B', 7);
    inline static sjsu::stm32f10x::Pin pb8 = sjsu::stm32f10x::Pin('B', 8);
    inline static sjsu::stm32f10x::Pin pb9 = sjsu::stm32f10x::Pin('B', 9);

   public:
    inline static const PwmPin_t kA8 = {
      .pin       = pa8,
      .registers = TIM1,
      .channel   = 1,
      .id        = SystemController::Peripherals::kTimer1
    };

    inline static const PwmPin_t kA9 = {
      .pin       = pa9,
      .registers = TIM1,
      .channel   = 2,
      .id        = SystemController::Peripherals::kTimer1
    };

    inline static const PwmPin_t kA10 = {
      .pin       = pa10,
      .registers = TIM1,
      .channel   = 3,
      .id        = SystemController::Peripherals::kTimer1
    };

    inline static const PwmPin_t kA11 = {
      .pin       = pa11,
      .registers = TIM1,
      .channel   = 4,
      .id        = SystemController::Peripherals::kTimer1
    };

    inline static const PwmPin_t kA0 = {
      .pin       = pa0,
      .registers = TIM2,
      .channel   = 1,
      .id        = SystemController::Peripherals::kTimer2
    };

    inline static const PwmPin_t kA1 = {
      .pin       = pa1,
      .registers = TIM2,
      .channel   = 2,
      .id        = SystemController::Peripherals::kTimer2
    };

    inline static const PwmPin_t kA2 = {
      .pin       = pa2,
      .registers = TIM2,
      .channel   = 3,
      .id        = SystemController::Peripherals::kTimer2
    };

    inline static const PwmPin_t kA3 = {
      .pin       = pa3,
      .registers = TIM2,
      .channel   = 4,
      .id        = SystemController::Peripherals::kTimer2
    };

    inline static const PwmPin_t kA6 = {
      .pin       = pa6,
      .registers = TIM3,
      .channel   = 1,
      .id        = SystemController::Peripherals::kTimer3
    };

    inline static const PwmPin_t kA7 = {
      .pin       = pa7,
      .registers = TIM3,
      .channel   = 2,
      .id        = SystemController::Peripherals::kTimer3
    };

    inline static const PwmPin_t kB0 = {
      .pin       = pb0,
      .registers = TIM3,
      .channel   = 3,
      .id        = SystemController::Peripherals::kTimer3
    };

    inline static const PwmPin_t kB1 = {
      .pin       = pb1,
      .registers = TIM3,
      .channel   = 4,
      .id        = SystemController::Peripherals::kTimer3
    };

    inline static const PwmPin_t kB6 = {
      .pin       = pb6,
      .registers = TIM4,
      .channel   = 1,
      .id        = SystemController::Peripherals::kTimer4
    };

    inline static const PwmPin_t kB7 = {
      .pin       = pb7,
      .registers = TIM4,
      .channel   = 2,
      .id        = SystemController::Peripherals::kTimer4
    };

    inline static const PwmPin_t kB8 = {
      .pin       = pb8,
      .registers = TIM4,
      .channel   = 3,
      .id        = SystemController::Peripherals::kTimer4
    };

    inline static const PwmPin_t kB9 = {
      .pin       = pb9,
      .registers = TIM4,
      .channel   = 4,
      .id        = SystemController::Peripherals::kTimer4
    };
  };  // Timer

  /// Constructor for a STM32F10x PWM channel.
  ///
  /// @param timer - Reference to a const timer description for this
  ///        instance of the PWM driver.
  explicit constexpr Pwm(const PwmPin_t & timer) : pwm_pin_(timer) {}

  void ConfigureFrequency(units::frequency::hertz_t frequency) override
  {
    static constexpr auto kUpdateGeneration = bit::MaskFromRange(0);

    auto & system                   = SystemController::GetPlatformController();
    const auto kPeripheralFrequency = system.GetClockRate(pwm_pin_.id);

    auto period      = 1 / frequency;
    uint32_t product = kPeripheralFrequency * period;

    pwm_pin_.registers->PSC = GetPrescalarValue(product);
    pwm_pin_.registers->ARR =
      static_cast<uint16_t>(product / (pwm_pin_.registers->PSC + 1));

    bit::Register(&pwm_pin_.registers->EGR).Set(kUpdateGeneration).Save();
  }

  void SetDutyCycle(float duty_cycle) override
  {
    // Clamp the duty cycle to make sure it's in the right range
    const float kClampedDutyCycle = std::clamp(duty_cycle, 0.0f, 1.0f);

    switch (pwm_pin_.channel)
    {
    case 1:
      pwm_pin_.registers->CCR1 =
        static_cast<uint16_t>(kClampedDutyCycle * pwm_pin_.registers->ARR);
      break;

    case 2:
      pwm_pin_.registers->CCR2 =
        static_cast<uint16_t>(kClampedDutyCycle * pwm_pin_.registers->ARR);
      break;

    case 3:
      pwm_pin_.registers->CCR3 =
        static_cast<uint16_t>(kClampedDutyCycle * pwm_pin_.registers->ARR);
      break;

    case 4:
      pwm_pin_.registers->CCR4 =
        static_cast<uint16_t>(kClampedDutyCycle * pwm_pin_.registers->ARR);
      break;
    }
  }

  float GetDutyCycle() override
  {
    float duty_cycle;

    switch (pwm_pin_.channel)
    {
    case 1:
      duty_cycle =
        static_cast<float>(pwm_pin_.registers->CCR1 / pwm_pin_.registers->ARR);
      break;
    case 2:
      duty_cycle =
        static_cast<float>(pwm_pin_.registers->CCR2 / pwm_pin_.registers->ARR);
      break;
    case 3:
      duty_cycle =
        static_cast<float>(pwm_pin_.registers->CCR3 / pwm_pin_.registers->ARR);
      break;
    case 4:
      duty_cycle =
        static_cast<float>(pwm_pin_.registers->CCR4 / pwm_pin_.registers->ARR);
      break;
    }

    return duty_cycle;
  }

  void ModuleEnable(bool enable = true) override
  {
    // Preload enable
    static constexpr auto kPreloadEnableLow  = bit::MaskFromRange(3);
    static constexpr auto kPreloadEnableHigh = bit::MaskFromRange(11);

    // Output channel enable
    static constexpr auto kChannel1OutputEnable = bit::MaskFromRange(0);
    static constexpr auto kChannel2OutputEnable = bit::MaskFromRange(4);
    static constexpr auto kChannel3OutputEnable = bit::MaskFromRange(8);
    static constexpr auto kChannel4OutputEnable = bit::MaskFromRange(12);

    // Main Output Enable (Timer 1 only)
    static constexpr auto kMainOutputEnable = bit::MaskFromRange(15);

    // Timer enable
    static constexpr auto kTimerEnable = bit::MaskFromRange(0);

    if (enable)
    {
      switch (pwm_pin_.channel)
      {
      case 1:
        bit::Register(&pwm_pin_.registers->CCMR1)
          .Set(kPreloadEnableLow)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Set(kChannel1OutputEnable)
          .Save();
        break;
      case 2:
        bit::Register(&pwm_pin_.registers->CCMR1)
          .Set(kPreloadEnableHigh)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Set(kChannel2OutputEnable)
          .Save();
        break;
      case 3:
        bit::Register(&pwm_pin_.registers->CCMR2)
          .Set(kPreloadEnableLow)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Set(kChannel3OutputEnable)
          .Save();
        break;
      case 4:
        bit::Register(&pwm_pin_.registers->CCMR2)
          .Set(kPreloadEnableHigh)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Set(kChannel4OutputEnable)
          .Save();
        break;
      }

      if (pwm_pin_.id == SystemController::Peripherals::kTimer1)
      {
        bit::Register(&pwm_pin_.registers->BDTR)
          .Set(kMainOutputEnable)
          .Save();
      }

      bit::Register(&pwm_pin_.registers->CR1)
        .Set(kTimerEnable)
        .Save();
    }
    else
    {
      // Preload enable
      switch (pwm_pin_.channel)
      {
      case 1:
        bit::Register(&pwm_pin_.registers->CCMR1)
          .Clear(kPreloadEnableLow)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Clear(kChannel1OutputEnable)
          .Save();
        break;
      case 2:
        bit::Register(&pwm_pin_.registers->CCMR1)
          .Clear(kPreloadEnableHigh)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Clear(kChannel2OutputEnable)
          .Save();
        break;
      case 3:
        bit::Register(&pwm_pin_.registers->CCMR2)
          .Clear(kPreloadEnableLow)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Clear(kChannel3OutputEnable)
          .Save();
        break;
      case 4:
        bit::Register(&pwm_pin_.registers->CCMR2)
          .Clear(kPreloadEnableHigh)
          .Save();
        bit::Register(&pwm_pin_.registers->CCER)
          .Clear(kChannel4OutputEnable)
          .Save();
        break;
      }

      if (pwm_pin_.id == SystemController::Peripherals::kTimer1)
      {
        bit::Register(&pwm_pin_.registers->BDTR)
          .Clear(kMainOutputEnable)
          .Save();
      }

      bit::Register(&pwm_pin_.registers->CR1)
        .Clear(kTimerEnable)
        .Save();
    }
  }

  void ModuleInitialize() override
  {
    SystemController::GetPlatformController().PowerUpPeripheral(pwm_pin_.id);

    // Set pin to alternative function to support PWM
    pwm_pin_.pin.ConfigureFunction(1);

    // We only use PWM mode 1 in this driver. The high and low are because
    // different channels have different locations where the mode is set
    static constexpr auto kPwmModeLow  = bit::MaskFromRange(4,  6);
    static constexpr auto kPwmModeHigh = bit::MaskFromRange(12, 14);
    static constexpr uint8_t kPwmMode1 = 0b110;

    switch (pwm_pin_.channel)
    {
    case 1:
      bit::Register(&pwm_pin_.registers->CCMR1)
        .Insert(kPwmMode1, kPwmModeLow)
        .Save();
      break;
    case 2:
      bit::Register(&pwm_pin_.registers->CCMR1)
        .Insert(kPwmMode1, kPwmModeHigh)
        .Save();
      break;
    case 3:
      bit::Register(&pwm_pin_.registers->CCMR2)
        .Insert(kPwmMode1, kPwmModeLow)
        .Save();
      break;
    case 4:
      bit::Register(&pwm_pin_.registers->CCMR2)
        .Insert(kPwmMode1, kPwmModeHigh)
        .Save();
      break;
    }
  }

 private:
  uint16_t GetPrescalarValue(uint32_t product)
  {
    uint16_t prescalar  = 0;
    uint16_t k_max16_bits = ~0;

    uint32_t arr;

    // To select a prescalar and divider value, we use this equation:
    //      (PSC+1)(ARR+1) = (EventTime)(ClkFreq)
    // product is the value (EventTime)(ClkFreq)
    //
    // We want ARR to be a larger value for a more precise duty cycle
    // so we increment prescalar until ARR in the value (PSC+1)(ARR+1)
    // is less than 2^16
    do
    {
      prescalar++;
      arr = product / prescalar;
    } while (arr > k_max16_bits);

    return --prescalar;
  }

  const PwmPin_t & pwm_pin_;
};
}  // namespace sjsu::stm32f10x
