#include "L1_Peripheral/hardware_counter.hpp"

#include "L4_Testing/testing_frameworks.hpp"

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
  Fake(Method(mock_pin, ConfigurePullResistor));

  // Create mocked versions of the sjsu::Pin
  Mock<sjsu::Gpio> mock_gpio;
  Fake(Method(mock_gpio, ModuleInitialize));
  Fake(Method(mock_gpio, ModuleEnable));
  Fake(Method(mock_gpio, SetDirection));
  Fake(Method(mock_gpio, AttachInterrupt));
  Fake(Method(mock_gpio, DetachInterrupt));
  When(Method(mock_gpio, GetPin)).AlwaysReturn(mock_pin.get());

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
      Verify(Method(mock_gpio, ModuleInitialize));
      Verify(Method(mock_gpio, SetDirection).Using(Gpio::Direction::kInput));
      Verify(Method(mock_pin, ConfigurePullResistor)
                 .Using(Pin::Resistor::kPullUp));
    }

    SECTION("Use passed pull resistor settings")
    {
      // Setup
      GpioCounter test_subject(
          gpio, Gpio::Edge::kBoth, Pin::Resistor::kPullDown);

      // Exercise
      test_subject.Initialize();

      // Verify
      Verify(Method(mock_gpio, ModuleInitialize));
      Verify(Method(mock_gpio, SetDirection).Using(Gpio::Direction::kInput));
      Verify(Method(mock_pin, ConfigurePullResistor)
                 .Using(Pin::Resistor::kPullDown));
    }
  }

  SECTION("Enable()")
  {
    // Setup
    GpioCounter test_subject(gpio, Gpio::Edge::kFalling);

    // Exercise
    test_subject.SetStateToInitialized();
    test_subject.Enable();

    // Verify
    Verify(Method(mock_gpio, AttachInterrupt).Using(_, Gpio::Edge::kFalling))
        .Once();
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
    When(Method(mock_gpio, AttachInterrupt)).AlwaysDo(GetLambda(callback));
    Fake(Method(mock_gpio, DetachInterrupt));
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.Initialize();
    test_subject.Enable();

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

  SECTION("Disable() + ~GpioCounter()")
  {
    // Setup
    sjsu::InterruptCallback callback;
    Fake(Method(mock_gpio, DetachInterrupt));
    GpioCounter test_subject(gpio, Gpio::Edge::kBoth);

    // Exercise
    test_subject.SetStateToEnabled();
    test_subject.Enable(false);
    Verify(Method(mock_gpio, DetachInterrupt)).Once();

    test_subject.~GpioCounter();
    Verify(Method(mock_gpio, DetachInterrupt)).Twice();
  }
}
}  // namespace sjsu
