#include <cstdint>
#include <limits>

#include "testing/testing_frameworks.hpp"
#include "utility/time/timeout_timer.hpp"

namespace sjsu
{
TEST_CASE("Testing TimeoutTimer")
{
  // Setup
  TimeoutTimer timer(1000us);
  constexpr std::chrono::microseconds kDelayOffset = 1us;

  SECTION("Does not expire")
  {
    // Exercise
    Delay(500us - kDelayOffset);

    // Verify
    CHECK(500us == timer.GetTimeLeft());
    CHECK(!timer.HasExpired());
  }

  SECTION("Expires")
  {
    // Exercise
    Delay(1500us - kDelayOffset);

    // Verify
    CHECK(-500us == timer.GetTimeLeft());
    CHECK(timer.HasExpired());
  }

  SECTION("Expires on the dot")
  {
    // Exercise
    Delay(1000us - kDelayOffset);

    // Verify
    CHECK(0us == timer.GetTimeLeft());
    CHECK(timer.HasExpired());
  }

  SECTION("Multiple delays")
  {
    // Exercise
    Delay(250us - kDelayOffset);
    auto time_left_0   = timer.GetTimeLeft();
    auto has_expired_0 = timer.HasExpired();

    Delay(250us - (kDelayOffset * 2));
    auto time_left_1   = timer.GetTimeLeft();
    auto has_expired_1 = timer.HasExpired();

    Delay(250us - (kDelayOffset * 2));
    auto time_left_2   = timer.GetTimeLeft();
    auto has_expired_2 = timer.HasExpired();

    Delay(250us - (kDelayOffset * 2));
    auto time_left_3   = timer.GetTimeLeft();
    auto has_expired_3 = timer.HasExpired();

    Delay(250us - (kDelayOffset * 2));
    auto time_left_4   = timer.GetTimeLeft();
    auto has_expired_4 = timer.HasExpired();

    // Verify
    CHECK(750us == time_left_0);
    CHECK(500us == time_left_1);
    CHECK(250us == time_left_2);
    CHECK(0us == time_left_3);
    CHECK(-250us == time_left_4);

    CHECK(!has_expired_0);
    CHECK(!has_expired_1);
    CHECK(!has_expired_2);
    CHECK(has_expired_3);
    CHECK(has_expired_4);
  }
}
}  // namespace sjsu
