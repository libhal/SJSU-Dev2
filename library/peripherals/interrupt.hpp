#pragma once

#include <cstdint>
#include <functional>

#include "peripherals/inactive.hpp"
#include "module.hpp"

namespace sjsu
{
/// Used specifically for defining an interrupt vector table of addresses.
using InterruptVectorAddress = void (*)(void);

/// Define an alias for an interrupt service routine callable object.
using InterruptHandler = std::function<void(void)>;

/// Standard callback that should be executed when interrupts fire.
using InterruptCallback = std::function<void(void)>;

/// Definition of an interrupt callback that does... well... nothing really.
inline static InterruptCallback do_nothing = []() {};

/// An abstract interface for a platforms interrupt controller. This allows a
/// developer to enable and disable interrupts as well as assign handlers for
/// each.
///
/// @ingroup l1_peripheral
class InterruptController : public Module<>
{
 private:
  /// Global platform interrupt controller scoped within this class. Most
  /// systems only need a single platform interrupt controller, and thus this
  /// can hold a general/default platform interrupt controller that can be
  /// retrieved via SetPlatformController and GetPlatformController.
  static inline InterruptController * platform_interrupt_controller = nullptr;

 public:
  /// Contains all of the information required to setup an configure an
  /// interrupt service routine.
  struct RegistrationInfo_t
  {
    /// Interrupt request number or also called the interrupt vector that is
    /// associated with the interrupt you plan to manipulate.
    int interrupt_request_number;
    /// The handler for the interrupt. Can be set to nullptr if you plan to
    /// disable the interrupt. Setting this field to nullptr with an enabled
    /// interrupt, will cause std::abort to be called when the interrupt occurs.
    InterruptHandler interrupt_handler;
    /// Priority of the interrupt. The actual value here and what is means is
    /// dependent on the platform you are running on. -1 is considered the "do
    /// nothing" value.
    int priority = -1;
  };

  /// Set the controller for the platform
  ///
  /// @param interrupt_controller - a pointer to the current platform's
  ///        interrupt controller.
  static void SetPlatformController(InterruptController * interrupt_controller)
  {
    platform_interrupt_controller = interrupt_controller;
  }

  /// Retrieve a reference of the platforms interrupt controller
  static sjsu::InterruptController & GetPlatformController()
  {
    return *platform_interrupt_controller;
  }

  /// Configures and enables the interrupt on the platform and also registers
  /// the interrupt handler.
  ///
  /// @param register_info - the needed information to setup the interrupt.
  virtual void Enable(RegistrationInfo_t register_info) = 0;

  /// Disables and interrupt based on its interrupt request number
  ///
  /// @param interrupt_request_number - the interrupt request number to be
  ///        disabled.
  virtual void Disable(int interrupt_request_number) = 0;
};

/// Compare operator between two InterruptController::RegistrationInfo_t
/// objects.
///
/// @note this cannot compare if the callbacks are equal
///
/// @param lhs - left hand InterruptController::RegistrationInfo_t
/// @param rhs - right hand InterruptController::RegistrationInfo_t
/// @return true - request number and priority are the same.
constexpr bool operator==(const InterruptController::RegistrationInfo_t & lhs,
                          const InterruptController::RegistrationInfo_t & rhs)
{
  return lhs.interrupt_request_number == rhs.interrupt_request_number &&
         lhs.priority == rhs.priority;
}

/// Template specialization that generates an inactive sjsu::SystemController.
template <>
inline sjsu::InterruptController & GetInactive<sjsu::InterruptController>()
{
  class InactiveInterruptController : public sjsu::InterruptController
  {
   public:
    void ModuleInitialize() override {}
    void Enable(RegistrationInfo_t) override {}
    void Disable(int) override {}
  };

  static InactiveInterruptController inactive;
  return inactive;
}
}  // namespace sjsu
