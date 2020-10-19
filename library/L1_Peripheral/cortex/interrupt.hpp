#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

#include "L0_Platform/arm_cortex/m4/core_cm4.h"
#include "L1_Peripheral/interrupt.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace cortex
{
/// Cortex M interrupt controller
///
/// @tparam kNumberOfInterrupts - the number of interrupts the microcontroller
///         supports.
/// @tparam kNvicPriorityBits - the number of bits dedicated to priority
template <size_t kNumberOfInterrupts, uint32_t kNvicPriorityBits>
class InterruptController final : public sjsu::InterruptController
{
 public:
  /// The number of ARM exceptions before reaching the MCUs
  static constexpr int32_t kArmExceptionOffset = (-cortex::Reset_IRQn) + 1;
  /// Pointer to Cortex M system control block registers
  inline static SCB_Type * scb = SCB;
  /// Pointer to Cortex M Nested Vector Interrupt Controller registers
  inline static NVIC_Type * nvic = NVIC;
  /// Holds the current_vector that is running
  inline static int current_vector = cortex::Reset_IRQn;

  /// @param irq - irq number to convert
  /// @return A convert an irq number into lookup table index
  static int IRQToIndex(int irq)
  {
    return irq + kArmExceptionOffset;
  }

  /// @param index - index to convert
  /// @return A convert a lookup table index into an irq number.
  static int IndexToIRQ(int index)
  {
    return index - kArmExceptionOffset;
  }

  /// This must be put into the interrupt vector table for all of the interrupts
  /// this lookup handler will work for in ROM at compile time.
  /// @note This function should only be called by the processor not by the
  /// application.
  static void LookupHandler()
  {
    int active_interrupt     = (scb->ICSR & 0xFF);
    current_vector           = IndexToIRQ(active_interrupt);
    InterruptHandler handler = table[active_interrupt];
    handler();
  }

  void Initialize(
      InterruptHandler unregistered_handler = UnregisteredHandler) override
  {
    std::fill(table.begin(), table.end(), unregistered_handler);
  }

  void Enable(RegistrationInfo_t register_info) override
  {
    int irq                = register_info.interrupt_request_number;
    table[IRQToIndex(irq)] = register_info.interrupt_handler;

    if (irq >= 0)
    {
      NvicEnableIRQ(irq);
    }
    if (register_info.priority > -1)
    {
      NvicSetPriority(irq, register_info.priority);
    }
  }

  void Disable(int interrupt_request_number) override
  {
    if (interrupt_request_number >= 0)
    {
      NvicDisableIRQ(interrupt_request_number);
    }
    table[IRQToIndex(interrupt_request_number)] = UnregisteredHandler;
  }

 private:
  static inline std::array<InterruptHandler,
                           kNumberOfInterrupts + kArmExceptionOffset>
      table;
  /// Enable External Interrupt
  /// Enables a device-specific interrupt in the NVIC interrupt controller.
  ///
  /// @param irq - External interrupt number. Value cannot be negative.
  static void NvicEnableIRQ(int irq)
  {
    nvic->ISER[(irq >> 5)] = (1 << (irq & 0x1F));
  }

  /// Disable External Interrupt
  /// Disables a device-specific interrupt in the NVIC interrupt controller.
  ///
  /// @param irq - External interrupt number. Value cannot be negative.
  static void NvicDisableIRQ(int irq)
  {
    nvic->ICER[(irq >> 5)] = (1 << (irq & 0x1F));
  }

  /// Set Interrupt Priority
  /// Sets the priority of an interrupt.
  /// @note    The priority cannot be set for every core interrupt.
  /// @param irq -  Interrupt number.
  /// @param priority -  Priority to set.
  static void NvicSetPriority(int irq, uint32_t priority)
  {
    uint32_t priority_mask = priority << (8U - kNvicPriorityBits);
    if (irq < 0)
    {
      scb->SHP[(irq & 0xFUL) - 4UL] = static_cast<uint8_t>(priority_mask);
    }
    else
    {
      nvic->IP[irq] = static_cast<uint8_t>(priority_mask);
    }
  }

  /// Program will call this if an unexpected interrupt occurs or a specific
  /// handler is not present in the application code.
  static void UnregisteredHandler()
  {
    sjsu::LogDebug(
        "No interrupt handler found! Disabling interrupt request %d!",
        current_vector);
    if (current_vector >= 0)
    {
      NvicDisableIRQ(current_vector);
    }
  }
};
}  // namespace cortex
}  // namespace sjsu
