#include "L2_HAL/sensors/battery/ltc4150.hpp"

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L2_HAL/sensors/battery/coulomb_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/log.hpp"
#include "utility/units.hpp"

namespace sjsu
{
auto GetLambda(InterruptCallback & isr)
{
  return [&isr](InterruptCallback callback, Gpio::Edge) -> void {
    isr = callback;
  };
}

TEST_CASE("Test LTC4150 Coulomb Counter/ Battery Gas Gauge")
{
  units::impedance::ohm_t resistance = 0.00050_Ohm;

  Mock<Gpio> mock_primary_pol_gpio;
  Mock<Gpio> mock_backup_pol_gpio;

  Mock<HardwareCounter> mock_primary_hardware_counter;
  Mock<HardwareCounter> mock_backup_hardware_counter;

  InterruptCallback primary_input_isr;
  InterruptCallback primary_pol_isr;
  InterruptCallback backup_input_isr;
  InterruptCallback backup_pol_isr;

  Fake(
      Method(mock_primary_hardware_counter, HardwareCounter::ModuleInitialize));
  Fake(Method(mock_primary_hardware_counter, HardwareCounter::ModuleEnable));
  Fake(Method(mock_primary_hardware_counter, HardwareCounter::GetCount));
  Fake(Method(mock_primary_hardware_counter, HardwareCounter::Set));
  Fake(Method(mock_primary_hardware_counter, HardwareCounter::SetDirection));

  Fake(Method(mock_backup_hardware_counter, HardwareCounter::ModuleInitialize));
  Fake(Method(mock_backup_hardware_counter, HardwareCounter::ModuleEnable));
  Fake(Method(mock_backup_hardware_counter, HardwareCounter::GetCount));
  Fake(Method(mock_backup_hardware_counter, HardwareCounter::Set));
  Fake(Method(mock_backup_hardware_counter, HardwareCounter::SetDirection));

  Fake(Method(mock_primary_pol_gpio, Gpio::ModuleInitialize));
  Fake(Method(mock_primary_pol_gpio, Gpio::ModuleEnable));
  Fake(Method(mock_primary_pol_gpio, Gpio::SetDirection));
  Fake(Method(mock_primary_pol_gpio, Gpio::Read));
  Fake(Method(mock_primary_pol_gpio, Gpio::DetachInterrupt));
  When(Method(mock_primary_pol_gpio, Gpio::AttachInterrupt))
      .AlwaysDo(GetLambda(primary_pol_isr));

  Fake(Method(mock_backup_pol_gpio, Gpio::ModuleInitialize));
  Fake(Method(mock_backup_pol_gpio, Gpio::ModuleEnable));
  Fake(Method(mock_backup_pol_gpio, Gpio::SetDirection));
  Fake(Method(mock_backup_pol_gpio, Gpio::Read));
  Fake(Method(mock_backup_pol_gpio, Gpio::DetachInterrupt));
  When(Method(mock_backup_pol_gpio, Gpio::AttachInterrupt))
      .AlwaysDo(GetLambda(backup_pol_isr));

  SECTION("LTC4150 Initialization")
  {
    // Setup
    Ltc4150 primary_counter(mock_primary_hardware_counter.get(),
                            mock_primary_pol_gpio.get(),
                            resistance);
    Ltc4150 backup_counter(mock_backup_hardware_counter.get(),
                           mock_backup_pol_gpio.get(),
                           resistance);

    When(Method(mock_primary_pol_gpio, Gpio::Read)).Return(true);
    When(Method(mock_backup_pol_gpio, Gpio::Read)).Return(false);

    // Exercise
    primary_counter.ModuleInitialize();
    backup_counter.ModuleInitialize();

    // Verify
    Verify(Method(mock_primary_hardware_counter,
                  HardwareCounter::ModuleInitialize))
        .Exactly(1);
    Verify(
        Method(mock_backup_hardware_counter, HardwareCounter::ModuleInitialize))
        .Exactly(1);
    Verify(Method(mock_primary_pol_gpio, Gpio::SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_backup_pol_gpio, Gpio::SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_primary_hardware_counter, HardwareCounter::ModuleEnable))
        .Exactly(1);
    Verify(Method(mock_backup_hardware_counter, HardwareCounter::ModuleEnable))
        .Exactly(1);
  }

  SECTION("Enable()")
  {
    // Setup
    Ltc4150 primary_counter(mock_primary_hardware_counter.get(),
                            mock_primary_pol_gpio.get(),
                            resistance);
    Ltc4150 backup_counter(mock_backup_hardware_counter.get(),
                           mock_backup_pol_gpio.get(),
                           resistance);

    // Exercise
    primary_counter.ModuleEnable();
    backup_counter.ModuleEnable();

    // Verify
    Verify(Method(mock_primary_pol_gpio, Gpio::AttachInterrupt));
    Verify(Method(mock_backup_pol_gpio, Gpio::AttachInterrupt));
  }

  SECTION("Count Ticks and Calculate mAh")
  {
    constexpr float kPrimaryCharge = -0.068271f;
    constexpr float kBackupCharge  = 0.102407f;

    Ltc4150 primary_counter(mock_primary_hardware_counter.get(),
                            mock_primary_pol_gpio.get(),
                            resistance);
    Ltc4150 backup_counter(mock_backup_hardware_counter.get(),
                           mock_backup_pol_gpio.get(),
                           resistance);
    When(Method(mock_primary_pol_gpio, Gpio::Read)).Return(false);
    When(Method(mock_backup_pol_gpio, Gpio::Read)).Return(true);
    When(Method(mock_primary_hardware_counter, HardwareCounter::GetCount))
        .Return(-4);
    When(Method(mock_backup_hardware_counter, HardwareCounter::GetCount))
        .Return(6);

    primary_counter.ModuleInitialize();
    primary_counter.ModuleEnable();
    CHECK(primary_counter.GetCharge().to<float>() ==
          doctest::Approx(kPrimaryCharge * 1000));

    backup_counter.ModuleInitialize();
    backup_counter.ModuleEnable();
    CHECK(backup_counter.GetCharge().to<float>() ==
          doctest::Approx(kBackupCharge * 1000));
  }
}
}  // namespace sjsu
