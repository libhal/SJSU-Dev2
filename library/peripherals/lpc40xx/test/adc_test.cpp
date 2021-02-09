#include "peripherals/lpc40xx/adc.hpp"

#include <span>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "testing/peripherals.hpp"
#include "testing/testing_frameworks.hpp"
#include "utility/math/bit.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx adc")
{
  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Set mock for sjsu::Pin
  constexpr uint8_t kPinFunction = 0b101;

  // Set mock for sjsu::Pin
  Mock<sjsu::Pin> mock_adc_pin0;
  Fake(Method(mock_adc_pin0, ModuleInitialize));

  Mock<sjsu::Pin> mock_adc_pin1;
  Fake(Method(mock_adc_pin1, ModuleInitialize));

  const Adc::Channel_t kMockChannel0 = {
    .adc_pin      = mock_adc_pin0.get(),
    .channel      = 0,
    .pin_function = kPinFunction,
  };

  const Adc::Channel_t kMockChannel1 = {
    .adc_pin      = mock_adc_pin1.get(),
    .channel      = 1,
    .pin_function = kPinFunction,
  };

  // Create ports and pins to test and mock
  Adc test_subject0(kMockChannel0);
  Adc test_subject1(kMockChannel1);

  AutoVerifyPeripheralMemory mock_peripheral(&Adc::adc_base);

  SECTION("Initialize()")
  {
    // Setup
    constexpr uint32_t kExpectedDivider =
        kDummySystemControllerClockFrequency / Adc::kClockFrequency;

    bit::Register(&mock_peripheral.Expected()->CR)
        .Insert(kExpectedDivider, Adc::Control::kClockDivider)
        .Set(Adc::Control::kPowerEnable)
        .Set(Adc::Control::kBurstEnable)
        .Set(bit::MaskFromRange(kMockChannel0.channel))
        .Set(bit::MaskFromRange(kMockChannel1.channel))
        .Save();

    const PinSettings_t kExpectedSettings0 = {
      .function   = kPinFunction,
      .resistor   = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    const PinSettings_t kExpectedSettings1 = {
      .function   = kPinFunction,
      .resistor   = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    // Exercise
    test_subject0.Initialize();
    test_subject1.Initialize();

    // Verify
    // Verify: that PowerUpPeripheral() was called with the ADC peripheral id
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kAdc));

    // Verify: that the pins were setup correctly
    Verify(Method(mock_adc_pin0, ModuleInitialize));
    Verify(Method(mock_adc_pin1, ModuleInitialize));
    CHECK(kExpectedSettings0 == mock_adc_pin0.get().CurrentSettings());
    CHECK(kExpectedSettings1 == mock_adc_pin1.get().CurrentSettings());
  }

  SECTION("PowerDown()")
  {
    // Setup
    bit::Register(&mock_peripheral.Mock()->CR)
        .Set(bit::MaskFromRange(kMockChannel0.channel))
        .Set(bit::MaskFromRange(kMockChannel1.channel))
        .Save();

    bit::Register(&mock_peripheral.Expected()->CR)
        .Set(bit::MaskFromRange(kMockChannel0.channel))
        .Clear(bit::MaskFromRange(kMockChannel1.channel))
        .Save();

    // Required to get PowerDown to work
    test_subject1.UnitTestEnterInitialized();

    // Exercise
    test_subject1.PowerDown();
  }

  SECTION("Read ADC")
  {
    // Setup
    constexpr uint32_t kExpectedAdcValue = 1555;

    // Setup: Set the expected results
    bit::Register(&mock_peripheral.Mock()->DR[0])
        .Insert(kExpectedAdcValue, Adc::DataRegister::kResult)
        .Save();

    // Setup: Read should only read the registers not modify them, so we copy
    //        the mock memory map to the expected map.
    mock_peripheral.CopyMockToExpected();

    // Exercise
    uint32_t adc_raw = test_subject0.Read();

    // Verify
    CHECK(adc_raw == kExpectedAdcValue);
  }

  SECTION("Get Active Bits")
  {
    constexpr uint16_t kExpectedActiveBits = 12;
    CHECK(kExpectedActiveBits == test_subject0.GetActiveBits());
  }

  SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx
