#pragma once

#include <cstdint>
#include <functional>

namespace sjsu
{
// Define an alias for an interrupt service routine function pointer.
using InterruptHandler = void (*)(void);
// Standard callback for most callbacks when interrupts fire.
using InterruptCallback = std::function<void(void)>;

class InterruptController
{
 public:
  struct RegistrationInfo_t
  {
    int interrupt_request_number;
    InterruptHandler interrupt_service_routine;
    bool enable_interrupt = true;
    int priority          = -1;
  };

  virtual void Register(RegistrationInfo_t register_info) const = 0;
  virtual void Deregister(int irq) const                        = 0;
};
}  // namespace sjsu
