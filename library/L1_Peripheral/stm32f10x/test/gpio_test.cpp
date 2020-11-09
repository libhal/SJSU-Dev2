#include "L1_Peripheral/stm32f10x/gpio.hpp"

#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
EMIT_ALL_METHODS(Gpio);

namespace
{
bit::Mask Mask4Bit(sjsu::Gpio & gpio)
{
  return {
    .position = static_cast<uint32_t>(gpio.GetPin().GetPin() * 4),
    .width    = 4,
  };
}

IRQn GetIrqForPin(uint8_t pin)
{
  switch (pin)
  {
    case 0: return stm32f10x::EXTI0_IRQn; break;
    case 1: return stm32f10x::EXTI1_IRQn; break;
    case 2: return stm32f10x::EXTI2_IRQn; break;
    case 3: return stm32f10x::EXTI3_IRQn; break;
    case 4: return stm32f10x::EXTI4_IRQn; break;
    case 5: [[fallthrough]];
    case 6: [[fallthrough]];
    case 7: [[fallthrough]];
    case 8: [[fallthrough]];
    case 9: return stm32f10x::EXTI9_5_IRQn; break;
    case 10: [[fallthrough]];
    case 11: [[fallthrough]];
    case 12: [[fallthrough]];
    case 13: [[fallthrough]];
    case 14: [[fallthrough]];
    case 15: return stm32f10x::EXTI15_10_IRQn; break;
    default: return static_cast<stm32f10x::IRQn>(0xFFFF);
  }
}
}  // namespace

