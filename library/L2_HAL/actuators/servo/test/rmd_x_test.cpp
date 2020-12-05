// INCOMPLETE!
#include "L2_HAL/actuators/servo/rmd_x.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(RmdX);

TEST_CASE("Testing RMD-X7")
{
  Mock<Can> mock_can;

  Fake(Method(mock_can, ModuleInitialize));
  Fake(Method(mock_can, ModuleEnable));
  Fake(Method(mock_can, ConfigureBaudRate));
  Fake(OverloadedMethod(mock_can, Send, void(const Can::Message_t &)));
  Fake(Method(mock_can, Receive));
  Fake(Method(mock_can, HasData));

  constexpr auto kId = 0x140;

  // Inject test_gpio into button object
  RmdX test_servo(mock_can.get(), kId);

  SECTION("ModuleInitialize")
  {
    test_servo.ModuleInitialize();
    Verify(Method(mock_can, ModuleInitialize),
           Method(mock_can, ConfigureBaudRate).Using(1_MHz),
           Method(mock_can, ModuleEnable))
        .Once();
  }

  SECTION("ModuleEnable")
  {
    SECTION("true")
    {
      // Nothing should happen so this always succeeds
    }

    SECTION("false")
    {
      // Setup
      mock_can.get().SetStateToEnabled();

      // Exercise
      test_servo.ModuleEnable(false);

      // Verify
      Verify(OverloadedMethod(mock_can, Send, void(const Can::Message_t &)))
          .Once();
    }
  }
}
}  // namespace sjsu
