//  Usage:
//      // Two different methods of creating the Pwm object
//      Pwm p2_0 = Pwm::CreatePwm<1>();
//      Pwm p2_0(1);
//      p2_0.Initialize(500);
//      p2_0.SetDutyCycle(0.75);
//      p2_0.SetFrequency(1'000);
#pragma once

#include <cstdint>
#include <cstdio>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "utility/bit.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// An implementation of the sjsu::Pwm peripheral for the LPC40xx family of
/// microcontrollers.
///
/// @warning - Changing the frequency of 1 PWM channel, changes the frequency
/// for all other PWM channels.
class Pwm final : public sjsu::Pwm
{
 public:
  /// Output Control bit masks.
  struct OutputControl  // NOLINT
  {
    /// Enables/disables double edge PWM for each channel. Each bit in this
    /// field corrissponds to a channel.
    static constexpr auto kEnableDoubleEdge = bit::MaskFromRange(2, 6);

    /// Enables/disables output of PWM channels. Each bit from the start
    /// corrissponds to a PWM channel.
    static constexpr auto kEnableOutput = bit::MaskFromRange(8, 14);
  };

  /// Match register control bit masks.
  struct MatchControl  // NOLINT
  {
    /// If set to a 1, tells the PWM hardare to reset the PWM total count
    /// register to be reset to 0 when it is equal to the match register 0.
    static constexpr auto kPwm0Reset = bit::MaskFromRange(1);
  };

  /// Register controls PWM peripheral wide timer control.
  struct Timer  // NOLINT
  {
    /// When set to a 1, enables the TC (total count) register and begins
    /// counting.
    static constexpr auto kCounterEnable = bit::MaskFromRange(0);

    /// When set to a 1, will reset the total count register.
    static constexpr auto kCounterReset = bit::MaskFromRange(1);

    /// Enables PWM mode. Without setting the match registers cannot operate
    /// with the timer.
    static constexpr auto kPwmEnable = bit::MaskFromRange(3);

    /// Allows PWM0 peripheral to control the PWM1 peripheral. This is generally
    /// set to 0, to allow both of them to work independently.
    static constexpr auto kMasterDisable = bit::MaskFromRange(4);
  };

  /// PWM channel count control register
  struct CountControl  // NOLINT
  {
    /// Controls the counting mode of the PWM peripheral. When set to 0, counts
    /// using the internal prescale counter which is driven by the peripheral
    /// clock. Other modes involve use of an external clock source.
    static constexpr auto kMode = bit::MaskFromRange(0, 1);

    /// Controls the count input source. For this driver, this should be kept to
    /// zero, since we want to utilize the internal peripheral clock source.
    static constexpr auto kCountInput = bit::MaskFromRange(2, 3);
  };

  /// Defines a LPC PWM peripheral definition that contains all of the
  /// information needed to use it.
  struct Peripheral_t
  {
    /// Holds the address to the LPC PWM peripheral.
    LPC_PWM_TypeDef * registers;

    /// The power on id for the above LPC PWM peripheral.
    sjsu::SystemController::ResourceID id;
  };

  /// Defines all information necessary to control a specific PWM channel.
  struct Channel_t
  {
    /// Reference to the Peripheral_t that this channel belongs to.
    const Peripheral_t & peripheral;

    /// Reference to the pwm pin.
    sjsu::Pin & pin;

    /// Reference to the PWM channel number.
    uint8_t channel : 3;

    /// Contains the pin function id, used to select PWM output for the pin.
    uint8_t pin_function_code : 3;
  };

  /// Structure used as a namespace for predefined Channel definitions
  struct Channel  // NOLINT
  {
   private:
    inline static sjsu::lpc40xx::Pin pwm_pin0 = sjsu::lpc40xx::Pin(2, 0);
    inline static sjsu::lpc40xx::Pin pwm_pin1 = sjsu::lpc40xx::Pin(2, 1);
    inline static sjsu::lpc40xx::Pin pwm_pin2 = sjsu::lpc40xx::Pin(2, 2);
    inline static sjsu::lpc40xx::Pin pwm_pin3 = sjsu::lpc40xx::Pin(2, 3);
    inline static sjsu::lpc40xx::Pin pwm_pin4 = sjsu::lpc40xx::Pin(2, 4);
    inline static sjsu::lpc40xx::Pin pwm_pin5 = sjsu::lpc40xx::Pin(2, 5);

