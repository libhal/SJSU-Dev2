#include "peripherals/lpc40xx/i2c.hpp"

#include <cstdint>

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/cortex/interrupt.hpp"
#include "testing/testing_frameworks.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx I2C")
{
  // Dummy address used by test sections
  constexpr uint8_t kAddress = 0x33;
  // Create a local register
  LPC_I2C_TypeDef local_i2c;
  // Clear local i2c registers
  testing::ClearStructure(&local_i2c);

  Mock<sjsu::Pin> mock_sda_pin;
  Mock<sjsu::Pin> mock_scl_pin;
  Fake(Method(mock_sda_pin, Pin::ModuleInitialize));
  Fake(Method(mock_scl_pin, Pin::ModuleInitialize));

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  Fake(Method(mock_interrupt_controller, Disable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  I2c::Transaction_t mock_i2c_transaction;
  // The mock object must be statically linked, otherwise a reference to an
  // object in the stack cannot be used as a template parameter for creating
  // a I2c::I2cHandler<kMockI2c> below in Bus_t kMockI2c.
  const I2c::Port_t kMockI2c = {
    .registers    = &local_i2c,
    .id           = sjsu::lpc40xx::SystemController::Peripherals::kI2c0,
    .irq_number   = I2C0_IRQn,
    .transaction  = mock_i2c_transaction,
    .sda_pin      = mock_sda_pin.get(),
    .scl_pin      = mock_scl_pin.get(),
    .pin_function = 0b010,
  };

  I2c test_subject(kMockI2c);

  SECTION("Initialize")
  {
    // Setup
    constexpr auto kSystemFrequency = kDummySystemControllerClockFrequency;

    // Source: "UM10562 LPC408x/407x User manual" table 84 page 443
    constexpr uint32_t kExpectedControlClear =
        I2c::Control::kAssertAcknowledge | I2c::Control::kStart |
        I2c::Control::kStop | I2c::Control::kInterrupt;

    I2cSettings_t settings = {};

    SECTION("Default")
    {
      settings.frequency = 100_kHz;
    }

    SECTION("200_kHz")
    {
      settings.frequency = 200_kHz;
    }

    SECTION("500_kHz")
    {
      settings.frequency = 500_kHz;
    }

    SECTION("1_MHz")
    {
      settings.frequency = 1_MHz;
    }

    SECTION("Duty 30%")
    {
      settings.duty_cycle = .3;
    }

    SECTION("Duty 70%")
    {
      settings.duty_cycle = .7;
    }

    const float kDivider = (kSystemFrequency / settings.frequency);
    const float kExpectedScll = kDivider * settings.duty_cycle;
    const float kExpectedSclh = kDivider * (1 - settings.duty_cycle);

    const uint32_t kLow  = static_cast<uint32_t>(kExpectedScll);
    const uint32_t kHigh = static_cast<uint32_t>(kExpectedSclh);

    // Exercise
    test_subject.settings = settings;
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::ResourceID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kI2c0
                            .device_id == id.device_id;
               }));

    // Verify: Clock rate
    CHECK(kLow == local_i2c.SCLL);
    CHECK(kHigh == local_i2c.SCLH);

    // Verify: Interrupts
    CHECK(local_i2c.CONCLR == kExpectedControlClear);
    CHECK(local_i2c.CONSET == I2c::Control::kInterfaceEnable);

    Verify(
        Method(mock_interrupt_controller, Enable)
            .Matching([&kMockI2c](
                          sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number == kMockI2c.irq_number) &&
                     (info.priority == -1);
            }));

    CHECK(mock_sda_pin.get().CurrentSettings() ==
          PinSettings_t{
              .function   = kMockI2c.pin_function,
              .resistor   = sjsu::PinSettings_t::Resistor::kNone,
              .open_drain = true,
              .as_analog  = false,
          });

    CHECK(mock_scl_pin.get().CurrentSettings() ==
          PinSettings_t{
              .function   = kMockI2c.pin_function,
              .resistor   = sjsu::PinSettings_t::Resistor::kNone,
              .open_drain = true,
              .as_analog  = false,
          });
  }

  SECTION("~I2c()")
  {
    // Setup
    // Exercise
    test_subject.~I2c();

    // Verify
    CHECK(I2c::Control::kInterfaceEnable == local_i2c.CONCLR);

    Verify(
        Method(mock_interrupt_controller, Disable).Using(kMockI2c.irq_number))
        .Once();
  }

