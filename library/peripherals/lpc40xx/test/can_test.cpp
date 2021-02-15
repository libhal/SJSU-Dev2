#include "peripherals/lpc40xx/can.hpp"

#include <chrono>

#include "testing/peripherals.hpp"
#include "testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx Can")
{
  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, InterruptController::Enable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  // Set up Mock for Pin
  Mock<sjsu::Pin> mock_td;
  Mock<sjsu::Pin> mock_rd;
  Fake(Method(mock_td, Pin::ModuleInitialize));
  Fake(Method(mock_rd, Pin::ModuleInitialize));

  // Set up SSP Bus configuration object
  Can::Port_t mock_can_info = {
    .td_pin           = mock_td.get(),
    .td_function_code = 2,
    .rd_pin           = mock_rd.get(),
    .rd_function_code = 2,
    .registers        = nullptr,
    .id               = sjsu::lpc40xx::SystemController::Peripherals::kCan1,
  };

  AutoVerifyPeripheralMemory mock_peripheral(&mock_can_info.registers,
                                             "can_peripheral");
  AutoVerifyPeripheralMemory mock_can_acceptance_filter(
      &Can::can_acceptance_filter_register, "can_acceptance_filter");

  Can test_can(mock_can_info);

  SECTION("Initialize()")
  {
    // Setup
    constexpr uint32_t kExpectedSyncJumpWidth = 0;
    constexpr uint32_t kExpectedTimeSegment1  = 0;
    constexpr uint32_t kExpectedTimeSegment2  = 0;
    uint32_t expected_bus_sampling            = 1;

    units::frequency::hertz_t expected_baud_rate;

    SECTION("Baudrate 50 kHz")
    {
      expected_baud_rate = 50_kHz;
    }
    SECTION("Baudrate 100 kHz")
    {
      expected_baud_rate = 100_kHz;
    }
    SECTION("Baudrate 1 MHz")
    {
      expected_baud_rate    = 1_MHz;
      expected_bus_sampling = 0;
    }
    SECTION("Baudrate 250 kHz")
    {
      expected_baud_rate    = 250_kHz;
      expected_bus_sampling = 0;
    }

    const uint32_t kAdjust = kExpectedSyncJumpWidth + kExpectedTimeSegment1 +
                             kExpectedTimeSegment2 + 3;
    const uint32_t kExpectedPreAdjustedPrescalar =
        (kDummySystemControllerClockFrequency / expected_baud_rate) - 1;

    const uint32_t kExpectedPrescalar = kExpectedPreAdjustedPrescalar / kAdjust;

    auto check_power_up = [](sjsu::SystemController::ResourceID id) -> bool {
      return sjsu::lpc40xx::SystemController::Peripherals::kCan1.device_id ==
             id.device_id;
    };

    // Verify: Baud rate registers
    mock_peripheral.ExpectedRegister(&LPC_CAN_TypeDef::BTR)
        .Insert(kExpectedPrescalar, Can::BusTiming::kPrescalar)
        .Insert(kExpectedSyncJumpWidth, Can::BusTiming::kSyncJumpWidth)
        .Insert(kExpectedTimeSegment1, Can::BusTiming::kTimeSegment1)
        .Insert(kExpectedTimeSegment2, Can::BusTiming::kTimeSegment2)
        .Insert(expected_bus_sampling, Can::BusTiming::kSampling)
        .Save();

    // Setup: Mode is out of reset
    mock_peripheral.ExpectedRegister(&LPC_CAN_TypeDef::MOD)
        .Clear(Can::Mode::kReset)
        .Save();

    // Setup: Acceptance filter
    mock_can_acceptance_filter.Expected()->AFMR =
        Value(Can::Commands::kAcceptAllMessages);

    test_can.settings.baud_rate = expected_baud_rate;
    test_can.settings.handler   = [](sjsu::Can &) {};

    // Exercise
    test_can.Initialize();

    // Verify
    // Verify: Peripheral is powered on
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching(check_power_up));

    // Verify: Pins are configured
    CHECK(mock_td.get().CurrentSettings().function ==
          mock_can_info.td_function_code);
    CHECK(mock_rd.get().CurrentSettings().function ==
          mock_can_info.rd_function_code);
    Verify(Method(mock_td, Pin::ModuleInitialize)).Once();
    Verify(Method(mock_rd, Pin::ModuleInitialize)).Once();

    // Verify: Interrupt set
    Verify(Method(mock_interrupt_controller, InterruptController::Enable))
        .Once();
  }

  SECTION("HasData()")
  {
    SECTION("CANBUS has data")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
          .Set(Can::GlobalStatus::kReceiveBuffer)
          .Save();
      mock_peripheral.CopyMockToExpected();

      // Exercise + Verify
      CHECK(test_can.HasData());
    }
    SECTION("CANBUS does NOT have data")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
          .Clear(Can::GlobalStatus::kReceiveBuffer)
          .Save();
      mock_peripheral.CopyMockToExpected();

      // Exercise + Verify
      CHECK(!test_can.HasData());
    }
  }

  SECTION("IsBusOff()")
  {
    SECTION("CANBUS is bus off")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
          .Set(Can::GlobalStatus::kBusError)
          .Save();
      mock_peripheral.CopyMockToExpected();

      // Exercise + Verify
      CHECK(test_can.IsBusOff());
    }
    SECTION("CANBUS NOT bus off")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
          .Clear(Can::GlobalStatus::kBusError)
          .Save();
      mock_peripheral.CopyMockToExpected();

      // Exercise + Verify
      CHECK(!test_can.IsBusOff());
    }
  }

  SECTION("Send()")
  {
    // Setup
    Can::Message_t message;
    message.id                = 0x25;
    message.format            = Can::Message_t::Format::kStandard;
    message.is_remote_request = false;
    message.length            = 5;
    message.payload           = { 1, 2, 3, 4, 5, 0, 0, 0 };

    uint32_t expected_frame =
        bit::Value(0)
            .Insert(message.length, Can::FrameInfo::kLength)
            .Insert(message.is_remote_request, Can::FrameInfo::kRemoteRequest)
            .Insert(Value(message.format), Can::FrameInfo::kFormat);

    const uint32_t kExpectedDataA =
        message.payload[0] | message.payload[1] << 8 |
        message.payload[2] << 16 | message.payload[3] << 24;
    const uint32_t kExpectedDataB = message.payload[4];

    SECTION("Buffer 1")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
          .Set(Can::BufferStatus::kTx1Released)
          .Save();

      mock_peripheral.Mock()->TID1 = message.id;
      mock_peripheral.Mock()->TFI1 = expected_frame;
      mock_peripheral.Mock()->TDA1 = kExpectedDataA;
      mock_peripheral.Mock()->TDB1 = kExpectedDataB;
      mock_peripheral.Mock()->CMR  = Value(Can::Commands::kSendTxBuffer1);

      mock_peripheral.CopyMockToExpected();

      // Exercise
      test_can.Send(message);
    }

    SECTION("Buffer 2")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
          .Set(Can::BufferStatus::kTx2Released)
          .Save();

      mock_peripheral.Mock()->TID2 = message.id;
      mock_peripheral.Mock()->TFI2 = expected_frame;
      mock_peripheral.Mock()->TDA2 = kExpectedDataA;
      mock_peripheral.Mock()->TDB2 = kExpectedDataB;
      mock_peripheral.Mock()->CMR  = Value(Can::Commands::kSendTxBuffer2);

      mock_peripheral.CopyMockToExpected();

      // Exercise
      test_can.Send(message);
    }

    SECTION("Buffer 3")
    {
      // Setup
      mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
          .Set(Can::BufferStatus::kTx3Released)
          .Save();

      mock_peripheral.Mock()->TID3 = message.id;
      mock_peripheral.Mock()->TFI3 = expected_frame;
      mock_peripheral.Mock()->TDA3 = kExpectedDataA;
      mock_peripheral.Mock()->TDB3 = kExpectedDataB;
      mock_peripheral.Mock()->CMR  = Value(Can::Commands::kSendTxBuffer3);

      mock_peripheral.CopyMockToExpected();

      // Exercise
      test_can.Send(message);
    }

    SECTION("All buffers full, release TX3 after ")
    {
      testing::PollingVerification({
          .locking_function =
              [&mock_peripheral]() {
                mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
                    .Clear(Can::BufferStatus::kTx1Released)
                    .Clear(Can::BufferStatus::kTx2Released)
                    .Clear(Can::BufferStatus::kTx3Released)
                    .Save();
              },
          .polling_function = [&test_can,
                               &message]() { test_can.Send(message); },
          .release_function =
              [&mock_peripheral]() {
                mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
                    .Set(Can::BufferStatus::kTx3Released)
                    .Save();
              },
          .delay_time = 5ms,
      });

      mock_peripheral.CopyMockToExpected();
    }
  }

  SECTION("Receive()")
  {
    // Setup
    Can::Message_t message;
    message.id                = 0x25;
    message.format            = Can::Message_t::Format::kStandard;
    message.is_remote_request = false;
    message.length            = 5;
    message.payload           = { 1, 2, 3, 4, 5, 0, 0, 0 };

    uint32_t expected_frame =
        bit::Value(0)
            .Insert(message.length, Can::FrameInfo::kLength)
            .Insert(message.is_remote_request, Can::FrameInfo::kRemoteRequest)
            .Insert(Value(message.format), Can::FrameInfo::kFormat);

    mock_peripheral.Mock()->RFS = expected_frame;
    mock_peripheral.Mock()->RID = message.id;
    mock_peripheral.Mock()->RDA = message.payload[0] | message.payload[1] << 8 |
                                  message.payload[2] << 16 |
                                  message.payload[3] << 24;
    mock_peripheral.Mock()->RDB = message.payload[4];

    // Exercise
    Can::Message_t actual_message = test_can.Receive();

    // Verify
    CHECK(message.id == actual_message.id);
    CHECK(message.format == actual_message.format);
    CHECK(message.is_remote_request == actual_message.is_remote_request);
    CHECK(message.length == actual_message.length);
    CHECK(message.payload == actual_message.payload);

    mock_peripheral.CopyMockToExpected();
  }

  SECTION("SelfTest() Polling Test")
  {
    mock_peripheral.Expected()->CMR =
        Value(Can::Commands::kSelfReceptionSendTxBuffer1);

    uint32_t expected_frame =
        bit::Value(0)
            .Insert(0, Can::FrameInfo::kLength)
            .Insert(false, Can::FrameInfo::kRemoteRequest)
            .Insert(Value(Can::Message_t::Format::kStandard),
                    Can::FrameInfo::kFormat);

    mock_peripheral.Expected()->TID1 = 0xAA;
    mock_peripheral.Expected()->TFI1 = expected_frame;
    mock_peripheral.Expected()->TDA1 = 0;
    mock_peripheral.Expected()->TDB1 = 0;

    testing::PollingVerification({
        .locking_function =
            [&mock_peripheral]() {
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
                  .Set(Can::BufferStatus::kTx1Released)
                  .Save();
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
                  .Clear(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
              mock_peripheral.CopyMockToExpected();
            },
        .polling_function = [&test_can]() { test_can.SelfTest(0xAA); },
        .release_function =
            [&mock_peripheral]() {
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
                  .Set(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
            },
    });

    testing::PollingVerification({
        .locking_function =
            [&mock_peripheral]() {
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
                  .Clear(Can::BufferStatus::kTx1Released)
                  .Save();
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
                  .Set(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
              mock_peripheral.CopyMockToExpected();
            },
        .polling_function = [&test_can]() { test_can.SelfTest(0xAA); },
        .release_function =
            [&mock_peripheral]() {
              mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
                  .Set(Can::BufferStatus::kTx1Released)
                  .Save();
            },
        .delay_time = 100ms,
    });
  }

  SECTION("SelfTest()")
  {
    // Setup
    constexpr uint32_t kID = 0xAA;

    Can::Message_t mock_message;
    uint32_t alter_id       = 0;

    SECTION("Should fail Self Test by incorrect ID")
    {
      // This will change the received ID and will cause the self test to fail.
      alter_id = 1;
    }

    uint32_t expected_frame =
        bit::Value(0)
            .Insert(mock_message.length, Can::FrameInfo::kLength)
            .Insert(mock_message.is_remote_request,
                    Can::FrameInfo::kRemoteRequest)
            .Insert(Value(mock_message.format), Can::FrameInfo::kFormat);

    // Setup: Set receive buffer and allow SelfTest() to read from the
    // buffer.
    mock_peripheral.MockRegister(&LPC_CAN_TypeDef::GSR)
        .Set(Can::GlobalStatus::kReceiveBuffer)
        .Save();

    // Release the TX1 buffer to allow writing, SelfTest() should be
    // looping right now.
    mock_peripheral.MockRegister(&LPC_CAN_TypeDef::SR)
        .Set(Can::BufferStatus::kTx1Released)
        .Save();

    // Setup: Setup receive buffer with message contents
    mock_peripheral.Mock()->RFS = expected_frame;
    mock_peripheral.Mock()->RID = kID + alter_id;
    mock_peripheral.Mock()->RDA = 0;
    mock_peripheral.Mock()->RDB = 0;

    mock_peripheral.CopyMockToExpected();

    // Exercise
    bool success = test_can.SelfTest(kID);

    // Verify: Transmission
    mock_peripheral.Expected()->TFI1 = expected_frame;
    mock_peripheral.Expected()->TDA1 = 0;
    mock_peripheral.Expected()->TDB1 = 0;
    mock_peripheral.Expected()->CMR  = Value(Can::Commands::kReleaseRxBuffer);
    mock_peripheral.Expected()->TID1 = kID;

    // Verify: If alter_id is not zero, then the received ID will not be equal
    // to the given ID thus self test should fail.
    if (alter_id != 0)
    {
      CHECK(!success);
    }
    else
    {
      CHECK(success);
    }
  }

  SECTION("~Can()")
  {
    // Setup
    mock_peripheral.MockRegister(&LPC_CAN_TypeDef::IER)
        .Set(Can::Interrupts::kReceivedMessage)
        .Save();

    // Exercise
    test_can.~Can();
  }

  Can::can_acceptance_filter_register = LPC_CANAF;
}
}  // namespace sjsu::lpc40xx
