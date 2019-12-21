#include <cstdint>

#include "L0_Platform/stm32f4xx/stm32f4xx.h"
#include "L1_Peripheral/stm32f4xx/pin.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f4xx
{
EMIT_ALL_METHODS(Pin);

namespace
{
bit::Mask Mask2Bit(const sjsu::Pin & pin)
{
  return {
    .position = static_cast<uint8_t>(pin.GetPin() * 2),
    .width    = 2,
  };
}
}  // namespace

TEST_CASE("Testing stm32f4xx Pin", "[stm32f4xx-pin]")
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

  Pin::gpio[0] = &local_gpio_a;
  Pin::gpio[1] = &local_gpio_b;
  Pin::gpio[2] = &local_gpio_c;
  Pin::gpio[3] = &local_gpio_d;
  Pin::gpio[4] = &local_gpio_e;
  Pin::gpio[5] = &local_gpio_f;
  Pin::gpio[6] = &local_gpio_g;
  Pin::gpio[7] = &local_gpio_h;
  Pin::gpio[8] = &local_gpio_i;

  stm32f4xx::Pin pin_a0('A', 0);    // A
  stm32f4xx::Pin pin_a4('A', 4);    // Middle of first half word
  stm32f4xx::Pin pin_b0('B', 0);    // B
  stm32f4xx::Pin pin_b7('B', 7);    // End of first half word
  stm32f4xx::Pin pin_c0('C', 0);    // C
  stm32f4xx::Pin pin_c8('C', 8);    // First of last half word
  stm32f4xx::Pin pin_d0('D', 0);    // D
  stm32f4xx::Pin pin_d12('D', 12);  // Middle of last half word
  stm32f4xx::Pin pin_e0('E', 0);    // E
  stm32f4xx::Pin pin_e15('E', 15);  // Last of last half word
  stm32f4xx::Pin pin_f0('F', 0);    // F
  stm32f4xx::Pin pin_g0('G', 0);    // G
  stm32f4xx::Pin pin_h0('H', 0);    // H
  stm32f4xx::Pin pin_i0('I', 0);    // I

  struct TestStruct_t
  {
    sjsu::Pin & pin;
    GPIO_TypeDef & gpio;
    const SystemController::PeripheralID & id;
  };

  std::array<TestStruct_t, 14> test = {
    TestStruct_t{
        .pin  = pin_a0,
        .gpio = local_gpio_a,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .pin  = pin_a4,
        .gpio = local_gpio_a,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        .pin  = pin_b0,
        .gpio = local_gpio_b,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .pin  = pin_b7,
        .gpio = local_gpio_b,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        .pin  = pin_c0,
        .gpio = local_gpio_c,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .pin  = pin_c8,
        .gpio = local_gpio_c,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        .pin  = pin_d0,
        .gpio = local_gpio_d,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .pin  = pin_d12,
        .gpio = local_gpio_d,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        .pin  = pin_e0,
        .gpio = local_gpio_e,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .pin  = pin_e15,
        .gpio = local_gpio_e,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        .pin  = pin_f0,
        .gpio = local_gpio_f,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioF,
    },
    TestStruct_t{
        .pin  = pin_g0,
        .gpio = local_gpio_g,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioG,
    },
    TestStruct_t{
        .pin  = pin_h0,
        .gpio = local_gpio_h,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioH,
    },
    TestStruct_t{
        .pin  = pin_i0,
        .gpio = local_gpio_i,
        .id   = stm32f4xx::SystemController::Peripherals::kGpioI,
    },
  };

  SECTION("Initialize()")
  {
    auto power_up_matcher =
        [](sjsu::SystemController::PeripheralID expected_id) {
          return [expected_id](sjsu::SystemController::PeripheralID actual_id) {
            return expected_id.device_id == actual_id.device_id;
          };
        };

    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);

      // Exercise
      test[i].pin.Initialize();

      // Verify
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test[i].id)));
      // Cleanup
      mock_system_controller.ClearInvocationHistory();
    }
  }

  SECTION("SetPinFunction()")
  {
    auto get_4bit_pin_mask = [](const sjsu::Pin & pin) -> bit::Mask {
      return {
        .position = static_cast<uint8_t>((pin.GetPin() % 8) * 4),
        .width    = 4,
      };
    };
    // Setup
    constexpr uint8_t kExpectedFunction[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    };
    constexpr uint8_t kAlternativeFunctionCode = 0b10;

    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);
      test[i].pin.Initialize();

      // Exercise
      test[i].pin.SetPinFunction(kExpectedFunction[i]);

      // Verify
      CHECK(bit::Extract(test[i].gpio.MODER, Mask2Bit(test[i].pin)) ==
            kAlternativeFunctionCode);

      uint32_t afr_index = test[i].pin.GetPin() / 8;
      CHECK(bit::Extract(test[i].gpio.AFR[afr_index],
                         get_4bit_pin_mask(test[i].pin)) ==
            kExpectedFunction[i]);
    }
  }

  SECTION("SetPull()")
  {
    // 00: No pull-up, pull-down
    // 01: Pull-up
    // 10: Pull-down
    // 11: Reserved

    constexpr uint8_t kExpectedNoPull   = 0b00;
    constexpr uint8_t kExpectedPullUp   = 0b01;
    constexpr uint8_t kExpectedPullDown = 0b10;

    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);
      test[i].pin.Initialize();

      {
        // Exercise
        test[i].pin.SetFloating();
        // Verify
        CHECK(bit::Extract(test[i].gpio.PUPDR, Mask2Bit(test[i].pin)) ==
              kExpectedNoPull);
      }

      {
        // Exercise
        test[i].pin.PullUp();
        // Verify
        CHECK(bit::Extract(test[i].gpio.PUPDR, Mask2Bit(test[i].pin)) ==
              kExpectedPullUp);
      }

      {
        // Exercise
        test[i].pin.PullDown();
        // Verify
        CHECK(bit::Extract(test[i].gpio.PUPDR, Mask2Bit(test[i].pin)) ==
              kExpectedPullDown);
      }
    }
  }

  SECTION("SetAsOpenDrain()")
  {
    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);
      test[i].pin.Initialize();

      {
        // Exercise
        test[i].pin.SetAsOpenDrain(true);

        // Verify
        CHECK(bit::Read(test[i].gpio.OTYPER, test[i].pin.GetPin()));
      }

      {
        // Exercise
        test[i].pin.SetAsOpenDrain(false);

        // Verify
        CHECK(!bit::Read(test[i].gpio.OTYPER, test[i].pin.GetPin()));
      }
    }
  }

  SECTION("SetAsAnalogMode()")
  {
    constexpr uint8_t kAnalogCode = 0b11;
    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);
      test[i].pin.Initialize();
      {
        // Setup
        test[i].gpio.MODER = 0;

        // Exercise
        test[i].pin.SetAsAnalogMode(true);

        // Verify
        CHECK(bit::Extract(test[i].gpio.MODER, Mask2Bit(test[i].pin)) ==
              kAnalogCode);
      }
      {
        // Setup
        test[i].gpio.MODER = 0;

        // Exercise
        test[i].pin.SetAsAnalogMode(true);
        test[i].pin.SetAsAnalogMode(false);

        // Verify
        CHECK(bit::Extract(test[i].gpio.MODER, Mask2Bit(test[i].pin)) ==
              kAnalogCode);
      }
      {
        // Setup
        test[i].gpio.MODER = 0;

        // Exercise
        test[i].pin.SetAsAnalogMode(false);

        // Verify
        CHECK(bit::Extract(test[i].gpio.MODER, Mask2Bit(test[i].pin)) == 0b00);
      }
    }
  }

  Pin::gpio[0] = GPIOA;
  Pin::gpio[1] = GPIOB;
  Pin::gpio[2] = GPIOC;
  Pin::gpio[3] = GPIOD;
  Pin::gpio[4] = GPIOE;
  Pin::gpio[5] = GPIOF;
  Pin::gpio[6] = GPIOG;
  Pin::gpio[7] = GPIOH;
  Pin::gpio[8] = GPIOI;
}
}  // namespace sjsu::stm32f4xx
