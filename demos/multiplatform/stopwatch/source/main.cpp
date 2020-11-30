#include <cinttypes>
#include <cstdint>

#include "utility/log.hpp"
#include "utility/stopwatch.hpp"

int main()
{
  sjsu::LogInfo("Stopwatch Application Starting...");
  sjsu::StopWatch stopwatch;

  sjsu::LogInfo("Calibrating stop watch...");
  stopwatch.Calibrate();
  sjsu::LogInfo(
      "Time it between Start() and Stop() of the stopwatch is %" PRId64 "ns.",
      stopwatch.GetCalibrationDelta().count());

  sjsu::LogInfo(
      "Calibration figures out how much time it takes to call Uptime()");
  sjsu::LogInfo("This time can differ depending on the platform.");

  while (true)
  {
    // Setup
    sjsu::LogInfo("Starting timer...");
    stopwatch.Start();

    // Exercise
    // Exercise: Print something
    printf("Printing message! Usually via JTAG or UART.\n");
    // Exercise: Delay for a period of time
    sjsu::Delay(200ms);

    // Record the time it took to run the printf command and stream the data to
    // stdout.
    auto time_delta = stopwatch.Stop();
    sjsu::LogInfo("Stopping timer...");

    // Displaying to the user the amount of time it took.
    // The time delta shouldn't be greater than what a 32-bit number can hold
    sjsu::LogInfo("Printing the message above took = %" PRId64 "ns",
                  time_delta.count());

    sjsu::Delay(1s);
  }

  return 0;
}
