#pragma once

#include "utility/build_info.hpp"

namespace sjsu
{
namespace cortex
{
// Initialize the FPU. Must be done before any floating point instructions
// are executed on an ARM platform
// Found here:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABGHFIB.html
// // NOLINT
inline void InitializeFloatingPointUnit()
{
  if constexpr (build::kPlatform != build::Platform::host)
  {
    __asm__ __volatile__(
        // CPACR is located at address 0xE000ED88
        "LDR.W   R0, =0xE000ED88\n"
        // Read CPACR
        "LDR     R1, [R0]\n"
        // Set bits 20-23 to enable CP10 and CP11 coprocessors
        "ORR     R1, R1, #(0xF << 20)\n"
        // Write back the modified value to the CPACR
        "STR     R1, [R0]\n"
        // Wait for store to complete
        "DSB\n"
        // reset pipeline now the FPU is enabled
        "ISB\n");
  }
}
}  // namespace cortex
}  // namespace sjsu
