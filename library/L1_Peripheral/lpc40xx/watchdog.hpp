#pragma once

#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/watchdog_interface.hpp"
#include "utility/time.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Watchdog : public sjsu::WatchdogInterface
{
 public:
  inline static LPC_WDT_TypeDef * wdt_base = LPC_WDT;

  static void WatchdogIrqHandler()
  {
    Watchdog watchdog;
    uint32_t timer_value;
    timer_value = watchdog.CheckTimeout();
    printf("TV: 0x%x \n", timer_value);
  }

  inline static const InterruptController::RegistrationInfo_t
      kWatchdogInterruptInfo = {
        .interrupt_request_number   = WDT_IRQn,
        .interrupt_service_routine  = &WatchdogIrqHandler,
        .enable_interrupt           = true,
        .priority                   = 0,
      };

  inline static const sjsu::cortex::InterruptController
      kCortexInterruptController = sjsu::cortex::InterruptController();

  // Initializes the watchdog timer
  void Initialize(uint32_t timer_constant, uint32_t timer_window
    , uint32_t timer_warning)
  {
    // Insert timer_constant value into TC register
    uint32_t timer_constant_register = sjsu::bit::Insert(0x00'0000,
      timer_constant,
    {
      .position = 0,
      .width = 24,
    });
    wdt_base->TC = timer_constant_register;

    // Set Bit 0 and Bit 1 in MOD register
    uint8_t timer_mode_register = sjsu::bit::Set(0b0000'0000, 0);
    timer_mode_register = sjsu::bit::Set(timer_mode_register, 1);
    wdt_base->MOD = timer_mode_register;

    // Insert timer_window value into WINDOW register
    uint32_t timer_window_register = sjsu::bit::Insert(0x00'0000,
      timer_window,
        {
          .position = 0,
          .width = 24,
        });
    wdt_base->WINDOW = timer_window_register;

    // Insert timer_warning value into WARNINT register
    uint32_t timer_warning_register = sjsu::bit::Insert(0x000,
     timer_warning,
        {
          .position = 0,
          .width = 10,
        });
    wdt_base->WARNINT = timer_warning_register;
  }
  // Register the WatchdogIRQ defined by the structure
  void RegisterIrqHandler()
  {
    kCortexInterruptController.Register(kWatchdogInterruptInfo);
  }

  // Feeds the watchdog timer (Verify feed is working)
  void FeedSequence() {
    // Insert valid feed sequence
    uint8_t first_feed_sequence = sjsu::bit::Insert(0x00, 0xAA,
        {
          .position = 0,
          .width = 8,
        });
    wdt_base->FEED = first_feed_sequence;
    uint8_t second_feed_sequence = sjsu::bit::Insert(0x00, 0x55,
        {
          .position = 0,
          .width = 8,
        });
    wdt_base->FEED = second_feed_sequence;
  }
  // Reads the current counter value of the watchdog timer
  uint32_t CheckTimeout() {
    return wdt_base->TV;
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
