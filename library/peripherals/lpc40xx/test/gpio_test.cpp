#include "peripherals/lpc40xx/gpio.hpp"

#include <cstdint>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/cortex/interrupt.hpp"
#include "testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx Gpio")
{
  // Declared constants that are to be used within the different sections
  // of this unit test
  constexpr uint8_t kPin0 = 0;
  constexpr uint8_t kPin7 = 7;

  // Initialized local LPC_GPIO_TypeDef objects with 0 to observe how the Gpio
  // class manipulates the data in the registers
  LPC_GPIO_TypeDef local_gpio_port[2];
  // Simulated version of LPC_GPIOINT
  LPC_GPIOINT_TypeDef local_eint;

  testing::ClearStructure(&local_gpio_port);
  testing::ClearStructure(&local_eint);

  // Create mock pins
  Mock<sjsu::Pin> mock_pin0;
  Mock<sjsu::Pin> mock_pin1;

  // Get gpio register pointer and replace the address with the local GPIOs.
  // Only GPIO port 1 & 2 will be used in this unit test
  auto ** gpio_register0 = sjsu::lpc40xx::Gpio::GpioRegister(0);
  auto ** gpio_register1 = sjsu::lpc40xx::Gpio::GpioRegister(1);
  *gpio_register0        = &local_gpio_port[0];
  *gpio_register1        = &local_gpio_port[1];

  auto * interrupt0 = Gpio::InterruptRegister(0);
  auto * interrupt2 = Gpio::InterruptRegister(1);
  // Reassign external interrupt registers to simulated LPC_GPIOINT
  interrupt0->rising_status  = &(local_eint.IO0IntStatR);
  interrupt0->falling_status = &(local_eint.IO0IntStatF);
  interrupt0->clear          = &(local_eint.IO0IntClr);
  interrupt0->rising_enable  = &(local_eint.IO0IntEnR);
  interrupt0->falling_enable = &(local_eint.IO0IntEnF);

  interrupt2->rising_status  = &(local_eint.IO2IntStatR);
  interrupt2->falling_status = &(local_eint.IO2IntStatF);
  interrupt2->clear          = &(local_eint.IO2IntClr);
  interrupt2->rising_enable  = &(local_eint.IO2IntEnR);
  interrupt2->falling_enable = &(local_eint.IO2IntEnF);

  // Create
  Gpio p0_00(0, 0, &mock_pin0.get());
  Gpio p1_07(1, 7, &mock_pin1.get());

  SECTION("Initialize()")
  {
    Fake(Method(mock_pin0, Pin::ModuleInitialize));
    Fake(Method(mock_pin1, Pin::ModuleInitialize));

    p0_00.Initialize();
    p1_07.Initialize();

    CHECK(mock_pin0.get().CurrentSettings().function == 0);
    CHECK(mock_pin1.get().CurrentSettings().function == 0);
  }

  SECTION("Set as Output and Input")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 96 page 148
    constexpr uint8_t kInputSet  = 0b0;
    constexpr uint8_t kOutputSet = 0b1;

    p0_00.SetAsInput();
    p1_07.SetAsOutput();

    // Check bit 0 of local_gpio_port[0].DIR (port 0 pin 0)
    // to see if it is cleared
    CHECK(bit::Read(local_gpio_port[0].DIR, kPin0) == kInputSet);
    // Check bit 7 of local_gpio_port[1].DIR (port 1 pin 7)
    // to see if it is set
    CHECK(bit::Read(local_gpio_port[1].DIR, kPin7) == kOutputSet);

    p0_00.SetDirection(sjsu::Gpio::kOutput);
    p1_07.SetDirection(sjsu::Gpio::kInput);
    // Check bit 0 of local_gpio_port[0].DIR (port 0 pin 0)
    // to see if it is set
    CHECK(bit::Read(local_gpio_port[0].DIR, kPin0) == kOutputSet);
    // Check bit 7 of local_gpio_port[1].DIR (port 1 pin 7)
    // to see if it is cleared
    CHECK(bit::Read(local_gpio_port[1].DIR, kPin7) == kInputSet);
  }

  SECTION("Set High")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 99 page 149
    constexpr uint8_t kHighSet = 0b1;

    p0_00.SetHigh();
    p1_07.Set(sjsu::Gpio::kHigh);

    // Check bit 0 of local_gpio_port[0].SET (port 0 pin 0)
    // to see if it is set
    CHECK(((local_gpio_port[0].SET >> kPin0) & 1) == kHighSet);
    // Check bit 7 of local_gpio_port[1].SET (port 1 pin 7)
    // to see if it is set
    CHECK(((local_gpio_port[1].SET >> kPin7) & 1) == kHighSet);
  }
  SECTION("Set Low")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 100 page 150
    constexpr uint8_t kLowSet = 0b1;

    p0_00.SetLow();
    p1_07.Set(sjsu::Gpio::kLow);

    // Check bit 0 of local_gpio_port[0].CLR (port 0 pin 0)
    // to see if it is set
    CHECK(((local_gpio_port[0].CLR >> kPin0) & 1) == kLowSet);
    // Check bit 7 of local_gpio_port[1].CLR (port 1 pin 7)
    // to see if it is set
    CHECK(((local_gpio_port[1].CLR >> kPin7) & 1) == kLowSet);
  }
  SECTION("Read Pin")
  {
    // Clearing bit 0 of local_gpio_port[0].PIN (port 0 pin 0) in order to
    // read the pin value through the Read method
    local_gpio_port[0].PIN &= ~(1 << kPin0);
    // Setting bit 7 of local_gpio_port[1].PIN (port 1 pin 7) in order to
    // read the pin value through the Read method
    local_gpio_port[1].PIN |= (1 << kPin7);

    CHECK(p0_00.Read() == false);
    CHECK(p1_07.Read() == true);
  }
  SECTION("Toggle")
  {
    // Clearing bit 0 of local_gpio_port[0].PIN (port 0 pin 0) in order to
    // read the pin value through the Read method
    local_gpio_port[0].PIN &= ~(1 << kPin0);
    // Setting bit 7 of local_gpio_port[1].PIN (port 1 pin 7) in order to
    // read the pin value through the Read method
    local_gpio_port[1].PIN |= (1 << kPin7);
    // Should change to 1
    p0_00.Toggle();
    // Should change to 0
    p1_07.Toggle();

    CHECK(p0_00.Read() == true);
    CHECK(p1_07.Read() == false);
  }
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}

