#include <cstdint>
#include <limits>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/time.hpp"

namespace sjsu
{
TEST_CASE("Testing Time Utility", "[time]")
{
  // The behavior of DefaultUptime() should be a function that returns an
  // increasing up time counter each time it is called.
  SECTION("DefaultUptime() Increments with each call")
  {
    // Setup
    auto first_uptime = DefaultUptime();

    // Exercise + Verify
    CHECK(first_uptime + 1us == DefaultUptime());
    CHECK(first_uptime + 2us == DefaultUptime());
    CHECK(first_uptime + 3us == DefaultUptime());
    CHECK(first_uptime + 4us == DefaultUptime());
    CHECK(first_uptime + 5us == DefaultUptime());
  }

  SECTION("SetUptimeFunction()")
  {
    // Setup
    bool uptime_was_set        = false;
    const auto kExpectedUptime = 11984us;

    // Exercise
    SetUptimeFunction([&uptime_was_set, kExpectedUptime]() {
      uptime_was_set = true;
      return kExpectedUptime;
    });

    // Verify
    CHECK(kExpectedUptime == Uptime());
    CHECK(uptime_was_set);
  }

  SECTION("Delay()")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp = Uptime();
    auto delay_time        = 500us;

    // Exercise
    Delay(delay_time);

    // Verify
    CHECK((current_timestamp + 2us + delay_time) == Uptime());
  }

  SECTION("Wait() with callback times out")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp = Uptime();
    auto timeout_time      = 500us;

    // Exercise
    // Exercise: if the callback always returns false, then the waiting period
    //           will be the whole timeout.
    Status wait_status = Wait(timeout_time, []() { return false; });

    // Verify
    CHECK(wait_status == Status::kTimedOut);
    CHECK((current_timestamp + 2us + timeout_time) == Uptime());
  }

  SECTION("Wait() function times out")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp   = Uptime();
    auto timeout_time        = 500us;
    auto time_until_complete = 100us;
    int callback_counter     = 0;

    // Exercise
    Status wait_status =
        Wait(timeout_time, [&callback_counter, time_until_complete]() {
          if (callback_counter > time_until_complete.count())
          {
            return true;
          }
          callback_counter++;
          return false;
        });

    // Verify
    auto final_uptime = Uptime();
    CHECK(wait_status == Status::kSuccess);
    CHECK((current_timestamp + 4us + time_until_complete).count() ==
          final_uptime.count());
    CHECK((current_timestamp + 4us + timeout_time) != final_uptime);
  }

  SECTION("Wait() max time")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp   = Uptime();
    auto timeout_time        = std::chrono::microseconds::max();
    auto time_until_complete = 5us;
    int callback_counter     = 0;

    // Exercise: If I waited for the actual max time, this test would not end
    //           for a very long time. The problem with using max() is that it
    //           overflows the timeout counter. If it had overflowed then it
    //           would not have delayed at all, as the timeout time would
    //           naturally be the current_time + max() which will overflow to be
    //           the current_time, thus the next iteration will cause a timeout.
    //           If we are actually able to iterate more then a few times the
    //           the max has been ceiled.
    Status wait_status =
        Wait(timeout_time, [&callback_counter, time_until_complete]() {
          if (callback_counter > time_until_complete.count())
          {
            return true;
          }
          callback_counter++;
          return false;
        });

    // Verify
    auto final_uptime = Uptime();
    CHECK(wait_status == Status::kSuccess);
    CHECK((current_timestamp + 3us + time_until_complete).count() ==
          final_uptime.count());
    CHECK((current_timestamp + 3us + timeout_time) != final_uptime);
  }
}
}  // namespace sjsu
