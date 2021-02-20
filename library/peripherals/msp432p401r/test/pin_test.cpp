#include "peripherals/msp432p401r/pin.hpp"

#include <bitset>

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
namespace msp432p401r
{
TEST_CASE("Testing Msp432p401r Pin")
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

  Pin p1_3(1, 3);
  Pin p2_0(2, 0);
  Pin p3_7(3, 7);
  Pin p4_6(4, 6);
  Pin p5_2(5, 2);
  Pin p6_5(6, 5);
  Pin p7_1(7, 1);
  Pin p8_2(8, 2);
  Pin p9_4(9, 4);
  Pin p10_0(10, 0);
  Pin pJ_1('J', 1);  // NOLINT

  struct TestStruct_t
  {
    Pin & pin;
    DIO_PORT_Interruptable_Type & registers;
  };

  std::array test_pins = {
    TestStruct_t{
        .pin       = p1_3,
        .registers = local_dio_a,
    },
    TestStruct_t{
        .pin       = p2_0,
        .registers = local_dio_a,
    },
    TestStruct_t{
        .pin       = p3_7,
        .registers = local_dio_b,
    },
    TestStruct_t{
        .pin       = p4_6,
        .registers = local_dio_b,
    },
    TestStruct_t{
        .pin       = p5_2,
        .registers = local_dio_c,
    },
    TestStruct_t{
        .pin       = p6_5,
        .registers = local_dio_c,
    },
    TestStruct_t{
        .pin       = p7_1,
        .registers = local_dio_d,
    },
    TestStruct_t{
        .pin       = p8_2,
        .registers = local_dio_d,
    },
    TestStruct_t{
        .pin       = p9_4,
        .registers = local_dio_e,
    },
    TestStruct_t{
        .pin       = p10_0,
        .registers = local_dio_e,
    },
    TestStruct_t{
        .pin = pJ_1,
        .registers =
            reinterpret_cast<DIO_PORT_Interruptable_Type &>(local_dio_j),
    },
  };

  SECTION("PinFunction")
  {
    SECTION("Valid function codes")
    {
      constexpr auto kDirectionBit = bit::MaskFromRange(2);
      constexpr auto kSel1Bit      = bit::MaskFromRange(1);
      constexpr auto kSel0Bit      = bit::MaskFromRange(0);

      for (size_t i = 0; i < test_pins.size(); i++)
      {
        Pin & pin                 = test_pins[i].pin;
        const uint8_t kPortNumber = pin.GetPort();
        const uint8_t kPinNumber  = pin.GetPin();

        INFO("port: " << static_cast<size_t>(kPortNumber));
        INFO("pin: " << static_cast<size_t>(kPinNumber));

        for (uint8_t function_code = 0; function_code <= 0b111; function_code++)
        {
          // Setup
          INFO("function: 0b" << std::bitset<3>(function_code));
          // By default, assume port number is even and use high register
          volatile uint8_t * direction_register = &test_pins[i].registers.DIR_H;
          volatile uint8_t * sel1_register = &test_pins[i].registers.SEL1_H;
          volatile uint8_t * sel0_register = &test_pins[i].registers.SEL0_H;
          // use low register if port number is odd
          if ((kPortNumber % 2) || (kPortNumber == 'J'))
          {
            direction_register = &test_pins[i].registers.DIR_L;
            sel1_register      = &test_pins[i].registers.SEL1_L;
            sel0_register      = &test_pins[i].registers.SEL0_L;
          }

          bool expected_direction = bit::Read(function_code, kDirectionBit);
          bool expected_sel1      = bit::Read(function_code, kSel1Bit);
          bool expected_sel0      = bit::Read(function_code, kSel0Bit);

          // Exercise
          pin.settings.function = function_code;
          pin.Initialize();

          // Verify
          CHECK(bit::Read(*direction_register, kPinNumber) ==
                expected_direction);
          CHECK(bit::Read(*sel1_register, kPinNumber) == expected_sel1);
          CHECK(bit::Read(*sel0_register, kPinNumber) == expected_sel0);
        }
      }
    }

    SECTION("Invalid function codes")
    {
      // Setup
      uint8_t function_code;

      SUBCASE("0")
      {
        function_code = 0b1111;
      }

      SUBCASE("1")
      {
        function_code = 0b1101;
      }

      SUBCASE("2")
      {
        function_code = 0b1000;
      }

      // Exercise & Verify
      test_pins[0].pin.settings.function = function_code;
      SJ2_CHECK_EXCEPTION(test_pins[0].pin.Initialize(),
                          std::errc::invalid_argument);
    }
  }

  SECTION("ConfigurePullResistor")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Pin & pin                 = test_pins[i].pin;
      const uint8_t kPortNumber = pin.GetPort();
      const uint8_t kPinNumber  = pin.GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * resistor_enable_register =
          &test_pins[i].registers.REN_H;
      volatile uint8_t * out_register = &test_pins[i].registers.OUT_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        resistor_enable_register = &test_pins[i].registers.REN_L;
        out_register             = &test_pins[i].registers.OUT_L;
      }

      bool actual_resistor_enable;
      bool actual_out;

      // Exercise - Setting pull up resistor
      pin.settings.PullUp();
      pin.Initialize();
      actual_resistor_enable = bit::Read(*resistor_enable_register, kPinNumber);
      actual_out             = bit::Read(*out_register, kPinNumber);

      // Verify
      CHECK(actual_resistor_enable == true);
      CHECK(actual_out == true);

      // Exercise - Setting pull down
      pin.settings.PullDown();
      pin.Initialize();
      actual_resistor_enable = bit::Read(*resistor_enable_register, kPinNumber);
      actual_out             = bit::Read(*out_register, kPinNumber);

      // Verify
      CHECK(actual_resistor_enable == true);
      CHECK(actual_out == false);

      // Exercise - Setting no resistor
      pin.settings.Floating();
      pin.Initialize();
      actual_resistor_enable = bit::Read(*resistor_enable_register, kPinNumber);

      // Verify
      CHECK(actual_resistor_enable == false);
    }
  }

  SECTION("SetDriveStrength")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Pin & pin                 = test_pins[i].pin;
      const uint8_t kPortNumber = pin.GetPort();
      const uint8_t kPinNumber  = pin.GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      // By default, assume port number is even and use high register
      volatile uint8_t * driver_strength_register =
          &test_pins[i].registers.DS_H;
      // use low register if port number is odd
      if ((kPortNumber % 2) || (kPortNumber == 'J'))
      {
        driver_strength_register = &test_pins[i].registers.DS_L;
      }

      // Exercise - Set high driver strength
      pin.SetDriveStrength(Pin::DriveStrength::kHigh);

      // Verify
      CHECK(bit::Read(*driver_strength_register, pin.GetPin()) == true);

      // Exercise - Set regular driver strength
      pin.SetDriveStrength(Pin::DriveStrength::kRegular);

      // Verify
      CHECK(bit::Read(*driver_strength_register, pin.GetPin()) == false);
    }
  }

  SECTION("ConfigureAsOpenDrain")
  {
    for (size_t i = 0; i < test_pins.size(); i++)
    {
      // Setup
      Pin & pin                 = test_pins[i].pin;
      const uint8_t kPortNumber = pin.GetPort();
      const uint8_t kPinNumber  = pin.GetPin();

      INFO("port: " << static_cast<size_t>(kPortNumber));
      INFO("pin: " << static_cast<size_t>(kPinNumber));

      pin.settings.open_drain = true;
      SJ2_CHECK_EXCEPTION(pin.Initialize(), std::errc::operation_not_supported);
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
