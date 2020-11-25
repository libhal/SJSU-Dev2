#pragma once

#include <initializer_list>
#include <scope>
#include <string_view>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/can.hpp"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/macros.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// CANBUS implemenation for the LPC40xx series of devices.
class Can final : public sjsu::Can
{
 public:
  /// Adding this so Send() with the std::initializer_list is within the scope
  /// of this class.
  using sjsu::Can::Send;

  /// This struct holds bit timing values. It is used to configure the CAN bus
  /// clock. It is HW mapped to a 32-bit register: BTR (pg. 562)
  struct BusTiming  // NOLINT
  {
    /// The peripheral bus clock is divided by this value
    static constexpr bit::Mask kPrescalar = bit::MaskFromRange(0, 9);

    /// Used to compensate for positive and negative edge phase errors
    static constexpr bit::Mask kSyncJumpWidth =

        bit::MaskFromRange(14, 15);
    /// The delay from the nominal Sync point to the sample point is (this value
    /// plus one) CAN clocks.
    static constexpr bit::Mask kTimeSegment1 = bit::MaskFromRange(16, 19);

    /// The delay from the sample point to the next nominal sync point isCAN
    /// clocks. The nominal CAN bit time is (this value plus the value in
    /// kTimeSegment1 plus 3) CAN clocks.
    static constexpr bit::Mask kTimeSegment2 = bit::MaskFromRange(20, 22);

    /// How many times the bus is sampled; 0 == once, 1 == 3 times
    static constexpr bit::Mask kSampling = bit::MaskFromRange(23);
  };

  /// This struct holds interrupt flags and capture flag status. It is HW mapped
  /// to a 16-bit register: ICR (pg. 557)
  struct Interrupts  // NOLINT
  {
    // ICR - Interrupt and Capture Register
    // NOTE: Bits 1-10 are cleared by the CAN controller
    //       as soon as they are read.
    //       Bits 16-23 & 24-31 are released by the CAN
    //       controller as soon as they are read.

    /// Assert interrupt when the receive buffer is full
    static constexpr bit::Mask kRxBufferFull = bit::MaskFromRange(0);

    /// Assert interrupt when TX Buffer 1 has finished or aborted its
    /// transmission.
    static constexpr bit::Mask kTx1Ready = bit::MaskFromRange(1);

    /// Assert interrupt when bus status or error status is asserted.
    static constexpr bit::Mask kErrorWarning = bit::MaskFromRange(2);

    /// Assert interrupt on data overrun occurs
    static constexpr bit::Mask kDataOverrun = bit::MaskFromRange(3);

    /// Assert interrupt when CAN controller is sleeping and was woken up from
    /// bus activity.
    static constexpr bit::Mask kWakeup = bit::MaskFromRange(4);

    /// Assert interrupt when the CAN Controller has reached the Error Passive
    /// Status (error counter exceeds 127)
    static constexpr bit::Mask kErrorPassive = bit::MaskFromRange(5);

    /// Assert interrupt when arbitration is lost
    static constexpr bit::Mask kArbitrationLost = bit::MaskFromRange(6);

    /// Assert interrupt on bus error
    static constexpr bit::Mask kBusError = bit::MaskFromRange(7);

    /// Assert interrupt when any message has been successfully transmitted.
    static constexpr bit::Mask kIdentifierReady = bit::MaskFromRange(8);

    /// Assert interrupt when TX Buffer 2 has finished or aborted its
    /// transmission.
    static constexpr bit::Mask kTx2Ready = bit::MaskFromRange(9);

    /// Assert interrupt when TX Buffer 3 has finished or aborted its
    /// transmission.
    static constexpr bit::Mask kTx3Ready = bit::MaskFromRange(10);

    /// Error Code Capture status bits to be read during an interrupt
    static constexpr bit::Mask kErrorCodeLocation =

        bit::MaskFromRange(16, 20);
    /// Indicates if the error occurred during transmission (0) or receiving (1)
    static constexpr bit::Mask kErrorCodeDirection =