   public:
    /// Definition of the PWM 0 peripheral.
    inline static const Peripheral_t kPwm0Peripheral = {
      .registers = LPC_PWM0,
      .id        = sjsu::lpc40xx::SystemController::Peripherals::kPwm1,
    };

    /// Definition of the PWM 1 peripheral.
    inline static const Peripheral_t kPwm1Peripheral = {
      .registers = LPC_PWM1,
      .id        = sjsu::lpc40xx::SystemController::Peripherals::kPwm1,
    };

    /// Definition for channel 0 of PWM peripheral 1.
    inline static const Channel_t kPwm0 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin0,
      .channel           = 1,
      .pin_function_code = 0b001,
    };

    /// Definition for channel 1 of PWM peripheral 1.
    inline static const Channel_t kPwm1 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin1,
      .channel           = 2,
      .pin_function_code = 0b001,
    };

    /// Definition for channel 2 of PWM peripheral 1.
    inline static const Channel_t kPwm2 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin2,
      .channel           = 3,
      .pin_function_code = 0b001,
    };

    /// Definition for channel 3 of PWM peripheral 1.
    inline static const Channel_t kPwm3 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin3,
      .channel           = 4,
      .pin_function_code = 0b001,
    };

    /// Definition for channel 4 of PWM peripheral 1.
    inline static const Channel_t kPwm4 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin4,
      .channel           = 5,
      .pin_function_code = 0b001,
    };

    /// Definition for channel 5 of PWM peripheral 1.
    inline static const Channel_t kPwm5 = {
      .peripheral        = kPwm1Peripheral,
      .pin               = pwm_pin5,
      .channel           = 6,
      .pin_function_code = 0b001,
    };
  };

  /// Constructor for a LPC40xx PWM channel.
  ///
  /// @param channel - Reference to a const channel description for this
  ///        instance of the PWM driver.
  explicit constexpr Pwm(const Channel_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  {
    if (!(1 <= channel_.channel && channel_.channel <= 6))
    {
      throw Exception(std::errc::invalid_argument,
                      "Channel must be between 1 and 6 on LPC40xx platforms.");
    }
    auto & system = sjsu::SystemController::GetPlatformController();
    system.PowerUpPeripheral(channel_.peripheral.id);

    // Get a pointer to the PWM registers
    auto * pwm = channel_.peripheral.registers;

    // Set prescalar to 1 so the input frequency to the PWM peripheral is
    // equal to the peripheral clock frequency.
    //
    // PR = Prescale register = defines the maximum value for the prescaler
    pwm->PR = 0;

    // PC = Prescale counter. Set to 0 to cause the timer counter to increment
    // after each peripheral clock tick.
    pwm->PC = 0;

    // Mode 0x0 means increment time counter (TC) after the peripheral clock
    // has cycled the amount inside of the prescale.
    pwm->CTCR = bit::Insert(pwm->CTCR, 0, CountControl::kMode);

    // 0x0 for this register says to use the TC for input counts.
    pwm->CTCR = bit::Insert(pwm->CTCR, 0, CountControl::kCountInput);

    // Sets match register 0 to reset when TC and Match 0 match each other,
    // meaning that the PWM pulse will cycle continuously.
    pwm->MCR = bit::Set(pwm->MCR, MatchControl::kPwm0Reset);

    // Enables PWM[channel] output
    const uint32_t kOutputEnableStart  = OutputControl::kEnableOutput.position;
    const uint32_t kChannelBitPosition = kOutputEnableStart + channel_.channel;
    pwm->PCR = bit::Set(pwm->PCR, kChannelBitPosition);

    // Set the pin function
    channel_.pin.ConfigureFunction(channel_.pin_function_code);
  }

  void ModuleEnable(bool enable = true) override
  {
    bit::Register pwm_timer(&channel_.peripheral.registers->TCR);

    if (enable)
    {
      // Reset the Timer Counter
      pwm_timer.Set(Timer::kCounterReset).Save();

      // Clear reset and allow timer to count
      pwm_timer.Clear(Timer::kCounterReset).Save();

      // Enable PWM output
      pwm_timer.Set(Timer::kPwmEnable).Save();

      // Enable counting
      pwm_timer.Set(Timer::kCounterEnable).Save();
    }
    else
    {
      // Disable PWM output
      pwm_timer.Clear(Timer::kPwmEnable).Save();
    }
  }

  void ConfigureFrequency(units::frequency::hertz_t frequency_hz) override
  {
    if (frequency_hz <= 0_Hz)
    {
      SJ2_PRINT_VARIABLE(frequency_hz, "%f");
      throw Exception(
          std::errc::invalid_argument,
          "Frequency must be greater than 0 Hz. Frequency will not be "
          "updated.");
    }

    ModuleEnable(false);

    auto & system             = sjsu::SystemController::GetPlatformController();
    auto peripheral_frequency = system.GetClockRate(channel_.peripheral.id);

    // Get the current duty cycle so we can match it to the updated frequency.
    float previous_duty_cycle = GetDutyCycle();

    // In order to avoid PWM glitches, the PWM must be disabled while updating
    // the MR0 register. Doing this will reset all counters to 0 and allow us to
    // update MR0.
    const auto kNewFrequency = peripheral_frequency / frequency_hz;
    GetMatchRegisters(0)     = kNewFrequency.to<uint32_t>();

    // Re-enable PWM which will also reset all of the counters which allow
    // setting the duty cycle of other PWM channels.
    ModuleEnable(true);

    SetDutyCycle(previous_duty_cycle);
  }

  void SetDutyCycle(float duty_cycle) override
  {
    // Clamp duty cycle
    const float kClampedDuty = std::clamp(duty_cycle, 0.0f, 1.0f);

    // Set match register for this channel
    GetMatchRegisters(channel_.channel) = CalculateDutyCycle(kClampedDuty);

    // LER (Load Enable Register) sets the hardware up to load the new duty
    // cycle period in the next reset cycle after MR0 match. Hardware requires
    // this as a means ot prevent itself from glitching and changing pwm
    // instantaniously.
    channel_.peripheral.registers->LER |= (1 << channel_.channel);
  }

  float GetDutyCycle() override
  {
    auto period     = static_cast<float>(GetMatchRegisters(channel_.channel));
    auto max_period = static_cast<float>(GetMatchRegisters(0));
    return period / max_period;
  }

 private:
  /// Helper method to make getting a pointer to the Match Register 0 more
  /// readable in the code.
  ///
  /// @return a pointer to the match 0 register.
  volatile uint32_t & GetMatchRegisters(uint8_t match) const
  {
    switch (match)
    {
      case 0: return channel_.peripheral.registers->MR0;
      case 1: return channel_.peripheral.registers->MR1;
      case 2: return channel_.peripheral.registers->MR2;
      case 3: return channel_.peripheral.registers->MR3;
      case 4: return channel_.peripheral.registers->MR4;
      case 5: return channel_.peripheral.registers->MR5;
      case 6: return channel_.peripheral.registers->MR6;
      default: return channel_.peripheral.registers->MR0;
    }
  }

  /// Converts a percent value from 0.0f to 1.0f to the closest approximate
  /// match register value and returns that value.
  ///
  /// @param duty_cycle_percent - value from 0.0f to 1.0f indicating the duty
  ///        cycle precent.
  /// @return the nearest approximate match register value to achieve the
  ///         supplied duty cycle.
  uint32_t CalculateDutyCycle(float duty_cycle_percent) const
  {
    float pwm_period = static_cast<float>(GetMatchRegisters(0));
    return static_cast<uint32_t>(duty_cycle_percent * pwm_period);
  }

  /// Reference to a const channel description for this instance of the PWM
  /// driver.
  const Channel_t & channel_;
};
}  // namespace lpc40xx
}  // namespace sjsu
