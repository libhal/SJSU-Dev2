#pragma once

#include <cstdint>

#include "L2_HAL/sensors/battery/coulomb_counter.hpp"
#include "L1_Peripheral/gpio.hpp"

namespace sjsu
{
/// Represents the default Rsense value from calculating the mAh for a given
/// number of ticks.
constexpr float kRsense = 0.00005f;
/// Represents the voltage to frequency gain in millivolts.
constexpr float kGvf = 32.55f;

/// LTC4150 control driver for the LPC40xx and LPC17xx microcontrollers. It
/// keeps track of a connected battery's current milliamp hours.
class Ltc4150 : public CoulombCounter
{
 public:
  /// struct for holding the number of times an ISR invoked for a coulomb
  /// counter
  struct Battery_t
  {
    /// int variable holding the number of times an ISR invoked for a coulomb
    /// counter
    int num_ticks;
  };

  /// Class that handles the ticks from the LTC4150. The class uses a template
  /// so there can be different handlers for different LTC4150s connected to a
  /// board.
  template <int id>
  class TickHandler
  {
   public:
    /// This member variable will hold the number of ticks sent from the
    /// LTC4150. The interrupt handler increments this value by one every time
    /// it is invoked.
    inline static Battery_t battery_info;

    /// handles the ISR by incrementing the num_ticks variable of TickHandler's
    /// Battery_t
    static void IsrHandler()
    {
      battery_info.num_ticks += 1;
    }

    /// @return the calculated milliamp hours based on the number of ticks.
    /// 3600 is X 32.55 is kGvF and 0.00005 is kRsense
    static float GetBatterymAh()
    {
      return static_cast<float>(static_cast<float>(battery_info.num_ticks) /
                                (3600 * kGvf * kRsense));
    }
  };

  /// @param isr - handler for coulomb counter ticks.
  /// @param int_pin - gpio pin for handling handling interrupts from the
  /// LTC4150
  /// @param pol - gpio pin to determine the polarity output of the LTC4150.
  template <int id>
  explicit constexpr Ltc4150(const TickHandler<id> & isr,
                             Gpio & int_pin,
                             Gpio & pol)
      : int_pin_(int_pin),
        pol_(pol),
        get_mah_(isr.GetBatterymAh),
        isr_handler_(isr.IsrHandler)
  {
  }

  /// Initialize hardware, setting pins as inputs and attaching ISR handlers to
  /// the interrupt pin.
  void Initialize() const override
  {
    pol_.SetAsInput();
    int_pin_.SetAsInput();
    int_pin_.AttachInterrupt(isr_handler_, Gpio::Edge::kEdgeFalling);
  }

  /// @return the calculated mAh from the TickHandler instance
  float GetBatterymAh() const override
  {
    return get_mah_();
  }

 private:
  /// Gpio pin to recieve tick interrupts from the LTC4150.
  sjsu::Gpio & int_pin_;
  /// Gpio pin to check polarity output from the LTC4150.
  sjsu::Gpio & pol_;
  /// function pointer to grab current battery mAh
  float (*get_mah_)(void) = nullptr;
  /// function pointer to isr handler
  void (*isr_handler_)(void) = nullptr;
};
}  // namespace sjsu
