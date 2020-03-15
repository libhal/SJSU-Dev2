#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace msp432p401r
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("Testing msp432p401r SystemController",
          "[msp432p401r-system-controller]")
{
  // Simulate local version of clock system (CS) registers.
  CS_Type local_cs;
  memset(&local_cs, 0, sizeof(local_cs));
  SystemController::clock_system = &local_cs;
  // Simulate local version of tag length value (TLV) registers.
  TLV_Type local_tlv;
  memset(&local_tlv, 0, sizeof(local_tlv));
  SystemController::device_descriptors = &local_tlv;

  constexpr std::array kClockDividers = { 1, 2, 4, 8, 16, 32, 64, 128 };
  SystemController system_controller;

  SECTION("Set/Get System clock frequency")
  {
    constexpr std::array kExpectedSpeeds = { 1_MHz,  3_MHz,  6_MHz,
                                             14_MHz, 28_MHz, 48_MHz };
    for (uint8_t i = 0; i < kExpectedSpeeds.size(); i++)
    {
      INFO("Testing SetSystemClockFrequency with frequency: "
           << kExpectedSpeeds[i].to<uint32_t>() << " MHz");

      system_controller.SetSystemClockFrequency(kExpectedSpeeds[i]);

      const uint8_t kExpectedFrequencySelect = static_cast<uint8_t>(i);
      const bool kDcoEnabled =
          bit::Read(local_cs.CTL0, SystemController::Control0Register::kEnable);
      const uint8_t kFrequencySelect = bit::Extract(
          local_cs.CTL0, SystemController::Control0Register::kFrequencySelect);
      CHECK(kDcoEnabled);
      CHECK(kExpectedFrequencySelect == kFrequencySelect);
      CHECK(system_controller.GetSystemFrequency() == kExpectedSpeeds[i]);

      constexpr auto kExpectedMasterClockSource =
          SystemController::Oscillator::kDigitallyControlled;
      const auto kMasterClockSource = bit::Extract(
          local_cs.CTL1,
          SystemController::Control1Register::kMasterClockSourceSelect);
      CHECK(kMasterClockSource == Value(kExpectedMasterClockSource));

      constexpr auto kExpectedSubsystemMasterClockSource =
          SystemController::Oscillator::kDigitallyControlled;
      const auto kSubsystemMasterClockSource = bit::Extract(
          local_cs.CTL1,
          SystemController::Control1Register::kSubsystemClockSourceSelect);
      CHECK(kSubsystemMasterClockSource ==
            Value(kExpectedSubsystemMasterClockSource));
    }
  }

  SECTION("Set/Get Peripheral clock divider")
  {
    for (uint8_t i = 0; i < kClockDividers.size(); i++)
    {
      INFO("peripheral_divider: " << kClockDividers[i]);
      system_controller.SetPeripheralClockDivider({}, kClockDividers[i]);

      const uint8_t kExpectedDividerSelect = i;
      const uint8_t kDividerSelect         = bit::Extract(
          local_cs.CTL1,
          SystemController::Control1Register::kSubsystemClockDividerSelect);
      CHECK(kDividerSelect == kExpectedDividerSelect);
      CHECK(system_controller.GetPeripheralClockDivider({}) ==
            kClockDividers[i]);
    }
  }

  SECTION("Set clock divider")
  {
    constexpr std::array kClocks = {
      SystemController::Clock::kAuxiliary,
      SystemController::Clock::kMaster,
      SystemController::Clock::kSubsystemMaster,
      SystemController::Clock::kLowSpeedSubsystemMaster,
    };
    constexpr bit::Mask kDividerSelectMasks[] = {
      SystemController::Control1Register::kAuxiliaryClockDividerSelect,
      SystemController::Control1Register::kMasterClockDividerSelect,
      SystemController::Control1Register::kSubsystemClockDividerSelect,
      SystemController::Control1Register::kLowSpeedSubsystemClockDividerSelect,
    };

    for (size_t clock_index = 0; clock_index < kClocks.size(); clock_index++)
    {
      const auto kClock = SystemController::Clock(clock_index);
      for (size_t divider_index = 0; divider_index < kClockDividers.size();
           divider_index++)
      {
        INFO("clock_index: " << clock_index);
        INFO("divider_index: " << divider_index);
        system_controller.SetClockDivider(kClock,
                                          kClockDividers[divider_index]);

        const uint8_t kExpectedDividerSelect = divider_index;
        const uint8_t kDividerSelect =
            bit::Extract(local_cs.CTL1, kDividerSelectMasks[clock_index]);
        CHECK(kDividerSelect == kExpectedDividerSelect);
      }
    }
  }

  SECTION("Is peripheral powered up")
  {
    CHECK(!system_controller.IsPeripheralPoweredUp({}));
  }

  SystemController::clock_system       = msp432p401r::CS;
  SystemController::device_descriptors = msp432p401r::TLV;
}
}  // namespace msp432p401r
}  // namespace sjsu
