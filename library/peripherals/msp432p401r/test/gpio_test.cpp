#include "peripherals/msp432p401r/gpio.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
namespace msp432p401r
{
TEST_CASE("Testing Msp432p401r Gpio")
{
  DIO_PORT_Interruptable_Type local_dio_a;
  DIO_PORT_Interruptable_Type local_dio_b;
  DIO_PORT_Interruptable_Type local_dio_c;
  DIO_PORT_Interruptable_Type local_dio_d;
  DIO_PORT_Interruptable_Type local_dio_e;
  DIO_PORT_Not_Interruptable_Type local_dio_j;

  testing::ClearStructure(&local_dio_a);
  testing::ClearStructure(&local_dio_b);
  testing::ClearStructure(&local_dio_c);
  testing::ClearStructure(&local_dio_d);
  testing::ClearStructure(&local_dio_e);
  testing::ClearStructure(&local_dio_e);

  Pin::ports[0] = &local_dio_a;
  Pin::ports[1] = &local_dio_b;
  Pin::ports[2] = &local_dio_c;
  Pin::ports[3] = &local_dio_d;
  Pin::ports[4] = &local_dio_e;
  Pin::ports[5] = reinterpret_cast<DIO_PORT_Interruptable_Type *>(&local_dio_j);

  Gpio p1_3(1, 3);
  Gpio p2_0(2, 0);
  Gpio p3_7(3, 7);
  Gpio p4_6(4, 6);
  Gpio p5_2(5, 2);
  Gpio p6_5(6, 5);
  Gpio p7_1(7, 1);
  Gpio p8_2(8, 2);
  Gpio p9_4(9, 4);
  Gpio p10_0(10, 0);
  Gpio pJ_1('J', 1);  // NOLINT

  struct TestStruct_t
  {
    Gpio & gpio;
    DIO_PORT_Interruptable_Type & registers;
  };

  std::array test_pins = {
    TestStruct_t{
        .gpio      = p1_3,
        .registers = local_dio_a,
    },
    TestStruct_t{
        .gpio      = p2_0,
        .registers = local_dio_a,
    },
    TestStruct_t{
        .gpio      = p3_7,
        .registers = local_dio_b,
    },
    TestStruct_t{
        .gpio      = p4_6,
        .registers = local_dio_b,
    },
    TestStruct_t{
        .gpio      = p5_2,
        .registers = local_dio_c,
    },
    TestStruct_t{
        .gpio      = p6_5,
        .registers = local_dio_c,
    },
    TestStruct_t{
        .gpio      = p7_1,
        .registers = local_dio_d,
    },
    TestStruct_t{
        .gpio      = p8_2,
        .registers = local_dio_d,
    },
    TestStruct_t{
        .gpio      = p9_4,
        .registers = local_dio_e,
    },
    TestStruct_t{
        .gpio      = p10_0,
        .registers = local_dio_e,
    },
    TestStruct_t{
        .gpio = pJ_1,
        .registers =
            reinterpret_cast<DIO_PORT_Interruptable_Type &>(local_dio_j),
    },
  };

  SECTION("SetDirection")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Gpio & pin                = test_pins[i].gpio;
      const uint8_t kPortNumber = pin.GetPin().GetPort();
      const uint8_t kPinNumber  = pin.GetPin().GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * direction_register = &test_pins[i].registers.DIR_H;
      volatile uint8_t * sel1_register      = &test_pins[i].registers.SEL1_H;
      volatile uint8_t * sel0_register      = &test_pins[i].registers.SEL0_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        direction_register = &test_pins[i].registers.DIR_L;
        sel1_register      = &test_pins[i].registers.SEL1_L;
        sel0_register      = &test_pins[i].registers.SEL0_L;
      }

      // Exercise - Set as output
      pin.SetDirection(Gpio::Direction::kOutput);

      // Verify
      CHECK(bit::Read(*direction_register, kPinNumber) == true);
      CHECK(bit::Read(*sel1_register, kPinNumber) == false);
      CHECK(bit::Read(*sel0_register, kPinNumber) == false);

      // Exercise - Set as input
      pin.SetDirection(Gpio::Direction::kInput);

      // Verify
      CHECK(bit::Read(*direction_register, kPinNumber) == false);
      CHECK(bit::Read(*sel1_register, kPinNumber) == false);
      CHECK(bit::Read(*sel0_register, kPinNumber) == false);
    }
  }

  SECTION("Set")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Gpio & pin                = test_pins[i].gpio;
      const uint8_t kPortNumber = pin.GetPin().GetPort();
      const uint8_t kPinNumber  = pin.GetPin().GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * out_register = &test_pins[i].registers.OUT_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        out_register = &test_pins[i].registers.OUT_L;
      }

      // Exercise - Set as output
      pin.Set(Gpio::State::kHigh);

      // Verify
      CHECK(bit::Read(*out_register, kPinNumber) == true);

      // Exercise - Set as input
      pin.Set(Gpio::State::kLow);

      // Verify
      CHECK(bit::Read(*out_register, kPinNumber) == false);
    }
  }

  SECTION("Toggle")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Gpio & pin                = test_pins[i].gpio;
      const uint8_t kPortNumber = pin.GetPin().GetPort();
      const uint8_t kPinNumber  = pin.GetPin().GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * out_register = &test_pins[i].registers.OUT_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        out_register = &test_pins[i].registers.OUT_L;
      }

      // Initial state should be false.
      CHECK(bit::Read(*out_register, kPinNumber) == false);

      // Exercise - Set as output
      pin.Toggle();

      // Verify
      CHECK(bit::Read(*out_register, kPinNumber) == true);

      // Exercise - Set as input
      pin.Toggle();

      // Verify
      CHECK(bit::Read(*out_register, kPinNumber) == false);
    }
  }

  SECTION("Read")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Gpio & pin                = test_pins[i].gpio;
      const uint8_t kPortNumber = pin.GetPin().GetPort();
      const uint8_t kPinNumber  = pin.GetPin().GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * in_register = &test_pins[i].registers.IN_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        in_register = &test_pins[i].registers.IN_L;
      }

      // Exercise - Read high
      *in_register = bit::Set(*in_register, kPinNumber);

      // Verify
      CHECK(pin.Read() == true);

      // Exercise - Read low
      *in_register = bit::Clear(*in_register, kPinNumber);

      // Verify
      CHECK(pin.Read() == false);
    }
  }

  Pin::ports[0] = PA;
  Pin::ports[1] = PB;
  Pin::ports[2] = PC;
  Pin::ports[3] = PD;
  Pin::ports[4] = PE;
  Pin::ports[5] = reinterpret_cast<DIO_PORT_Interruptable_Type *>(PJ);
}
}  // namespace msp432p401r
}  // namespace sjsu
