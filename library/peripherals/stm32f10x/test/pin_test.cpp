#include "peripherals/stm32f10x/pin.hpp"

#include <cstdint>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
namespace
{
bit::Mask Mask4Bit(const sjsu::Pin & pin)
{
  return {
    .position = static_cast<uint32_t>(pin.GetPin() * 4),
    .width    = 4,
  };
}
}  // namespace

TEST_CASE("Testing stm32f10x Pin")
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
  AFIO_TypeDef local_afio;

  testing::ClearStructure(&local_gpio_a);
  testing::ClearStructure(&local_gpio_b);
  testing::ClearStructure(&local_gpio_c);
  testing::ClearStructure(&local_gpio_d);
  testing::ClearStructure(&local_gpio_e);
  testing::ClearStructure(&local_gpio_f);
  testing::ClearStructure(&local_gpio_g);
  testing::ClearStructure(&local_gpio_h);
  testing::ClearStructure(&local_gpio_i);
  testing::ClearStructure(&local_afio);

  Pin::gpio[0] = &local_gpio_a;
  Pin::gpio[1] = &local_gpio_b;
  Pin::gpio[2] = &local_gpio_c;
  Pin::gpio[3] = &local_gpio_d;
  Pin::gpio[4] = &local_gpio_e;
  Pin::gpio[5] = &local_gpio_f;
  Pin::gpio[6] = &local_gpio_g;
  Pin::afio    = &local_afio;

  stm32f10x::Pin pin_a0('A', 0);    // A
  stm32f10x::Pin pin_a4('A', 4);    // Middle of first half word
  stm32f10x::Pin pin_b0('B', 0);    // B
  stm32f10x::Pin pin_b7('B', 7);    // End of first half word
  stm32f10x::Pin pin_c0('C', 0);    // C
  stm32f10x::Pin pin_c8('C', 8);    // First of last half word
  stm32f10x::Pin pin_d0('D', 0);    // D
  stm32f10x::Pin pin_d12('D', 12);  // Middle of last half word
  stm32f10x::Pin pin_e0('E', 0);    // E
  stm32f10x::Pin pin_e15('E', 15);  // Last of last half word
  stm32f10x::Pin pin_f0('F', 0);    // F
  stm32f10x::Pin pin_g0('G', 0);    // G

  auto power_up_matcher = [](sjsu::SystemController::ResourceID expected_id) {
    return [expected_id](sjsu::SystemController::ResourceID actual_id) {
      return expected_id.device_id == actual_id.device_id;
    };
  };

  struct TestStruct_t
  {
    sjsu::Pin & pin;
    GPIO_TypeDef & reg;
    SystemController::ResourceID id;
  };

  std::array<TestStruct_t, 12> test = {
    TestStruct_t{
        // 0
        .pin = pin_a0,
        .reg = local_gpio_a,
        .id  = stm32f10x::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        // 1
        .pin = pin_a4,
        .reg = local_gpio_a,
        .id  = stm32f10x::SystemController::Peripherals::kGpioA,
    },
    TestStruct_t{
        // 2
        .pin = pin_b0,
        .reg = local_gpio_b,
        .id  = stm32f10x::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        // 3
        .pin = pin_b7,
        .reg = local_gpio_b,
        .id  = stm32f10x::SystemController::Peripherals::kGpioB,
    },
    TestStruct_t{
        // 4
        .pin = pin_c0,
        .reg = local_gpio_c,
        .id  = stm32f10x::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        // 5
        .pin = pin_c8,
        .reg = local_gpio_c,
        .id  = stm32f10x::SystemController::Peripherals::kGpioC,
    },
    TestStruct_t{
        // 6
        .pin = pin_d0,
        .reg = local_gpio_d,
        .id  = stm32f10x::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        // 7
        .pin = pin_d12,
        .reg = local_gpio_d,
        .id  = stm32f10x::SystemController::Peripherals::kGpioD,
    },
    TestStruct_t{
        // 8
        .pin = pin_e0,
        .reg = local_gpio_e,
        .id  = stm32f10x::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        // 9
        .pin = pin_e15,
        .reg = local_gpio_e,
        .id  = stm32f10x::SystemController::Peripherals::kGpioE,
    },
    TestStruct_t{
        // 10
        .pin = pin_f0,
        .reg = local_gpio_f,
        .id  = stm32f10x::SystemController::Peripherals::kGpioF,
    },
    TestStruct_t{
        // 11
        .pin = pin_g0,
        .reg = local_gpio_g,
        .id  = stm32f10x::SystemController::Peripherals::kGpioG,
    },
  };

  SECTION("Initialize()")
  {
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

  SECTION("ConfigureFunction()")
  {
    SECTION("Gpio full speed")
    {
      constexpr uint8_t kGpioFullSpeedCode = 0b0011;

      for (size_t i = 0; i < test.size(); i++)
      {
        INFO("Set as GPIO: Failure on test index: " << i);
        // Setup
        // Setup: Fill with 1s so that by setting it to input they get replaced
        //        with the correct input code of zero.
        test[i].reg.CRL = 0xFFFF'FFFF;
        test[i].reg.CRH = 0xFFFF'FFFF;

        // Exercise
        test[i].pin.settings.function = 0;
        test[i].pin.settings.Floating();
        test[i].pin.Initialize();
        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;

        // Verify
        CHECK(kGpioFullSpeedCode == bit::Extract(cr, Mask4Bit(test[i].pin)));
      }
    }

    SECTION("Alternative function full speed")
    {
      constexpr uint8_t kAlternativeFullSpeedCode = 0b1011;

      for (size_t i = 0; i < test.size(); i++)
      {
        INFO("Set as Alternative: Failure on test index: " << i);
        // Setup
        // Setup: Fill with 1s so that by setting it to input they get replaced
        //        with the correct input code of zero.
        test[i].reg.CRL = 0xFFFF'FFFF;
        test[i].reg.CRH = 0xFFFF'FFFF;

        // Exercise
        test[i].pin.settings.function = 1;
        test[i].pin.settings.Floating();
        test[i].pin.Initialize();
        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;

        // Verify
        CHECK(bit::Extract(cr, Mask4Bit(test[i].pin)) ==
              kAlternativeFullSpeedCode);
      }
    }

    SECTION("Invalid function")
    {
      for (size_t i = 0; i < test.size(); i++)
      {
        // Exercise & Verify
        test[i].pin.settings.function = 3;
        SJ2_CHECK_EXCEPTION(test[i].pin.Initialize(),
                            std::errc::invalid_argument);
      }
    }
  }

  SECTION("ConfigurePullResistor()")
  {
    constexpr uint8_t kPullDownCode = 0b1000;
    constexpr uint8_t kFloating     = 0b0011;

    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);
      // Setup: Fill with 1s so that by setting it to input they get replaced
      //        with the correct input code of zero.
      test[i].reg.CRL = 0xFFFF'FFFF;
      test[i].reg.CRH = 0xFFFF'FFFF;

      {
        // Exercise
        test[i].pin.settings.function = 0;
        test[i].pin.settings.Floating();
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;

        // Verify
        CHECK(kFloating == bit::Extract(cr, Mask4Bit(test[i].pin)));
      }

      {
        // Exercise
        test[i].pin.settings.function = 0;
        test[i].pin.settings.PullUp();
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;
        uint32_t odr = test[i].reg.ODR;

        // Verify
        CHECK(bit::Extract(cr, Mask4Bit(test[i].pin)) == kPullDownCode);
        // Verify: ODR bit is 1 for pull up
        CHECK(bit::Read(odr, test[i].pin.GetPin()));
      }

      {
        // Exercise
        test[i].pin.settings.PullDown();
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;
        uint32_t odr = test[i].reg.ODR;

        // Verify
        CHECK(bit::Extract(cr, Mask4Bit(test[i].pin)) == kPullDownCode);
        // Verify: ODR bit is 0 for pull up
        CHECK(!bit::Read(odr, test[i].pin.GetPin()));
      }
    }
  }

  SECTION("ConfigureAsOpenDrain()")
  {
    constexpr uint32_t kOutputWithOpenDrain = 0b0111;
    constexpr uint32_t kOutputWithPushPull  = 0b0011;

    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);

      {
        // Setup: Fill with 1s so that by setting it to input they get replaced
        //        with the correct input code of zero.
        test[i].reg.CRL = 0;
        test[i].reg.CRH = 0;

        // Exercise
        test[i].pin.settings.open_drain = true;
        test[i].pin.settings.function   = 0;
        test[i].pin.settings.Floating();
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;
        uint32_t odr = test[i].reg.ODR;

        // Verify
        CHECK(kOutputWithOpenDrain == bit::Extract(cr, Mask4Bit(test[i].pin)));
        // Verify: ODR should be set to 0 (LOW Voltage)
        CHECK(!bit::Read(odr, test[i].pin.GetPin()));
      }

      {
        // Setup: Fill with 1s so that by setting it to input they get replaced
        //        with the correct input code of zero.
        test[i].reg.CRL = 0xFFFF'FFFF;
        test[i].reg.CRH = 0xFFFF'FFFF;

        // Exercise
        test[i].pin.settings.open_drain = false;
        test[i].pin.settings.function   = 0;
        test[i].pin.settings.Floating();
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;
        uint32_t odr = test[i].reg.ODR;

        // Verify
        CHECK(kOutputWithPushPull == bit::Extract(cr, Mask4Bit(test[i].pin)));
        // Verify: ODR should be set to 0 (LOW Voltage)
        CHECK(!bit::Read(odr, test[i].pin.GetPin()));
      }
    }
  }

  SECTION("ConfigureAsAnalogMode()")
  {
    constexpr uint8_t kAnalogCode = 0b0000;
    for (size_t i = 0; i < test.size(); i++)
    {
      // Setup
      INFO("Failure on test index: " << i);

      {
        // Setup
        // Setup: Fill with 1s so that by setting it to input they get replaced
        //        with the correct input code of zero.
        test[i].reg.CRL = 0xFFFF'FFFF;
        test[i].reg.CRH = 0xFFFF'FFFF;

        // Exercise
        test[i].pin.settings.as_analog = true;
        test[i].pin.Initialize();

        // Exercise: Combine the two registers into 1 variable to make
        //           extraction easier.
        uint64_t crh = test[i].reg.CRH;
        uint64_t crl = test[i].reg.CRL;
        uint64_t cr  = (crh << 32) | crl;

        // Verify
        CHECK(kAnalogCode == bit::Extract(cr, Mask4Bit(test[i].pin)));
      }
    }
  }

  SECTION("ReleaseJTAGPins()")
  {
    // Setup
    local_afio.MAPR = 0;

    // Exercise
    Pin::ReleaseJTAGPins();

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching(power_up_matcher(
                   stm32f10x::SystemController::Peripherals::kAFIO)));

    // Set the JTAG Release
    CHECK(0b010 == sjsu::bit::Extract(local_afio.MAPR,
                                      sjsu::bit::MaskFromRange(24, 26)));
  }

  Pin::gpio[0] = GPIOA;
  Pin::gpio[1] = GPIOB;
  Pin::gpio[2] = GPIOC;
  Pin::gpio[3] = GPIOD;
  Pin::gpio[4] = GPIOE;
  Pin::gpio[5] = GPIOF;
  Pin::gpio[6] = GPIOG;
  Pin::afio    = AFIO;
}
}  // namespace sjsu::stm32f10x