FAKE_VOID_FUNC(InterruptCallback0);
FAKE_VOID_FUNC(InterruptCallback1);

TEST_CASE("Testing lpc40xx Gpio External Interrupts")
{
  // Declared constants that are to be used within the different sections
  // of this unit test
  constexpr uint8_t kPin15 = 15;
  constexpr uint8_t kPin7  = 7;
  constexpr uint8_t kPort0 = 0;
  constexpr uint8_t kPort2 = 1;

  // Simulated version of LPC_GPIOINT
  LPC_GPIOINT_TypeDef local_eint;
  testing::ClearStructure(&local_eint);

  auto * interrupt0 = Gpio::InterruptRegister(0);
  auto * interrupt2 = Gpio::InterruptRegister(1);
  // Reassign external interrupt registers to simulated LPC_GPIOINT
  interrupt0->rising_status  = &(local_eint.IO0IntStatR);
  interrupt0->falling_status = &(local_eint.IO0IntStatF);
  interrupt0->clear          = &(local_eint.IO0IntClr);
  interrupt0->rising_enable  = &(local_eint.IO0IntEnR);
  interrupt0->falling_enable = &(local_eint.IO0IntEnF);

  interrupt2->rising_status  = &(local_eint.IO2IntStatR);
  interrupt2->falling_status = &(local_eint.IO2IntStatF);
  interrupt2->clear          = &(local_eint.IO2IntClr);
  interrupt2->rising_enable  = &(local_eint.IO2IntEnR);
  interrupt2->falling_enable = &(local_eint.IO2IntEnF);

  // Pins that are to be used in the unit test
  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  Fake(Method(mock_interrupt_controller, Disable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  // Create mock pin
  Mock<sjsu::Pin> mock_pin;
  Fake(Method(mock_pin, Pin::ModuleInitialize));

  Gpio p0_15(0, 15, &mock_pin.get());
  Gpio p2_7(2, 7, &mock_pin.get());

  SECTION("Attach then Detattach Interrupt from pin")
  {
    // Setup & Execute
    p0_15.AttachInterrupt(&InterruptCallback0, sjsu::Gpio::Edge::kBoth);
    p2_7.AttachInterrupt(&InterruptCallback1, sjsu::Gpio::Edge::kRising);
    // Verify
    CHECK(bit::Read(local_eint.IO0IntEnR, kPin15));
    CHECK(bit::Read(local_eint.IO0IntEnF, kPin15));
    CHECK(bit::Read(local_eint.IO2IntEnR, kPin7));
    CHECK(!bit::Read(local_eint.IO2IntEnF, kPin7));

    // Verify: Check Developer's ISR is attached
    auto * save_callback0 =
        p0_15.handlers[kPort0][kPin15].target<void (*)(void)>();
    auto * save_callback1 =
        p2_7.handlers[kPort2][kPin7].target<void (*)(void)>();
    REQUIRE(save_callback0 != nullptr);
    REQUIRE(save_callback1 != nullptr);
    CHECK(&InterruptCallback0 == *save_callback0);
    CHECK(&InterruptCallback1 == *save_callback1);

    // Setup & Execute
    p0_15.DetachInterrupt();
    p2_7.DetachInterrupt();

    // Verify
    CHECK(!bit::Read(local_eint.IO0IntEnR, kPin15));
    CHECK(!bit::Read(local_eint.IO0IntEnF, kPin15));
    CHECK(!bit::Read(local_eint.IO2IntEnR, kPin7));
    CHECK(!bit::Read(local_eint.IO2IntEnF, kPin7));
    CHECK(p0_15.handlers[kPort0][kPin15] == nullptr);
    CHECK(p2_7.handlers[kPort2][kPin7] == nullptr);
  }

  SECTION("Call the Interrupt handler to service the pin.")
  {
    // Setup
    bool was_called = false;
    p0_15.AttachInterrupt([&was_called]() { was_called = true; },
                          sjsu::Gpio::Edge::kBoth);
    // Setup: Manually trigger an Interrupt
    *Gpio::InterruptStatus() |= (1 << kPort0);
    local_eint.IO0IntStatR |= (1 << kPin15);

    // Execute
    p0_15.InterruptHandler();

    // Verify
    CHECK(bit::Read(local_eint.IO0IntClr, kPin15));
    CHECK(was_called);
  }
}
}  // namespace sjsu::lpc40xx
