#include <FreeRTOS.h>

#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "platforms/processors/arm_cortex/m4/core_cm4.h"
#include "platforms/utility/ram.hpp"
#include "platforms/utility/startup.hpp"
#include "peripherals/cortex/fpu.hpp"
#include "peripherals/cortex/interrupt.hpp"
#include "third_party/semihost/trace.h"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time/time.hpp"

extern "C"
{
  void GetRegistersFromStack(uint32_t * fault_stack_address)
  {
    // These are volatile to try and prevent the compiler/linker optimizing them
    // away as the variables never actually get used.  If the debugger won't
    // show the values of the variables, make them global my moving their
    // declaration outside of this function.
    volatile uint32_t r0  = fault_stack_address[0];
    volatile uint32_t r1  = fault_stack_address[1];
    volatile uint32_t r2  = fault_stack_address[2];
    volatile uint32_t r3  = fault_stack_address[3];
    volatile uint32_t r12 = fault_stack_address[4];
    // Link register.
    volatile uint32_t lr = fault_stack_address[5];
    // Program counter.
    volatile uint32_t pc = fault_stack_address[6];
    // Program status register.
    volatile uint32_t psr = fault_stack_address[7];

    printf(SJ2_BACKGROUND_RED
           "Hard Fault Exception Occurred!\n" SJ2_COLOR_RESET);
    printf("r0: 0x%08" PRIX32 ", r1: 0x%08" PRIX32
           ", "
           "r2: 0x%08" PRIX32 ", r3: 0x%08" PRIX32 "\n",
           r0,
           r1,
           r2,
           r3);
    printf("r12: 0x%08" PRIX32 ", lr: 0x%08" PRIX32
           ", "
           "pc: 0x%08" PRIX32 ", psr: 0x%08" PRIX32 "\n",
           r12,
           lr,
           pc,
           psr);
    sjsu::debug::PrintBacktrace(true, reinterpret_cast<void *>(pc));
    // When the following line is hit, the variables contain the register values
    // Use a JTAG debugger to inspect these variables
    sjsu::Halt();
  }

  void ArmHardFaultHandler(void)
  {
    if constexpr (sjsu::build::kPlatform != sjsu::build::Platform::host)
    {
      __asm volatile(
          " tst lr, #4                                          \n"
          " ite eq                                              \n"
          " mrseq r0, msp                                       \n"
          " mrsne r0, psp                                       \n"
          " ldr r1, [r0, #24]                                   \n"
          " ldr r2, handler2_address_const                      \n"
          " bx r2                                               \n"
          " handler2_address_const: .word GetRegistersFromStack \n");
    }
  }

  // Required to get FreeRTOS support in OpenOCD + GDB
  const volatile int uxTopUsedPriority = configMAX_PRIORITIES - 1;  // NOLINT

  // Reset entry point for your code.
  // Sets up a simple runtime environment and initializes the C/C++ library.
  void ArmResetHandler()
  {
    // External declaration for the pointer to the stack top from the linker
    // script
    // The Hyperload bootloader takes up stack space to execute. The Hyperload
    // bootloader function launches this ISR manually, but it never returns thus
    // it never cleans up the memory it uses. To get that memory back, we have
    // to manually move the stack pointers back to the top of stack.
    const uint32_t kTopOfStack = reinterpret_cast<intptr_t>(&StackTop);
    sjsu::cortex::__set_PSP(kTopOfStack);
    sjsu::cortex::__set_MSP(kTopOfStack);

    // Required to get the compiler to keep this symbol in the symbol table
    // rather than garbage collecting it at link time.
    _SJ2_USED(uxTopUsedPriority);

    // Enable FPU (Floating Point Unit)
    // System will crash if floating point instruction is executed before
    // Initializing the FPU first.
    // Processors without FPU, like cortex M3, will ignore this and do nothing.
    sjsu::cortex::InitializeFloatingPointUnit();

    sjsu::SystemInitialize();
    // Check if Debugger is connected
    {
      using sjsu::cortex::CoreDebug_Type;
      if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
      {
        trace_initialize();  // Enable debug tracing
      }
    }
    sjsu::InitializePlatform();
// #pragma ignored "-Wpedantic" to suppress main function call warning
#pragma GCC diagnostic push ignored "-Wpedantic"
    int32_t result = main();
// Enforce the warning after this point
#pragma GCC diagnostic pop
    // main() shouldn't return, but if it does, we'll just enter an infinite
    // loop
    if (result >= 0)
    {
      printf("\n" SJ2_BOLD_WHITE SJ2_BACKGROUND_GREEN
             "Program Returned Exit Code: %" PRId32 "\n" SJ2_COLOR_RESET,
             result);
    }
    else
    {
      printf("\n" SJ2_BOLD_WHITE SJ2_BACKGROUND_RED
             "Program Returned Exit Code: %" PRId32 "\n" SJ2_COLOR_RESET,
             result);
    }
    sjsu::Halt();
  }
}  // extern "C"
