#pragma once

#include <FreeRTOS.h>
#include <task.h>

#include <cinttypes>
#include <iterator>

#include "utility/console/console.hpp"

namespace sjsu
{
/// Print RTOS runtime stats of FreeRTOS
class RtosCommand final : public Command
{
 public:
  /// Starting description string
  static constexpr const char kDescription[] =
      "Display FreeRTOS runtime stats.";
  /// Table header
  static constexpr const char kHeader[] =
      "|    Task Name     |   State   | CPU% | Stack Left |   Priority  |\n"
      "|                  |           |      |  in words  | Base : Curr |";
  /// Table divider between tasks
  static constexpr const char kDivider[] =
      "+------------------+-----------+------+------------+-------------+";

  /// Default constructor of RTOS Command
  constexpr RtosCommand() : Command("rtos", kDescription) {}

  /// Returns the string representation of a FreeRTOS TaskState enumeration.
  ///
  /// @param state - The state of the task
  /// @return const char* of the task state into a string
  static const char * RtosStateToString(eTaskState state)
  {
    switch (state)
    {
      case eTaskState::eBlocked: return "BLOCKED";
      case eTaskState::eDeleted: return "DELETED";
      case eTaskState::eInvalid: return "INVALID";
      case eTaskState::eReady: return "READY";
      case eTaskState::eRunning: return "RUNNING";
      case eTaskState::eSuspended: return "SUSPENDED";
      default: return "UNKNOWN";
    }
  }

  int Program(int, const char * const[]) override
  {
    TaskStatus_t task_status[32];
    uint32_t total_runtime;
    // Generate raw status information about each task.
    UBaseType_t number_of_tasks = uxTaskGetSystemState(
        task_status, std::size(task_status), &total_runtime);
    // For percentage calculations.
    total_runtime /= 100UL;
    // Avoid divide by zero errors.
    if (total_runtime > 0)
    {
      puts(kDivider);
      puts(kHeader);
      // For each populated position in the task_status array,
      // format the raw data as human readable ASCII data.
      for (size_t x = 0; x < number_of_tasks; x++)
      {
        // What percentage of the total run time has the task used?
        // This will always be rounded down to the nearest integer.
        // total_runtimeDiv / 100 has already been divided by 100.
        uint32_t cpu_utilization_percent =
            task_status[x].ulRunTimeCounter / total_runtime;
        puts(kDivider);
        printf("| %16.16s | %9.9s | %3" PRIu32 "%% | %10" PRIu16
               " | %4lu : %-4lu |\n",
               task_status[x].pcTaskName,
               RtosStateToString(task_status[x].eCurrentState),
               cpu_utilization_percent, task_status[x].usStackHighWaterMark,
               task_status[x].uxBasePriority, task_status[x].uxCurrentPriority);
      }
      puts(kDivider);
    }
    return 0;
  }
};
}  // namespace sjsu
