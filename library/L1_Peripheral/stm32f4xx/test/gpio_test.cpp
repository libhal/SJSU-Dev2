#include <cstdint>

#include "L0_Platform/stm32f4xx/stm32f4xx.h"
#include "L1_Peripheral/stm32f4xx/gpio.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f4xx
{
EMIT_ALL_METHODS(Gpio);

namespace
{
bit::Mask Mask2Bit(const sjsu::Gpio & gpio)
{
  return {
    .position = static_cast<uint8_t>(gpio.GetPin().GetPin() * 2),
    .width    = 2,
  };
}
}  // namespace

TEST_CASE("Testing stm32f4xx Gpio", "[stm32f4xx-gpio]")
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
  GPIO_TypeDef local_gpio_h;
  GPIO_TypeDef local_gpio_i;

  memset(&local_gpio_a, 0, sizeof(local_gpio_a));
  memset(&local_gpio_b, 0, sizeof(local_gpio_b));
  memset(&local_gpio_c, 0, sizeof(local_gpio_c));
  memset(&local_gpio_d, 0, sizeof(local_gpio_d));
  memset(&local_gpio_e, 0, sizeof(local_gpio_e));
  memset(&local_gpio_f, 0, sizeof(local_gpio_f));
  memset(&local_gpio_g, 0, sizeof(local_gpio_g));
  memset(&local_gpio_h, 0, sizeof(local_gpio_h));
  memset(&local_gpio_i, 0, sizeof(local_gpio_i));

  // The stm32f4xx::Gpio class uses the stm32f4xx::Pin registers directly
  Pin::gpio[0] = &local_gpio_a;
  Pin::gpio[1] = &local_gpio_b;
  Pin::gpio[2] = &local_gpio_c;
  Pin::gpio[3] = &local_gpio_d;
  Pin::gpio[4] = &local_gpio_e;
  Pin::gpio[5] = &local_gpio_f;
  Pin::gpio[6] = &local_gpio_g;
  Pin::gpio[7] = &local_gpio_h;
  Pin::gpio[8] = &local_gpio_i;

  struct TestStruct_t
  {
    sjsu::stm32f4xx::Gpio gpio;
    GPIO_TypeDef & reg;
    const SystemController::PeripheralID & id;
  };

  std::array<TestStruct_t, 14> test = {
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('A', 0),  // A
        .reg  = local_gpio_a,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('A', 4),  // Middle of first half word
        .reg  = local_gpio_a,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('B', 0),  // B
        .reg  = local_gpio_b,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('B', 7),  // End of first half word
        .reg  = local_gpio_b,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('C', 0),  // C
        .reg  = local_gpio_c,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('C', 8),  // First of last half word
        .reg  = local_gpio_c,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('D', 0),  // D
        .reg  = local_gpio_d,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('D', 12),  // Middle of last half word
        .reg  = local_gpio_d,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('E', 0),  // E
        .reg  = local_gpio_e,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('E', 15),  // Last of last half word
        .reg  = local_gpio_e,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('F', 0),  // F
        .reg  = local_gpio_f,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioF,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('G', 0),  // G
        .reg  = local_gpio_g,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioG,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('H', 0),  // H
        .reg  = local_gpio_h,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioH,
    },
    TestStruct_t{
        .gpio = stm32f4xx::Gpio('I', 0),  // I
        .reg  = local_gpio_i,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioI,
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

    // RM0090 p.281
    //
    // --> 00: Input (reset state)
    // --> 01: General purpose output mode
    //     10: Alternate function mode
    //     11: Analog mode

    constexpr uint8_t kInputCode  = 0b00;
    constexpr uint8_t kOutputCode = 0b01;

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.MODER = 0xFFFF'FFFF;

      // Exercise
      test[i].gpio.SetAsInput();

      // Verify
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test[i].id)));
      mock_system_controller.ClearInvocationHistory();
      CHECK(bit::Extract(test[i].reg.MODER, Mask2Bit(test[i].gpio)) ==
            kInputCode);
    }

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.MODER = 0xFFFF'FFFF;

      // Exercise
      test[i].gpio.SetAsOutput();

      // Verify
      // Verify: Should call Pin's Initialize method which simply calls
      //         PowerUpPeripheral()
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test[i].id)));
      mock_system_controller.ClearInvocationHistory();
      CHECK(bit::Extract(test[i].reg.MODER, Mask2Bit(test[i].gpio)) ==
            kOutputCode);
    }
  }
  SECTION("Set()")
  {
    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.BSRRH = 0;
      test[i].reg.BSRRL = 0;

      // Exercise
      test[i].gpio.SetLow();

      // Verify
      CHECK(bit::Read(test[i].reg.BSRRH, test[i].gpio.GetPin().GetPin()));
      CHECK(!bit::Read(test[i].reg.BSRRL, test[i].gpio.GetPin().GetPin()));
    }

    for (uint32_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure at index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.BSRRH = 0;
      test[i].reg.BSRRL = 0;

      // Exercise
      test[i].gpio.SetHigh();

      // Verify
      CHECK(!bit::Read(test[i].reg.BSRRH, test[i].gpio.GetPin().GetPin()));
      CHECK(bit::Read(test[i].reg.BSRRL, test[i].gpio.GetPin().GetPin()));
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
}
}  // namespace sjsu::stm32f4xx
