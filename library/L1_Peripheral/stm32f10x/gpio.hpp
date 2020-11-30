#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "utility/error_handling.hpp"

namespace sjsu::stm32f10x
{
/// An abstract interface for General Purpose I/O
class Gpio : public sjsu::Gpio
{
 public:
  /// The highest pin value for stm32f10x MCUs is 0 to 15, thus we have a limit
  /// of 15 pins per port.
  static constexpr uint8_t kPinMaximum = 16;

  /// Lookup table that holds developer gpio interrupt handlers.
  inline static InterruptCallback handlers[kPinMaximum];

  /// Pointer to the external interrupt peripheral
  inline static EXTI_TypeDef * external_interrupt = EXTI;

  /// @param port - must be a capitol letter from 'A' to 'G'
  /// @param pin - must be between 0 to 15
  constexpr Gpio(uint8_t port, uint8_t pin) : pin_{ port, pin } {}

  void ModuleInitialize() override
  {
    pin_.Initialize();
  }

  /// Does nothing
  void ModuleEnable(bool = true) override {}

  void SetDirection(Direction direction) override
  {
    if (direction == Direction::kInput)
    {
      // Using the `ConfigureFloating()` method here will do the work of setting
      // the pin to an input as well as setting the pin to the its reset state
      // as defined within the RM0008 user manual for the STM32F10x.
      pin_.ConfigureFloating();
    }
    else
    {
      // Setting pin function to 0 converts pin to an output GPIO pin.
      pin_.ConfigureFunction(0);
    }
  }

  void Set(State output) override
  {
    if (output == State::kHigh)
    {
      // The first 16 bits of the register set the output state
      pin_.Port()->BSRR = (1 << pin_.GetPin());
    }
    else
    {
      // The last 16 bits of the register reset the output state
      pin_.Port()->BSRR = (1 << (pin_.GetPin() + 16));
    }
  }

  void Toggle() override
  {
    pin_.Port()->ODR ^= (1 << pin_.GetPin());
  }

  bool Read() override
  {
    return bit::Read(pin_.Port()->IDR, pin_.GetPin());
  }

  sjsu::Pin & GetPin() override
  {
    return pin_;
  }

  /// The gpio interrupt handler that calls the attached interrupt callbacks.
  static void InterruptHandler()
  {
    // Find the pin that triggered this interrupt by reading the Pending
    // Register. Each bit location number represents the pin number that
    // triggered this interrupt.
    // We can count the number of zeros from the right to return the bit number
    // of the least significant set bit in the register, which will be at least
    // one of the pins that triggered this handler.
    int pin = __builtin_ctz(external_interrupt->PR);

    // Use the pin to lookup and run the associated callback
    handlers[pin]();

    // Clear the pending interrupt by writing a 1 back to the PR register.
    external_interrupt->PR = (1 << pin);
  }

  void AttachInterrupt(InterruptCallback callback, Edge edge) override
  {
    // Clear both falling and raising edges bits
    // They will be assigned in the conditionals below
    DetachInterrupt();

    // Add callback to list of handlers
    handlers[pin_.GetPin()] = callback;

    if (edge == Edge::kBoth || edge == Edge::kRising)
    {
      external_interrupt->RTSR =
          bit::Set(external_interrupt->RTSR, pin_.GetPin());
    }
    if (edge == Edge::kBoth || edge == Edge::kFalling)
    {
      external_interrupt->FTSR =
          bit::Set(external_interrupt->FTSR, pin_.GetPin());
    }

    // Enable interrupts for this particular pin
    external_interrupt->IMR = bit::Set(external_interrupt->IMR, pin_.GetPin());

    // Fetch the external interrupt control register from the AFIO peripheral
    // There are 4 EXTICR registers which only use the first 16 bits of the
    // 32-bit register.
    // We have 16 EXTI interrupt signals which can be assigned to any of the
    // ports A, B, ..., G.
    // Divide the pin number by 4 to select the EXTICR register in the array
    // to update.
    volatile uint32_t * control = &Pin::afio->EXTICR[pin_.GetPin() / 4];

    // The location within the EXTICR[x] register for the port select
    // information
    auto interrupt_alternative_function_mask = bit::Mask{
      .position = static_cast<uint32_t>((pin_.GetPin() * 4) % 16),
      .width    = 4,
    };

    // Assign the control port value, which is equal to the port value minus
    // 'A', thus A would be 0, B would be 1, etc.
    *control = bit::Insert(
        *control, pin_.GetPort() - 'A', interrupt_alternative_function_mask);

    // Enable interrupts for this external interrupt line
    switch (pin_.GetPin())
    {
      case 0:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI0_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 1:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI1_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 2:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI2_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 3:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI3_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 4:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI4_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 5: [[fallthrough]];
      case 6: [[fallthrough]];
      case 7: [[fallthrough]];
      case 8: [[fallthrough]];
      case 9:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI9_5_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      case 10: [[fallthrough]];
      case 11: [[fallthrough]];
      case 12: [[fallthrough]];
      case 13: [[fallthrough]];
      case 14: [[fallthrough]];
      case 15:
        sjsu::InterruptController::GetPlatformController().Enable({
            .interrupt_request_number = stm32f10x::EXTI15_10_IRQn,
            .interrupt_handler        = InterruptHandler,
        });
        break;
      default:
      {
        throw Exception(std::errc::invalid_argument,
                        "Pin must be between 0-15");
      }
    }
  }

  void DetachInterrupt() override
  {
    // No need to Enable/Disable the interrupt via the interrupt controller,
    // especially since you would need logic to determine if this is the last
    // pin used for a particular interrupt request line. For example,
    // EXTI15_10_IRQn can only be disabled if we know that no other pins need
    // that IRQ enabled. To simply the logic simply disabling falling edge or
    // rising edge detection is all that is needed.
    external_interrupt->RTSR =
        bit::Clear(external_interrupt->RTSR, pin_.GetPin());
    external_interrupt->FTSR =
        bit::Clear(external_interrupt->FTSR, pin_.GetPin());
  }

 private:
  sjsu::stm32f10x::Pin pin_;
};
}  // namespace sjsu::stm32f10x
