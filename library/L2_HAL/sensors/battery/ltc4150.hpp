#pragma once

#include <cstdint>
#include <atomic>

#include "L2_HAL/sensors/battery/coulomb_counter.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Represents the voltage to frequency gain in millivolts.
const units::voltage::millivolt_t kGvf = 32.55_mV;
/// 3600 is the number of coulombs per amp hour
const float kCoulombsPerAh = 3600.0f;

/// LTC4150 control driver for the LPC40xx and LPC17xx microcontrollers. It
/// keeps track of a connected battery's current milliamp hours.
class Ltc4150 : public CoulombCounter
{
 public:
  /// Defines what state the Polarity pin can be in.
  enum class Polarity : uint8_t
  {
    kDischarging,
    kCharging,
  };
  /// @param int_pin - gpio pin for handling handling interrupts from the
  ///        LTC4150
  /// @param pol - gpio pin to determine the polarity output of the LTC4150.
  /// @param resistance - value of impedance represented in ohms for
  ///        calculating battery charge
  explicit constexpr Ltc4150(sjsu::Gpio & int_pin,
                             sjsu::Gpio & pol,
                             units::impedance::ohm_t resistance)
      : int_pin_(int_pin),
        pol_pin_(pol),
        resistance_(resistance),
        pulses_(0),
        polarity_(Polarity::kDischarging)
  {
  }

  /// Initialize hardware, setting pins as inputs and attaching ISR handlers to
  /// the interrupt pin.
  void Initialize() override
  {
    auto pol_isr = [this]() {
      polarity_ =
          (pol_pin_.Read()) ? Polarity::kDischarging : Polarity::kCharging;
    };
    auto int_pin_isr = [this]() {
      pulses_ =
          (polarity_ == Polarity::kDischarging) ? pulses_ - 1 : pulses_ + 1;
    };
    pol_pin_.SetAsInput();
    int_pin_.SetAsInput();
    pol_isr();
    pol_pin_.AttachInterrupt(pol_isr, Gpio::Edge::kEdgeFalling);
    int_pin_.AttachInterrupt(int_pin_isr, Gpio::Edge::kEdgeFalling);
  }

  /// @return the calculated mAh
  units::charge::milliampere_hour_t GetCharge() const override
  {
    /// We cast the pulses to scalar so we can calculate mAh
    float pulses = static_cast<float>(pulses_);
    return units::charge::milliampere_hour_t{
      pulses / (kCoulombsPerAh * kGvf.to<float>() * resistance_.to<float>())
    };
  }

 private:
  sjsu::Gpio & int_pin_;
  sjsu::Gpio & pol_pin_;
  units::impedance::ohm_t resistance_;
  std::atomic<int32_t> pulses_;
  std::atomic<Polarity> polarity_;
};
}  // namespace sjsu
