#include "peripherals/stm32f4xx/gpio.hpp"

#include <cstdint>

#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "testing/testing_frameworks.hpp"

namespace sjsu::stm32f4xx
{
namespace
{
bit::Mask Mask2Bit(sjsu::Gpio & gpio)
{
  return {
    .position = static_cast<uint8_t>(gpio.GetPin().GetPin() * 2),
    .width    = 2,
  };
}
}  // namespace

TEST_CASE("Testing stm32f4xx Gpio")
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

  testing::ClearStructure(&local_gpio_a);
  testing::ClearStructure(&local_gpio_b);
  testing::ClearStructure(&local_gpio_c);
  testing::ClearStructure(&local_gpio_d);
  testing::ClearStructure(&local_gpio_e);
  testing::ClearStructure(&local_gpio_f);
  testing::ClearStructure(&local_gpio_g);
  testing::ClearStructure(&local_gpio_h);
  testing::ClearStructure(&local_gpio_i);

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
    const SystemController::ResourceID & id;
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

      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.MODER = 0xFFFF'FFFF;

      // Exercise
      test_subject.gpio.Initialize();

      // Verify
      Verify(Method(mock_system_controller, PowerUpPeripheral)
                 .Matching(power_up_matcher(test_subject.id)));
    }
  }

  SECTION("SetDirection()")
  {
    // RM0090 p.281
    //
    // --> 00: Input (reset state)
    // --> 01: General purpose output mode
    //     10: Alternate function mode
    //     11: Analog mode

    constexpr uint8_t kInputCode  = 0b00;
    constexpr uint8_t kOutputCode = 0b01;

    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsInput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.MODER = 0xFFFF'FFFF;

      // Exercise
      test_subject.gpio.SetAsInput();

      // Verify
      CHECK(bit::Extract(test_subject.reg.MODER, Mask2Bit(test_subject.gpio)) ==
            kInputCode);
    }

    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsOutput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.MODER = 0xFFFF'FFFF;

      // Exercise
      test_subject.gpio.SetAsOutput();

      // Verify
      CHECK(bit::Extract(test_subject.reg.MODER, Mask2Bit(test_subject.gpio)) ==
            kOutputCode);
    }
  }

  SECTION("Set()")
  {
    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsInput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      // Setup: Set register to all zeros. A 1 in this field will set the bit if
      //        its in the first 15 bits. A 1 in the last 15 bits of the
      //        register will clear the output.
      test_subject.reg.BSRRH = 0;
      test_subject.reg.BSRRL = 0;

      // Exercise
      test_subject.gpio.SetLow();

      // Verify
      CHECK(bit::Read(test_subject.reg.BSRRH,
                      test_subject.gpio.GetPin().GetPin()));
      CHECK(!bit::Read(test_subject.reg.BSRRL,
                       test_subject.gpio.GetPin().GetPin()));
    }

    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsInput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test_subject.reg.BSRRH = 0;
      test_subject.reg.BSRRL = 0;

      // Exercise
      test_subject.gpio.SetHigh();

      // Verify
      CHECK(!bit::Read(test_subject.reg.BSRRH,
                       test_subject.gpio.GetPin().GetPin()));
      CHECK(bit::Read(test_subject.reg.BSRRL,
                      test_subject.gpio.GetPin().GetPin()));
    }
  }
  SECTION("Toggle()")
  {
    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsInput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      // Setup: Initialize output register as zero
      test_subject.reg.ODR = 0;

      // Exercise
      test_subject.gpio.Toggle();
      bool should_be_set =
          bit::Read(test_subject.reg.ODR, test_subject.gpio.GetPin().GetPin());
      test_subject.gpio.Toggle();
      bool should_be_cleared =
          bit::Read(test_subject.reg.ODR, test_subject.gpio.GetPin().GetPin());

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

    for (auto & test_subject : test)
    {
      // Setup
      INFO("SetAsInput Failure at Pin "
           << test_subject.gpio.GetPin().GetPort() << "["
           << test_subject.gpio.GetPin().GetPin() << "]");

      for (const auto & gpio_field : kIdr)
      {
        test_subject.reg.IDR = gpio_field;
        // Setup
        INFO(" IDR: " << gpio_field);

        // Setup: Initialize output register as all 1s
        bool expected_read = bit::Read(test_subject.reg.IDR,
                                       test_subject.gpio.GetPin().GetPin());

        // Exercise + Verify
        CHECK(expected_read == test_subject.gpio.Read());
      }
    }
  }
}
}  // namespace sjsu::stm32f4xx
