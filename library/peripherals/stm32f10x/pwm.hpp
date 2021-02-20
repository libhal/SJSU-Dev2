#pragma once

#include <utility>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "peripherals/pwm.hpp"
#include "peripherals/stm32f10x/pin.hpp"
#include "peripherals/stm32f10x/system_controller.hpp"
#include "utility/log.hpp"

namespace sjsu::stm32f10x
{
/// An implementation of the sjsu::Pwm peripheral for the stm32f10x family of
/// microcontrollers.
class Pwm final : public sjsu::Pwm
{
 public:
  /// Preload LOW enable
  static constexpr auto kPreloadEnableLow = bit::MaskFromRange(3);
  /// Preload HIGH enable
  static constexpr auto kPreloadEnableHigh = bit::MaskFromRange(11);

  /// Output channel enable 1
  static constexpr auto kChannel1OutputEnable = bit::MaskFromRange(0);
  /// Output channel enable 2
  static constexpr auto kChannel2OutputEnable = bit::MaskFromRange(4);
  /// Output channel enable 3
  static constexpr auto kChannel3OutputEnable = bit::MaskFromRange(8);
  /// Output channel enable 4
  static constexpr auto kChannel4OutputEnable = bit::MaskFromRange(12);

  /// Main Output Enable (Timer 1 only)
  static constexpr auto kMainOutputEnable = bit::MaskFromRange(15);

  /// Timer enable
  static constexpr auto kTimerEnable = bit::MaskFromRange(0);

  /// We use pins spread out across Timers 1 - 5
  struct Channel_t
  {
    /// Reference to the pin
    sjsu::Pin & pin;

    /// Holds the address to the corresponding Timer peripheral
    TIM_TypeDef * registers;

    /// Holds the channel (1 - 4) which corresponds to the pin
    uint8_t channel;

    /// Peripheral ID for the timer
    sjsu::SystemController::ResourceID id;
  };

  /// Constructor for a STM32F10x PWM channel.
  ///
  /// @param timer - Reference to a const timer description for this
  ///        instance of the PWM driver.
  explicit constexpr Pwm(const Channel_t & timer) : channel_(timer) {}

  void ModuleInitialize() override
  {
    SystemController::GetPlatformController().PowerUpPeripheral(channel_.id);

    // We only use PWM mode 1 in this driver. The high and low are because
    // different channels have different locations where the mode is set
    static constexpr auto kPwmModeLow  = bit::MaskFromRange(4, 6);
    static constexpr auto kPwmModeHigh = bit::MaskFromRange(12, 14);
    static constexpr uint8_t kPwmMode1 = 0b110;

    ConfigureFrequency();

    switch (channel_.channel)
    {
      case 1:
        bit::Register(&channel_.registers->CCMR1)
            .Insert(kPwmMode1, kPwmModeLow)
            .Save();
        bit::Register(&channel_.registers->CCMR1).Set(kPreloadEnableLow).Save();
        bit::Register(&channel_.registers->CCER)
            .Set(kChannel1OutputEnable)
            .Save();
        break;
      case 2:
        bit::Register(&channel_.registers->CCMR1)
            .Insert(kPwmMode1, kPwmModeHigh)
            .Save();

        bit::Register(&channel_.registers->CCMR1)
            .Set(kPreloadEnableHigh)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Set(kChannel2OutputEnable)
            .Save();
        break;
      case 3:
        bit::Register(&channel_.registers->CCMR2)
            .Insert(kPwmMode1, kPwmModeLow)
            .Save();
        bit::Register(&channel_.registers->CCMR2).Set(kPreloadEnableLow).Save();
        bit::Register(&channel_.registers->CCER)
            .Set(kChannel3OutputEnable)
            .Save();
        break;
      case 4:
        bit::Register(&channel_.registers->CCMR2)
            .Insert(kPwmMode1, kPwmModeHigh)
            .Save();
        bit::Register(&channel_.registers->CCMR2)
            .Set(kPreloadEnableHigh)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Set(kChannel4OutputEnable)
            .Save();
        break;
    }

    if (channel_.id == SystemController::Peripherals::kTimer1)
    {
      bit::Register(&channel_.registers->BDTR).Set(kMainOutputEnable).Save();
    }

    bit::Register(&channel_.registers->CR1).Set(kTimerEnable).Save();

    // Set pin to alternative function to support PWM
    channel_.pin.settings.function = 1;
    channel_.pin.settings.PullDown();
    channel_.pin.Initialize();
  }

