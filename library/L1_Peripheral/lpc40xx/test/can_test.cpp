#include <chrono>  // NOLINT
#include <thread>  // NOLINT

#include "L1_Peripheral/lpc40xx/can.hpp"
#include "L4_Testing/testing_frameworks.hpp"


namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Can);

TEST_CASE("Testing lpc40xx Can", "[lpc40xx-can]")
{
  // Simulate local version of LPC_CAN
  LPC_CAN_TypeDef local_can;
  // Clear memory locations
  memset(&local_can, 0, sizeof(local_can));
  LPC_CANAF_TypeDef local_can_acceptance;
  memset(&local_can_acceptance, 0, sizeof(local_can_acceptance));

  // Set acceptance filter to the local acceptance filter
  Can::can_acceptance_filter_register = &local_can_acceptance;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Set up Mock for Pin
  Mock<sjsu::Pin> mock_td;
  Mock<sjsu::Pin> mock_rd;

  Fake(Method(mock_td, SetPinFunction));
  Fake(Method(mock_rd, SetPinFunction));

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
    auto check_power_up = [](sjsu::SystemController::PeripheralID id) -> bool {
      return sjsu::lpc40xx::SystemController::Peripherals::kCan1.device_id ==
             id.device_id;
    };

    // Exercise
    test_can.Initialize();

    // Verify
    // Verify: Peripheral is powered on
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching(check_power_up));

    // Verify: Pins are configured
    Verify(Method(mock_td, SetPinFunction).Using(kMockCan.td_function_code))
        .Once();
    Verify(Method(mock_rd, SetPinFunction).Using(kMockCan.rd_function_code))
        .Once();

    // Verify: Mode is in reset
    CHECK(!bit::Read(local_can.MOD, Can::Mode::kReset));

    SECTION("Default Baud rate")
    {
      constexpr uint32_t kExpectedPrescalar =
          (kDummySystemControllerClockFrequency / Can::kStandardBaudRate) - 1;
      constexpr uint32_t kExpectedSyncJumpWidth = 3;
      constexpr uint32_t kExpectedTimeSegment1  = 6;
      constexpr uint32_t kExpectedTimeSegment2  = 1;
      constexpr uint32_t kExpectedBusSampling   = 0;
      // Verify: Baud rate is set correctly
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

    SECTION("With non-default BAUD rate")
    {
      constexpr units::frequency::hertz_t kDifferentBaudRate = 50'000_Hz;
      constexpr uint32_t kExpectedPrescalar =
          (kDummySystemControllerClockFrequency / kDifferentBaudRate) - 1;
      constexpr uint32_t kExpectedSyncJumpWidth = 3;
      constexpr uint32_t kExpectedTimeSegment1  = 6;
      constexpr uint32_t kExpectedTimeSegment2  = 1;
      constexpr uint32_t kExpectedBusSampling   = 1;

      Can can_with_different_baud(kMockCan, kDifferentBaudRate);
      can_with_different_baud.Initialize();

      // Verify: Baud rate is set correctly
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
    test_can.Enable();

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
    expected_frame = bit::Insert(expected_frame, message.is_remote_request,
                                 Can::FrameInfo::kRemoteRequest);
    expected_frame = bit::Insert(expected_frame, Value(message.format),
                                 Can::FrameInfo::kFormat);

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
      local_can.SR = bit::Clear(local_can.SR, Can::BufferStatus::kTx1Released);
      local_can.SR = bit::Clear(local_can.SR, Can::BufferStatus::kTx2Released);
      local_can.SR = bit::Clear(local_can.SR, Can::BufferStatus::kTx3Released);

      std::thread open_up_tx3([&local_can]() {
        std::this_thread::sleep_for(100ms);
        local_can.SR = bit::Set(local_can.SR, Can::BufferStatus::kTx3Released);
      });

      // Exercise
      test_can.Send(message);
      open_up_tx3.join();

      // Verify
      CHECK(expected_frame == local_can.TFI3);
      CHECK(message.id == local_can.TID3);
      CHECK(kExpectedDataA == local_can.TDA3);
      CHECK(kExpectedDataB == local_can.TDB3);
      CHECK(Value(Can::Commands::kSendTxBuffer3) == local_can.CMR);
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
    expected_frame = bit::Insert(expected_frame, message.is_remote_request,
                                 Can::FrameInfo::kRemoteRequest);
    expected_frame = bit::Insert(expected_frame, Value(message.format),
                                 Can::FrameInfo::kFormat);

    local_can.RFS = expected_frame;
    local_can.RID = message.id;
    local_can.RDA = message.payload[0] | message.payload[1] << 8 |
                    message.payload[2] << 16 | message.payload[3] << 24;
    local_can.RDB = message.payload[4];

    Can::Message_t actual_message = test_can.Receive();

    CHECK(message.id == actual_message.id);
    CHECK(message.format == actual_message.format);
    CHECK(message.is_remote_request == actual_message.is_remote_request);
    CHECK(message.length == actual_message.length);
    CHECK(message.payload == actual_message.payload);
  }

  SECTION("SelfTest()")
  {
    // TODO(#): Add unit test for this
  }

  Can::can_acceptance_filter_register = LPC_CANAF;
}
}  // namespace sjsu::lpc40xx
