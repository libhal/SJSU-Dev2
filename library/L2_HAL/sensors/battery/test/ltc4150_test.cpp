#include "L2_HAL/sensors/battery/ltc4150.hpp"
#include "L2_HAL/sensors/battery/coulomb_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "utility/units.hpp"
#include "utility/log.hpp"

namespace sjsu
{
auto GetLambda(InterruptCallback & isr)
{
  return [&isr](InterruptCallback callback, Gpio::Edge) { isr = callback; };
}

TEST_CASE("Test LTC4150 Coulomb Counter/ Battery Gas Gauge", "[ltc4150]")
{
  constexpr float kResolution        = 0.001f;
  units::impedance::ohm_t resistance = 0.00050_Ohm;

  Mock<Gpio> mock_primary_pol_pin;
  Mock<Gpio> mock_backup_pol_pin;

  Mock<HardwareCounter> mock_primary_hardware_counter;
  Mock<HardwareCounter> mock_backup_hardware_counter;

  InterruptCallback primary_input_isr;
  InterruptCallback primary_pol_isr;
  InterruptCallback backup_input_isr;
  InterruptCallback backup_pol_isr;

  When(Method(mock_primary_pol_pin, AttachInterrupt))
      .AlwaysDo(GetLambda(primary_pol_isr));
  When(Method(mock_backup_pol_pin, AttachInterrupt))
      .AlwaysDo(GetLambda(backup_pol_isr));

  Fake(Method(mock_primary_hardware_counter, Initialize));
  Fake(Method(mock_backup_hardware_counter, Initialize));
  Fake(Method(mock_primary_hardware_counter, Enable));
  Fake(Method(mock_backup_hardware_counter, Enable));
  Fake(Method(mock_primary_hardware_counter, GetCount));
  Fake(Method(mock_backup_hardware_counter, GetCount));
  Fake(Method(mock_primary_hardware_counter, SetDirection));
  Fake(Method(mock_backup_hardware_counter, SetDirection));

  Fake(Method(mock_primary_pol_pin, SetDirection));
  Fake(Method(mock_backup_pol_pin, SetDirection));
  Fake(Method(mock_primary_pol_pin, DetachInterrupt));
  Fake(Method(mock_backup_pol_pin, DetachInterrupt));

  SECTION("LTC4150 Initialization")
  {
    Ltc4150 primary_counter = Ltc4150(mock_primary_hardware_counter.get(),
                                      mock_primary_pol_pin.get(),
                                      resistance);
    Ltc4150 backup_counter  = Ltc4150(mock_backup_hardware_counter.get(),
                                     mock_backup_pol_pin.get(),
                                     resistance);

    When(Method(mock_primary_pol_pin, Read)).Return(true);
    When(Method(mock_backup_pol_pin, Read)).Return(false);
    primary_counter.Initialize();
    backup_counter.Initialize();

    Verify(Method(mock_primary_hardware_counter, Initialize)).Exactly(1);
    Verify(Method(mock_backup_hardware_counter, Initialize)).Exactly(1);
    Verify(Method(mock_primary_pol_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_backup_pol_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_primary_pol_pin, AttachInterrupt)).Exactly(1);
    Verify(Method(mock_backup_pol_pin, AttachInterrupt)).Exactly(1);
    Verify(Method(mock_primary_hardware_counter, Enable)).Exactly(1);
    Verify(Method(mock_backup_hardware_counter, Enable)).Exactly(1);
  }

  SECTION("Count Ticks and Calculate mAh")
  {
    constexpr float kPrimaryCharge = -0.068271f;
    constexpr float kBackupCharge  = 0.102407f;
    Ltc4150 primary_counter = Ltc4150(mock_primary_hardware_counter.get(),
                                      mock_primary_pol_pin.get(),
                                      resistance);
    Ltc4150 backup_counter  = Ltc4150(mock_backup_hardware_counter.get(),
                                     mock_backup_pol_pin.get(),
                                     resistance);
    When(Method(mock_primary_pol_pin, Read)).Return(false);
    When(Method(mock_backup_pol_pin, Read)).Return(true);
    When(Method(mock_primary_hardware_counter, GetCount)).Return(-4);
    When(Method(mock_backup_hardware_counter, GetCount)).Return(6);

    primary_counter.Initialize();
    CHECK(primary_counter.GetCharge().to<float>() == Approx(kPrimaryCharge));

    backup_counter.Initialize();
    CHECK(backup_counter.GetCharge().to<float>() == Approx(kBackupCharge));
  }

  SECTION("Direction of Counter changes with the Polarity Pin's State Change")
  {
    Ltc4150 primary_counter = Ltc4150(mock_primary_hardware_counter.get(),
                                      mock_primary_pol_pin.get(),
                                      resistance);
    Ltc4150 backup_counter  = Ltc4150(mock_backup_hardware_counter.get(),
                                     mock_backup_pol_pin.get(),
                                     resistance);
    When(Method(mock_primary_pol_pin, Read)).Return(true, false);
    When(Method(mock_backup_pol_pin, Read)).Return(false, true);

    primary_counter.Initialize();
    Verify(Method(mock_primary_hardware_counter, SetDirection)
               .Using(HardwareCounter::Direction::kUp));
    primary_pol_isr();
    Verify(Method(mock_primary_hardware_counter, SetDirection)
               .Using(HardwareCounter::Direction::kDown));

    backup_counter.Initialize();
    Verify(Method(mock_backup_hardware_counter, SetDirection)
               .Using(HardwareCounter::Direction::kDown));
    backup_pol_isr();
    Verify(Method(mock_backup_hardware_counter, SetDirection)
               .Using(HardwareCounter::Direction::kUp));
  }

  SECTION("Charge Flips Sign with the Polarity Pin's State Change")
  {
    constexpr float kPositivePrimaryCharge = 0.08534f;
    constexpr float kNegativePrimaryCharge = -0.0512f;
    constexpr float kPositiveBackupCharge  = 0.0512f;
    constexpr float kNegativeBackupCharge  = -0.08534f;
    Ltc4150 primary_counter = Ltc4150(mock_primary_hardware_counter.get(),
                                      mock_primary_pol_pin.get(),
                                      resistance);
    Ltc4150 backup_counter  = Ltc4150(mock_backup_hardware_counter.get(),
                                     mock_backup_pol_pin.get(),
                                     resistance);
    When(Method(mock_primary_pol_pin, Read)).Return(false, true);
    When(Method(mock_backup_pol_pin, Read)).Return(true, false);
    When(Method(mock_primary_hardware_counter, GetCount)).Return(-3, 5);
    When(Method(mock_backup_hardware_counter, GetCount)).Return(3, -5);
    primary_counter.Initialize();
    CHECK(primary_counter.GetCharge().to<float>() ==
          Approx(kNegativePrimaryCharge).epsilon(kResolution));
    primary_pol_isr();
    CHECK(primary_counter.GetCharge().to<float>() ==
          Approx(kPositivePrimaryCharge).epsilon(kResolution));

    backup_counter.Initialize();
    CHECK(backup_counter.GetCharge().to<float>() ==
          Approx(kPositiveBackupCharge).epsilon(kResolution));
    backup_pol_isr();
    CHECK(backup_counter.GetCharge().to<float>() ==
          Approx(kNegativeBackupCharge).epsilon(kResolution));
  }
}
}  // namespace sjsu