        bit::MaskFromRange(21);
    /// The type of bus error that occurred such as bit error, stuff error, etc
    static constexpr bit::Mask kErrorCodeType =

        bit::MaskFromRange(22, 23);
    /// Bit location of where arbitration was lost.
    static constexpr bit::Mask kArbitrationLostLocation =

        bit::MaskFromRange(24, 31);
  };

  /// This struct holds CAN controller global status information.
  /// It is a condensed version of the status register.
  /// It is HW mapped to a 32-bit register: GSR (pg. 555)
  struct GlobalStatus  // NOLINT
  {
    /// If 1, receive buffer has at least 1 complete message stored
    static constexpr bit::Mask kReceiveBuffer = bit::MaskFromRange(0);

    /// Bus status bit. If this is '1' then the bus is active, otherwise the bus
    /// is bus off.
    static constexpr bit::Mask kBusError = bit::MaskFromRange(7);
  };

  /// This struct holds CAN controller status information. It is HW mapped to a
  /// 32-bit register: SR (pg. 564). Many of them are not here because they have
  /// counter parts in GSR (global status register).
  struct BufferStatus  // NOLINT
  {
    /// TX1 Buffer has been released
    static constexpr bit::Mask kTx1Released = bit::MaskFromRange(2);

    /// TX2 Buffer has been released
    static constexpr bit::Mask kTx2Released = bit::MaskFromRange(10);

    /// TX3 Buffer has been released
    static constexpr bit::Mask kTx3Released = bit::MaskFromRange(18);
  };

  /// CANBUS modes
  struct Mode  // NOLINT
  {
    /// Reset CAN Controller, allows configuration registers to be modified.
    static constexpr bit::Mask kReset = bit::MaskFromRange(0);

    /// Put device into Listen Only Mode, device will not acknowledge, messages.
    static constexpr bit::Mask kListenOnly = bit::MaskFromRange(1);

    /// Put device on self test mode.
    static constexpr bit::Mask kSelfTest = bit::MaskFromRange(2);

    /// Enable transmit priority control. When enabled, allows a particular
    static constexpr bit::Mask kTxPriority = bit::MaskFromRange(3);

    /// Put device to Sleep Mode.
    static constexpr bit::Mask kSleepMode = bit::MaskFromRange(4);

    /// Receive polarity mode. If 1 RD input is active high
    static constexpr bit::Mask kRxPolarity = bit::MaskFromRange(5);

    /// Put CAN into test mode, which allows the TD pin to reflect its bits ot
    /// the RD pin.
    static constexpr bit::Mask kTest = bit::MaskFromRange(7);
  };

  /// CANBus frame bit masks for the TFM and RFM registers
  struct FrameInfo  // NOLINT
  {
    /// The message priority bits (not used in this implementation)
    static constexpr bit::Mask kPriority = bit::MaskFromRange(0, 7);

    /// The length of the data
    static constexpr bit::Mask kLength = bit::MaskFromRange(16, 19);

    /// If set to 1, the message becomes a remote request message
    static constexpr bit::Mask kRemoteRequest = bit::MaskFromRange(30);

    /// If 0, the ID is 11-bits, if 1, the ID is 29-bits.
    static constexpr bit::Mask kFormat = bit::MaskFromRange(31);
  };

  /// https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 554)
  enum class Commands : uint32_t
  {
    kReleaseRxBuffer            = 0x04,
    kSendTxBuffer1              = 0x21,
    kSendTxBuffer2              = 0x41,
    kSendTxBuffer3              = 0x81,
    kSelfReceptionSendTxBuffer1 = 0x30,
    kAcceptAllMessages          = 0x02,
  };

