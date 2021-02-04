#include "peripherals/lpc40xx/adc.hpp"

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "testing/testing_frameworks.hpp"
#include "utility/math/bit.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx adc")
{
  // Create local version of LPC_ADC
  LPC_ADC_TypeDef local_adc;

  // Clear local adc registers
  testing::ClearStructure(&local_adc);

  // Set base registers to respective local variables to check
  // for any bit manipulations
  // Any manipulation will be directed to the respective local registers
  Adc::adc_base = &local_adc;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Set mock for sjsu::Pin
  Mock<sjsu::Pin> mock_adc_pin0;
  Fake(Method(mock_adc_pin0, ModuleInitialize));

  Mock<sjsu::Pin> mock_adc_pin1;
  Fake(Method(mock_adc_pin1, ModuleInitialize));

  const Adc::Channel_t kMockChannel0 = {
    .adc_pin      = mock_adc_pin0.get(),
    .channel      = 0,
    .pin_function = 0b101,
  };

  const Adc::Channel_t kMockChannel1 = {
    .adc_pin      = mock_adc_pin1.get(),
    .channel      = 1,
    .pin_function = 0b101,
  };

  // Create ports and pins to test and mock
  Adc test_subject0(kMockChannel0);
  Adc test_subject1(kMockChannel1);

  auto setup_adc_channel = [&](Adc::Channel_t channel,
                               uint32_t expected_value) {
    local_adc.CR = bit::Insert(local_adc.CR, 0, Adc::Control::kChannelSelect);
    local_adc.DR[channel.channel] =
        bit::Set(local_adc.DR[channel.channel], Adc::DataRegister::kDone);
    // Set done bit so conversion does not loop forever
    // Set the expected results
    local_adc.DR[channel.channel] = bit::Insert(local_adc.DR[channel.channel],
                                                expected_value,
                                                Adc::DataRegister::kResult);
  };

  SECTION("Initialize()")
  {
    // Setup
    constexpr uint32_t kExpectedDivider =
        kDummySystemControllerClockFrequency / Adc::kClockFrequency;

    const PinSettings_t kExpectedSettings0 = {
      .function   = kMockChannel0.pin_function,
      .resistor   = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    const PinSettings_t kExpectedSettings1 = {
      .function   = kMockChannel1.pin_function,
      .resistor   = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    // Exercise
    test_subject0.ModuleInitialize();
    test_subject1.ModuleInitialize();

    // Verify
    // Verify: that PowerUpPeripheral() was called with the ADC peripheral id
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kAdc));

    // Verify: that the pins were setup correctly
    Verify(Method(mock_adc_pin0, ModuleInitialize));
    Verify(Method(mock_adc_pin1, ModuleInitialize));
    CHECK(kExpectedSettings0 == mock_adc_pin0.get().CurrentSettings());
    CHECK(kExpectedSettings1 == mock_adc_pin1.get().CurrentSettings());

    CHECK(kExpectedDivider ==
          bit::Extract(local_adc.CR, Adc::Control::kClockDivider));

    // Check bit 21 to see if power down bit is set in local_adc.CR
    CHECK(bit::Read(local_adc.CR, Adc::Control::kPowerEnable));
    CHECK(bit::Read(local_adc.CR, Adc::Control::kBurstEnable));

    // Verify
    // Check that the channel enable bits (the first 8 bits of the control
    // register) has been set to 1 (enabled)
    CHECK(bit::Read(local_adc.CR, kMockChannel0.channel));
    CHECK(bit::Read(local_adc.CR, kMockChannel1.channel));
    CHECK(!bit::Read(local_adc.CR, 3));  // make sure channel 3 is not enabled
    CHECK(!bit::Read(local_adc.CR, 4));  // make sure channel 4 is not enabled
    CHECK(!bit::Read(local_adc.CR, 5));  // make sure channel 5 is not enabled
    CHECK(!bit::Read(local_adc.CR, 6));  // make sure channel 6 is not enabled
    CHECK(!bit::Read(local_adc.CR, 7));  // make sure channel 7 is not enabled
  }

  SECTION("PowerDown()")
  {
    // Setup
    test_subject0.Initialize();
    test_subject1.Initialize();
    REQUIRE(bit::Read(local_adc.CR, kMockChannel0.channel));
    REQUIRE(bit::Read(local_adc.CR, kMockChannel1.channel));

    // Exercise
    test_subject1.PowerDown();

    // Check that the channel enable bits (the first 8 bits of the control
    // register) has been set to 1 (enabled)
    CHECK(bit::Read(local_adc.CR, kMockChannel0.channel));
    CHECK(!bit::Read(local_adc.CR, kMockChannel1.channel));
    CHECK(!bit::Read(local_adc.CR, 3));  // make sure channel 3 is not enabled
    CHECK(!bit::Read(local_adc.CR, 4));  // make sure channel 4 is not enabled
    CHECK(!bit::Read(local_adc.CR, 5));  // make sure channel 5 is not enabled
    CHECK(!bit::Read(local_adc.CR, 6));  // make sure channel 6 is not enabled
    CHECK(!bit::Read(local_adc.CR, 7));  // make sure channel 7 is not enabled
  }

  SECTION("Read ADC")
  {
    // Setup
    constexpr uint32_t kExpectedAdcValue = 1555;
    constexpr uint32_t kBurstStartCode   = 0b000;
    setup_adc_channel(kMockChannel0, kExpectedAdcValue);

    // Exercise
    uint32_t adc_raw = test_subject0.Read();

    // Verify
    // Verify: channel select should be set to 0 as burst mode will sample all
    //         enabled channels.
    CHECK(0 == bit::Extract(local_adc.CR, Adc::Control::kChannelSelect));
    // Verify: The adc_raw value should be what was stored in the result
    //         register.
    CHECK(adc_raw == bit::Extract(local_adc.DR[0], Adc::DataRegister::kResult));
    // Verify: That the burst start code is set in start code as any other code
    //         will cause sampling to break.
    CHECK(kBurstStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
  }

  SECTION("Get Active Bits")
  {
    constexpr uint16_t kExpectedActiveBits = 12;
    CHECK(kExpectedActiveBits == test_subject0.GetActiveBits());
  }

  SystemController::system_controller = LPC_SC;
  Adc::adc_base                       = LPC_ADC;
}
}  // namespace sjsu::lpc40xx
