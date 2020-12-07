// INCOMPLETE!
#include "L2_HAL/actuators/servo/rmd_x.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(RmdX);

TEST_CASE("Testing RMD-X7")
{
  Mock<Can> mock_can;

  Fake(Method(mock_can, Can::ModuleInitialize));
  Fake(Method(mock_can, Can::ModuleEnable));
  Fake(Method(mock_can, Can::ConfigureBaudRate));
  Fake(Method(mock_can, Can::ConfigureReceiveHandler));
  Fake(OverloadedMethod(mock_can, Can::Send, void(const Can::Message_t &)));
  Fake(Method(mock_can, Can::Receive));
  Fake(Method(mock_can, Can::HasData));

  StaticAllocator<1024> memory_resource;
  CanNetwork network(mock_can.get(), &memory_resource);

  constexpr auto kId = 0x140;

  // Inject test_gpio into button object
  RmdX test_servo(network, kId);

  SECTION("ModuleInitialize")
  {
    test_servo.ModuleInitialize();

    Verify(Method(mock_can, ModuleInitialize),
           Method(mock_can, ConfigureBaudRate).Using(1_MHz),
           Method(mock_can, ModuleEnable))
        .Once();

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
