#include "L2_HAL/sensors/battery/ltc4150.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
#define APPROX_EQUALITY(expected, actual, resolution) \
  CHECK(-resolution < (actual - expected));           \
  CHECK((actual - expected) < resolution);

EMIT_ALL_METHODS(Ltc4150);
TEST_CASE("Test LTC4150 Coulomb Counter/ Battery Gas Gauge", "[ltc4150]")
{
  constexpr float kResolution = 0.001f;

  Ltc4150::TickHandler<0> primary_isr;
  Ltc4150::TickHandler<1> backup_isr;

  Mock<Gpio> mock_primary_input_pin;
  Mock<Gpio> mock_primary_pol_pin;
  Mock<Gpio> mock_backup_input_pin;
  Mock<Gpio> mock_backup_pol_pin;

  Fake(Method(mock_primary_input_pin, AttachInterrupt));
  Fake(Method(mock_primary_input_pin, SetDirection));
  Fake(Method(mock_primary_pol_pin, SetDirection));
  Fake(Method(mock_backup_input_pin, AttachInterrupt));
  Fake(Method(mock_backup_input_pin, SetDirection));
  Fake(Method(mock_backup_pol_pin, SetDirection));

  SECTION("LTC4150 Initialization")
  {
    Ltc4150 primary_counter = Ltc4150(
        primary_isr, mock_primary_input_pin.get(), mock_primary_pol_pin.get());
    Ltc4150 backup_counter = Ltc4150(
        backup_isr, mock_backup_input_pin.get(), mock_backup_pol_pin.get());

    primary_counter.Initialize();
    backup_counter.Initialize();

    Verify(Method(mock_primary_input_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_backup_input_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_primary_pol_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_backup_pol_pin, SetDirection)
               .Using(Gpio::Direction::kInput));
    Verify(Method(mock_primary_input_pin, AttachInterrupt)).Exactly(1);
    Verify(Method(mock_backup_input_pin, AttachInterrupt)).Exactly(1);
  }

  SECTION("Count Ticks and Calculate mAh")
  {
    Ltc4150 primary_counter = Ltc4150(
        primary_isr, mock_primary_input_pin.get(), mock_primary_pol_pin.get());
    Ltc4150 backup_counter = Ltc4150(
        backup_isr, mock_backup_input_pin.get(), mock_backup_pol_pin.get());
    primary_isr.IsrHandler();
    primary_isr.IsrHandler();
    primary_isr.IsrHandler();
    primary_isr.IsrHandler();
    APPROX_EQUALITY(primary_counter.GetBatterymAh(), 0.68271f, kResolution);

    backup_isr.IsrHandler();
    backup_isr.IsrHandler();
    backup_isr.IsrHandler();
    backup_isr.IsrHandler();
    backup_isr.IsrHandler();
    backup_isr.IsrHandler();
    APPROX_EQUALITY(backup_counter.GetBatterymAh(), 1.02407f, kResolution);
  }
}
}  // namespace sjsu
