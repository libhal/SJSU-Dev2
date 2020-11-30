#include <cstdint>
#include <limits>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/stopwatch.hpp"
#include "utility/log.hpp"

namespace sjsu
{
TEST_CASE("Testing StopWatch Utilities")
{
  SECTION("Calibrate")
  {
    // Setup
    StopWatch test_subject;

    // Exercise
    test_subject.Calibrate();

    // Verify
    CHECK(1us == test_subject.GetCalibrationDelta());
  }

  SECTION("Start and Stop")
  {
    // Setup
    StopWatch test_subject;
    test_subject.Calibrate();

    constexpr std::chrono::nanoseconds kDelay[] = {
      200us, 1215us, 1124us, 6752us, 10us, 5us, 983us,
    };
    constexpr std::chrono::nanoseconds kAfterDelay[] = {
      111us, 451us, 972us, 248us, 0us, 463us, 1045us,
    };

    // Exercise
    test_subject.Start();
    Delay(kDelay[0]);
    auto actual_delta0 = test_subject.Stop();
    Delay(kAfterDelay[0]);

    test_subject.Start();
    Delay(kDelay[1]);
    auto actual_delta1 = test_subject.Stop();
    Delay(kAfterDelay[1]);

    test_subject.Start();
    Delay(kDelay[2]);
    auto actual_delta2 = test_subject.Stop();
    Delay(kAfterDelay[2]);

    test_subject.Start();
    Delay(kDelay[3]);
    auto actual_delta3 = test_subject.Stop();
    Delay(kAfterDelay[3]);

    test_subject.Start();
    Delay(kDelay[4]);
    auto actual_delta4 = test_subject.Stop();
    Delay(kAfterDelay[4]);

    test_subject.Start();
    Delay(kDelay[5]);
    auto actual_delta5 = test_subject.Stop();
    Delay(kAfterDelay[5]);

    test_subject.Start();
    Delay(kDelay[6]);
    auto actual_delta6 = test_subject.Stop();
    Delay(kAfterDelay[6]);

    // Verify
    CHECK(kDelay[0] == actual_delta0);
    CHECK(kDelay[1] == actual_delta1);
    CHECK(kDelay[2] == actual_delta2);
    CHECK(kDelay[3] == actual_delta3);
    CHECK(kDelay[4] == actual_delta4);
    CHECK(kDelay[5] == actual_delta5);
    CHECK(kDelay[6] == actual_delta6);
  }
}
}  // namespace sjsu
