#include <string>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/build_info.hpp"

namespace sjsu
{
namespace build
{
TEST_CASE("Testing Build Info")
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
    CHECK(0 == std::string_view(Stringify(Platform::host)).compare("host"));
    CHECK(0 ==
          std::string_view(Stringify(Platform::lpc17xx)).compare("lpc17xx"));
    CHECK(0 ==
          std::string_view(Stringify(Platform::lpc40xx)).compare("lpc40xx"));
    CHECK(0 == std::string_view(Stringify(Platform::linux)).compare("linux"));
    CHECK(
        0 ==
        std::string_view(Stringify(Platform::stm32f10x)).compare("stm32f10x"));
    CHECK(
        0 ==
        std::string_view(Stringify(Platform::stm32f4xx)).compare("stm32f4xx"));
    CHECK(0 == std::string_view(Stringify(static_cast<Platform>(153'142'532)))
                   .compare("invalid"));
  }
}
}  // namespace build
}  // namespace sjsu
