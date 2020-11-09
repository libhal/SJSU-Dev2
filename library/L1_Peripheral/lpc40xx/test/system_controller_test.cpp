#include "L1_Peripheral/lpc40xx/system_controller.hpp"

#include <thread>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("sjsu::lpc40xx::SystemController")
{
  // Creating local instance of System Control (SC) registers
  LPC_SC_TypeDef local_sc;
  testing::ClearStructure(&local_sc);

  // Point system controller to modify our local instance of LPC_SC_TypeDef for
  // testing.
  sjsu::lpc40xx::SystemController::system_controller = &local_sc;

  // Create a default config
  sjsu::lpc40xx::SystemController::ClockConfiguration config;

  // Test subject object to test against
  sjsu::lpc40xx::SystemController test_subject(config);

  SECTION("GetClockConfiguration()")
  {
    CHECK(&config == test_subject.GetClockConfiguration());
  }

  SECTION("PowerUpPeripheral()")
  {
    for (uint32_t i = 0; i < sizeof(local_sc.PCONP) * 8; i++)
    {
      // Setup
      INFO("Error on peripheral ID " << i);
      const sjsu::SystemController::ResourceID kTestPeripheral = { .device_id =
                                                                       i };
      // Setup: Set the power bits to all zero to isolate the change made by
      //        PowerUpPeripheral()
      local_sc.PCONP = 0;

      // Exercise
      test_subject.PowerUpPeripheral(kTestPeripheral);

      // Verify
      // Verify: That only the bit we wanted to change was changed.
      CHECK(local_sc.PCONP == 1 << i);
    }
  }

  SECTION("PowerDownPeripheral()")
  {
    for (uint32_t i = 0; i < sizeof(local_sc.PCONP) * 8; i++)
    {
      // Setup
      INFO("Error on peripheral ID " << i);
      const sjsu::SystemController::ResourceID kTestPeripheral = { .device_id =
                                                                       i };
      // Setup: Set all of the power bits to all 1s to isolate the change made
      //        by PowerDownPeripheral()
      local_sc.PCONP = std::numeric_limits<decltype(local_sc.PCONP)>::max();

      // Exercise
      test_subject.PowerDownPeripheral(kTestPeripheral);

      // Verify
      // Verify: That only the bit we wanted to change was changed. We should
      //         see a zero in the bit position we expected, thus we perform
      //         ~(1 << i).
      CHECK(local_sc.PCONP == ~(1 << i));
    }
  }

  SECTION("IsPeripheralPoweredUp()")
  {
    for (uint32_t i = 0; i < sizeof(local_sc.PCONP) * 8; i++)
    {
      // Setup
      INFO("Error on peripheral ID " << i);
      const sjsu::SystemController::ResourceID kTestPeripheral = { .device_id =
                                                                       i };
      // Setup: Set all of the power bits to all 1s to isolate the change made
      //        by PowerDownPeripheral()
      local_sc.PCONP = 1 << i;

      // Exercise + Verify
      CHECK(test_subject.IsPeripheralPoweredUp(kTestPeripheral));
    }

    for (uint32_t i = 0; i < sizeof(local_sc.PCONP) * 8; i++)
    {
      // Setup
      INFO("Error on peripheral ID " << i);
      const sjsu::SystemController::ResourceID kTestPeripheral = { .device_id =
                                                                       i };
      // Setup: Set all of the power bits to all 1s to isolate the change made
      //        by PowerDownPeripheral()
      local_sc.PCONP = ~(1 << i);

      // Exercise + Verify
      CHECK(!test_subject.IsPeripheralPoweredUp(kTestPeripheral));
    }
  }

  SECTION("Initialize()")
  {
    auto external_oscillator_becomes_available = [&local_sc]() {
      std::this_thread::sleep_for(1ms);

      local_sc.SCS = bit::Set(
          local_sc.SCS, SystemController::OscillatorRegister::kExternalReady);
    };

    SECTION("Default Initialize")
    {
      // Exercise
      test_subject.Initialize();
    }

    SECTION("Using external oscillator")
    {
      std::thread simulated_hardware_trigger(
          external_oscillator_becomes_available);

      SECTION("External oscillator frequency = 12 MHz")
      {
        // Setup
        config.system_oscillator =
            SystemController::OscillatorSource::kExternal;
        config.external_oscillator_frequency = 12_MHz;

        // Exercise
        test_subject.Initialize();
        simulated_hardware_trigger.join();

        // Verify
        CHECK(!bit::Read(local_sc.SCS,
                         SystemController::OscillatorRegister::kRangeSelect));
      }

      SECTION("External oscillator frequency = 24 MHz")
      {
        // Setup
        config.system_oscillator =
            SystemController::OscillatorSource::kExternal;
        config.external_oscillator_frequency = 24_MHz;

        // Exercise
        test_subject.Initialize();
        simulated_hardware_trigger.join();

        // Verify
        CHECK(bit::Read(local_sc.SCS,
                        SystemController::OscillatorRegister::kRangeSelect));
      }

      // Verify
      CHECK(bit::Read(local_sc.SCS,
                      SystemController::OscillatorRegister::kExternalEnable));
    }

    SECTION("PLL0 Configuration")
    {
      // Setup
      std::thread simulate_pll0_locks([&local_sc, &config]() {
        std::this_thread::sleep_for(1ms);
        local_sc.PLL0STAT = bit::Set(
            local_sc.PLL0STAT, SystemController::PllRegister::kPllLockStatus);
      });

      // Frequency of IRC is = 12 MHz
      config.system_oscillator = SystemController::OscillatorSource::kIrc;
      config.pll[0].enabled    = true;

      // NOTE: The divide codes are used to multiply the internal frequency of
      //       the PLL into a stable range which is between 156 MHz and 320 MHz.
      SECTION("Divide Code = 0")
      {
        config.pll[0].multiply = 13;

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify: Mid Initialize
        CHECK(config.pll[0].multiply - 1 ==
              bit::Extract(local_sc.PLL0CFG,
                           SystemController::PllRegister::kMultiplier));
        CHECK(0 == bit::Extract(local_sc.PLL0CFG,
                                SystemController::PllRegister::kDivider));
      }

      SECTION("Divide Code = 1")
      {
        config.pll[0].multiply = 6;

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(config.pll[0].multiply - 1 ==
              bit::Extract(local_sc.PLL0CFG,
                           SystemController::PllRegister::kMultiplier));
        CHECK(2 == bit::Extract(local_sc.PLL0CFG,
                                SystemController::PllRegister::kDivider));
      }

      SECTION("Divide Code = 2")
      {
        config.pll[0].multiply = 4;

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(config.pll[0].multiply - 1 ==
              bit::Extract(local_sc.PLL0CFG,
                           SystemController::PllRegister::kMultiplier));
        CHECK(2 == bit::Extract(local_sc.PLL0CFG,
                                SystemController::PllRegister::kDivider));
      }

      SECTION("Divide Code = 3")
      {
        config.pll[0].multiply = 2;

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(config.pll[0].multiply - 1 ==
              bit::Extract(local_sc.PLL0CFG,
                           SystemController::PllRegister::kMultiplier));
        CHECK(3 == bit::Extract(local_sc.PLL0CFG,
                                SystemController::PllRegister::kDivider));
      }

      SECTION("Using external oscillator")
      {
        std::thread simulated_hardware_trigger(
            external_oscillator_becomes_available);

        config.external_oscillator_frequency = 25_MHz;
        config.system_oscillator =
            SystemController::OscillatorSource::kExternal;
        config.pll[0].multiply = 2;

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();
        simulated_hardware_trigger.join();

        // Verify
        CHECK(config.pll[0].multiply - 1 ==
              bit::Extract(local_sc.PLL0CFG,
                           SystemController::PllRegister::kMultiplier));
        CHECK(2 == bit::Extract(local_sc.PLL0CFG,
                                SystemController::PllRegister::kDivider));
      }

      SECTION("CPU Speed is <= 20MHz")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 1;  // 12_MHz

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock1));
        CHECK(local_sc.PBOOST == 0b00);
      }

      SECTION("20MHz <= CPU Speed <= 40MHz")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 2;  // 24_MHz

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock2));
        CHECK(local_sc.PBOOST == 0b00);
      }

      SECTION("40MHz <= CPU Speed <= 60MHz")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 4;  // 48_MHz

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock3));
        CHECK(local_sc.PBOOST == 0b00);
      }

      SECTION("60MHz <= CPU Speed <= 80MHz")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 5;  // 60_MHz

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock4));
        CHECK(local_sc.PBOOST == 0b00);
      }

      SECTION("80MHz <= CPU Speed <= 100MHz")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 7;  // 84_MHz

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock5));
        CHECK(local_sc.PBOOST == 0b00);
      }

      SECTION("100MHz >= CPU Speed")
      {
        config.cpu.clock       = SystemController::CpuClockSelect::kPll0;
        config.pll[0].multiply = 10;  // 120_MHz (max)

        // Exercise
        test_subject.Initialize();
        simulate_pll0_locks.join();

        // Verify
        CHECK(local_sc.FLASHCFG ==
              Value(SystemController::FlashConfiguration::kClock5));
        CHECK(local_sc.PBOOST == 0b11);
      }

      // Verify
      CHECK(1 == local_sc.PLL0CON);
      CHECK(0x55 == local_sc.PLL0FEED);
    }

    SECTION("PLL1 Configuration")
    {
      // Setup
      std::thread simulate_pll1_locks([&local_sc, &config]() {
        std::this_thread::sleep_for(1ms);
        local_sc.PLL1STAT = bit::Set(
            local_sc.PLL1STAT, SystemController::PllRegister::kPllLockStatus);
      });
      std::thread simulated_hardware_trigger(
          external_oscillator_becomes_available);

      config.external_oscillator_frequency = 25_MHz;
      config.pll[1].enabled                = true;
      config.pll[1].multiply               = 4;

      // Exercise
      test_subject.Initialize();
      simulate_pll1_locks.join();
      simulated_hardware_trigger.join();

      // Verify
      CHECK(config.pll[1].multiply - 1 ==
            bit::Extract(local_sc.PLL1CFG,
                         SystemController::PllRegister::kMultiplier));
      CHECK(1 == bit::Extract(local_sc.PLL1CFG,
                              SystemController::PllRegister::kDivider));
      // Verify
      CHECK(1 == local_sc.PLL1CON);
      CHECK(0x55 == local_sc.PLL1FEED);
    }

    // Verify
    CHECK(Value(config.system_oscillator) == local_sc.CLKSRCSEL);

    CHECK(Value(config.cpu.clock) ==
          bit::Extract(local_sc.CCLKSEL,
                       SystemController::CpuClockRegister::kSelect));
    CHECK(Value(config.usb.clock) ==
          bit::Extract(local_sc.USBCLKSEL,
                       SystemController::UsbClockRegister::kSelect));
    CHECK(Value(config.spifi.clock) ==
          bit::Extract(local_sc.SPIFISEL,
                       SystemController::SpiFiClockRegister::kSelect));

    CHECK(config.cpu.divider ==
          bit::Extract(local_sc.CCLKSEL,
                       SystemController::CpuClockRegister::kDivider));

    CHECK(Value(config.emc_divider) ==
          bit::Extract(local_sc.EMCCLKSEL,
                       SystemController::EmcClockRegister::kDivider));

    CHECK(config.peripheral_divider ==
          bit::Extract(local_sc.PCLKSEL,
                       SystemController::PeripheralClockRegister::kDivider));

    CHECK(Value(config.usb.divider) ==
          bit::Extract(local_sc.USBCLKSEL,
                       SystemController::UsbClockRegister::kDivider));

    CHECK(config.spifi.divider ==
          bit::Extract(local_sc.SPIFISEL,
                       SystemController::SpiFiClockRegister::kDivider));
  }
}
}  // namespace sjsu::lpc40xx
