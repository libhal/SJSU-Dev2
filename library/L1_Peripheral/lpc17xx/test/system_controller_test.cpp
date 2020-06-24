#include <bitset>
#include <thread>

#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc17xx
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("Testing LPC176x/5x System Controller", "[lpc17xx-SystemController]")
{
  // Simulate local version of LPC_SC
  LPC_SC_TypeDef local_sc;
  testing::ClearStructure(&local_sc);
  SystemController::system_controller = &local_sc;

  SystemController::ClockConfiguration_t clock_configuration;
  SystemController system_controller(clock_configuration);

  auto main_oscillator_becomes_available = [&local_sc]() {
    std::this_thread::sleep_for(1ms);

    local_sc.SCS = bit::Set(
        local_sc.SCS,
        SystemController::SystemControlsRegister::kOscillatorStatusMask);
  };

  auto pll0_becomes_available = [&local_sc]() {
    std::this_thread::sleep_for(1ms);

    local_sc.PLL0STAT =
        bit::Set(local_sc.PLL0STAT,
                 SystemController::Pll0::StatusRegister::kLockStatusMask);
    local_sc.PLL0STAT = bit::Set(
        local_sc.PLL0STAT, SystemController::Pll0::StatusRegister::kEnableMask);
    local_sc.PLL0STAT =
        bit::Set(local_sc.PLL0STAT,
                 SystemController::Pll0::StatusRegister::kConnectMask);
  };

  auto pll1_becomes_available = [&local_sc]() {
    std::this_thread::sleep_for(1ms);

    local_sc.PLL1STAT =
        bit::Set(local_sc.PLL1STAT,
                 SystemController::Pll1::StatusRegister::kLockStatusMask);
    local_sc.PLL1STAT = bit::Set(
        local_sc.PLL1STAT, SystemController::Pll1::StatusRegister::kEnableMask);
    local_sc.PLL1STAT =
        bit::Set(local_sc.PLL1STAT,
                 SystemController::Pll1::StatusRegister::kConnectMask);
  };

  SECTION("GetClockConfiguration()")
  {
    CHECK(&clock_configuration == system_controller.GetClockConfiguration());
  }

  SECTION("Initialize")
  {
    // Setup
    units::frequency::hertz_t expected_system_clock_rate =
        SystemController::kIrcFrequency;
    units::frequency::hertz_t expected_cpu_clock_rate = 48_MHz;

    SECTION("Configure system clock to drive CPU clock")
    {
      clock_configuration.cpu.clock_source =
          SystemController::CpuClockSource::kSystemClock;
      clock_configuration.cpu.divider = 1;

      SECTION("Using internal RC oscillator (default configuration)")
      {
        // Setup
        expected_cpu_clock_rate = SystemController::kIrcFrequency;

        // Exercise
        system_controller.Initialize();
      }

      SECTION("Using main oscillator")
      {
        // Setup
        constexpr auto kClockRate = 12_MHz;
        std::thread simulated_main_oscillator_is_ready(
            main_oscillator_becomes_available);

        expected_cpu_clock_rate = kClockRate;
        clock_configuration.system.clock_source =
            SystemController::Oscillator::kMain;
        clock_configuration.main_oscillator.frequency = kClockRate;

        // Exercise
        system_controller.Initialize();
        simulated_main_oscillator_is_ready.join();
      }

      SECTION("Using RTC oscillator")
      {
        // Setup
        expected_cpu_clock_rate = SystemController::kRTCFrequency;
        clock_configuration.system.clock_source =
            SystemController::Oscillator::kRtc;

        // Exercise
        system_controller.Initialize();
      }
    }  // Configure system clock to drive CPU clock

    SECTION("Configure CPU clock with PLL0")
    {
      std::thread simulated_pll0_is_ready(pll0_becomes_available);

      SECTION("Using internal RC oscillator")
      {
        // Setup
        expected_system_clock_rate = SystemController::kIrcFrequency;

        // Exercise
        system_controller.Initialize();
        simulated_pll0_is_ready.join();
      }

      SECTION("Using main oscillator")
      {
        // Setup
        std::thread simulated_main_oscillator_is_ready(
            main_oscillator_becomes_available);

        expected_system_clock_rate = 12_MHz;

        clock_configuration.system.clock_source =
            SystemController::Oscillator::kMain;
        clock_configuration.main_oscillator.frequency =
            expected_system_clock_rate;
        clock_configuration.pll0.multiplier  = 12;
        clock_configuration.pll0.pre_divider = 1;

        // Exercise
        system_controller.Initialize();
        simulated_main_oscillator_is_ready.join();
        simulated_pll0_is_ready.join();
      }

      SECTION("Using RTC oscillator")
      {
        expected_system_clock_rate = SystemController::kRTCFrequency;

        clock_configuration.system.clock_source =
            SystemController::Oscillator::kRtc;
        clock_configuration.pll0.multiplier  = 4395;
        clock_configuration.pll0.pre_divider = 1;

        // Exercise
        system_controller.Initialize();
        simulated_pll0_is_ready.join();
      }
    }  // Configure CPU clock with PLL0

    SECTION("Configure USB clock")
    {
      // Setup
      std::thread simulated_main_oscillator_is_ready(
          main_oscillator_becomes_available);
      std::thread simulated_pll0_is_ready(pll0_becomes_available);

      SECTION("Using PLL0")
      {
        // Setup
        expected_system_clock_rate = 12_MHz;

        clock_configuration.main_oscillator.frequency =
            expected_system_clock_rate;
        clock_configuration.system.clock_source =
            SystemController::Oscillator::kMain;
        clock_configuration.usb.clock_source =
            SystemController::UsbClockSource::kPll0;

        SECTION("when PLL0 outputs 288 MHz")
        {
          clock_configuration.pll0.multiplier  = 12;
          clock_configuration.pll0.pre_divider = 1;
          clock_configuration.usb.divider =
              SystemController::UsbClockDivider::kDivideBy6;
        }

        SECTION("when PLL0 outputs 384 MHz")
        {
          clock_configuration.pll0.multiplier  = 16;
          clock_configuration.pll0.pre_divider = 1;
          clock_configuration.cpu.divider      = 8;
          clock_configuration.usb.divider =
              SystemController::UsbClockDivider::kDivideBy8;
        }

        SECTION("when PLL0 outputs 480 MHz")
        {
          clock_configuration.pll0.multiplier  = 20;
          clock_configuration.pll0.pre_divider = 1;
          clock_configuration.cpu.divider      = 10;
          clock_configuration.usb.divider =
              SystemController::UsbClockDivider::kDivideBy10;
        }

        // Exercise
        system_controller.Initialize();
        simulated_main_oscillator_is_ready.join();
        simulated_pll0_is_ready.join();

        // Verify: USB clock configuration
        uint32_t actual_usb_clock_divider_select =
            bit::Extract(local_sc.USBCLKCFG,
                         SystemController::UsbClockRegister::kDividerMask);

        CHECK(actual_usb_clock_divider_select ==
              Value(clock_configuration.usb.divider));
      }  // Using PLL0

      SECTION("Using PLL1")
      {
        // Setup
        std::thread simulated_pll1_is_ready(pll1_becomes_available);

        constexpr std::array<units::frequency::hertz_t, 3>
            kOscillatorClockRates             = { 12_MHz, 16_MHz, 24_MHz };
        constexpr std::array kPll1Multipliers = {
          SystemController::Pll1::Multiplier::kMultiplyBy4,
          SystemController::Pll1::Multiplier::kMultiplyBy3,
          SystemController::Pll1::Multiplier::kMultiplyBy2,
        };
        constexpr uint8_t kDefaultPll1Divider = 2;

        // Generate a test case for each of the main oscillator frequencies.
        size_t oscillator_index = GENERATE(0, 1, 2);

        clock_configuration.main_oscillator.frequency =
            kOscillatorClockRates[oscillator_index];
        clock_configuration.pll1.multiplier =
            kPll1Multipliers[oscillator_index];
        clock_configuration.usb.clock_source =
            SystemController::UsbClockSource::kPll1;

        // Exercise
        system_controller.Initialize();
        simulated_main_oscillator_is_ready.join();
        simulated_pll0_is_ready.join();
        simulated_pll1_is_ready.join();

        // Verify: PLL1 configuration
        uint8_t actual_pll1_multiplier = bit::Extract(
            local_sc.PLL1CFG,
            SystemController::Pll1::ConfigurationRegister::kMultiplierMask);
        uint8_t actual_pll1_divider = bit::Extract(
            local_sc.PLL1CFG,
            SystemController::Pll1::ConfigurationRegister::kDividerMask);

        CHECK(actual_pll1_multiplier ==
              Value(clock_configuration.pll1.multiplier));
        CHECK(actual_pll1_divider == (kDefaultPll1Divider - 1));
      }  // Using PLL1

      // Verify: USB clock configuration
      units::frequency::hertz_t expected_usb_clock_rate = 48_MHz;

      uint8_t actual_usb_clock_select =
          bit::Read(local_sc.PLL1STAT,
                    SystemController::Pll1::StatusRegister::kConnectMask);
      auto actual_usb_clock_rate =
          system_controller.GetClockRate(SystemController::Clocks::kUsb);

      CHECK(actual_usb_clock_select ==
            Value(clock_configuration.usb.clock_source));
      CHECK(actual_usb_clock_rate == expected_usb_clock_rate);
    }  // Configure USB clock

    // Verify: PLL0 configuration if it is enabled
    if (clock_configuration.cpu.clock_source ==
        SystemController::CpuClockSource::kPll0)
    {
      // Calculate expected CPU clock rate:
      //   cpu_clk = (2 * sys_clk * multiplier) / (pre-divider * cpu_divider)
      expected_cpu_clock_rate = (2 * expected_system_clock_rate *
                                 clock_configuration.pll0.multiplier) /
                                clock_configuration.pll0.pre_divider;
      expected_cpu_clock_rate /= clock_configuration.cpu.divider;

      uint16_t actual_pll0_multiplier = bit::Extract(
          local_sc.PLL0CFG,
          SystemController::Pll0::ConfigurationRegister::kMultiplierMask);
      uint8_t actual_pll0_pre_divider = bit::Extract(
          local_sc.PLL0CFG,
          SystemController::Pll0::ConfigurationRegister::kPreDividerMask);

      CHECK(actual_pll0_multiplier ==
            (clock_configuration.pll0.multiplier - 1));
      CHECK(actual_pll0_pre_divider ==
            (clock_configuration.pll0.pre_divider - 1));
    }

    // Verify: PLL0 is set to be enabled and connected if it is used a s clock
    //         source for the CPU clock.
    CHECK(bit::Read(local_sc.PLL0CON,
                    SystemController::PllControlRegister::kEnableMask) ==
          Value(clock_configuration.cpu.clock_source));
    CHECK(bit::Read(local_sc.PLL0CON,
                    SystemController::PllControlRegister::kConnectMask) ==
          Value(clock_configuration.cpu.clock_source));

    // Verify: PLL1 is set to be enabled and connected if it is used a s clock
    //         source for the USB clock.
    CHECK(bit::Read(local_sc.PLL1CON,
                    SystemController::PllControlRegister::kEnableMask) ==
          Value(clock_configuration.usb.clock_source));
    CHECK(bit::Read(local_sc.PLL1CON,
                    SystemController::PllControlRegister::kConnectMask) ==
          Value(clock_configuration.usb.clock_source));

    // Verify: System clock configuration
    uint8_t actual_system_clock_select =
        bit::Extract(local_sc.CLKSRCSEL,
                     SystemController::ClockSourceSelectRegister::kSelectMask);

    CHECK(actual_system_clock_select ==
          Value(clock_configuration.system.clock_source));

    // Verify: CPU clock configuration
    uint8_t actual_cpu_clock_select = bit::Extract(
        local_sc.PLL0CFG, SystemController::PllControlRegister::kConnectMask);
    auto actual_cpu_clock_rate =
        system_controller.GetClockRate(SystemController::Clocks::kCpu);
    uint8_t actual_cpu_clock_divider = bit::Extract(
        local_sc.CCLKCFG, SystemController::CpuClockRegister::kDividerMask);

    CHECK(actual_cpu_clock_select ==
          Value(clock_configuration.cpu.clock_source));
    CHECK(actual_cpu_clock_rate == expected_cpu_clock_rate);
    CHECK((actual_cpu_clock_divider + 1) == clock_configuration.cpu.divider);

    // Verify: Peripheral clock rates
    constexpr uint32_t kPeripheralCount = 32;
    for (uint32_t i = 0; i < kPeripheralCount; i++)
    {
      SystemController::ResourceID peripheral = { .device_id = i };
      CHECK(system_controller.GetClockRate(peripheral) ==
            expected_cpu_clock_rate);
    }
  }  // Initialize

  SECTION("Peripheral Power Control")
  {
    // Generate a test cases for each peripheral.
    auto peripheral =
        GENERATE(SystemController::Peripherals::kTimer0,
                 SystemController::Peripherals::kTimer1,
                 SystemController::Peripherals::kUart0,
                 SystemController::Peripherals::kUart1,
                 SystemController::Peripherals::kPwm1,
                 SystemController::Peripherals::kI2c0,
                 SystemController::Peripherals::kSpi,
                 SystemController::Peripherals::kRtc,
                 SystemController::Peripherals::kSsp1,
                 SystemController::Peripherals::kAdc,
                 SystemController::Peripherals::kCan1,
                 SystemController::Peripherals::kCan2,
                 SystemController::Peripherals::kGpio,
                 SystemController::Peripherals::kRit,
                 SystemController::Peripherals::kMotorControlPwm,
                 SystemController::Peripherals::kQuadratureEncoder,
                 SystemController::Peripherals::kI2c1,
                 SystemController::Peripherals::kSsp0,
                 SystemController::Peripherals::kTimer2,
                 SystemController::Peripherals::kTimer3,
                 SystemController::Peripherals::kUart2,
                 SystemController::Peripherals::kUart3,
                 SystemController::Peripherals::kI2c2,
                 SystemController::Peripherals::kI2s);

    INFO("device id: " << peripheral.device_id);

    SECTION("IsPeripheralPoweredUp")
    {
      SECTION("Peripheral is on")
      {
        // Setup: Set all peripheral power bits to be off except for the
        //        peripheral to test
        local_sc.PCONP = 0;
        local_sc.PCONP = (1 << peripheral.device_id);

        // Exercise + Verify
        CHECK(system_controller.IsPeripheralPoweredUp(peripheral) == true);
      }

      SECTION("Peripheral is off")
      {
        // Setup: Set all peripheral power bits to be on except for the
        //        peripheral to test
        local_sc.PCONP = std::numeric_limits<decltype(local_sc.PCONP)>::max();
        local_sc.PCONP = ~(1 << peripheral.device_id);

        // Exercise + Verify
        CHECK(system_controller.IsPeripheralPoweredUp(peripheral) == false);
      }
    }

    SECTION("PowerUpPeripheral")
    {
      // Setup: Set all peripherals power bits to be initially off
      local_sc.PCONP = 0;

      // Exercise
      system_controller.PowerUpPeripheral(peripheral);

      // Verify
      CHECK(local_sc.PCONP == (1 << peripheral.device_id));
    }

    SECTION("PowerDownPeripheral")
    {
      // Setup: Set all peripherals power bits to be initially on
      local_sc.PCONP = std::numeric_limits<decltype(local_sc.PCONP)>::max();

      // Exercise
      system_controller.PowerDownPeripheral(peripheral);

      // Verify
      CHECK(local_sc.PCONP == ~(1 << peripheral.device_id));
    }
  }  // Peripheral Power Control

  SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc17xx
