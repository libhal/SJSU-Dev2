//  Usage:
//      // Two different methods of creating the Pwm object
//      Pwm p2_0 = Pwm::CreatePwm<1>();
//      Pwm p2_0(1);
//      p2_0.Initialize(500);
//      p2_0.SetDutyCycle(0.75);
//      p2_0.SetFrequeny(1'000);
#pragma once

#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/pwm.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Pwm final : public sjsu::Pwm
{
 public:
  struct OutputControl  // NOLINT
  {
    constexpr static bit::Mask kEnableDoubleEdge =
        bit::CreateMaskFromRange(0, 6);
    constexpr static bit::Mask kEnableOutput = bit::CreateMaskFromRange(8, 14);
  };

  struct MatchControl  // NOLINT
  {
    constexpr static bit::Mask kPwm0Reset = bit::CreateMaskFromRange(1);
  };

  struct Timer  // NOLINT
  {
    constexpr static bit::Mask kCounterEnable = bit::CreateMaskFromRange(0);
    constexpr static bit::Mask kCounterReset  = bit::CreateMaskFromRange(1);
    constexpr static bit::Mask kPwmEnable     = bit::CreateMaskFromRange(3);
    constexpr static bit::Mask kMasterDisable = bit::CreateMaskFromRange(4);
  };

  struct CountControl  // NOLINT
  {
    constexpr static bit::Mask kMode       = bit::CreateMaskFromRange(0, 1);
    constexpr static bit::Mask kCountInput = bit::CreateMaskFromRange(2, 3);
  };

  struct Peripheral_t
  {
    LPC_PWM_TypeDef * registers;
    sjsu::SystemController::PeripheralID power_on_id;
  };

  struct Channel_t
  {
    const Peripheral_t & peripheral;
    const sjsu::Pin & pin;
    uint8_t channel : 3;
    uint8_t pin_function_id : 3;
  };

  struct Channel  // NOLINT
  {
   public:
    inline static const Peripheral_t kPwm1PeripheralCommon = {
      .registers   = LPC_PWM1,
      .power_on_id = sjsu::lpc40xx::SystemController::Peripherals::kPwm1,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin0 =
        sjsu::lpc40xx::Pin::CreatePin<2, 0>();
    inline static const Channel_t kPwm0 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin0,
      .channel         = 1,
      .pin_function_id = 0b001,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin1 =
        sjsu::lpc40xx::Pin::CreatePin<2, 1>();
    inline static const Channel_t kPwm1 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin1,
      .channel         = 2,
      .pin_function_id = 0b001,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin2 =
        sjsu::lpc40xx::Pin::CreatePin<2, 2>();
    inline static const Channel_t kPwm2 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin2,
      .channel         = 3,
      .pin_function_id = 0b001,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin3 =
        sjsu::lpc40xx::Pin::CreatePin<2, 3>();
    inline static const Channel_t kPwm3 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin3,
      .channel         = 4,
      .pin_function_id = 0b001,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin4 =
        sjsu::lpc40xx::Pin::CreatePin<2, 4>();
    inline static const Channel_t kPwm4 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin4,
      .channel         = 5,
      .pin_function_id = 0b001,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin5 =
        sjsu::lpc40xx::Pin::CreatePin<2, 5>();
    inline static const Channel_t kPwm5 = {
      .peripheral      = kPwm1PeripheralCommon,
      .pin             = kPwmPin5,
      .channel         = 6,
      .pin_function_id = 0b001,
    };
  };

  static constexpr sjsu::lpc40xx::SystemController kLpc40xxSystemController =
      sjsu::lpc40xx::SystemController();

  explicit constexpr Pwm(const Channel_t & channel,
                         const sjsu::SystemController & system_controller =
                             kLpc40xxSystemController)
      : channel_(channel), system_controller_(system_controller)
  {
  }

  /// @param frequency_hz - Pulse width modulation frequency
  Status Initialize(uint32_t frequency_hz = kDefaultFrequency) const override
  {
    SJ2_ASSERT_FATAL(1 <= channel_.channel && channel_.channel <= 6,
                     "Channel must be between 1 and 6 on LPC40xx platforms.");

    system_controller_.PowerUpPeripheral(channel_.peripheral.power_on_id);
    // Set prescalar to 1 so the input frequency to the PWM peripheral is equal
    // to the peripheral clock frequency.
    //
    // PR = Prescale register = defines the maximum value for the prescaler
    channel_.peripheral.registers->PR = 0;
    // PC = Prescale counter. Set to 0 to cause the timer counter to increment
    // after each peripheral clock tick.
    channel_.peripheral.registers->PC = 0;
    // Mode 0x0 means increment time counter (TC) after the peripheral clock
    // has cycled the amount inside of the prescale.
    channel_.peripheral.registers->CTCR = bit::Insert(
        channel_.peripheral.registers->CTCR, 0, CountControl::kMode);
    // 0x0 for this register says to use the TC for input counts.
    channel_.peripheral.registers->CTCR = bit::Insert(
        channel_.peripheral.registers->CTCR, 0, CountControl::kCountInput);
    // Match register 0 is used to generate the desired frequency. If the time
    // counter TC is equal to MR0
    const uint32_t kPeripheralFrequency =
        system_controller_.GetPeripheralFrequency(
            channel_.peripheral.power_on_id);
    channel_.peripheral.registers->MR0 = kPeripheralFrequency / frequency_hz;
    // Sets match register 0 to reset when TC and Match 0 match each other,
    // meaning that the PWM pulse will cycle continuously.
    channel_.peripheral.registers->MCR = bit::Set(
        channel_.peripheral.registers->MCR, MatchControl::kPwm0Reset.position);
    // Enables PWM TC and PC for counting and enables PWM mode
    EnablePwm();
    // Enables PWM[channel] output
    channel_.peripheral.registers->PCR =
        bit::Set(channel_.peripheral.registers->PCR,
                 OutputControl::kEnableOutput.position + channel_.channel);

    channel_.pin.SetPinFunction(channel_.pin_function_id);

    return Status::kSuccess;
  }

  void SetDutyCycle(float duty_cycle) const override
  {
    SJ2_ASSERT_FATAL(0.0f <= duty_cycle && duty_cycle <= 1.0f,
                     "duty_cycle of Duty Cycle provided is out of bounds.");
    GetMatchRegisters()[channel_.channel] = CalculateDutyCycle(duty_cycle);
    channel_.peripheral.registers->LER |= (1 << channel_.channel);
  }

  float GetDutyCycle() const override
  {
    return (static_cast<float>(GetMatchRegisters()[channel_.channel]) /
            static_cast<float>(GetMatchRegisters()[0]));
  }

  void SetFrequency(uint32_t frequency_hz) const override
  {
    SJ2_ASSERT_FATAL(frequency_hz != 0, "Pwm Frequency cannot be zero Hz.");
    // Disables PWM mode; this will reset all counters to 0
    // And allow us to update MR0
    float previous_duty_cycle = GetDutyCycle();
    EnablePwm(false);
    const uint32_t kPeripheralFrequency =
        system_controller_.GetPeripheralFrequency(
            channel_.peripheral.power_on_id);
    channel_.peripheral.registers->MR0 = kPeripheralFrequency / frequency_hz;
    SetDutyCycle(previous_duty_cycle);
    EnablePwm();
  }

  uint32_t GetFrequency() const
  {
    uint32_t match_register0 = GetMatchRegisters()[0];
    uint32_t result          = 0;
    if (match_register0 != 0)
    {
      const uint32_t kPeripheralFrequency =
          system_controller_.GetPeripheralFrequency(
              channel_.peripheral.power_on_id);
      result = kPeripheralFrequency / match_register0;
    }
    return result;
  }

  void EnablePwm(bool enable = true) const
  {
    volatile uint32_t * pwm_timer = &channel_.peripheral.registers->TCR;
    if (enable)
    {
      // Reset the Timer Counter
      *pwm_timer = bit::Set(*pwm_timer, Timer::kCounterReset.position);
      // Clear reset and allow timer to count
      *pwm_timer = bit::Clear(*pwm_timer, Timer::kCounterReset.position);
      // Enable PWM output
      *pwm_timer = bit::Set(*pwm_timer, Timer::kPwmEnable.position);
      // Enable counting
      *pwm_timer = bit::Set(*pwm_timer, Timer::kCounterEnable.position);
    }
    else
    {
      // Disable PWM output
      *pwm_timer = bit::Clear(*pwm_timer, Timer::kPwmEnable.position);
    }
  }

  volatile uint32_t * GetMatchRegisters() const
  {
    return &channel_.peripheral.registers->MR0;
  }

  uint32_t CalculateDutyCycle(float percent) const
  {
    float pwm_period = static_cast<float>(GetMatchRegisters()[0]);
    return static_cast<uint32_t>(percent * pwm_period);
  }

 private:
  const Channel_t & channel_;
  const sjsu::SystemController & system_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
