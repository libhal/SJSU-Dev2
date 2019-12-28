#include <cinttypes>
#include <cstdint>

#include "utility/log.hpp"
#include "utility/stopwatch.hpp"

int main()
{
  LOG_INFO("Stopwatch Application Starting...");
  sjsu::StopWatch stopwatch;
  LOG_INFO("Calibrating stop watch...");
  stopwatch.Calibrate();
  LOG_INFO("Uptime() call time = %" PRId32 ".",
           static_cast<int32_t>(stopwatch.GetCalibrationDelta().count()));

  LOG_INFO("Calibration figures out how much time it takes to call Uptime()");
  LOG_INFO("This time can differ depending on the platform.");

  while (true)
  {
    // Setup
    LOG_INFO(
        "Starting timer and executing code that will take time perform...");
    stopwatch.Start();

    // Exercise
    printf(
        "Payload message! This typically requires communication via JTAG or "
        "UART to perform!\n");
    sjsu::Delay(200ms);

    // Record the time it took to run the printf command and stream the data to
    // stdout.
    auto time_delta = stopwatch.Stop();

    // Displaying to the user the amount of time it took.
    LOG_INFO("Printing the message above took = %" PRId32 " us",
             static_cast<int32_t>(time_delta.count()));

    sjsu::Delay(1s);
  }

  return 0;
}
