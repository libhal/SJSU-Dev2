// =============================================================================
// Utilties
// =============================================================================

#include "utility/test/build_info_test.cpp"           // NOLINT
#include "utility/test/config_test.cpp"               // NOLINT
#include "utility/test/constexpr_test.cpp"            // NOLINT
#include "utility/test/debug_test.cpp"                // NOLINT
#include "utility/test/enum_test.cpp"                 // NOLINT
#include "utility/test/error_handling_test.cpp"       // NOLINT
#include "utility/test/infrared_algorithms_test.cpp"  // NOLINT
#include "utility/test/log_levels_test.cpp"           // NOLINT
#include "utility/test/memory_resource_test.cpp"      // NOLINT

// =============================================================================
// Math
// =============================================================================

#include "utility/math/test/average_test.cpp"  // NOLINT
#include "utility/math/test/bit_test.cpp"      // NOLINT
#include "utility/math/test/crc_test.cpp"      // NOLINT
#include "utility/math/test/limits_test.cpp"   // NOLINT
#include "utility/math/test/map_test.cpp"      // NOLINT

// =============================================================================
// RTOS
// =============================================================================

#include "utility/rtos/freertos/test/periodic_scheduler_test.cpp"  // NOLINT
#include "utility/rtos/freertos/test/rtos_test.cpp"                // NOLINT
#include "utility/rtos/freertos/test/task_scheduler_test.cpp"      // NOLINT

// =============================================================================
// FILE I/O
// =============================================================================

#include "third_party/fatfs/source/sjsu-dev2/diskio.cpp"  // NOLINT
#include "utility/fatfs/test/fatfs_test.cpp"              // NOLINT

// =============================================================================
// Command line
// =============================================================================

#include "utility/console/commands/test/arm_system_command_test.cpp"  // NOLINT
#include "utility/console/commands/test/common_test.cpp"              // NOLINT
#include "utility/console/commands/test/i2c_command_test.cpp"         // NOLINT
#include "utility/console/commands/test/rtos_command_test.cpp"        // NOLINT
#include "utility/console/test/console_test.cpp"                      // NOLINT

// =============================================================================
// Time
// =============================================================================

#include "utility/time/test/stopwatch_test.cpp"      // NOLINT
#include "utility/time/test/time_test.cpp"           // NOLINT
#include "utility/time/test/timeout_timer_test.cpp"  // NOLINT