  /// https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 560)
  /// CAN frame format: https://goo.gl/images/XLjzn5
  enum FrameErrorCodes : uint8_t
  {
    kStartOfFrame         = 0x03,
    kID28toID21           = 0x02,
    kID20toID18           = 0x06,
    kSrtrBit              = 0x04,
    kIdeBit               = 0x05,
    kID17toID13           = 0x07,
    kID12toID5            = 0x0F,
    kID4toID0             = 0x0E,
    kRtrBit               = 0x0C,
    kReservedBit1         = 0x0D,
    kReservedBit0         = 0x09,
    kDataLengthCode       = 0x0B,
    kDataField            = 0x0A,
    kCrcSequence          = 0x08,
    kCrcDelimiter         = 0x18,
    kAcknowledgeSlot      = 0x19,
    kAcknowledgeDelimiter = 0x1B,
    kEndOfFrame           = 0x1A,
    kIntermission         = 0x12
  };

  /// Frame Error container with error codes and error messages.
  struct FrameError_t
  {
    /// Error code
    uint8_t error_code;
    /// Error message string
    std::string_view error_message;
  };

  /// Lookup table with all of the CANBUS FrameErrors.
  static constexpr std::array<FrameError_t, 19> kFrameErrorTable = {
    FrameError_t{ kStartOfFrame, "Start of Frame" },
    FrameError_t{ kID28toID21, "ID28 ... ID21" },
    FrameError_t{ kID20toID18, "ID20 ... ID18" },
    FrameError_t{ kSrtrBit, "SRTR Bit" },
    FrameError_t{ kIdeBit, "IDE Bit" },
    FrameError_t{ kID17toID13, "ID17 ... ID13" },
    FrameError_t{ kID12toID5, "ID12 ... ID5" },
    FrameError_t{ kID4toID0, "ID4 ... ID0" },
    FrameError_t{ kRtrBit, "RTR Bit" },
    FrameError_t{ kReservedBit1, "Reserved Bit 1" },
    FrameError_t{ kReservedBit0, "Reserved Bit 0" },
    FrameError_t{ kDataLengthCode, "Data Length Code" },
    FrameError_t{ kDataField, "Data Field" },
    FrameError_t{ kCrcSequence, "CRC Sequence" },
    FrameError_t{ kCrcDelimiter, "CRC Delimiter" },
    FrameError_t{ kAcknowledgeSlot, "Acknowledge Slot" },
    FrameError_t{ kAcknowledgeDelimiter, "Acknowledge Delimiter" },
    FrameError_t{ kEndOfFrame, "End of Frame" },
    FrameError_t{ kIntermission, "Intermission" }
  };

  /// Contains all of the information for to control and configure a CANBUS bus
  /// on the LPC40xx platform.
  struct Channel_t
  {
    /// Reference to transmit pin object
    sjsu::Pin & td_pin;

    /// Pin function code for transmit
    uint8_t td_function_code;

    /// Reference to read pin object
    sjsu::Pin & rd_pin;

    /// Pin function code for receive
    uint8_t rd_function_code;

    /// Pointer to the LPC CAN peripheral in memory
    LPC_CAN_TypeDef * registers;

    /// Peripheral's ID
    sjsu::SystemController::ResourceID id;
  };

  /// Container for the LPC40xx CANBUS registers
  struct LpcRegisters_t
  {
    /// TFI register contents
    uint32_t frame = 0;
    /// TID register contents
    uint32_t id = 0;
    /// TDA register contents
    uint32_t data_a = 0;
    /// TDB register contents
    uint32_t data_b = 0;
  };

  /// List of supported CANBUS channels
  struct Channel  // NOLINT
  {
   private:
    inline static auto port1_transmit_pin = Pin(0, 1);
    inline static auto port1_read_pin     = Pin(0, 0);
    inline static auto port2_transmit_pin = Pin(2, 8);
    inline static auto port2_read_pin     = Pin(2, 7);

   public:
    /// Predefined definition for CAN1
    inline static const Channel_t kCan1 = {
      .td_pin           = port1_transmit_pin,
      .td_function_code = 1,
      .rd_pin           = port1_read_pin,
      .rd_function_code = 1,
      .registers        = lpc40xx::LPC_CAN1,
      .id               = sjsu::lpc40xx::SystemController::Peripherals::kCan1,
    };

