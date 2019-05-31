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
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Pwm final : public sjsu::Pwm, protected sjsu::lpc40xx::SystemController
{
 public:
  union [[gnu::packed]] OutputControlRegister_t {
    uint32_t data;
    struct
    {
      uint8_t reserved0 : 2;
      // First bit corrisponds to PWM2, final bit corrisponds to PWM6
      uint8_t enable_double_edge : 5;
      uint8_t reserved1 : 2;
      // First bit corrisponds to PWM1, final bit corrisponds to PWM6
      uint8_t enable_output : 6;
    } bits;
  };

  union [[gnu::packed]] MatchControlRegister_t {
    struct [[gnu::packed]] MatchSection_t
    {
      uint8_t interrupt : 1;
      uint8_t reset : 1;
      uint8_t stop : 1;
    };
    uint32_t data;
    struct
    {
      MatchSection_t pwm0;
      MatchSection_t pwm1;
      MatchSection_t pwm2;
      MatchSection_t pwm3;
      MatchSection_t pwm4;
      MatchSection_t pwm5;
      MatchSection_t pwm6;
    } bits;
  };

  union [[gnu::packed]] CountControlRegister_t {
    uint32_t data;
    struct
    {
      uint8_t mode : 2;
      uint8_t count_input_select : 2;
    } bits;
  };

  union [[gnu::packed]] TimerControlRegister_t {
    uint32_t data;
    struct
    {
      uint8_t counter_enable : 1;
      uint8_t counter_reset : 1;
      uint8_t reserved0 : 1;
      uint8_t pwm_enable : 1;
      uint8_t master_disable : 1;  // only used with PWM0
    } bits;
  };

  struct Peripheral_t
  {
    LPC_PWM_TypeDef * registers;
    PeripheralID power_on_id;
    uint8_t pin_function_id : 3;
  };

  struct Channel_t
  {
    const Peripheral_t & peripheral;
    const sjsu::Pin & pin;
    uint8_t channel : 3;
  };

  struct Channel  // NOLINT
  {
   private:
    inline static const Peripheral_t kPwm1PeripheralCommon = {
      .registers       = LPC_PWM1,
      .power_on_id     = Peripherals::kPwm1,
      .pin_function_id = 0b001,
    };

   public:
    inline static const sjsu::lpc40xx::Pin kPwmPin0 =
        sjsu::lpc40xx::Pin::CreatePin<2, 0>();
    inline static const Channel_t kPwm0 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin0,
      .channel    = 1,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin1 =
        sjsu::lpc40xx::Pin::CreatePin<2, 1>();
    inline static const Channel_t kPwm1 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin1,
      .channel    = 2,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin2 =
        sjsu::lpc40xx::Pin::CreatePin<2, 2>();
    inline static const Channel_t kPwm2 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin2,
      .channel    = 3,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin3 =
        sjsu::lpc40xx::Pin::CreatePin<2, 3>();
    inline static const Channel_t kPwm3 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin3,
      .channel    = 4,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin4 =
        sjsu::lpc40xx::Pin::CreatePin<2, 4>();
    inline static const Channel_t kPwm4 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin4,
      .channel    = 5,
    };
    inline static const sjsu::lpc40xx::Pin kPwmPin5 =
        sjsu::lpc40xx::Pin::CreatePin<2, 5>();
    inline static const Channel_t kPwm5 = {
      .peripheral = kPwm1PeripheralCommon,
      .pin        = kPwmPin5,
      .channel    = 6,
    };
  };

  explicit constexpr Pwm(const Channel_t & channel) : channel_(channel) {}

  /// @param frequency_hz - Pulse width modulation frequency
  Status Initialize(uint32_t frequency_hz = kDefaultFrequency) const override
  {
    SJ2_ASSERT_FATAL(1 <= channel_.channel && channel_.channel <= 6,
                     "Channel must be between 1 and 6 on LPC40xx platforms.");

    PowerUpPeripheral(channel_.peripheral.power_on_id);
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
    GetCountControlRegister()->bits.mode = 0;
    // 0x0 for this register says to use the TC for input counts.
    GetCountControlRegister()->bits.count_input_select = 0;
    // Match register 0 is used to generate the desired frequency. If the time
    // counter TC is equal to MR0
    channel_.peripheral.registers->MR0 =
        GetPeripheralFrequency() / frequency_hz;
    // Sets match register 0 to reset when TC and Match 0 match each other,
    // meaning that the PWM pulse will cycle continuously.
    GetMatchControlRegister()->bits.pwm0.reset = 1;
    // Enables PWM TC and PC for counting and enables PWM mode
    EnablePwm();
    // Enables PWM[channel] output
    // Subtract 1 to move shift index to zero.
    const uint32_t kEnableChannelMask = 1 << (channel_.channel - 1);

    GetOutputControlRegister()->bits.enable_output =
        (GetOutputControlRegister()->bits.enable_output | kEnableChannelMask) &
        0b11'1111;

    channel_.pin.SetPinFunction(channel_.peripheral.pin_function_id);

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
    channel_.peripheral.registers->MR0 =
        GetPeripheralFrequency() / frequency_hz;
    SetDutyCycle(previous_duty_cycle);
    EnablePwm();
  }

  uint32_t GetFrequency() const
  {
    uint32_t match_register0 = GetMatchRegisters()[0];
    uint32_t result          = 0;
    if (match_register0 != 0)
    {
      result = GetPeripheralFrequency() / match_register0;
    }
    return result;
  }

  void EnablePwm(bool enable = true) const
  {
    if (enable)
    {
      GetTimerControlRegister()->bits.counter_reset  = 1;
      GetTimerControlRegister()->bits.counter_reset  = 0;
      GetTimerControlRegister()->bits.pwm_enable     = 1;
      GetTimerControlRegister()->bits.counter_enable = 1;
    }
    else
    {
      GetTimerControlRegister()->bits.pwm_enable = 0;
    }
  }

  [[gnu::always_inline]] volatile MatchControlRegister_t *
  GetMatchControlRegister() const
  {
    return reinterpret_cast<volatile MatchControlRegister_t *>(
        &channel_.peripheral.registers->MCR);
  }

  [[gnu::always_inline]] volatile CountControlRegister_t *
  GetCountControlRegister() const
  {
    return reinterpret_cast<volatile CountControlRegister_t *>(
        &channel_.peripheral.registers->CTCR);
  }

  [[gnu::always_inline]] volatile OutputControlRegister_t *
  GetOutputControlRegister() const
  {
    return reinterpret_cast<volatile OutputControlRegister_t *>(
        &channel_.peripheral.registers->PCR);
  }

  [[gnu::always_inline]] volatile TimerControlRegister_t *
  GetTimerControlRegister() const
  {
    return reinterpret_cast<volatile TimerControlRegister_t *>(
        &channel_.peripheral.registers->TCR);
  }

  [[gnu::always_inline]] volatile uint32_t * GetMatchRegisters() const
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
};
}  // namespace lpc40xx
}  // namespace sjsu