  void ModulePowerDown() override
  {
    switch (channel_.channel)
    {
      case 1:
        bit::Register(&channel_.registers->CCMR1)
            .Clear(kPreloadEnableLow)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Clear(kChannel1OutputEnable)
            .Save();
        break;
      case 2:
        bit::Register(&channel_.registers->CCMR1)
            .Clear(kPreloadEnableHigh)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Clear(kChannel2OutputEnable)
            .Save();
        break;
      case 3:
        bit::Register(&channel_.registers->CCMR2)
            .Clear(kPreloadEnableLow)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Clear(kChannel3OutputEnable)
            .Save();
        break;
      case 4:
        bit::Register(&channel_.registers->CCMR2)
            .Clear(kPreloadEnableHigh)
            .Save();
        bit::Register(&channel_.registers->CCER)
            .Clear(kChannel4OutputEnable)
            .Save();
        break;
    }
  }

  void SetDutyCycle(float duty_cycle) override
  {
    // Clamp the duty cycle to make sure it's in the right range
    const float kClampedDutyCycle = std::clamp(duty_cycle, 0.0f, 1.0f);
    const uint16_t kDutyCycle =
        static_cast<uint16_t>(kClampedDutyCycle * channel_.registers->ARR);

    switch (channel_.channel)
    {
      case 1: channel_.registers->CCR1 = kDutyCycle; break;
      case 2: channel_.registers->CCR2 = kDutyCycle; break;
      case 3: channel_.registers->CCR3 = kDutyCycle; break;
      case 4: channel_.registers->CCR4 = kDutyCycle; break;
    }
  }

  float GetDutyCycle() override
  {
    float duty_cycle;

    switch (channel_.channel)
    {
      case 1:
        duty_cycle = static_cast<float>(channel_.registers->CCR1 /
                                        channel_.registers->ARR);
        break;
      case 2:
        duty_cycle = static_cast<float>(channel_.registers->CCR2 /
                                        channel_.registers->ARR);
        break;
      case 3:
        duty_cycle = static_cast<float>(channel_.registers->CCR3 /
                                        channel_.registers->ARR);
        break;
      case 4:
        duty_cycle = static_cast<float>(channel_.registers->CCR4 /
                                        channel_.registers->ARR);
        break;
    }

    return duty_cycle;
  }

 private:
  void ConfigureFrequency()
  {
    static constexpr auto kUpdateGeneration = bit::MaskFromRange(0);

    auto & system                   = SystemController::GetPlatformController();
    const auto kPeripheralFrequency = system.GetClockRate(channel_.id);

    auto period      = 1 / settings.frequency;
    uint32_t product = kPeripheralFrequency * period;

    channel_.registers->PSC = GetPrescalarValue(product);
    channel_.registers->ARR =
        static_cast<uint16_t>(product / (channel_.registers->PSC + 1));

    bit::Register(&channel_.registers->EGR).Set(kUpdateGeneration).Save();
  }

