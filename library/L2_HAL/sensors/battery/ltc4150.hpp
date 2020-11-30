#pragma once

#include <atomic>
#include <cstdint>

#include "L1_Peripheral/hardware_counter.hpp"
#include "L2_HAL/sensors/battery/coulomb_counter.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// LTC4150 control driver for the LPC40xx and LPC17xx microcontrollers. It
/// keeps track of a connected battery's current milliamp hours.
class Ltc4150 : public CoulombCounter
{
 public:
  /// Represents the voltage to frequency gain in millivolts.
  static constexpr units::voltage::millivolt_t kGvf = 32.55_mV;
  /// 3600 is the number of coulombs per amp hour
  static constexpr float kCoulombsPerAh = 3600.0f;

  /// Defines what state the Polarity pin can be in.
  enum class Polarity : uint8_t
  {
    kDischarging,
    kCharging,
  };

  /// @param counter - a hardware counter that must count on rising edge pulses.
  /// @param pol - gpio pin to determine the polarity output of the LTC4150.
  /// @param resistance - value of impedance represented in ohms for
  ///        calculating battery charge
  explicit constexpr Ltc4150(sjsu::HardwareCounter & counter,
                             sjsu::Gpio & pol,
                             units::impedance::ohm_t resistance)
      : counter_(counter), pol_pin_(pol), kResistance(resistance)
  {
  }

  /// Initialize hardware, setting pins as inputs and attaching ISR handlers to
  /// the interrupt pin.
  void ModuleInitialize() override
  {
    // Phase 1: Initialize()
    // Initialize the hardware counter to allow counting of signal transitions.
    counter_.Initialize();
    pol_pin_.Initialize();

    // Phase 2: Configure()
    // Phase 3: Enable()
    counter_.Enable();
    pol_pin_.Enable();

    // Phase 4: Usage
    // Set the polarity pin as an input as we need to use it to read the state
    // polarity signal.
    pol_pin_.SetAsInput();

    // Set counter's initial value to zero.
    counter_.Set(0);
  }

  /// Initialize hardware, setting pins as inputs and attaching ISR handlers to
  /// the interrupt pin.
  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      auto polarity_interrupt = [this]() {
        if (pol_pin_.Read())
        {
          counter_.SetDirection(sjsu::HardwareCounter::Direction::kUp);
        }
        else
        {
          counter_.SetDirection(sjsu::HardwareCounter::Direction::kDown);
        }
      };

      // Sets the initial state of charging (counting down) or discharging
      // (counting up)
      polarity_interrupt();

      // Every time the polarity changes, this interrupt to be called to
      // determine the direction of the counter.
      pol_pin_.OnChange(polarity_interrupt);
    }
    else
    {
      pol_pin_.DetachInterrupt();
    }
  }

  /// @return the calculated mAh
  units::charge::microampere_hour_t GetCharge() override
  {
    /// We cast the pulses to scalar so we can calculate mAh
    const float kPulses = static_cast<float>(counter_.GetCount());

    const float kGvfFloat        = kGvf.to<float>();
    const float kResistanceFloat = kResistance.to<float>();
    const float kDenominator = (kCoulombsPerAh * kGvfFloat * kResistanceFloat);

    units::charge::milliampere_hour_t charge(kPulses / kDenominator);

    return charge;
  }

  /// Destructor of this object will detach the interrupt from the polarity
  /// GPIO pin. The counter_ class member variable automatically detaches the
  /// interrupt from the tick pin on destruction.
  ~Ltc4150()
  {
    pol_pin_.DetachInterrupt();
  }

 private:
  sjsu::HardwareCounter & counter_;
  sjsu::Gpio & pol_pin_;
  const units::impedance::ohm_t kResistance;
};
}  // namespace sjsu
