#include "peripherals/hardware_counter.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
// EMIT_ALL_METHODS macro cannot be used with GpioCounter as it contains
// atomics.

namespace
{
auto GetLambda(sjsu::InterruptCallback & isr)
{
  return [&isr](sjsu::InterruptCallback callback, Gpio::Edge) -> void {
    isr = callback;
  };
}
}  // namespace

TEST_CASE("Testing L1 GpioCounter")
{
  Mock<sjsu::Pin> mock_pin;

  // Create mocked versions of the sjsu::Pin
  Mock<sjsu::Gpio> mock_gpio;
  Fake(Method(mock_gpio, Gpio::ModuleInitialize));
  Fake(Method(mock_gpio, Gpio::SetDirection));
  Fake(Method(mock_gpio, Gpio::AttachInterrupt));
  Fake(Method(mock_gpio, Gpio::DetachInterrupt));
  When(Method(mock_gpio, Gpio::GetPin)).AlwaysReturn(mock_pin.get());

  sjsu::Gpio & gpio = mock_gpio.get();

  SECTION("Initialize()")
  {
    SECTION("Default pull up resistor")
    {
      // Setup
      GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

      // Exercise
      test_subject.Initialize();

      // Verify
      Verify(Method(mock_gpio, Gpio::ModuleInitialize));
      Verify(
          Method(mock_gpio, Gpio::SetDirection).Using(Gpio::Direction::kInput));
      Verify(
          Method(mock_gpio, Gpio::AttachInterrupt).Using(_, Gpio::Edge::kBoth))
          .Once();
      CHECK(mock_pin.get().settings.resistor ==
            PinSettings_t::Resistor::kPullUp);
    }

    SECTION("Use passed pull resistor settings")
    {
      // Setup
      GpioCounter test_subject(
          gpio, Gpio::Edge::kBoth, PinSettings_t::Resistor::kPullDown);

      // Exercise
      test_subject.Initialize();

      // Verify
      Verify(Method(mock_gpio, Gpio::ModuleInitialize));
      Verify(
          Method(mock_gpio, Gpio::SetDirection).Using(Gpio::Direction::kInput));
      Verify(
          Method(mock_gpio, Gpio::AttachInterrupt).Using(_, Gpio::Edge::kBoth))
          .Once();
      CHECK(mock_pin.get().settings.resistor ==
            PinSettings_t::Resistor::kPullDown);
    }
  }

  SECTION("Set() & Get()")
  {
    // Setup
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.Initialize();
    test_subject.Set(15);

    // Verify
    CHECK(test_subject.GetCount() == 15);
  }

  SECTION("Enable() + SetDirection()")
  {
    // Setup
    constexpr int kCountUps   = 12;
    constexpr int kCountDowns = 5;
    sjsu::InterruptCallback callback;
    When(Method(mock_gpio, Gpio::AttachInterrupt))
        .AlwaysDo(GetLambda(callback));
    Fake(Method(mock_gpio, Gpio::DetachInterrupt));
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.Initialize();

    // Exercise: (1) Count up
    test_subject.SetDirection(HardwareCounter::Direction::kUp);
    for (int i = 0; i < kCountUps; i++)
    {
      callback();
    }

    // Verify: (1)
    CHECK(test_subject.GetCount() == kCountUps);

    // Exercise: (2) Count down
    test_subject.SetDirection(HardwareCounter::Direction::kDown);
    for (int i = 0; i < kCountDowns; i++)
    {
      callback();
    }

    // Verify: (2)
    CHECK(test_subject.GetCount() == kCountUps - kCountDowns);
  }

  SECTION("PowerDown()")
  {
    // Setup
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.Initialize();
    test_subject.PowerDown();

    // Verify
    Verify(Method(mock_gpio, Gpio::DetachInterrupt)).Once();
  }

  SECTION("~GpioCounter()")
  {
    // Setup
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.~GpioCounter();

    // Verify
    Verify(Method(mock_gpio, Gpio::DetachInterrupt)).Once();
  }
}
}  // namespace sjsu