  uint16_t GetPrescalarValue(uint32_t product)
  {
    uint16_t prescalar    = 0;
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

  const Channel_t & channel_;
};

constexpr auto GetPwmPinPair(int peripheral, int channel)
{
  std::array<std::array<std::pair<int, int>, 4>, 4> pins = {
    std::array<std::pair<int, int>, 4>{
        std::make_pair('A', 8),
        std::make_pair('A', 9),
        std::make_pair('A', 10),
        std::make_pair('A', 11),  // TIMER 1
    },
    std::array<std::pair<int, int>, 4>{
        std::make_pair('A', 0),
        std::make_pair('A', 1),
        std::make_pair('A', 2),
        std::make_pair('A', 3),  // TIMER 2
    },
    std::array<std::pair<int, int>, 4>{
        std::make_pair('A', 6),
        std::make_pair('A', 7),
        std::make_pair('B', 0),
        std::make_pair('B', 1),  // TIMER 3
    },
    std::array<std::pair<int, int>, 4>{
        std::make_pair('B', 6),
        std::make_pair('B', 7),
        std::make_pair('B', 8),
        std::make_pair('B', 9),  // TIMER 4
    },
  };

  return pins[peripheral - 1][channel - 1];
}

template <int peripheral, int channel>
inline Pwm & GetPwm()
{
  static_assert(1 <= peripheral && peripheral <= 4,
                "Peripheral template parameter must be between 1 and 4 as the "
                "stm32f10x uses timers 1 to 4 for PWM generation.");

  static_assert(1 <= channel && channel <= 4,
                "stm31f10x only supports PWM channels from 0 to 4.");

  constexpr auto pair   = GetPwmPinPair(peripheral, channel);
  static auto & pwm_pin = GetPin<pair.first, pair.second>();

  if constexpr (peripheral == 1)
  {
    static const Pwm::Channel_t kChannelInfo = {
      .pin       = pwm_pin,
      .registers = TIM1,
      .channel   = channel,
      .id        = SystemController::Peripherals::kTimer1
    };
    static Pwm pwm(kChannelInfo);
    return pwm;
  }
  else if constexpr (peripheral == 2)
  {
    static const Pwm::Channel_t kChannelInfo = {
      .pin       = pwm_pin,
      .registers = TIM2,
      .channel   = channel,
      .id        = SystemController::Peripherals::kTimer2
    };
    static Pwm pwm(kChannelInfo);
    return pwm;
  }
  else if constexpr (peripheral == 3)
  {
    static const Pwm::Channel_t kChannelInfo = {
      .pin       = pwm_pin,
      .registers = TIM3,
      .channel   = channel,
      .id        = SystemController::Peripherals::kTimer3
    };
    static Pwm pwm(kChannelInfo);
    return pwm;
  }
  else if constexpr (peripheral == 4)
  {
    static const Pwm::Channel_t kChannelInfo = {
      .pin       = pwm_pin,
      .registers = TIM4,
      .channel   = channel,
      .id        = SystemController::Peripherals::kTimer4
    };
    static Pwm pwm(kChannelInfo);
    return pwm;
  }

  return GetPwm<1, 1>();
}

template <int port, int pin>
inline Pwm & GetPwmFromPin()
{
  // Don't worry, this gets optimized out.
  constexpr auto pin_to_peripheral_channel = []() -> std::pair<int, int> {
    for (int peripheral = 1; peripheral <= 4; peripheral++)
    {
      for (int channel = 1; channel <= 4; channel++)
      {
        auto pair = GetPwmPinPair(peripheral, channel);
        if (port == pair.first && pin == pair.second)
        {
          return std::make_pair(peripheral, channel);
        }
      }
    }
    return std::make_pair(-1, -1);
  };

  constexpr std::pair<int, int> combination = pin_to_peripheral_channel();

  if constexpr (combination.first != -1 && combination.second != -1)
  {
    return GetPwm<combination.first, combination.second>();
  }
  else
  {
    static_assert(
        InvalidOption<port, pin>,
        SJ2_ERROR_MESSAGE_DECORATOR("   stm32f10x TIMER1 only supports PWM "
                                    "pins: PA8, PA9, PA10, PA11. \n"
                                    "   stm32f10x TIMER2 only supports PWM "
                                    "pins: PA0, PA1, PA2,  PA3. \n"
                                    "   stm32f10x TIMER3 only supports PWM "
                                    "pins: PA6, PA7, PB0,  PB1. \n"
                                    "   stm32f10x TIMER4 only supports PWM "
                                    "pins: PB6, PB7, PB8,  PB9. \n"
                                    "\n"));
    // NOTE: this is simply here to appease the compiler. If this block is
    // hit, this code will not compile and thus Channel<0>() will never be
    // used in this way.
    return GetPwm<1, 1>();
  }
}
}  // namespace sjsu::stm32f10x