    /// Predefined definition for CAN2
    inline static const Channel_t kCan2 = {
      .td_pin           = port2_transmit_pin,
      .td_function_code = 1,
      .rd_pin           = port2_read_pin,
      .rd_function_code = 1,
      .registers        = lpc40xx::LPC_CAN2,
      .id               = sjsu::lpc40xx::SystemController::Peripherals::kCan2,
    };
  };

  /// Pointer to the LPC CANBUS acceptance filter peripheral in memory
  inline static LPC_CANAF_TypeDef * can_acceptance_filter_register = LPC_CANAF;

  /// @param channel - Which CANBUS channel to use
  explicit constexpr Can(const Channel_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  {
    /// Power on CANBUS peripheral
    auto & platform = sjsu::SystemController::GetPlatformController();
    platform.PowerUpPeripheral(channel_.id);

    /// Configure pins
    channel_.td_pin.ConfigureFunction(channel_.td_function_code);
    channel_.rd_pin.ConfigureFunction(channel_.rd_function_code);

    // Enable reset mode in order to write to CAN registers.
    SetMode(Mode::kReset, true);

    // Accept all messages
    // TODO(#343): Allow the user to configure their own filter.
    EnableAcceptanceFilter();
  }

  void ModuleEnable(bool enable = true) override
  {
    // Flip logic of enable such that, if enable = true, set reset mode to false
    SetMode(Mode::kReset, !enable);
  }

  /// @param baud - baud rate to configure the CANBUS to
  void ConfigureBaudRate(units::frequency::hertz_t baud) override
  {
    // According to the BOSCH CAN spec, the nominal bit time is divided into 4
    // time segments. These segments need to be programmed for the internal
    // bit timing logic/state machine. Refer to the link below for more
    // details about the importance of these time segments:
    // http://www.keil.com/dd/docs/datashts/silabs/boschcan_ug.pdf
    //
    // Nominal Bit Time : 1 / 100 000 == 10^-5s
    //
    //   0_______________________________10^-5
    //  _/             1 bit             \_
    //   \_______________________________/
    //
    //   | SYNC | PROP | PHASE1 | PHASE2 |        BOSCH
    //   | TSCL |     TSEG1     | TSEG2  |        LPC
    //                          ^
    //                    sample point (industry standard: 80%)

    constexpr int kTseg1               = 0;
    constexpr int kTseg2               = 0;
    constexpr int kSyncJump            = 0;
    constexpr uint32_t kBaudRateAdjust = kTseg1 + kTseg2 + kSyncJump + 3;

    // Equation found p.563 of the user manual
    //    tSCL = CANsuppliedCLK * ((prescaler * kBaudRateAdjust) -  1)
    // Configure the baud rate divider
    auto & system         = sjsu::SystemController::GetPlatformController();
    const auto kFrequency = system.GetClockRate(channel_.id);
    uint32_t prescaler    = (kFrequency / ((baud * kBaudRateAdjust)) - 1);

    sjsu::LogDebug(
        "freq = %f :: prescale = %lu", kFrequency.to<double>(), prescaler);

    // Hold the results in RAM rather than altering the register directly
    // multiple times.
    bit::Value bus_timing;

    // Used to compensate for positive and negative edge phase errors. Defines
    // how much the sample point can be shifted.
    // These time segments determine the location of the "sample point".
    bus_timing.Insert(0, BusTiming::kSyncJumpWidth)
        .Insert(0, BusTiming::kTimeSegment1)
        .Insert(0, BusTiming::kTimeSegment2)
        .Insert(prescaler, BusTiming::kPrescalar);

    if (baud < kStandardBaudRate)
    {
      // The bus is sampled 3 times (recommended for low speeds, 100kHz is
      // considered HIGH).
      bus_timing.Insert(1, BusTiming::kSampling);
    }
    else
    {
      bus_timing.Insert(0, BusTiming::kSampling);
    }

    channel_.registers->BTR = bus_timing;
  }

  bool ConfigureFilter([[maybe_unused]] uint32_t id,
                       [[maybe_unused]] uint32_t mask,
                       [[maybe_unused]] bool is_extended = false) override
  {
    throw Exception(std::errc::not_supported,
                    "This implementation does not support filtering");
  }

  void ConfigureAcceptanceFilter(bool enable) override
  {
    if (enable)
    {
      throw Exception(std::errc::not_supported,
                      "This implementation does not support");
    }
  }

  void Send(const Message_t & message) override
  {
    LpcRegisters_t registers = ConvertMessageToRegisters(message);

    // Wait for one of the buffers to be free so we can transmit a message
    // through it.
    bool sent = false;
    while (!sent)
    {
      uint32_t status_register = channel_.registers->SR;
      // Check if any buffer is available.
      if (bit::Read(status_register, BufferStatus::kTx1Released))
      {
        channel_.registers->TFI1 = registers.frame;
        channel_.registers->TID1 = registers.id;
        channel_.registers->TDA1 = registers.data_a;
        channel_.registers->TDB1 = registers.data_b;
        channel_.registers->CMR  = Value(Commands::kSendTxBuffer1);
        sent                     = true;
      }
      else if (bit::Read(status_register, BufferStatus::kTx2Released))
      {
        channel_.registers->TFI2 = registers.frame;
        channel_.registers->TID2 = registers.id;
        channel_.registers->TDA2 = registers.data_a;
        channel_.registers->TDB2 = registers.data_b;
        channel_.registers->CMR  = Value(Commands::kSendTxBuffer2);
        sent                     = true;
      }
      else if (bit::Read(status_register, BufferStatus::kTx3Released))
      {
        channel_.registers->TFI3 = registers.frame;
        channel_.registers->TID3 = registers.id;
        channel_.registers->TDA3 = registers.data_a;
        channel_.registers->TDB3 = registers.data_b;
        channel_.registers->CMR  = Value(Commands::kSendTxBuffer3);
        sent                     = true;
      }
    }
  }

  bool HasData([[maybe_unused]] uint32_t id = 0) override
  {
    // GlobalStatus::kReceiveBuffer returns 1 (true) if it has data.
    return bit::Read(channel_.registers->GSR, GlobalStatus::kReceiveBuffer);
  }

  Message_t Receive([[maybe_unused]] uint32_t id = 0) override
  {
    Message_t message;

    uint32_t frame = channel_.registers->RFS;

    // Extract all of the information from the message frame
    bool is_remote_request = bit::Extract(frame, FrameInfo::kRemoteRequest);
    uint32_t length        = bit::Extract(frame, FrameInfo::kLength);
    uint32_t format        = bit::Extract(frame, FrameInfo::kFormat);

    message.is_remote_request = is_remote_request;
    message.length            = static_cast<uint8_t>(length);
    message.format            = static_cast<Message_t::Format>(format);

    // Get the frame ID
    const uint32_t kRID = channel_.registers->RID;
    if (message.format == Message_t::Format::kExtended)
    {
      message.id = bit::Extract(kRID, bit::MaskFromRange(0, 10));
    }
    else
    {
      message.id = bit::Extract(kRID, bit::MaskFromRange(0, 28));
    }

    // Pull the bytes from RDA into the payload array
    message.payload[0] = (channel_.registers->RDA >> (0 * 8)) & 0xFF;
    message.payload[1] = (channel_.registers->RDA >> (1 * 8)) & 0xFF;
    message.payload[2] = (channel_.registers->RDA >> (2 * 8)) & 0xFF;
    message.payload[3] = (channel_.registers->RDA >> (3 * 8)) & 0xFF;

    // Pull the bytes from RDB into the payload array
    message.payload[4] = (channel_.registers->RDB >> (0 * 8)) & 0xFF;
    message.payload[5] = (channel_.registers->RDB >> (1 * 8)) & 0xFF;
    message.payload[6] = (channel_.registers->RDB >> (2 * 8)) & 0xFF;
    message.payload[7] = (channel_.registers->RDB >> (3 * 8)) & 0xFF;

    // Release the RX buffer and allow another buffer to be read.
    channel_.registers->CMR = Value(Commands::kReleaseRxBuffer);

    // sjsu::lpc40xx::LPC_CANAF_RAM->mask[0]

    return message;
  }

  bool SelfTest(uint32_t id) override
  {
    Message_t test_message;
    test_message.id = id;

    // Enable reset mode in order to write to registers
    SetMode(Mode::kReset, true);
    // Enable self-test mode
    SetMode(Mode::kSelfTest, true);
    // Disable reset mode
    SetMode(Mode::kReset, false);

    // Write test message to tx buffer 1
    LpcRegisters_t registers = ConvertMessageToRegisters(test_message);

    // Set self-test request and send buffer 1
    while (true)
    {
      uint32_t status_register = channel_.registers->SR;
      // Check if any buffer is available.
      if (bit::Read(status_register, BufferStatus::kTx1Released))
      {
        channel_.registers->TFI1 = registers.frame;
        channel_.registers->TID1 = registers.id;
        channel_.registers->TDA1 = 0;
        channel_.registers->TDB1 = 0;
        channel_.registers->CMR  = Value(Commands::kSelfReceptionSendTxBuffer1);
        break;
      }
    }

    // Allow time for RX to fire
    Wait(100ms, [this]() { return HasData(); });

    // Read the message from the rx buffer and enqueue it into the rx queue.
    auto received_message = Receive();

    // Check if the received message matches the one we sent
    if (received_message.id != test_message.id)
    {
      return false;
    }

    // Disable self-test mode
    SetMode(Mode::kReset, true);
    SetMode(Mode::kSelfTest, false);
    SetMode(Mode::kReset, false);

    return true;
  }

  bool IsBusOff() override
  {
    return bit::Read(channel_.registers->GSR, GlobalStatus::kBusError);
  }

 private:
  /// Convert message into the registers LPC40xx can bus registers.
  ///
  /// @param message - message to convert.
  LpcRegisters_t ConvertMessageToRegisters(const Message_t & message) const
  {
    LpcRegisters_t registers;

    uint32_t frame_info =
        bit::Value()
            .Insert(message.length, FrameInfo::kLength)
            .Insert(message.is_remote_request, FrameInfo::kRemoteRequest)
            .Insert(Value(message.format), FrameInfo::kFormat);

    uint32_t data_a = 0;
    data_a |= message.payload[0] << (0 * 8);
    data_a |= message.payload[1] << (1 * 8);
    data_a |= message.payload[2] << (2 * 8);
    data_a |= message.payload[3] << (3 * 8);

    uint32_t data_b = 0;
    data_b |= message.payload[4] << (0 * 8);
    data_b |= message.payload[5] << (1 * 8);
    data_b |= message.payload[6] << (2 * 8);
    data_b |= message.payload[7] << (3 * 8);

    registers.frame  = frame_info;
    registers.id     = message.id;
    registers.data_a = data_a;
    registers.data_b = data_b;

    return registers;
  }

  /// Enable/Disable controller modes
  ///
  /// @param mode - which mode to enable/disable
  /// @param enable_mode - true if you want to enable the mode. False otherwise.
  void SetMode(bit::Mask mode, bool enable_mode) const
  {
    channel_.registers->MOD =
        bit::Insert(channel_.registers->MOD, enable_mode, mode);
  }

  static void EnableAcceptanceFilter()
  {
    can_acceptance_filter_register->AFMR = Value(Commands::kAcceptAllMessages);
  }

  const Channel_t & channel_;
};
}  // namespace lpc40xx
}  // namespace sjsu
