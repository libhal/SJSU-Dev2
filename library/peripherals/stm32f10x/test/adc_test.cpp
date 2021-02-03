#include "peripherals/stm32f10x/adc.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
TEST_CASE("Testing stm32f10x ADC")
{
  Mock<SystemController> mock_controller;
  Fake(Method(mock_controller, PowerUpPeripheral));
  Fake(Method(mock_controller, IsPeripheralPoweredUp));

  SystemController::SetPlatformController(&mock_controller.get());

  Mock<sjsu::Pin> mock_adc_pin;
  Fake(Method(mock_adc_pin, ModuleInitialize));

  DMA_Channel_TypeDef local_dma;
  ADC_TypeDef local_adc;

  testing::ClearStructure(&local_adc);
  testing::ClearStructure(&local_dma);

  Adc::adc1 = &local_adc;
  Adc::dma  = &local_dma;

  Adc::Channel_t mock_channel = {
    .pin   = mock_adc_pin.get(),
    .index = 5,
  };

  Adc test_subject(mock_channel);

  SECTION("Initialize: Verify Calibration Lock")
  {
    const auto kPeripheralDataAddress =
        reinterpret_cast<intptr_t>(&local_adc.DR);
    const auto kRamAddress =
        reinterpret_cast<intptr_t>(Adc::adc_storage.data());
    constexpr uint32_t kExpectedControl2 =
        bit::Value(Adc::Control2::kDefault)
            .Clear(Adc::Control2::kCalibration)
            .Set(Adc::Control2::kAdcOn)
            .Set(Adc::Control2::kSoftwareStart);

    testing::PollingVerification({
        .locking_function =
            [&local_adc]() {
              bit::Register(&local_adc.CR2)
                  .Clear(Adc::Control2::kCalibration)
                  .Save();
            },
        .polling_function =
            [&test_subject, &mock_controller]() {
              test_subject.ModuleInitialize();
            },
        .release_function =
            [&local_adc]() {
              bit::Register(&local_adc.CR2)
                  .Clear(Adc::Control2::kCalibration)
                  .Save();
            },
    });

    // Verify
    Verify(Method(mock_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kAdc1))
        .Once();
    Verify(Method(mock_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kDma1))
        .Once();

    CHECK(Adc::RegularSequence::kRegisterSequence3 == local_adc.SQR3);
    CHECK(Adc::RegularSequence::kRegisterSequence2 == local_adc.SQR2);
    CHECK(Adc::RegularSequence::kRegisterSequence1 == local_adc.SQR1);
    CHECK(Adc::Control1::kDefault == local_adc.CR1);
    CHECK(kExpectedControl2 == local_adc.CR2);

    CHECK(Adc::adc_storage.size() == local_dma.CNDTR);
    CHECK(static_cast<uint32_t>(kPeripheralDataAddress) == local_dma.CPAR);
    CHECK(static_cast<uint32_t>(kRamAddress) == local_dma.CMAR);
    CHECK(Adc::kDmaSettings == local_dma.CCR);

    Verify(Method(mock_adc_pin, ModuleInitialize)).Once();
    CHECK(mock_adc_pin.get().CurrentSettings().as_analog == true);
  }

  SECTION("ReferenceVoltage()")
  {
    // CHECK(test_subject.ReferenceVoltage() == 3.3_V);
  }

  SECTION("GetActiveBits()")
  {
    CHECK(test_subject.GetActiveBits() == 12);
  }

  SECTION("Read()")
  {
    {
      // Setup
      Adc::adc_storage[5] = 4000;

      // Exercise + Verify
      CHECK(test_subject.Read() == 4000);
    }

    {
      // Setup
      Adc::adc_storage[5] = 1234;

      // Exercise + Verify
      CHECK(test_subject.Read() == 1234);
    }

    {
      // Setup
      Adc::adc_storage[5] = 11;

      // Exercise + Verify
      CHECK(test_subject.Read() == 11);
    }
  }

  Adc::adc1 = ADC1;
}
}  // namespace sjsu::stm32f10x
