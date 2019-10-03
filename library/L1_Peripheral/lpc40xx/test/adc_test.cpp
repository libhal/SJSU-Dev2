#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/adc.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/bit.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Adc);

TEST_CASE("Testing lpc40xx adc", "[lpc40xx-adc]")
{
  // Create local version of LPC_ADC
  LPC_ADC_TypeDef local_adc;
  // Clear local adc registers
  memset(&local_adc, 0, sizeof(local_adc));
  // Set base registers to respective local variables to check
  // for any bit manipulations
  // Any manipulation will be directed to the respective local registers
  Adc::adc_base = &local_adc;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);

  // Set mock for sjsu::Pin
  Mock<sjsu::Pin> mock_adc_pin0;
  Fake(Method(mock_adc_pin0, SetAsAnalogMode),
       Method(mock_adc_pin0, SetPull),
       Method(mock_adc_pin0, SetPinFunction));

  Mock<sjsu::Pin> mock_adc_pin1;
  Fake(Method(mock_adc_pin1, SetAsAnalogMode),
       Method(mock_adc_pin1, SetPull),
       Method(mock_adc_pin1, SetPinFunction));

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
  Adc channel0_mock(kMockChannel0, mock_system_controller.get());
  Adc channel1_mock(kMockChannel1, mock_system_controller.get());

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

  SECTION("Software Initialization")
  {
    // Setup
    constexpr uint32_t kExpectedDivider =
        kDummySystemControllerClockFrequency / Adc::kClockFrequency;

    // Exercise
    channel0_mock.Initialize();
    channel1_mock.Initialize();

    // Verify
    // Verify that PowerUpPeripheral() was called with the ADC peripheral id
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return SystemController::Peripherals::kAdc.device_id ==
                        id.device_id;
               }));

    // Verify that the pins were setup correctly
    Verify(
        Method(mock_adc_pin0, SetPinFunction).Using(kMockChannel0.pin_function),
        Method(mock_adc_pin0, SetPull).Using(sjsu::Pin::Resistor::kNone),
        Method(mock_adc_pin0, SetAsAnalogMode).Using(true));

    Verify(
        Method(mock_adc_pin1, SetPinFunction).Using(kMockChannel1.pin_function),
        Method(mock_adc_pin1, SetPull).Using(sjsu::Pin::Resistor::kNone),
        Method(mock_adc_pin1, SetAsAnalogMode).Using(true));

    CHECK(kExpectedDivider ==
          bit::Extract(local_adc.CR, Adc::Control::kClockDivider));
    // If this register was zero before, then it should remain that way. This
    // register should not be tampered with in software mode.
    CHECK(0 == bit::Extract(local_adc.CR, Adc::Control::kChannelSelect));
    // Check bit 21 to see if power down bit is set in local_adc.CR
    CHECK(bit::Read(local_adc.CR, Adc::Control::kPowerEnable));
  }

  // This test only tests that the channel select lines were enabled. The rest
  // of the initialize is tested above.
  SECTION("Burst Mode Initialization")
  {
    // Setup
    Adc::BurstMode();
    // Exercise
    channel0_mock.Initialize();
    channel1_mock.Initialize();
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
  SECTION("Burst mode")
  {
    // Only need to test if burst mode will turn on and off
    // Burst mode applies to all channels that are initialized
    Adc::BurstMode(true);

    CHECK(channel0_mock.BurstModeIsEnabled());
    CHECK(bit::Read(local_adc.CR, Adc::Control::kBurstEnable));

    Adc::BurstMode(false);

    CHECK(!channel0_mock.BurstModeIsEnabled());
    CHECK(!bit::Read(local_adc.CR, Adc::Control::kBurstEnable));

    constexpr uint8_t kBurstModeStartCode = 0b000;
    // Check if bits 24 to 26 in local_adc.CR are cleared for burst mode
    // conversion
    Adc::BurstMode(true);
    CHECK(kBurstModeStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
  }
  SECTION("Read channel 0 using software mode")
  {
    // Setup
    constexpr uint32_t kExpectedAdcValue  = 2425;
    constexpr uint32_t kSoftwareStartCode = 0b001;
    setup_adc_channel(kMockChannel0, kExpectedAdcValue);
    Adc::BurstMode(false);
    channel0_mock.Initialize();
    // Exercise
    uint32_t adc_raw = channel0_mock.Read();
    // Verify
    CHECK(adc_raw == bit::Extract(local_adc.DR[0], Adc::DataRegister::kResult));
    CHECK(kSoftwareStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
    CHECK(1 << kMockChannel0.channel ==
          bit::Extract(local_adc.CR, Adc::Control::kChannelSelect));
  }
  SECTION("Read channel 1 using software mode")
  {
    // Setup
    constexpr uint32_t kExpectedAdcValue  = 3341;
    constexpr uint32_t kSoftwareStartCode = 0b001;
    setup_adc_channel(kMockChannel1, kExpectedAdcValue);
    Adc::BurstMode(false);
    channel1_mock.Initialize();
    // Exercise
    uint32_t adc_raw = channel1_mock.Read();
    // Verify
    CHECK(adc_raw == bit::Extract(local_adc.DR[1], Adc::DataRegister::kResult));
    CHECK(kSoftwareStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
    CHECK(1 << kMockChannel1.channel ==
          bit::Extract(local_adc.CR, Adc::Control::kChannelSelect));
  }
  SECTION("Read using burst mode")
  {
    // Setup
    constexpr uint32_t kExpectedAdcValue = 1555;
    constexpr uint32_t kBurstStartCode   = 0b000;
    setup_adc_channel(kMockChannel0, kExpectedAdcValue);
    Adc::BurstMode(true);
    channel0_mock.Initialize();
    // Exercise
    uint32_t adc_raw = channel0_mock.Read();
    // Verify
    CHECK(1 << kMockChannel0.channel ==
          bit::Extract(local_adc.CR, Adc::Control::kChannelSelect));
    CHECK(adc_raw == bit::Extract(local_adc.DR[0], Adc::DataRegister::kResult));
    CHECK(kBurstStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
  }
  SECTION("Read Result from two different channels")
  {
    constexpr uint32_t kExpectedAdcValue[3] = { 0, 125, 4000 };
    uint32_t adc_read_result[2]             = { 0 };

    // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
    constexpr uint8_t kSoftwareStartCode = 0b001;

    // Setup
    // Set done flag so the functions do not loop forever. Only need to do this
    // once since the driver should not be modifying these flags.
    local_adc.DR[kMockChannel0.channel] =
        bit::Set(local_adc.DR[kMockChannel0.channel], Adc::DataRegister::kDone);
    local_adc.DR[kMockChannel1.channel] =
        bit::Set(local_adc.DR[kMockChannel1.channel], Adc::DataRegister::kDone);
    // Set done bit so conversion does not loop forever
    // Set the expected results
    local_adc.DR[kMockChannel0.channel] =
        bit::Insert(local_adc.DR[kMockChannel0.channel],
                    kExpectedAdcValue[0],
                    Adc::DataRegister::kResult);
    local_adc.DR[kMockChannel1.channel] =
        bit::Insert(local_adc.DR[kMockChannel1.channel],
                    kExpectedAdcValue[2],
                    Adc::DataRegister::kResult);
    // Exercise
    adc_read_result[0] = channel0_mock.Read();
    adc_read_result[1] = channel1_mock.Read();
    // Verify
    CHECK(kExpectedAdcValue[0] == adc_read_result[0]);
    CHECK(kExpectedAdcValue[2] == adc_read_result[1]);
    CHECK(kSoftwareStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));

    // Setup
    local_adc.DR[kMockChannel0.channel] =
        bit::Insert(local_adc.DR[kMockChannel0.channel],
                    kExpectedAdcValue[2],
                    Adc::DataRegister::kResult);
    local_adc.DR[kMockChannel1.channel] =
        bit::Insert(local_adc.DR[kMockChannel1.channel],
                    kExpectedAdcValue[1],
                    Adc::DataRegister::kResult);
    // Exercise
    adc_read_result[0] = channel0_mock.Read();
    adc_read_result[1] = channel1_mock.Read();
    // Verify
    CHECK(kExpectedAdcValue[2] == adc_read_result[0]);
    CHECK(kExpectedAdcValue[1] == adc_read_result[1]);
    CHECK(kSoftwareStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));

    // Setup
    local_adc.DR[kMockChannel0.channel] =
        bit::Insert(local_adc.DR[kMockChannel0.channel],
                    kExpectedAdcValue[1],
                    Adc::DataRegister::kResult);
    local_adc.DR[kMockChannel1.channel] =
        bit::Insert(local_adc.DR[kMockChannel1.channel],
                    kExpectedAdcValue[0],
                    Adc::DataRegister::kResult);
    // Exercise
    adc_read_result[0] = channel0_mock.Read();
    adc_read_result[1] = channel1_mock.Read();
    // Verify
    CHECK(kExpectedAdcValue[1] == adc_read_result[0]);
    CHECK(kExpectedAdcValue[0] == adc_read_result[1]);
    CHECK(kSoftwareStartCode ==
          bit::Extract(local_adc.CR, Adc::Control::kStartCode));
  }
  SECTION("Get Active Bits")
  {
    constexpr uint16_t kExpectedActiveBits = 12;
    CHECK(kExpectedActiveBits == channel0_mock.GetActiveBits());
  }

  SystemController::system_controller = LPC_SC;
  Adc::adc_base                       = LPC_ADC;
}
}  // namespace sjsu::lpc40xx
