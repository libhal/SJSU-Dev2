// INCOMPLETE!
#include "L2_HAL/actuators/servo/rmd_x.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing RMD-X7")
{
  Mock<Can> mock_can;

  Fake(Method(mock_can, Can::ModuleInitialize));
  Fake(OverloadedMethod(mock_can, Can::Send, void(const Can::Message_t &)));
  Fake(Method(mock_can, Can::Receive));
  Fake(Method(mock_can, Can::HasData));

  StaticAllocator<1024> memory_resource;
  CanNetwork network(mock_can.get(), &memory_resource);

  constexpr auto kId = 0x140;

  // Inject test_gpio into button object
  RmdX test_servo(network, kId);

  SECTION("Initialize()")
  {
    // Exercise
    test_servo.Initialize();

    // Verify
    Verify(Method(mock_can, ModuleInitialize)).Once();
    CHECK(mock_can.get().CurrentSettings().baud_rate == 1_MHz);
    Verify(OverloadedMethod(mock_can, Send, void(const Can::Message_t &)))
        .Once();
  }
}
}  // namespace sjsu