TEST_CASE("Testing stm32f10x Gpio")
{
  Mock<SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  InterruptController::SetPlatformController(&mock_interrupt_controller.get());

  GPIO_TypeDef local_gpio_a;
  GPIO_TypeDef local_gpio_b;
  GPIO_TypeDef local_gpio_c;
  GPIO_TypeDef local_gpio_d;
  GPIO_TypeDef local_gpio_e;
  GPIO_TypeDef local_gpio_f;
  GPIO_TypeDef local_gpio_g;
  AFIO_TypeDef local_afio;
  EXTI_TypeDef local_exti;

  testing::ClearStructure(&local_gpio_a);
  testing::ClearStructure(&local_gpio_b);
  testing::ClearStructure(&local_gpio_c);
  testing::ClearStructure(&local_gpio_d);
  testing::ClearStructure(&local_gpio_e);
  testing::ClearStructure(&local_gpio_f);
  testing::ClearStructure(&local_gpio_g);
  testing::ClearStructure(&local_afio);
  testing::ClearStructure(&local_exti);

  // The stm32f10x::Gpio class uses the stm32f10x::Pin registers directly
  Pin::gpio[0] = &local_gpio_a;
  Pin::gpio[1] = &local_gpio_b;
  Pin::gpio[2] = &local_gpio_c;
  Pin::gpio[3] = &local_gpio_d;
  Pin::gpio[4] = &local_gpio_e;
  Pin::gpio[5] = &local_gpio_f;
  Pin::gpio[6] = &local_gpio_g;
  Pin::afio    = &local_afio;

  Gpio::external_interrupt = &local_exti;

  struct TestStruct_t
  {
    sjsu::stm32f10x::Gpio gpio = stm32f10x::Gpio('A', 0);
    GPIO_TypeDef & reg;
    SystemController::ResourceID id;
  };

  std::array<TestStruct_t, 15> test = {
    TestStruct_t{
        .gpio = stm32f10x::Gpio('A', 0),  // A
        .reg  = local_gpio_a,
        .id   = stm32f10x::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('A', 4),  // Middle of first half word
        .reg  = local_gpio_a,
        .id   = stm32f10x::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('B', 0),  // B
        .reg  = local_gpio_b,
        .id   = stm32f10x::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('B', 7),  // End of first half word
        .reg  = local_gpio_b,
        .id   = stm32f10x::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('C', 0),  // C
        .reg  = local_gpio_c,
        .id   = stm32f10x::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('C', 8),  // First of last half word
        .reg  = local_gpio_c,
        .id   = stm32f10x::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('D', 0),  // D
        .reg  = local_gpio_d,
        .id   = stm32f10x::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('D', 12),  // Middle of last half word
        .reg  = local_gpio_d,
        .id   = stm32f10x::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('E', 0),  // E
        .reg  = local_gpio_e,
        .id   = stm32f10x::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('E', 15),  // Last of last half word
        .reg  = local_gpio_e,
        .id   = stm32f10x::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('F', 0),  // F
        .reg  = local_gpio_f,
        .id   = stm32f10x::SystemController::Peripherals::kGpioF,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('G', 0),  // G
        .reg  = local_gpio_g,
        .id   = stm32f10x::SystemController::Peripherals::kGpioG,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('G', 1),  // G
        .reg  = local_gpio_g,
        .id   = stm32f10x::SystemController::Peripherals::kGpioG,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('G', 2),  // G
        .reg  = local_gpio_g,
        .id   = stm32f10x::SystemController::Peripherals::kGpioG,
    },
    TestStruct_t{
        .gpio = stm32f10x::Gpio('G', 3),  // G
        .reg  = local_gpio_g,
        .id   = stm32f10x::SystemController::Peripherals::kGpioG,
    },
  };

  SECTION("Initialize()")
  {
    auto power_up_matcher = [](sjsu::SystemController::ResourceID expected_id) {
      return [expected_id](sjsu::SystemController::ResourceID actual_id) {
        return expected_id.device_id == actual_id.device_id;
      };
    };

    for (auto & test_subject : test)
    {
      // Setup
      INFO("Failure at Pin " << test_subject.gpio.GetPin().GetPort() << "["
                             << test_subject.gpio.GetPin().GetPin() << "]");

      // Exercise
      test_subject.gpio.Initialize();

      // Verify
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test_subject.id)));
      mock_system_controller.ClearInvocationHistory();
    }
  }

  SECTION("SetDirection()")
  {
    constexpr uint8_t kInputFloatingCode   = 0b0100;
    constexpr uint8_t kOutputFullSpeedCode = 0b0011;

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.CRL = 0xFFFF'FFFF;
      test[i].reg.CRH = 0xFFFF'FFFF;

      // Exercise
      test[i].gpio.SetAsInput();
      // Exercise: Combine the two registers into 1 variable to make extraction
      //           easier.
      uint64_t crh = test[i].reg.CRH;
      uint64_t crl = test[i].reg.CRL;
      uint64_t cr  = (crh << 32) | crl;

      // Verify
      CHECK(kInputFloatingCode == bit::Extract(cr, Mask4Bit(test[i].gpio)));
    }

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.CRL = 0xFFFF'FFFF;
      test[i].reg.CRH = 0xFFFF'FFFF;

      // Exercise
      test[i].gpio.SetAsOutput();
      // Exercise: Combine the two registers into 1 variable to make extraction
      //           easier. They should contain the newly updated control
      //           information.
      uint64_t crh = test[i].reg.CRH;
      uint64_t crl = test[i].reg.CRL;
      uint64_t cr  = (crh << 32) | crl;

      // Verify
      CHECK(kOutputFullSpeedCode == bit::Extract(cr, Mask4Bit(test[i].gpio)));
    }
  }

  SECTION("Set()")
  {
    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Set register to all zeros.
      //        A 1 in bits [0:15] will set the output HIGH.
      //        A 1 in bits [16:31] will set the output LOW.
      test[i].reg.BSRR = 0;

      // Exercise
      test[i].gpio.SetLow();

      // Verify
      CHECK((1 << (test[i].gpio.GetPin().GetPin() + 16)) == test[i].reg.BSRR);
    }

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Set register to all zeros.
      //        A 1 in bits [0:15] will set the output HIGH.
      //        A 1 in bits [16:31] will set the output LOW.
      test[i].reg.BSRR = 0;

      // Exercise
      test[i].gpio.SetHigh();

      // Verify
      CHECK((1 << test[i].gpio.GetPin().GetPin()) == test[i].reg.BSRR);
    }
  }

  SECTION("Toggle()")
  {
    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Initialize output register as zero
      test[i].reg.ODR = 0;

      // Exercise
      test[i].gpio.Toggle();
      bool should_be_set =
          bit::Read(test[i].reg.ODR, test[i].gpio.GetPin().GetPin());
      test[i].gpio.Toggle();
      bool should_be_cleared =
          bit::Read(test[i].reg.ODR, test[i].gpio.GetPin().GetPin());

      // Verify
      CHECK(should_be_set == true);
      CHECK(should_be_cleared == false);
    }
  }

  SECTION("Read()")
  {
    constexpr std::array<uint32_t, 2> kIdr = {
      0x5555'5555,
      0xAAAA'AAAA,
    };

    for (uint32_t i = 0; i < kIdr.size(); i++)
    {
      test[i].reg.IDR = kIdr[i];
      for (uint32_t j = 0; j < test.size(); j++)
      {
        // Setup
        INFO("Failure at index: " << j << " IDR: " << test[i].reg.IDR);
        // Setup: Initialize output register as all 1s
        bool expected_read =
            bit::Read(test[j].reg.IDR, test[j].gpio.GetPin().GetPin());

        // Exercise + Verify
        CHECK(expected_read == test[j].gpio.Read());
      }
    }
  }

  SECTION("AttachInterrupt() + InterruptHandler()")
  {
    for (auto edge : {
             Gpio::Edge::kFalling,
             Gpio::Edge::kRising,
             Gpio::Edge::kBoth,
         })
    {
      for (uint32_t j = 0; j < test.size(); j++)
      {
        // Setup
        INFO("Failure at index: " << j);

        // Setup: Clear the EXTI register
        testing::ClearStructure(&local_exti);
        testing::ClearStructure(&local_afio);

        // Setup: Create shorthand variables for port, pin and IRQ
        uint8_t pin       = test[j].gpio.GetPin().GetPin();
        uint8_t port      = test[j].gpio.GetPin().GetPort();
        auto expected_irq = GetIrqForPin(pin);

        // Setup: A function to determine if the InterruptHandler() is calling
        // the
        //        correct callback.
        bool callback_was_called   = false;
        InterruptCallback callback = [&callback_was_called]() {
          callback_was_called = true;
        };

        // Setup: The expected interrupt registration information to be used
        //        when AttachInterrupt() is called.
        auto expected_registration = InterruptController::RegistrationInfo_t{
          .interrupt_request_number = expected_irq,
          .interrupt_handler        = Gpio::InterruptHandler,
        };

        // Setup: Store the EXTICR control register into a variable to make the
        //        code cleaner.
        volatile uint32_t * control = &local_afio.EXTICR[pin / 4];

        // Setup: Define the mask within the EXTICR register for code clarity.
        auto interrupt_mask = bit::Mask{
          .position = static_cast<uint32_t>((pin * 4) % 16),
          .width    = 4,
        };

        // Setup: Set Pending Register (PR) for this interrupt to a 1, asserting
        //        that there was an interrupt event for this pin. This should
        //        cause Gpio::InterruptHandler() to call `callback()`.
        local_exti.PR = (1 << pin);

        // Exercise
        test[j].gpio.AttachInterrupt(callback, edge);

        // Exercise: Call the handler directly
        Gpio::InterruptHandler();

        // Verify
        CHECK(callback_was_called);

        if (edge == Gpio::Edge::kBoth)
        {
          CHECK(local_exti.RTSR == 1 << pin);
          CHECK(local_exti.FTSR == 1 << pin);
        }
        else if (edge == Gpio::Edge::kRising)
        {
          CHECK(local_exti.RTSR == 1 << pin);
          CHECK(local_exti.FTSR == 0);
        }
        else if (edge == Gpio::Edge::kFalling)
        {
          CHECK(local_exti.RTSR == 0);
          CHECK(local_exti.FTSR == 1 << pin);
        }

        CHECK(local_exti.IMR == 1 << pin);
        CHECK(*control == bit::Value(0).Insert(port - 'A', interrupt_mask));
        CHECK(local_exti.PR == (1 << pin));
        Verify(Method(mock_interrupt_controller, Enable)
                   .Using(expected_registration));
      }
    }
  }

  SECTION("DetachInterrupt()")
  {
    for (uint32_t j = 0; j < test.size(); j++)
    {
      // Setup
      INFO("Failure at index: " << j);

      // Setup: Create shorthand variables for port, pin and IRQ
      uint8_t pin = test[j].gpio.GetPin().GetPin();

      // Setup: Set both registers to all 1s
      local_exti.RTSR = std::numeric_limits<decltype(local_exti.RTSR)>::max();
      local_exti.FTSR = std::numeric_limits<decltype(local_exti.FTSR)>::max();

      // Setup: The register should contain all 1s except for a single 0 in the
      //        pin bit location.
      uint32_t expected_result = ~(1 << pin);

      // Exercise
      test[j].gpio.DetachInterrupt();

      // Verify
      // Verify: That the specific
      CHECK(local_exti.RTSR == expected_result);
      CHECK(local_exti.FTSR == expected_result);
    }
  }

  // The stm32f10x::Gpio class uses the stm32f10x::Pin registers directly
  Pin::gpio[0]             = GPIOA;
  Pin::gpio[1]             = GPIOB;
  Pin::gpio[2]             = GPIOC;
  Pin::gpio[3]             = GPIOD;
  Pin::gpio[4]             = GPIOE;
  Pin::gpio[5]             = GPIOF;
  Pin::gpio[6]             = GPIOG;
  Gpio::external_interrupt = EXTI;
}
}  // namespace sjsu::stm32f10x
