#pragma once

#include <cstddef>
#include <iterator>

#include "L0_Platform/arm_cortex/m4/core_cm4.h"
#include "L1_Peripheral/interrupt.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace cortex
{
class InterruptController final : public sjsu::InterruptController
{
 public:
  static constexpr int32_t kArmIrqOffset      = (-cortex::Reset_IRQn) + 1;
  static constexpr size_t kNumberOfInterrupts = 64;

  inline static SCB_Type * scb     = SCB;
  inline static int current_vector = cortex::Reset_IRQn;

  static void UnregisteredArmExceptions() {}

  static void UnregisteredInterruptHandler()
  {
    LOG_WARNING(
        "No interrupt service routine found for the vector %d! Disabling ISR",
        current_vector);
    NVIC_DisableIRQ(current_vector - kArmIrqOffset);
  }

  struct VectorTable_t
  {
    IsrPointer vector[kNumberOfInterrupts];
    void Print()
    {
      for (size_t i = 0; i < std::size(vector); i++)
      {
        LOG_INFO("vector[%zu] = %p", i, vector[i]);
      }
    }

    static constexpr VectorTable_t GenerateDefaultTable()
    {
      VectorTable_t temp_table = { 0 };
      // The Arm exceptions may be enabled by default and should simply be
      // called and do nothing.
      for (size_t i = 0; i < kArmIrqOffset; i++)
      {
        temp_table.vector[i] = UnregisteredArmExceptions;
      }
      // For all other exceptions, give a handler that will disable the ISR if
      // it is enabled but has not been registered.
      for (size_t i = kArmIrqOffset; i < std::size(temp_table.vector); i++)
      {
        temp_table.vector[i] = UnregisteredInterruptHandler;
      }
      return temp_table;
    }
  };

  inline static VectorTable_t table = VectorTable_t::GenerateDefaultTable();

  static int IrqToIndex(int irq)
  {
    return irq + kArmIrqOffset;
  }

  static int IndexToIrq(int index)
  {
    return index - kArmIrqOffset;
  }

  static IsrPointer * GetVector(int irq)
  {
    return &table.vector[IrqToIndex(irq)];
  }
  /// Program ends up here if an unexpected interrupt occurs or a specific
  /// handler is not present in the application code.
  static void LookupHandler()
  {
    int active_isr = (scb->ICSR & 0xFF);
    current_vector = active_isr;
    IsrPointer isr = table.vector[active_isr];
    isr();
  }

  void Register(RegistrationInfo_t register_info) const override
  {
    int irq         = register_info.interrupt_request_number;
    *GetVector(irq) = register_info.interrupt_service_routine;
    if (register_info.enable_interrupt && irq >= 0)
    {
      NVIC_EnableIRQ(irq);
    }
    if (register_info.priority > -1)
    {
      NVIC_SetPriority(irq, register_info.priority);
    }
  }

  void Deregister(int irq) const override
  {
    NVIC_DisableIRQ(irq);
    *GetVector(irq) = UnregisteredInterruptHandler;
  }
};
}  // namespace cortex
}  // namespace sjsu
