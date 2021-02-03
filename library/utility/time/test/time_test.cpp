#include <cstdint>
#include <limits>

#include "testing/testing_frameworks.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
TEST_CASE("Testing Time Utility")
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
    CHECK((current_timestamp + 1us + delay_time) == Uptime());
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
    auto wait_successful = Wait(timeout_time, []() { return false; });

    // Verify
    CHECK(!wait_successful);
    CHECK((current_timestamp + 1us + timeout_time) == Uptime());
  }

  SECTION("Wait() function times out")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp   = Uptime();
    auto timeout_time        = 500us;
    auto time_until_complete = 10us;
    auto callback_counter    = 0us;

    // Exercise
    REQUIRE(Wait(timeout_time, [&callback_counter, time_until_complete]() {
      if (callback_counter > time_until_complete)
      {
        return true;
      }
      callback_counter++;
      return false;
    }));

    // Verify
    auto final_uptime = Uptime();
    CHECK((current_timestamp + 4us + time_until_complete) == final_uptime);
    CHECK((current_timestamp + 4us + timeout_time) != final_uptime);
  }

  SECTION("Wait() max time")
  {
    // Setup
    SetUptimeFunction(DefaultUptime);
    auto current_timestamp   = Uptime();
    auto timeout_time        = std::chrono::nanoseconds::max();
    auto time_until_complete = 5us;
    auto callback_counter    = 0us;

    // Exercise: If I waited for the actual max time, this test would not end
    //           for a very long time. The problem with using max() is that it
    //           overflows the timeout counter. If it had overflowed then it
    //           would not have delayed at all, as the timeout time would
    //           naturally be the current_time + max() which will overflow to be
    //           the current_time, thus the next iteration will cause a timeout.
    //           If we are actually able to iterate more then a few times the
    //           the max has been ceiled.
    REQUIRE(Wait(timeout_time, [&callback_counter, time_until_complete]() {
      if (callback_counter > time_until_complete)
      {
        return true;
      }
      callback_counter++;
      return false;
    }));

    // Verify
    auto final_uptime = Uptime();
    CHECK((current_timestamp + 3us + time_until_complete) == final_uptime);
    CHECK((current_timestamp + 3us + timeout_time) != final_uptime);
  }
}
}  // namespace sjsu
