#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/adc.hpp"
#include "L4_Testing/testing_frameworks.hpp"

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
  constexpr uint32_t kDummySystemControllerClockFrequency = 12'000'000;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);
  When(Method(mock_system_controller, GetPeripheralFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  // Set mock for sjsu::Pin
  Mock<sjsu::Pin> mock_adc_pin;
  Fake(Method(mock_adc_pin, SetAsAnalogMode),
       Method(mock_adc_pin, SetPull),
       Method(mock_adc_pin, SetPinFunction));

  const Adc::Channel_t kMockChannel1 = {
    .adc_pin      = mock_adc_pin.get(),
    .channel      = 1,
    .pin_function = 0b101,
  };
  // Create ports and pins to test and mock
  Adc channel0_mock(kMockChannel1, mock_system_controller.get());

  constexpr uint8_t kBurstBit = 16;
  SECTION("Initialization")
  {
    // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
    constexpr uint8_t kPowerDownBit       = 21;
    constexpr uint8_t kPowerDown          = 0b1;
    constexpr uint16_t kChannelClkDivMask = 0b11'1111'1111;
    constexpr uint8_t kChannelClkDivBit   = 8;

    channel0_mock.Initialize();

    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kAdc
                            .device_id == id.device_id;
               }));
    Verify(
        Method(mock_adc_pin, SetPinFunction).Using(kMockChannel1.pin_function),
        Method(mock_adc_pin, SetPull).Using(sjsu::Pin::Resistor::kNone),
        Method(mock_adc_pin, SetAsAnalogMode).Using(true));

    // Check if any bits in the clock divider are set given a frequency of
    uint32_t expected_frequency =
        kDummySystemControllerClockFrequency / Adc::kClockFrequency;
    uint32_t actual_set_frequency =
        local_adc.CR >> kChannelClkDivBit & kChannelClkDivMask;
    CHECK(expected_frequency == actual_set_frequency);
    // Check bit 21 to see if power down bit is set in local_adc.CR
    CHECK(((local_adc.CR >> kPowerDownBit) & 1) == kPowerDown);
  }
  SECTION("Conversion and finished conversion")
  {
    // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
    constexpr uint8_t kStartNoBurst = 0b1;
    constexpr uint8_t kStartBurst   = 0;
    constexpr uint8_t kStartBit     = 24;
    constexpr uint8_t kDone         = 0b1;
    constexpr uint8_t kDoneBit      = 31;

    // Check if bit 24 in local_adc.CR for the start bits is set
    local_adc.DR[kMockChannel1.channel] |= (1 << kDoneBit);
    channel0_mock.Conversion();
    CHECK(((local_adc.CR >> kStartBit) & 1) == kStartNoBurst);
    channel0_mock.HasConversionFinished();
    CHECK(((local_adc.DR[kMockChannel1.channel] >> kDoneBit) & 1) == kDone);

    // Check if bits 24 to 26 in local_adc.CR are cleared for burst mode
    // conversion
    channel0_mock.BurstMode(true);
    channel0_mock.Conversion();
    CHECK(((local_adc.CR >> kStartBit) & 0b111) == kStartBurst);

    // Check if done bit is set after conversion
    channel0_mock.HasConversionFinished();
    CHECK(((local_adc.DR[kMockChannel1.channel] >> kDoneBit) & 1) == kDone);
  }
  SECTION("Burst mode")
  {
    // Source "UM10562 LPC408x/7x User Manual" table 678 page 805
    constexpr uint8_t kBurstOn  = 0b1;
    constexpr uint8_t kBurstOff = 0b0;

    // Only need to test if burst mode will turn on and off
    // Burst mode applies to all channels that are initialized
    channel0_mock.BurstMode(true);
    CHECK(((local_adc.CR >> kBurstBit) & 1) == kBurstOn);
    channel0_mock.BurstMode(false);
    CHECK(((local_adc.CR >> kBurstBit) & 1) == kBurstOff);
  }
  SECTION("Read result")
  {
    constexpr uint16_t kResultMask = 0xfff;
    constexpr uint8_t kResultBit   = 4;

    // Check if there is any value in the global data reg
    channel0_mock.Read();
    CHECK(((local_adc.DR[kMockChannel1.channel] >> kResultBit) & kResultMask) ==
          0);
  }
  SECTION("Get Active Bits")
  {
    constexpr uint16_t kExpectedActiveBits = 12;
    CHECK(kExpectedActiveBits == channel0_mock.GetActiveBits());
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
  Adc::adc_base                                      = LPC_ADC;
}
}  // namespace sjsu::lpc40xx