#define CHECK_BITS(mask, reg) CHECK(mask == (reg & mask))

  auto setup_state_machine = [&](I2c::MasterState state) {
    local_i2c.STAT   = Value(state);
    local_i2c.CONSET = I2c::Control::kInterfaceEnable;
    local_i2c.CONCLR = 0;
    local_i2c.DAT    = 0;
  };

  SECTION("I2C State Machine: kBusError")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kBusError);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(std::errc::io_error == test_subject.GetTransactionInfo().status);
    CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kStartCondition")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kStartCondition);

    // Exercise
    // Exercise: This will throw a timeout exception, which we will catch. We
    // just want this to setup the transaction register and to start the
    // I2cHandler().
    SJ2_CHECK_EXCEPTION(test_subject.Write(kAddress, nullptr, 0),
                        std::errc::timed_out);

    // Exercise: Handle the I2c transaction for one more step.
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK((kAddress << 1) == local_i2c.DAT);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kRepeatedStart")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kRepeatedStart);

    // Exercise
    SJ2_CHECK_EXCEPTION(test_subject.Write(kAddress, nullptr, 0),
                        std::errc::timed_out);
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(((kAddress << 1) | 1) == local_i2c.DAT);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kSlaveAddressWriteSentReceivedAck")
  {
    SECTION("Non-zero length buffer")
    {
      // Setup
      setup_state_machine(I2c::MasterState::kSlaveAddressWriteSentReceivedAck);
      uint8_t write_buffer[] = { 0x55, 0x22, 0xAA };

      // Exercise
      SJ2_CHECK_EXCEPTION(
          test_subject.Write(kAddress, write_buffer, sizeof(write_buffer)),
          std::errc::timed_out);

      test_subject.I2cHandler(kMockI2c);

      // Verify
      // In this state, you will send the first byte from the write buffer
      // and thus, no other value needs to be check except for [0].
      CHECK(write_buffer[0] == local_i2c.DAT);
      CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
      CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
    }
    SECTION("Zero length buffer")
    {
      // Setup
      setup_state_machine(I2c::MasterState::kSlaveAddressWriteSentReceivedAck);

      // Exercise
      SJ2_CHECK_EXCEPTION(test_subject.Write(kAddress, nullptr, 0),
                          std::errc::timed_out);
      test_subject.I2cHandler(kMockI2c);

      // Verify
      CHECK(!test_subject.GetTransactionInfo().busy);
      CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
      CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
      CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
    }
  }

  SECTION("I2C State Machine: kSlaveAddressWriteSentReceivedNack")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kSlaveAddressWriteSentReceivedNack);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kTransmittedDataReceivedAck wo/ repeat start")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kTransmittedDataReceivedAck);
    uint8_t write_buffer[] = { 'A', 'B', 'C', 'D' };

    // Exercise
    SJ2_CHECK_EXCEPTION(
        test_subject.Write(kAddress, write_buffer, sizeof(write_buffer)),
        std::errc::timed_out);

    // Exercise: Each iteration should be load the next byte from write_buffer
    //           into the local_i2c.DAT
    for (size_t i = 0; i < sizeof(write_buffer); i++)
    {
      // Setup
      setup_state_machine(I2c::MasterState::kTransmittedDataReceivedAck);

      // Exercise
      test_subject.I2cHandler(kMockI2c);

      // Verify
      CHECK(local_i2c.DAT == write_buffer[i]);
      CHECK(test_subject.GetTransactionInfo().busy);
    }

    // Exercise: WriteThenRead should set the transaction to repeated
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
  }

  SECTION("I2C State Machine: kTransmittedDataReceivedAck w/ repeat start")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kTransmittedDataReceivedAck);
    uint8_t write_buffer[] = { 'A', 'B', 'C', 'D' };

    // Exercise
    SJ2_CHECK_EXCEPTION(
        test_subject.WriteThenRead(
            kAddress, write_buffer, sizeof(write_buffer), nullptr, 0),
        std::errc::timed_out);

    // Exercise: Each iteration should be load the next byte from write_buffer
    //           into the local_i2c.DAT
    for (size_t i = 0; i < sizeof(write_buffer); i++)
    {
      // Setup
      setup_state_machine(I2c::MasterState::kTransmittedDataReceivedAck);

      // Exercise
      test_subject.I2cHandler(kMockI2c);

      // Verify
      CHECK(local_i2c.DAT == write_buffer[i]);
      CHECK(test_subject.GetTransactionInfo().busy);
    }

    // Exercise: WriteThenRead should set the transaction to repeated
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(test_subject.GetTransactionInfo().operation ==
          sjsu::I2c::Operation::kRead);
    CHECK(test_subject.GetTransactionInfo().position == 0);
    CHECK_BITS(I2c::Control::kStart, local_i2c.CONSET);
  }

  SECTION("I2C State Machine: kTransmittedDataReceivedNack")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kTransmittedDataReceivedNack);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kArbitrationLost")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kArbitrationLost);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK_BITS(I2c::Control::kStart, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kSlaveAddressReadSentReceivedAck")
  {
    SECTION("Non-zero length buffer")
    {
      // Setup
      setup_state_machine(I2c::MasterState::kSlaveAddressReadSentReceivedAck);
      uint8_t read_buffer[3];

      // Exercise
      SJ2_CHECK_EXCEPTION(
          test_subject.Read(kAddress, read_buffer, sizeof(read_buffer)),
          std::errc::timed_out);

      test_subject.I2cHandler(kMockI2c);

      // Verify
      CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
      CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONSET);
      CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
    }
    SECTION("Zero length buffer")
    {
      // Setup
      setup_state_machine(I2c::MasterState::kSlaveAddressReadSentReceivedAck);

      // Exercise
      SJ2_CHECK_EXCEPTION(test_subject.Read(kAddress, nullptr, 0),
                          std::errc::timed_out);
      test_subject.I2cHandler(kMockI2c);

      // Verify
      CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
      CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
    }
  }

  SECTION("I2C State Machine: kSlaveAddressReadSentReceivedNack")
  {
    // Setup
    setup_state_machine(I2c::MasterState::kSlaveAddressReadSentReceivedNack);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(std::errc::no_such_device_or_address ==
          test_subject.GetTransactionInfo().status);
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kStart, local_i2c.CONCLR);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kReceivedDataReceivedAck")
  {
    // Setup
    constexpr uint8_t kI2cReadData[]          = { 'A', 'B', 'C', 'D' };
    uint8_t read_buffer[sizeof(kI2cReadData)] = { 0 };

    setup_state_machine(I2c::MasterState::kReceivedDataReceivedAck);
    SJ2_CHECK_EXCEPTION(
        test_subject.Read(kAddress, read_buffer, sizeof(read_buffer)),
        std::errc::timed_out);

    // Exercise: Each iteration should be load a new byte from kI2cReadData into
    //           the read_buffer array;
    for (size_t i = 0; i < sizeof(read_buffer) - 1; i++)
    {
      local_i2c.DAT = kI2cReadData[i];

      test_subject.I2cHandler(kMockI2c);

      CHECK(kI2cReadData[i] == read_buffer[i]);
      if (i < sizeof(read_buffer) - 2)
      {
        CHECK(test_subject.GetTransactionInfo().busy);
        CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONSET);
      }
      // Check for the NACK at the end of the read buffer
      else
      {
        CHECK(!test_subject.GetTransactionInfo().busy);
        CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONCLR);
      }
    }

    // Last byte should be transferred
    setup_state_machine(I2c::MasterState::kReceivedDataReceivedAck);
    constexpr size_t kEnd = sizeof(read_buffer) - 1;
    local_i2c.DAT         = kI2cReadData[kEnd];

    test_subject.I2cHandler(kMockI2c);

    // At this point, the limit for the length should have been reached.
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK(kI2cReadData[kEnd] == read_buffer[kEnd]);
    CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONCLR);

    // Another is loaded into the DAT register, but it should not be found
    // in the read_buffer
    setup_state_machine(I2c::MasterState::kReceivedDataReceivedAck);
    local_i2c.DAT = 'F';
    test_subject.I2cHandler(kMockI2c);

    for (size_t i = 0; i < sizeof(read_buffer); i++)
    {
      CHECK(local_i2c.DAT != read_buffer[i]);
    }

    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kAssertAcknowledge, local_i2c.CONCLR);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kReceivedDataReceivedNack")
  {
    setup_state_machine(I2c::MasterState::kReceivedDataReceivedNack);
    local_i2c.DAT = 0xDE;
    uint8_t read_buffer[5];

    // Exercise
    SJ2_CHECK_EXCEPTION(
        test_subject.Read(kAddress, read_buffer, sizeof(read_buffer)),
        std::errc::timed_out);
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK(local_i2c.DAT == read_buffer[0]);
    CHECK(!test_subject.GetTransactionInfo().busy);
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONSET);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: kDoNothing")
  {
    setup_state_machine(I2c::MasterState::kDoNothing);

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  SECTION("I2C State Machine: Unknown/Default")
  {
    setup_state_machine(I2c::MasterState(0xFF));

    // Exercise
    test_subject.I2cHandler(kMockI2c);

    // Verify
    CHECK_BITS(I2c::Control::kStop, local_i2c.CONCLR);
    CHECK_BITS(I2c::Control::kInterrupt, local_i2c.CONCLR);
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;

#undef CHECK_BITS
}
}  // namespace sjsu::lpc40xx
