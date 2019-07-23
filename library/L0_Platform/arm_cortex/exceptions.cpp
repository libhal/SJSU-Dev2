#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/cortex/interrupt.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

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

  printf(SJ2_BACKGROUND_RED "Hard Fault Exception Occurred!\n" SJ2_COLOR_RESET);
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

void HardFaultHandler(void)
{
  if constexpr (sjsu::build::kTarget != sjsu::build::Target::HostTest)
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
} // extern "C"
