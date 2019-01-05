#include <inttypes.h>
#include <cstdint>
#include "utility/log.hpp"

void PrintLogDescription(int a)
{
  LOG_INFO(
      "These log statements act just like printf statements: see that a = %d.",
      a);
  LOG_DEBUG(
      "Debug messages are extra verbose information that should generally not "
      "be present in a typical program. ");
  LOG_DEBUG(
      "Enable this level of log level only when you need to see as much "
      "information about the system as possible.");
  LOG_INFO(
      "Info messages are general messages that can put where ever to give "
      "general information about the program and how it is running.");
  LOG_WARNING(
      "Warning message typically alert/warn the user or developer about "
      "something possibly going wrong in the program.");
  LOG_ERROR(
      "Error messages should alert the user or developer of a possible error.");
  LOG_CRITICAL(
      "Critical messages should always be shown and typically occur when a "
      "fatal error has occurred during the runtime of the program.");
}

int main(void)
{
  LOG_INFO("Log Levels Print Application Starting...");

  LOG_DEBUG("This is a debug message.");
  LOG_INFO("This is an info message.");
  LOG_WARNING("This is an warning message.");
  LOG_ERROR("This is an error message.");
  LOG_CRITICAL("This is an critical message.");

  PrintLogDescription(5);
  return 0;
}
