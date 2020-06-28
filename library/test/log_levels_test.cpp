#include "log_levels.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Log Levels Test")
{
  static_assert(SJ2_LOG_LEVEL_NONESET == 00,
                "SJ2_LOG_LEVEL_NONESET must be equal to 00");
  static_assert(SJ2_LOG_LEVEL_DEBUG == 10,
                "SJ2_LOG_LEVEL_DEBUG must be equal to 10");
  static_assert(SJ2_LOG_LEVEL_INFO == 20,
                "SJ2_LOG_LEVEL_INFO must be equal to 20");
  static_assert(SJ2_LOG_LEVEL_WARNING == 30,
                "SJ2_LOG_LEVEL_WARNING must be equal to 30");
  static_assert(SJ2_LOG_LEVEL_ERROR == 40,
                "SJ2_LOG_LEVEL_ERROR must be equal to 40");
}
}  // namespace sjsu
