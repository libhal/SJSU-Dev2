#include <iterator>

#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "L2_HAL/io/parallel_bus/parallel_gpio.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
namespace
{
Mock<sjsu::Gpio> mock_gpio0;
Mock<sjsu::Gpio> mock_gpio1;
Mock<sjsu::Gpio> mock_gpio2;
Mock<sjsu::Gpio> mock_gpio3;

Mock<sjsu::Pin> mock_pin0;
Mock<sjsu::Pin> mock_pin1;
Mock<sjsu::Pin> mock_pin2;
Mock<sjsu::Pin> mock_pin3;
}  // namespace

TEST_CASE("Testing Parallel Gpio Implementation", "[parallel-gpio]")
{
  Fake(Method(mock_gpio0, SetDirection));
  Fake(Method(mock_gpio1, SetDirection));
  Fake(Method(mock_gpio2, SetDirection));
  Fake(Method(mock_gpio3, SetDirection));

  Fake(Method(mock_pin0, SetAsOpenDrain));
  Fake(Method(mock_pin1, SetAsOpenDrain));
  Fake(Method(mock_pin2, SetAsOpenDrain));
  Fake(Method(mock_pin3, SetAsOpenDrain));

  When(Method(mock_gpio0, GetPin)).AlwaysReturn(mock_pin0.get());
  When(Method(mock_gpio1, GetPin)).AlwaysReturn(mock_pin1.get());
  When(Method(mock_gpio2, GetPin)).AlwaysReturn(mock_pin2.get());
  When(Method(mock_gpio3, GetPin)).AlwaysReturn(mock_pin3.get());

  sjsu::Gpio * const kGpioArray[] = {
    &mock_gpio0.get(),
    &mock_gpio1.get(),
    &mock_gpio2.get(),
    &mock_gpio3.get(),
  };

  auto clean_mock_history = [&]() {
    mock_gpio0.ClearInvocationHistory();
    mock_gpio1.ClearInvocationHistory();
    mock_gpio2.ClearInvocationHistory();
    mock_gpio3.ClearInvocationHistory();

    mock_pin0.ClearInvocationHistory();
    mock_pin1.ClearInvocationHistory();
    mock_pin2.ClearInvocationHistory();
    mock_pin3.ClearInvocationHistory();
  };

  sjsu::ParallelGpio test_subject(kGpioArray, std::size(kGpioArray));

  clean_mock_history();

  SECTION("Array Constructor && Initialize")
  {
    test_subject.Initialize();

    Verify(
        Method(mock_gpio0, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio1, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio2, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio3, SetDirection).Using(sjsu::Gpio::Direction::kInput));
  }
  // NOTE: the following test cannot be tested using clang version with address
  // sanitizer as it causes a "stack-use-after-scope" exception.
  //
  // SECTION("Initializer List Constructor")
  // {
  //   sjsu::ParallelGpio test_subject_init_list{{
  //       &mock_gpio0.get(),
  //       &mock_gpio1.get(),
  //       &mock_gpio2.get(),
  //       &mock_gpio3.get(),
  //   }};
  //   test_subject_init_list.Initialize();
  // }
  SECTION("SetDirection")
  {
    test_subject.SetDirection(sjsu::Gpio::Direction::kOutput);

    Verify(
        Method(mock_gpio0, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio1, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio2, SetDirection).Using(sjsu::Gpio::Direction::kOutput));
    Verify(
        Method(mock_gpio3, SetDirection).Using(sjsu::Gpio::Direction::kOutput));

    clean_mock_history();
    test_subject.SetDirection(sjsu::Gpio::Direction::kInput);

    Verify(
        Method(mock_gpio0, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio1, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio2, SetDirection).Using(sjsu::Gpio::Direction::kInput));
    Verify(
        Method(mock_gpio3, SetDirection).Using(sjsu::Gpio::Direction::kInput));
  }
  SECTION("SetAsOpenDrain()")
  {
    test_subject.SetAsOpenDrain();

    Verify(Method(mock_pin0, SetAsOpenDrain).Using(true));
    Verify(Method(mock_pin1, SetAsOpenDrain).Using(true));
    Verify(Method(mock_pin2, SetAsOpenDrain).Using(true));
    Verify(Method(mock_pin3, SetAsOpenDrain).Using(true));

    clean_mock_history();
    test_subject.SetAsOpenDrain(false);

    Verify(Method(mock_pin0, SetAsOpenDrain).Using(false));
    Verify(Method(mock_pin1, SetAsOpenDrain).Using(false));
    Verify(Method(mock_pin2, SetAsOpenDrain).Using(false));
    Verify(Method(mock_pin3, SetAsOpenDrain).Using(false));
  }
}
}  // namespace sjsu
