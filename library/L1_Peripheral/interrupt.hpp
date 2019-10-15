#pragma once

#include <cstdint>

namespace sjsu
{
// Define an alias for an interrupt service routine function pointer.
using IsrPointer = void (*)(void);

class InterruptController
{
 public:
  struct RegistrationInfo_t
  {
    int interrupt_request_number;
    IsrPointer interrupt_service_routine;
    bool enable_interrupt = true;
    int priority          = -1;
  };

  virtual void Register(RegistrationInfo_t register_info) const = 0;
  virtual void Deregister(int irq) const                        = 0;
};
}  // namespace sjsu
