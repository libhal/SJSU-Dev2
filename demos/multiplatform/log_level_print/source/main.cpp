#include "utility/log.hpp"

void PrintLogDescription(int a)
{
  sjsu::LogInfo(
      "These log statements act just like printf statements: see that a = %d.",
      a);
  sjsu::LogDebug(
      "Debug messages are extra verbose information that should generally not "
      "be present in a typical program. ");
  sjsu::LogDebug(
      "Enable this level of log level only when you need to see as much "
      "information about the system as possible.");
  sjsu::LogInfo(
      "Info messages are general messages that can put where ever to give "
      "general information about the program and how it is running.");
  sjsu::LogWarning(
      "Warning message typically alert/warn the user or developer about "
      "something possibly going wrong in the program.");
  sjsu::LogError(
      "Error messages should alert the user or developer of a possible error.");
}

int main()
{
  sjsu::LogInfo("Log Levels Print Application Starting...");

  sjsu::LogDebug("This is a debug message.");
  sjsu::LogInfo("This is an info message.");
  sjsu::LogWarning("This is an warning message.");
  sjsu::LogError("This is an error message.");

  PrintLogDescription(5);
  return 0;
}
