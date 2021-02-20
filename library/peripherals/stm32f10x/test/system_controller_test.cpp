#include "peripherals/stm32f10x/system_controller.hpp"

#include <array>
#include <cstdint>
#include <numeric>
#include <thread>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "testing/testing_frameworks.hpp"
#include "utility/math/units.hpp"

namespace sjsu::stm32f10x
{
TEST_CASE("Testing stm32f10x SystemController")
{
  std::array<const SystemController::ResourceID *, 10> id = {
    // AHB
    &stm32f10x::SystemController::Peripherals::kFlitf,
    &stm32f10x::SystemController::Peripherals::kCrc,
    &stm32f10x::SystemController::Peripherals::kFsmc,
    // AHB1
    &stm32f10x::SystemController::Peripherals::kTimer14,
    &stm32f10x::SystemController::Peripherals::kWindowWatchdog,
    &stm32f10x::SystemController::Peripherals::kSpi2,
    // AHB2
    &stm32f10x::SystemController::Peripherals::kGpioG,
    &stm32f10x::SystemController::Peripherals::kAdc2,
    &stm32f10x::SystemController::Peripherals::kTimer1,
    &stm32f10x::SystemController::Peripherals::kAdc3,
  };

  std::array<volatile uint32_t, 3> local_enables;
  std::fill(local_enables.begin(), local_enables.end(), 0);

  FLASH_TypeDef local_flash;
  testing::ClearStructure(&local_flash);

  RCC_TypeDef local_rcc;
  testing::ClearStructure(&local_rcc);

  stm32f10x::SystemController::enable[0]     = &local_enables[0];
  stm32f10x::SystemController::enable[1]     = &local_enables[1];
  stm32f10x::SystemController::enable[2]     = &local_enables[2];
  stm32f10x::SystemController::clock_control = &local_rcc;
  stm32f10x::SystemController::flash         = &local_flash;

  SystemController::ClockConfiguration config;

  SystemController test_subject(config);

  SECTION("PowerUpPeripheral()")
  {
    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);

      // Exercise
      test_subject.PowerUpPeripheral(*id[i]);

      // Verify
      auto enable_word = local_enables[id[i]->device_id / 32];
      CHECK(bit::Read(enable_word, id[i]->device_id % 32));
    }
  }

  SECTION("PowerDownPeripheral()")
  {
    // Setup
    constexpr auto kAllOnes = std::numeric_limits<uint32_t>::max();
    // Setup: Set every bit in the enables to 1 indicating that the all
    // peripherals are enabled. The PowerDownPeripheral() should clear these
    // bits to 1.
    std::fill(local_enables.begin(), local_enables.end(), kAllOnes);

    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);

      // Exercise
      test_subject.PowerDownPeripheral(*id[i]);

      // Verify
      auto enable_word = local_enables[id[i]->device_id / 32];
      CHECK(!bit::Read(enable_word, id[i]->device_id % 32));
    }
  }

  SECTION("IsPeripheralPoweredUp()")
  {
    // Setup: Map of expected powered on and off peripherals
    std::array<bool, id.size()> is_set = {
      true,   // [0]
      true,   // [1]
      false,  // [2]
      true,   // [3]
      true,   // [4]
      true,   // [5]
      false,  // [6]
      true,   // [7]
      false,  // [8]
      true,   // [9]
    };

    for (uint32_t i = 0; i < is_set.size(); i++)
    {
      auto & enable_word = local_enables[id[i]->device_id / 32];
      enable_word =
          bit::Insert(enable_word, is_set[i], id[i]->device_id % 32, 1);
    }

    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);
      // Exercise + Verify
      CHECK(is_set[i] == test_subject.IsPeripheralPoweredUp(*id[i]));
    }
  }

  SECTION("GetClockConfiguration()")
  {
    REQUIRE(&config == test_subject.GetClockConfiguration());
  }

  SECTION("Initialize() Polling Verification")
  {
    // Setup
    // Setup: Will set all fields required to prevent polling. Each
    //        locking_function will need to change this in order to induce
    //        polling for each test case.

    auto control_reg     = bit::Register(&local_rcc.CR);
    auto control_config  = bit::Register(&local_rcc.CFGR);
    auto bd_control      = bit::Register(&local_rcc.BDCR);
    auto call_initialize = [&test_subject]() { test_subject.Initialize(); };

    // Setup: system_clock_becomes_ready
    control_config
        .Insert(
            Value(config.system_clock),
            SystemController::ClockConfigurationRegisters::kSystemClockStatus)
        .Save();

    // Setup: pll_lock
    control_reg.Set(SystemController::ClockControlRegisters::kPllReady).Save();

    // Setup: high_oscillator_ready
    control_reg.Set(SystemController::ClockControlRegisters::kExternalOscReady)
        .Save();

    // Setup: low_oscillator_ready
    bd_control.Set(SystemController::RtcRegisters::kLowSpeedOscReady).Save();

    SECTION("High Speed Lock")
    {
      testing::PollingVerification({
          .locking_function =
              [&control_reg, &config]() {
                config.high_speed_external = 10_MHz;
                control_reg
                    .Clear(SystemController::ClockControlRegisters::
                               kExternalOscReady)
                    .Save();
              },
          .polling_function = call_initialize,
          .release_function =
              [&control_reg]() {
                control_reg
                    .Set(SystemController::ClockControlRegisters::
                             kExternalOscReady)
                    .Save();
              },
      });
    }

    SECTION("Low Speed Lock")
    {
      testing::PollingVerification({
          .locking_function =
              [&bd_control, &config]() {
                config.low_speed_external = 1_MHz;
                bd_control
                    .Clear(SystemController::RtcRegisters::kLowSpeedOscReady)
                    .Save();
              },
          .polling_function = call_initialize,
          .release_function =
              [&bd_control]() {
                bd_control
                    .Set(SystemController::RtcRegisters::kLowSpeedOscReady)
                    .Save();
              },
      });
    }

    SECTION("Pll Lock")
    {
      testing::PollingVerification({
          .locking_function =
              [&control_reg, &config]() {
                config.pll.enable = true;
                control_reg
                    .Clear(SystemController::ClockControlRegisters::kPllReady)
                    .Save();
              },
          .polling_function = call_initialize,
          .release_function =
              [&control_reg]() {
                control_reg
                    .Set(SystemController::ClockControlRegisters::kPllReady)
                    .Save();
              },
      });
    }

    SECTION("Verify System Clock Ready")
    {
      testing::PollingVerification({
          .locking_function =
              [&control_config, &config]() {
                config.system_clock = SystemController::SystemClockSelect::kPll;

                control_config
                    .Insert(Value(SystemController::SystemClockSelect::
                                      kHighSpeedInternal),
                            SystemController::ClockConfigurationRegisters::
                                kSystemClockStatus)
                    .Save();
              },
          .polling_function = call_initialize,
          .release_function =
              [&control_config, &config]() {
                control_config
                    .Insert(Value(config.system_clock),
                            SystemController::ClockConfigurationRegisters::
                                kSystemClockStatus)
                    .Save();
              },
      });
    }
  }

  SECTION("Initialize()")
  {
    // Setup
    // Setup: system_clock_becomes_ready
    local_rcc.CFGR = bit::Insert(
        local_rcc.CFGR,
        Value(config.system_clock),
        SystemController::ClockConfigurationRegisters::kSystemClockStatus);

    // Setup: pll_lock
    local_rcc.CR = bit::Set(local_rcc.CR,
                            SystemController::ClockControlRegisters::kPllReady);

    // Setup: high_oscillator_ready
    local_rcc.CR =
        bit::Set(local_rcc.CR,
                 SystemController::ClockControlRegisters::kExternalOscReady);

    // Setup: low_oscillator_ready
    local_rcc.BDCR = bit::Set(
        local_rcc.BDCR, SystemController::RtcRegisters::kLowSpeedOscReady);

    SECTION("Default Initialize")
    {
      // Exercise
      test_subject.Initialize();
    }

    SECTION("Enable high speed external oscillator")
    {
      // Setup
      config.high_speed_external = 8_MHz;

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(bit::Read(
          local_rcc.CR,
          SystemController::ClockControlRegisters::kExternalOscEnable));
    }

    SECTION("Enable low speed external oscillator")
    {
      // Setup
      config.low_speed_external = 8_MHz;

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(bit::Read(local_rcc.BDCR,
                      SystemController::RtcRegisters::kLowSpeedOscEnable));
    }

    SECTION("PLL External Oscillator Configuration")
    {
      SECTION("Using HSE /2")
      {
        // Setup
        config.pll.source =
            SystemController::PllSource::kHighSpeedExternalDividedBy2;

        // Exercise
        test_subject.Initialize();

        // Verify
        CHECK(bit::Read(
            local_rcc.CFGR,
            SystemController::ClockConfigurationRegisters::kHsePreDivider));
      }

      SECTION("Using HSE /1")
      {
        SECTION("Using Internal")
        {
          // Setup
          config.pll.source = SystemController::PllSource::kHighSpeedInternal;
        }

        SECTION("Using External")
        {
          // Setup
          config.pll.source = SystemController::PllSource::kHighSpeedExternal;
        }

        // Exercise
        test_subject.Initialize();

        // Verify
        CHECK(!bit::Read(
            local_rcc.CFGR,
            SystemController::ClockConfigurationRegisters::kHsePreDivider));
      }
    }

    SECTION("PLL Configuration Enable")
    {
      // Setup
      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(
          Value(config.pll.multiply) ==
          bit::Extract(local_rcc.CFGR,
                       SystemController::ClockConfigurationRegisters::kPllMul));

      CHECK(Value(config.pll.multiply) ==
            bit::Read(local_rcc.CR,
                      SystemController::ClockControlRegisters::kPllEnable));
    }

    SECTION("Flash Wait State Configuration")
    {
      // Setup
      uint32_t flash_code = 0b000;

      // Setup: Make sure we are using the PLL source
      config.system_clock = SystemController::SystemClockSelect::kPll;
      config.pll.enable   = true;

      SECTION("Pll speed = 16 MHz")
      {
        // Setup
        // Setup: Internal is divided by 2 to get 4 MHz
        config.pll.source   = SystemController::PllSource::kHighSpeedInternal;
        config.pll.multiply = SystemController::PllMultiply::kMultiplyBy4;
        flash_code          = 0b000;
      }

      SECTION("Pll speed = 32 MHz")
      {
        // Setup
        // Setup: Internal is divided by 2 to get 4 MHz
        config.pll.source   = SystemController::PllSource::kHighSpeedInternal;
        config.pll.multiply = SystemController::PllMultiply::kMultiplyBy8;
        flash_code          = 0b001;
      }

      SECTION("Pll speed = 64 MHz")
      {
        // Setup
        // Setup: Internal is divided by 2 to get 4 MHz
        config.pll.source   = SystemController::PllSource::kHighSpeedInternal;
        config.pll.multiply = SystemController::PllMultiply::kMultiplyBy16;
        flash_code          = 0b010;
      }

      SECTION("Pll speed = 72 MHz, using external")
      {
        // Setup
        // Setup: External Oscillator set to 8_Mhz
        config.high_speed_external = 8_MHz;
        config.pll.source   = SystemController::PllSource::kHighSpeedExternal;
        config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
        flash_code          = 0b010;
      }

      // Setup: system_clock_becomes_ready
      local_rcc.CFGR = bit::Insert(
          local_rcc.CFGR,
          Value(config.system_clock),
          SystemController::ClockConfigurationRegisters::kSystemClockStatus);

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(flash_code ==
            bit::Extract(local_flash.ACR, sjsu::bit::MaskFromRange(0, 2)));
    }

    SECTION("Changing dividers")
    {
      // Setup
      config.ahb.divider          = SystemController::AHBDivider::kDivideBy512;
      config.ahb.apb1.divider     = SystemController::APBDivider::kDivideBy2;
      config.ahb.apb2.divider     = SystemController::APBDivider::kDivideBy4;
      config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;

      // Exercise
      test_subject.Initialize();
    }

    SECTION("Set System Clock to High Speed External")
    {
      // Setup
      config.system_clock =
          SystemController::SystemClockSelect::kHighSpeedExternal;

      local_rcc.CFGR = bit::Insert(
          local_rcc.CFGR,
          Value(config.system_clock),
          SystemController::ClockConfigurationRegisters::kSystemClockStatus);

      // Exercise
      test_subject.Initialize();
    }

    SECTION("RTC dividers")
    {
      // Setup
      config.rtc.enable = true;
      SECTION("kHighSpeedExternalDividedBy128")
      {
        config.rtc.source =
            SystemController::RtcSource::kHighSpeedExternalDividedBy128;
      }
      SECTION("kLowSpeedExternal")
      {
        config.rtc.source = SystemController::RtcSource::kLowSpeedExternal;
      }
      SECTION("kLowSpeedInternal")
      {
        config.rtc.source = SystemController::RtcSource::kLowSpeedInternal;
      }
      SECTION("kNoClock")
      {
        config.rtc.source = SystemController::RtcSource::kNoClock;
      }

      // Exercise
      test_subject.Initialize();
    }

    // Verify
    // Verify: Set the RTC oscillator source
    CHECK(Value(config.rtc.source) ==
          bit::Extract(local_rcc.BDCR,
                       SystemController::RtcRegisters::kRtcSourceSelect));

    // Verify: Enable/Disable the RTC
    CHECK(config.rtc.enable ==
          bit::Extract(local_rcc.BDCR,
                       SystemController::RtcRegisters::kRtcEnable));

    // Verify: Set System Clock Source
    CHECK(
        Value(config.system_clock) ==
        bit::Extract(
            local_rcc.CFGR,
            SystemController::ClockConfigurationRegisters::kSystemClockSelect));

    // Verify: Set PLL Source
    // Verify: Need to clear the second bit used to select the high speed
    //         external divide by 2.
    CHECK((Value(config.pll.source) & ~(0b10)) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kPllSource));

    // Verify: Set USB Divider
    CHECK(Value(config.pll.usb.divider) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kUsbPrescalar));

    // Verify: Set AHB divider
    CHECK(Value(config.ahb.divider) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kAHBDivider));

    // Verify: Set APB1 divider
    CHECK(Value(config.ahb.apb1.divider) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kAPB1Divider));

    // Verify: Set APB2 divider
    CHECK(Value(config.ahb.apb2.divider) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kAPB2Divider));

    // Verify: Set ADC divider
    CHECK(Value(config.ahb.apb2.adc.divider) ==
          bit::Extract(
              local_rcc.CFGR,
              SystemController::ClockConfigurationRegisters::kAdcDivider));
  }

  SECTION("GetClockRate()")
  {
    // Setup
    config.high_speed_external = 16_MHz;
    config.low_speed_external  = 32_kHz;

    config.system_clock = SystemController::SystemClockSelect::kPll;

    config.pll.enable   = true;
    config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
    config.pll.source =
        SystemController::PllSource::kHighSpeedExternalDividedBy2;

    // Setup: Set the registers with all of the enable bits so keep looping form
    //        happening.
    local_rcc.CFGR = bit::Insert(
        local_rcc.CFGR,
        Value(config.system_clock),
        SystemController::ClockConfigurationRegisters::kSystemClockStatus);
    local_rcc.CR = bit::Set(local_rcc.CR,
                            SystemController::ClockControlRegisters::kPllReady);
    local_rcc.CR =
        bit::Set(local_rcc.CR,
                 SystemController::ClockControlRegisters::kExternalOscReady);
    local_rcc.BDCR = bit::Set(
        local_rcc.BDCR, SystemController::RtcRegisters::kLowSpeedOscReady);

    SECTION("AHB Devices")
    {
      // Setup
      std::array<std::tuple<int, SystemController::AHBDivider>, 9> dividers = {
        std::make_tuple(1, SystemController::AHBDivider::kDivideBy1),
        std::make_tuple(2, SystemController::AHBDivider::kDivideBy2),
        std::make_tuple(4, SystemController::AHBDivider::kDivideBy4),
        std::make_tuple(8, SystemController::AHBDivider::kDivideBy8),
        std::make_tuple(16, SystemController::AHBDivider::kDivideBy16),
        std::make_tuple(64, SystemController::AHBDivider::kDivideBy64),
        std::make_tuple(128, SystemController::AHBDivider::kDivideBy128),
        std::make_tuple(256, SystemController::AHBDivider::kDivideBy256),
        std::make_tuple(512, SystemController::AHBDivider::kDivideBy512),
      };

      for (size_t i = 0; i < dividers.size(); i++)
      {
        // Setup
        INFO("Failure on index [" << i
                                  << "] divider: " << std::get<0>(dividers[i]));
        auto ahb_frequency = 72_MHz / std::get<0>(dividers[i]);
        config.ahb.divider = std::get<1>(dividers[i]);

        // Exercise
        test_subject.Initialize();

        // Exercise + Verify
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kDma1));
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kDma2));
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kSram));
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kCrc));
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kFsmc));
        CHECK(ahb_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kSdio));
      }
    }

    SECTION("APB1 Devices")
    {
      // Setup
      config.ahb.divider = SystemController::AHBDivider::kDivideBy1;

      std::array<std::tuple<int, SystemController::APBDivider>, 5> dividers = {
        std::make_tuple(1, SystemController::APBDivider::kDivideBy1),
        std::make_tuple(2, SystemController::APBDivider::kDivideBy2),
        std::make_tuple(4, SystemController::APBDivider::kDivideBy4),
        std::make_tuple(8, SystemController::APBDivider::kDivideBy8),
        std::make_tuple(16, SystemController::APBDivider::kDivideBy16),
      };

      for (size_t i = 0; i < dividers.size(); i++)
      {
        // Setup
        INFO("Failure on index [" << i
                                  << "] divider: " << std::get<0>(dividers[i]));
        auto expected_frequency = 72_MHz / std::get<0>(dividers[i]);
        config.ahb.apb1.divider = std::get<1>(dividers[i]);

        // Exercise
        test_subject.Initialize();

        // Exercise + Verify
        int timer_frequency_doubler = (i == 0) ? 1 : 2;

        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer2));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer3));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer4));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer5));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer6));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer7));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer12));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer13));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer14));

        CHECK(expected_frequency ==
              test_subject.GetClockRate(
                  SystemController::Peripherals::kWindowWatchdog));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kSpi2));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kSpi3));
        CHECK(
            expected_frequency ==
            test_subject.GetClockRate(SystemController::Peripherals::kUsart2));
        CHECK(
            expected_frequency ==
            test_subject.GetClockRate(SystemController::Peripherals::kUsart3));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kUart4));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kUart5));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kI2c1));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kI2c2));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kCan1));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(
                  SystemController::Peripherals::kBackupClock));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kPower));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kDac));
      }
    }

    SECTION("APB2 Devices")
    {
      // Setup
      config.ahb.divider = SystemController::AHBDivider::kDivideBy1;

      std::array<std::tuple<int, SystemController::APBDivider>, 5> dividers = {
        std::make_tuple(1, SystemController::APBDivider::kDivideBy1),
        std::make_tuple(2, SystemController::APBDivider::kDivideBy2),
        std::make_tuple(4, SystemController::APBDivider::kDivideBy4),
        std::make_tuple(8, SystemController::APBDivider::kDivideBy8),
        std::make_tuple(16, SystemController::APBDivider::kDivideBy16),
      };

      for (size_t i = 0; i < dividers.size(); i++)
      {
        // Setup
        INFO("Failure on index [" << i
                                  << "] divider: " << std::get<0>(dividers[i]));
        units::frequency::hertz_t expected_frequency =
            72_MHz / std::get<0>(dividers[i]);
        config.ahb.apb2.divider = std::get<1>(dividers[i]);

        // Exercise
        test_subject.Initialize();

        // Exercise + Verify
        int timer_frequency_doubler = (i == 0) ? 1 : 2;

        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kAFIO));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioA));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioB));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioC));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioD));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioE));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioF));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kGpioG));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer1));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kSpi1));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer8));
        CHECK(
            expected_frequency ==
            test_subject.GetClockRate(SystemController::Peripherals::kUsart1));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer9));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer10));
        CHECK(
            expected_frequency * timer_frequency_doubler ==
            test_subject.GetClockRate(SystemController::Peripherals::kTimer11));
      }
    }

    SECTION("ADC Devices")
    {
      // Setup
      config.ahb.divider      = SystemController::AHBDivider::kDivideBy1;
      config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;

      std::array<std::tuple<int, SystemController::AdcDivider>, 4> dividers = {
        std::make_tuple(2, SystemController::AdcDivider::kDivideBy2),
        std::make_tuple(4, SystemController::AdcDivider::kDivideBy4),
        std::make_tuple(6, SystemController::AdcDivider::kDivideBy6),
        std::make_tuple(8, SystemController::AdcDivider::kDivideBy8),
      };

      for (size_t i = 0; i < dividers.size(); i++)
      {
        // Setup
        INFO("Failure on index [" << i
                                  << "] divider: " << std::get<0>(dividers[i]));
        units::frequency::hertz_t expected_frequency =
            72_MHz / std::get<0>(dividers[i]);
        config.ahb.apb2.adc.divider = std::get<1>(dividers[i]);

        // Exercise
        test_subject.Initialize();

        // Exercise + Verify
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kAdc1));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kAdc2));
        CHECK(expected_frequency ==
              test_subject.GetClockRate(SystemController::Peripherals::kAdc3));
      }
    }

    SECTION("USB Clock Rate")
    {
      config.ahb.divider      = SystemController::AHBDivider::kDivideBy1;
      config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;

      SECTION("Divide by 1")
      {
        // Setup
        config.pll.usb.divider = SystemController::UsbDivider::kDivideBy1;

        // Exercise
        test_subject.Initialize();

        // Verify
        CHECK(72_MHz ==
              test_subject.GetClockRate(SystemController::Peripherals::kUsb));
      }

      SECTION("Divide by 1.5")
      {
        // Setup
        config.pll.usb.divider = SystemController::UsbDivider::kDivideBy1Point5;

        // Exercise
        test_subject.Initialize();

        // Verify
        CHECK(48_MHz ==
              test_subject.GetClockRate(SystemController::Peripherals::kUsb));
      }
    }

    SECTION("CPU Clock Rate")
    {
      // Setup
      config.ahb.divider      = SystemController::AHBDivider::kDivideBy1;
      config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(72_MHz ==
            test_subject.GetClockRate(SystemController::Peripherals::kCpu));
      CHECK(72_MHz == test_subject.GetClockRate(
                          SystemController::Peripherals::kSystemTimer));
    }

    SECTION("FLINTF Clock Rate")
    {
      // Setup
      config.ahb.divider      = SystemController::AHBDivider::kDivideBy1;
      config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(SystemController::kHighSpeedInternal ==
            test_subject.GetClockRate(SystemController::Peripherals::kFlitf));
    }

    SECTION("I2S Clock Rate")
    {
      // Setup
      // Make sure that I2S is not following the AHB divider
      config.ahb.divider = SystemController::AHBDivider::kDivideBy512;

      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(72_MHz ==
            test_subject.GetClockRate(SystemController::Peripherals::kI2s));
    }

    SECTION("Invalid Peripheral")
    {
      // Exercise
      test_subject.Initialize();

      // Verify
      CHECK(0_Hz == test_subject.GetClockRate(
                        SystemController::ResourceID::Define<0xFFFF>()));
    }
  }

  stm32f10x::SystemController::enable[0]     = &RCC->AHBENR;
  stm32f10x::SystemController::enable[1]     = &RCC->APB1ENR;
  stm32f10x::SystemController::enable[2]     = &RCC->APB2ENR;
  stm32f10x::SystemController::clock_control = RCC;
  stm32f10x::SystemController::flash         = FLASH;
}
}  // namespace sjsu::stm32f10x
