#include "L4_Testing/testing_frameworks.hpp"
#include "utility/build_info.hpp"

namespace sjsu
{
namespace build
{
TEST_CASE("Testing Build Info", "[build-info]")
{
  SECTION("IsPlatform(Platform)")
  {
    static_assert(IsPlatform(Platform::host));
    static_assert(!IsPlatform(Platform::lpc40xx));
    static_assert(!IsPlatform(Platform::lpc17xx));
    static_assert(!IsPlatform(Platform::stm32f10x));
    static_assert(!IsPlatform(Platform::linux));
  }

  SECTION("Stringify(Platform)")
  {
    CHECK_THAT(Stringify(Platform::host), Catch::Equals("host"));
    CHECK_THAT(Stringify(Platform::lpc17xx), Catch::Equals("lpc17xx"));
    CHECK_THAT(Stringify(Platform::lpc40xx), Catch::Equals("lpc40xx"));
    CHECK_THAT(Stringify(Platform::linux), Catch::Equals("linux"));
    CHECK_THAT(Stringify(Platform::stm32f10x), Catch::Equals("stm32f10x"));
    CHECK_THAT(Stringify(Platform::stm32f4xx), Catch::Equals("stm32f4xx"));
    CHECK_THAT(Stringify(static_cast<Platform>(153'142'532)),
               Catch::Equals("invalid"));
  }
}
}  // namespace build
}  // namespace sjsu
