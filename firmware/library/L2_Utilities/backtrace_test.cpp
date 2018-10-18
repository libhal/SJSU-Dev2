#include <unwind.h>
#include <cstdio>
#include "L2_Utilities/backtrace.hpp"
#include "L5_Testing/testing_frameworks.hpp"

FAKE_VALUE_FUNC_VARARG(int, test_logger, const char *, ...);

TEST_CASE("Testing Backtrace", "[backtrace]")
{
  log_function = test_logger;

  SECTION("Test that test_logger was called")
  {
    PrintTrace();
    CHECK(0 < test_logger_fake.call_count);
  }
}
