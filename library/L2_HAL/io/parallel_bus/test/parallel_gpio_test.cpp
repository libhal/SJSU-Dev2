#include "L2_HAL/io/parallel_bus/parallel_gpio.hpp"

#include <iterator>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/pin.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing Parallel Gpio Implementation")
{
  Mock<sjsu::Gpio> mock_gpio0;
  Mock<sjsu::Gpio> mock_gpio1;
  Mock<sjsu::Gpio> mock_gpio2;
  Mock<sjsu::Gpio> mock_gpio3;

  Mock<sjsu::Pin> mock_pin0;
  Mock<sjsu::Pin> mock_pin1;
  Mock<sjsu::Pin> mock_pin2;
  Mock<sjsu::Pin> mock_pin3;

  Fake(Method(mock_gpio0, ModuleInitialize));
  Fake(Method(mock_gpio1, ModuleInitialize));
  Fake(Method(mock_gpio2, ModuleInitialize));
  Fake(Method(mock_gpio3, ModuleInitialize));

  Fake(Method(mock_gpio0, ModuleEnable));
  Fake(Method(mock_gpio1, ModuleEnable));
  Fake(Method(mock_gpio2, ModuleEnable));
  Fake(Method(mock_gpio3, ModuleEnable));

  Fake(Method(mock_gpio0, SetDirection));
  Fake(Method(mock_gpio1, SetDirection));
  Fake(Method(mock_gpio2, SetDirection));
  Fake(Method(mock_gpio3, SetDirection));

  Fake(Method(mock_gpio0, Set));
  Fake(Method(mock_gpio1, Set));
  Fake(Method(mock_gpio2, Set));
  Fake(Method(mock_gpio3, Set));

  Fake(Method(mock_pin0, ConfigureAsOpenDrain));
  Fake(Method(mock_pin1, ConfigureAsOpenDrain));
  Fake(Method(mock_pin2, ConfigureAsOpenDrain));
  Fake(Method(mock_pin3, ConfigureAsOpenDrain));

  When(Method(mock_gpio0, GetPin)).AlwaysReturn(mock_pin0.get());
  When(Method(mock_gpio1, GetPin)).AlwaysReturn(mock_pin1.get());
  When(Method(mock_gpio2, GetPin)).AlwaysReturn(mock_pin2.get());
  When(Method(mock_gpio3, GetPin)).AlwaysReturn(mock_pin3.get());

  std::array<sjsu::Gpio *, 4> gpio_array = {
    &mock_gpio0.get(),
    &mock_gpio1.get(),
    &mock_gpio2.get(),
    &mock_gpio3.get(),
  };

  sjsu::ParallelGpio test_subject(gpio_array);

  SECTION("Array Constructor && Initialize")
  {
    // Exercise
    test_subject.ModuleInitialize();

    // Verify
    Verify(Method(mock_gpio0, ModuleInitialize));
    Verify(Method(mock_gpio1, ModuleInitialize));
    Verify(Method(mock_gpio2, ModuleInitialize));
    Verify(Method(mock_gpio3, ModuleInitialize));
  }

  SECTION("Enable()")
  {
    // Setup
    mock_gpio0.get().SetStateToInitialized();
    mock_gpio1.get().SetStateToInitialized();
    mock_gpio2.get().SetStateToInitialized();
    mock_gpio3.get().SetStateToInitialized();

    // Exercise
    test_subject.ModuleEnable();

    // Verify
    Verify(Method(mock_gpio0, ModuleEnable));
    Verify(Method(mock_gpio1, ModuleEnable));
    Verify(Method(mock_gpio2, ModuleEnable));
    Verify(Method(mock_gpio3, ModuleEnable));
  }

  SECTION("SetDirection")
  {
    SECTION("Output")
    {
      // Exercise
      test_subject.SetDirection(sjsu::Gpio::Direction::kOutput);

      // Verify
      Verify(Method(mock_gpio0, SetDirection)
                 .Using(sjsu::Gpio::Direction::kOutput));
      Verify(Method(mock_gpio1, SetDirection)
                 .Using(sjsu::Gpio::Direction::kOutput));
      Verify(Method(mock_gpio2, SetDirection)
                 .Using(sjsu::Gpio::Direction::kOutput));
      Verify(Method(mock_gpio3, SetDirection)
                 .Using(sjsu::Gpio::Direction::kOutput));
    }

    SECTION("Input")
    {
      // Exercise
      test_subject.SetDirection(sjsu::Gpio::Direction::kInput);

      // Verify
      Verify(Method(mock_gpio0, SetDirection)
                 .Using(sjsu::Gpio::Direction::kInput));
      Verify(Method(mock_gpio1, SetDirection)
                 .Using(sjsu::Gpio::Direction::kInput));
      Verify(Method(mock_gpio2, SetDirection)
                 .Using(sjsu::Gpio::Direction::kInput));
      Verify(Method(mock_gpio3, SetDirection)
                 .Using(sjsu::Gpio::Direction::kInput));
    }
  }

  SECTION("ConfigureAsOpenDrain()")
  {
    SECTION("Open Drain")
    {
      // Exercise
      test_subject.ConfigureAsOpenDrain();

      // Verify
      Verify(Method(mock_pin0, ConfigureAsOpenDrain).Using(true));
      Verify(Method(mock_pin1, ConfigureAsOpenDrain).Using(true));
      Verify(Method(mock_pin2, ConfigureAsOpenDrain).Using(true));
      Verify(Method(mock_pin3, ConfigureAsOpenDrain).Using(true));
    }

    SECTION("Open Drain Disabled")
    {
      // Exercise
      test_subject.ConfigureAsOpenDrain(false);

      // Verify
      Verify(Method(mock_pin0, ConfigureAsOpenDrain).Using(false));
      Verify(Method(mock_pin1, ConfigureAsOpenDrain).Using(false));
      Verify(Method(mock_pin2, ConfigureAsOpenDrain).Using(false));
      Verify(Method(mock_pin3, ConfigureAsOpenDrain).Using(false));
    }
  }

  SECTION("Write")
  {
    // Exercise
    test_subject.Write(0b1001);

    // Verify
    Verify(
        Method(mock_gpio0, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio1, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio2, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio3, SetDirection).Using(sjsu::Gpio::Direction::kOutput));

    Verify(Method(mock_gpio0, Set).Using(Gpio::State::kHigh));
    Verify(Method(mock_gpio1, Set).Using(Gpio::State::kLow));
    Verify(Method(mock_gpio2, Set).Using(Gpio::State::kLow));
    Verify(Method(mock_gpio3, Set).Using(Gpio::State::kHigh));
  }

  SECTION("Read")
  {
    // Setup
    When(Method(mock_gpio0, Read)).AlwaysReturn(Gpio::State::kHigh);
    When(Method(mock_gpio1, Read)).AlwaysReturn(Gpio::State::kLow);
    When(Method(mock_gpio2, Read)).AlwaysReturn(Gpio::State::kLow);
    When(Method(mock_gpio3, Read)).AlwaysReturn(Gpio::State::kHigh);

    // Exercise
    uint32_t actual_read_value = test_subject.Read();

    // Verify
    CHECK(0b1001 == actual_read_value);

    Verify(
        Method(mock_gpio0, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio1, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio2, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio3, SetDirection).Using(sjsu::Gpio::Direction::kInput));
  }
}
}  // namespace sjsu
