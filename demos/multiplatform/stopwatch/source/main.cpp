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
  sjsu::LogInfo("Time it takes to Start() and Stop() the stopwatch = %" PRId32
                "ns.",
                static_cast<int32_t>(stopwatch.GetCalibrationDelta().count()));

  sjsu::LogInfo(
      "Calibration figures out how much time it takes to call Uptime()");
  sjsu::LogInfo("This time can differ depending on the platform.");

  while (true)
  {
    // Setup
    sjsu::LogInfo(
        "Starting timer and executing code that will take time perform...");
    stopwatch.Start();

    // Exercise
    // Exercise: Print something
    printf(
        "Payload message! This typically requires communication via JTAG or "
        "UART to perform!\n");
    // Exercise: Delay for a period of time
    sjsu::Delay(200ms);

    // Record the time it took to run the printf command and stream the data to
    // stdout.
    auto time_delta = stopwatch.Stop();

    // Displaying to the user the amount of time it took.
    // The time delta shouldn't be greater than what a 32-bit number can hold
    sjsu::LogInfo("Printing the message above took = %" PRId32 "ns",
                  static_cast<int32_t>(time_delta.count()));

    sjsu::Delay(1s);
  }

  return 0;
}
