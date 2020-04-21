#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
EMIT_ALL_METHODS(Gpio);

namespace
{
bit::Mask Mask4Bit(const sjsu::Gpio & gpio)
{
  return {
    .position = static_cast<uint32_t>(gpio.GetPin().GetPin() * 4),
    .width    = 4,
  };
}
}  // namespace

TEST_CASE("Testing stm32f10x Gpio", "[stm32f10x-gpio]")
{
  Mock<SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  SystemController::SetPlatformController(&mock_system_controller.get());

  GPIO_TypeDef local_gpio_a;
  GPIO_TypeDef local_gpio_b;
  GPIO_TypeDef local_gpio_c;
  GPIO_TypeDef local_gpio_d;
  GPIO_TypeDef local_gpio_e;
  GPIO_TypeDef local_gpio_f;
  GPIO_TypeDef local_gpio_g;

  memset(&local_gpio_a, 0, sizeof(local_gpio_a));
  memset(&local_gpio_b, 0, sizeof(local_gpio_b));
  memset(&local_gpio_c, 0, sizeof(local_gpio_c));
  memset(&local_gpio_d, 0, sizeof(local_gpio_d));
  memset(&local_gpio_e, 0, sizeof(local_gpio_e));
  memset(&local_gpio_f, 0, sizeof(local_gpio_f));
  memset(&local_gpio_g, 0, sizeof(local_gpio_g));

  // The stm32f10x::Gpio class uses the stm32f10x::Pin registers directly
  Pin::gpio[0] = &local_gpio_a;
  Pin::gpio[1] = &local_gpio_b;
  Pin::gpio[2] = &local_gpio_c;
  Pin::gpio[3] = &local_gpio_d;
  Pin::gpio[4] = &local_gpio_e;
  Pin::gpio[5] = &local_gpio_f;
  Pin::gpio[6] = &local_gpio_g;

  struct TestStruct_t
  {
    sjsu::stm32f10x::Gpio gpio = stm32f10x::Gpio('A', 0);
    GPIO_TypeDef & reg;
    SystemController::PeripheralID id;
  };

  std::array<TestStruct_t, 12> test = {
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
  };

  SECTION("SetDirection()")
  {
    auto power_up_matcher =
        [](sjsu::SystemController::PeripheralID expected_id) {
          return [expected_id](sjsu::SystemController::PeripheralID actual_id) {
            return expected_id.device_id == actual_id.device_id;
          };
        };

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
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test[i].id)));
      mock_system_controller.ClearInvocationHistory();
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
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test[i].id)));
      CHECK(kOutputFullSpeedCode == bit::Extract(cr, Mask4Bit(test[i].gpio)));
      mock_system_controller.ClearInvocationHistory();
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
      CHECK(1 << (test[i].gpio.GetPin().GetPin() + 16) == test[i].reg.BSRR);
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
      CHECK(1 << test[i].gpio.GetPin().GetPin() == test[i].reg.BSRR);
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

  // The stm32f10x::Gpio class uses the stm32f10x::Pin registers directly
  Pin::gpio[0] = GPIOA;
  Pin::gpio[1] = GPIOB;
  Pin::gpio[2] = GPIOC;
  Pin::gpio[3] = GPIOD;
  Pin::gpio[4] = GPIOE;
  Pin::gpio[5] = GPIOF;
  Pin::gpio[6] = GPIOG;
}
}  // namespace sjsu::stm32f10x
