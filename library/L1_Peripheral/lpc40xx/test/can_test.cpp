#include "L1_Peripheral/lpc40xx/can.hpp"

#include <chrono>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Can);

TEST_CASE("Testing lpc40xx Can")
{
  // Simulate local version of LPC_CAN
  LPC_CAN_TypeDef local_can;
  // Clear memory locations
  testing::ClearStructure(&local_can);
  LPC_CANAF_TypeDef local_can_acceptance;
  testing::ClearStructure(&local_can_acceptance);

  // Set acceptance filter to the local acceptance filter
  Can::can_acceptance_filter_register = &local_can_acceptance;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Set up Mock for Pin
  Mock<sjsu::Pin> mock_td;
  Mock<sjsu::Pin> mock_rd;

  Fake(Method(mock_td, ConfigureFunction));
  Fake(Method(mock_rd, ConfigureFunction));

  // Set up SSP Bus configuration object
  const Can::Channel_t kMockCan = {
    .td_pin           = mock_td.get(),
    .td_function_code = 2,
    .rd_pin           = mock_rd.get(),
    .rd_function_code = 2,
    .registers        = &local_can,
    .id               = sjsu::lpc40xx::SystemController::Peripherals::kCan1,
  };

  Can test_can(kMockCan);

  SECTION("Initialize()")
  {
    // Setup
    auto check_power_up = [](sjsu::SystemController::ResourceID id) -> bool {
      return sjsu::lpc40xx::SystemController::Peripherals::kCan1.device_id ==
             id.device_id;
    };

    // Exercise
    test_can.ModuleInitialize();

    // Verify
    // Verify: Peripheral is powered on
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching(check_power_up));

    // Verify: Pins are configured
    Verify(Method(mock_td, ConfigureFunction).Using(kMockCan.td_function_code))
        .Once();
    Verify(Method(mock_rd, ConfigureFunction).Using(kMockCan.rd_function_code))
        .Once();

    // Verify: Mode is in reset
    CHECK(bit::Read(local_can.MOD, Can::Mode::kReset));

    // Verify: Acceptance filter has been set to accept everything
    CHECK(Value(Can::Commands::kAcceptAllMessages) ==
          local_can_acceptance.AFMR);
  }

  SECTION("Enable()")
  {
    // Setup
    SECTION("CANBUS was in Reset")
    {
      // Setup: Set MODE to kReset at the start
      local_can.MOD = bit::Set(local_can.MOD, Can::Mode::kReset);
    }
    SECTION("CANBUS was NOT in Reset")
    {
      // Setup: Set MODE to kReset at the start
      local_can.MOD = bit::Clear(local_can.MOD, Can::Mode::kReset);
    }

    // Exercise
    test_can.ModuleEnable();

    // Verify: Mode is in reset
    CHECK(!bit::Read(local_can.MOD, Can::Mode::kReset));
  }

  SECTION("HasData()")
  {
    SECTION("CANBUS has data")
    {
      // Setup
      local_can.GSR =
          bit::Set(local_can.GSR, Can::GlobalStatus::kReceiveBuffer);

      // Exercise + Verify
      CHECK(test_can.HasData());
    }
    SECTION("CANBUS does NOT have data")
    {
      // Setup
      local_can.GSR =
          bit::Clear(local_can.GSR, Can::GlobalStatus::kReceiveBuffer);

      // Exercise + Verify
      CHECK(!test_can.HasData());
    }
  }

  SECTION("IsBusOff()")
  {
    SECTION("CANBUS is bus off")
    {
      // Setup
      local_can.GSR = bit::Set(local_can.GSR, Can::GlobalStatus::kBusError);

      // Exercise + Verify
      CHECK(test_can.IsBusOff());
    }
    SECTION("CANBUS NOT bus off")
    {
      // Setup
      local_can.GSR = bit::Clear(local_can.GSR, Can::GlobalStatus::kBusError);

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

    uint32_t expected_frame = 0;
    expected_frame =
        bit::Insert(expected_frame, message.length, Can::FrameInfo::kLength);
    expected_frame = bit::Insert(expected_frame,
                                 message.is_remote_request,
                                 Can::FrameInfo::kRemoteRequest);
    expected_frame = bit::Insert(
        expected_frame, Value(message.format), Can::FrameInfo::kFormat);

    const uint32_t kExpectedDataA =
        message.payload[0] | message.payload[1] << 8 |
        message.payload[2] << 16 | message.payload[3] << 24;
    const uint32_t kExpectedDataB = message.payload[4];

    SECTION("Buffer 1")
    {
      local_can.SR = bit::Set(local_can.SR, Can::BufferStatus::kTx1Released);

      // Exercise
      test_can.Send(message);

      // Verify
      CHECK(expected_frame == local_can.TFI1);
      CHECK(message.id == local_can.TID1);
      CHECK(kExpectedDataA == local_can.TDA1);
      CHECK(kExpectedDataB == local_can.TDB1);
      CHECK(Value(Can::Commands::kSendTxBuffer1) == local_can.CMR);
    }

    SECTION("Buffer 2")
    {
      local_can.SR = bit::Set(local_can.SR, Can::BufferStatus::kTx2Released);

      // Exercise
      test_can.Send(message);

      // Verify
      CHECK(expected_frame == local_can.TFI2);
      CHECK(message.id == local_can.TID2);
      CHECK(kExpectedDataA == local_can.TDA2);
      CHECK(kExpectedDataB == local_can.TDB2);
      CHECK(Value(Can::Commands::kSendTxBuffer2) == local_can.CMR);
    }

    SECTION("Buffer 3")
    {
      local_can.SR = bit::Set(local_can.SR, Can::BufferStatus::kTx3Released);

      // Exercise
      test_can.Send(message);

      // Verify
      CHECK(expected_frame == local_can.TFI3);
      CHECK(message.id == local_can.TID3);
      CHECK(kExpectedDataA == local_can.TDA3);
      CHECK(kExpectedDataB == local_can.TDB3);
      CHECK(Value(Can::Commands::kSendTxBuffer3) == local_can.CMR);
    }

    SECTION("All buffers full, release TX3 after ")
    {
      testing::PollingVerification({
          .locking_function =
              [&local_can]() {
                local_can.SR =
                    bit::Clear(local_can.SR, Can::BufferStatus::kTx1Released);
                local_can.SR =
                    bit::Clear(local_can.SR, Can::BufferStatus::kTx2Released);
                local_can.SR =
                    bit::Clear(local_can.SR, Can::BufferStatus::kTx3Released);
              },
          .polling_function = [&test_can,
                               &message]() { test_can.Send(message); },
          .release_function =
              [&local_can]() {
                local_can.SR =
                    bit::Set(local_can.SR, Can::BufferStatus::kTx3Released);
              },
          .delay_time = 5ms,
      });
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

    uint32_t expected_frame = 0;
    expected_frame =
        bit::Insert(expected_frame, message.length, Can::FrameInfo::kLength);
    expected_frame = bit::Insert(expected_frame,
                                 message.is_remote_request,
                                 Can::FrameInfo::kRemoteRequest);
    expected_frame = bit::Insert(
        expected_frame, Value(message.format), Can::FrameInfo::kFormat);

    local_can.RFS = expected_frame;
    local_can.RID = message.id;
    local_can.RDA = message.payload[0] | message.payload[1] << 8 |
                    message.payload[2] << 16 | message.payload[3] << 24;
    local_can.RDB = message.payload[4];

    // Exercise
    Can::Message_t actual_message = test_can.Receive();

    // Verify
    CHECK(message.id == actual_message.id);
    CHECK(message.format == actual_message.format);
    CHECK(message.is_remote_request == actual_message.is_remote_request);
    CHECK(message.length == actual_message.length);
    CHECK(message.payload == actual_message.payload);
  }

  SECTION("SelfTest() Polling Test")
  {
    testing::PollingVerification({
        .locking_function =
            [&local_can]() {
              bit::Register(&local_can.SR)
                  .Set(Can::BufferStatus::kTx1Released)
                  .Save();
              bit::Register(&local_can.GSR)
                  .Clear(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
            },
        .polling_function = [&test_can]() { test_can.SelfTest(0xAA); },
        .release_function =
            [&local_can]() {
              REQUIRE(Value(Can::Commands::kSelfReceptionSendTxBuffer1) ==
                      local_can.CMR);
              bit::Register(&local_can.GSR)
                  .Set(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
            },
    });

    testing::PollingVerification({
        .locking_function =
            [&local_can]() {
              bit::Register(&local_can.SR)
                  .Clear(Can::BufferStatus::kTx1Released)
                  .Save();
              bit::Register(&local_can.GSR)
                  .Set(Can::GlobalStatus::kReceiveBuffer)
                  .Save();
            },
        .polling_function = [&test_can]() { test_can.SelfTest(0xAA); },
        .release_function =
            [&local_can]() {
              bit::Register(&local_can.SR)
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
    uint32_t expected_frame = 0;
    uint32_t alter_id       = 0;

    SECTION("Should fail Self Test by incorrect ID")
    {
      // This will change the received ID and will cause the self test to fail.
      alter_id = 1;
    }

    expected_frame = bit::Insert(
        expected_frame, mock_message.length, Can::FrameInfo::kLength);
    expected_frame = bit::Insert(expected_frame,
                                 mock_message.is_remote_request,
                                 Can::FrameInfo::kRemoteRequest);
    expected_frame = bit::Insert(
        expected_frame, Value(mock_message.format), Can::FrameInfo::kFormat);

    // Set the receive bit to 0, to indicate that no message is present.
    local_can.GSR =
        bit::Clear(local_can.GSR, Can::GlobalStatus::kReceiveBuffer);
    // Release the TX1 buffer to allow writing, SelfTest() should be
    // looping right now.
    local_can.SR = bit::Set(local_can.SR, Can::BufferStatus::kTx1Released);

    // Setup: Clear receive buffer and allow SelfTest() to read from the
    // buffer.
    local_can.GSR = bit::Set(local_can.GSR, Can::GlobalStatus::kReceiveBuffer);

    // Setup: Setup receive buffer with message contents
    local_can.RFS = expected_frame;
    local_can.RID = kID + alter_id;
    local_can.RDA = 0;
    local_can.RDB = 0;

    // Exercise
    bool success = test_can.SelfTest(kID);

    // Verify: Transmission
    REQUIRE(expected_frame == local_can.TFI1);
    REQUIRE(0 == local_can.TDA1);
    REQUIRE(0 == local_can.TDB1);
    REQUIRE(Value(Can::Commands::kReleaseRxBuffer) == local_can.CMR);
    REQUIRE(kID == local_can.TID1);

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

  SECTION("ConfigureBaudRate()")
  {
    // Setup
    constexpr units::frequency::hertz_t kDifferentBaudRate = 50'000_Hz;
    constexpr uint32_t kExpectedSyncJumpWidth              = 0;
    constexpr uint32_t kExpectedTimeSegment1               = 0;
    constexpr uint32_t kExpectedTimeSegment2               = 0;
    constexpr uint32_t kExpectedBusSampling                = 1;

    constexpr uint32_t kAdjust = kExpectedSyncJumpWidth +
                                 kExpectedTimeSegment1 + kExpectedTimeSegment2 +
                                 3;
    constexpr uint32_t kExpectedPreAdjustedPrescalar =
        (kDummySystemControllerClockFrequency / kDifferentBaudRate) - 1;

    constexpr uint32_t kExpectedPrescalar =
        kExpectedPreAdjustedPrescalar / kAdjust;

    // Exercise
    test_can.ConfigureBaudRate(kDifferentBaudRate);

    // Verify
    CHECK(kExpectedPrescalar ==
          bit::Extract(local_can.BTR, Can::BusTiming::kPrescalar));
    CHECK(kExpectedSyncJumpWidth ==
          bit::Extract(local_can.BTR, Can::BusTiming::kSyncJumpWidth));
    CHECK(kExpectedTimeSegment1 ==
          bit::Extract(local_can.BTR, Can::BusTiming::kTimeSegment1));
    CHECK(kExpectedTimeSegment2 ==
          bit::Extract(local_can.BTR, Can::BusTiming::kTimeSegment2));
    CHECK(kExpectedBusSampling ==
          bit::Extract(local_can.BTR, Can::BusTiming::kSampling));
  }

  Can::can_acceptance_filter_register = LPC_CANAF;
}
}  // namespace sjsu::lpc40xx
