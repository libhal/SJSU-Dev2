#pragma once

#include <initializer_list>
#include <scope>
#include <string_view>

#include "peripherals/can.hpp"
#include "peripherals/cortex/interrupt.hpp"
#include "peripherals/inactive.hpp"
#include "peripherals/stm32f10x/pin.hpp"
#include "peripherals/stm32f10x/system_controller.hpp"
#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// CANBUS implemenation for the stm32f10x series of devices.
class Can final : public sjsu::Can
{
 public:
  /// Adding this so Send() with the std::initializer_list is within the scope
  /// of this class.
  using sjsu::Can::Send;

  /// This struct holds bit timing values.
  /// It is HW mapped to a 32-bit register: BTR (pg. 683).
  struct BusTiming  // NOLINT
  {
    /// Baud Rate Prescaler
    static constexpr auto kPrescalar = bit::MaskFromRange(0, 9);
    /// Time Segment 1
    static constexpr auto kTimeSegment1 = bit::MaskFromRange(16, 19);
    /// Time Segment 2
    static constexpr auto kTimeSegment2 = bit::MaskFromRange(20, 22);
    /// Resynchronization Jump Width
    static constexpr auto kSyncJumpWidth = bit::MaskFromRange(24, 25);
    /// Loop back mode (debug)
    static constexpr auto kLoopBackMode = bit::MaskFromRange(30);
    /// Silent Mode (debug)
    static constexpr auto kSilentMode = bit::MaskFromRange(31);
  };

  /// This struct holds bit values for master control of CANx.
  /// It is HW mapped to a 32-bit register: MCR (pg. 674).
  struct MasterControl  // NOLINT
  {
    /// Software sets this bit to request the CAN hardware to enter
    /// initialization mode.
    static constexpr auto kInitializationRequest = bit::MaskFromRange(0);
    /// Software sets this bit to request the CAN hardware to enter sleep mode.
    static constexpr auto kSleepModeRequest = bit::MaskFromRange(1);
    /// Set the transmission order when several mailboxes are pending at the
    /// same time.
    static constexpr auto kTransimtFIFOPriority = bit::MaskFromRange(2);
    /// Lock the FIFO from receiving new messages.
    static constexpr auto kReceiveFIFOLocked = bit::MaskFromRange(3);
    /// Disable CAN hardware from retransmiting until successfully transmitted.
    static constexpr auto kNoAutomaticRetransmission = bit::MaskFromRange(4);
    /// Controls the behavior of the CAN hardware on message reception during
    /// Sleep.
    static constexpr auto kAutomaticWakeupMode = bit::MaskFromRange(5);
    /// Controls the behavior of the CAN hardware on leaving Bus-Off state.
    static constexpr auto kAutomaticBussOffManagement = bit::MaskFromRange(6);
    /// Enable Time Triggered Communication mode.
    static constexpr auto kTimeTriggeredCommMode = bit::MaskFromRange(7);
    /// Force a master reset of the bxCan and go to Sleep mode.
    static constexpr auto kCanMasterReset = bit::MaskFromRange(15);
    /// Freeze CAN reception/transmission during debug.
    static constexpr auto kDebugFreeze = bit::MaskFromRange(16);
  };

  /// This struct holds bit assignments for the Master Status Register (MSR)
  /// It is HW mapped to a 32-bit register: MSR (pg. 676).
  struct MasterStatus  // NOLINT
  {
    /// Indicates to the software that the CAN hardware is now in initialization
    /// mode
    static constexpr auto kInitializationAcknowledge = bit::MaskFromRange(0);
    /// Indicates to the software that the CAN hardware is now in Sleep mode.
    static constexpr auto kSleepAcknowledge = bit::MaskFromRange(1);
    /// Set by hardware when a bit of the ESR has been set
    static constexpr auto kErrorInterrupt = bit::MaskFromRange(2);
    /// Set by hardware to signal that a SOF bit has been set.
    static constexpr auto kWakeupInterrupt = bit::MaskFromRange(3);
    /// Set by hardware to signal that the bxCan has entered sleep.
    static constexpr auto kSleepAcknowledgeInterrupt = bit::MaskFromRange(4);
    /// Indicates if the CAN is a Transmitter
    static constexpr auto kTransmitMode = bit::MaskFromRange(8);
    /// Indicates if the CAN is a receiver
    static constexpr auto kReceiveMode = bit::MaskFromRange(9);
    /// Holds the last value of Rx
    static constexpr auto kLastSamplePoint = bit::MaskFromRange(10);
    /// Monitors the actual value of the CAN_Rx pin.
    static constexpr auto kCANRxSignal = bit::MaskFromRange(11);
  };

  /// This struct holds CANx transmit status information.
  /// It is HW mapped to a 32-bit register: TSR (pg. 677).
  struct TransmitStatus  // NOLINT
  {
    /// Mailbox 0 - Set by hardware when the last request (transmit or abort)
    /// has been completed
    static constexpr auto kRequestCompletedMailbox0 = bit::MaskFromRange(0);
    /// Mailbox 0 - Hardware updates this bit after each transmission attempt
    static constexpr auto kTransmissionOkMailbox0 = bit::MaskFromRange(1);
    /// Mailbox 0 - Set when the previous TX failed due to arbitration lost
    static constexpr auto kArbitrationLostMailbox0 = bit::MaskFromRange(2);
    /// Mailbox 0 - Set when the previous TX failed due to an error
    static constexpr auto kTransmissionErrorMailbox0 = bit::MaskFromRange(3);
    /// Mailbox 0 - Set by software to abort the transmission for the mailbox
    static constexpr auto kAbortRequestMailbox0 = bit::MaskFromRange(7);
    /// Mailbox 1 - Set by hardware when the last request (transmit or abort)
    /// has been completed
    static constexpr auto kRequestCompletedMailbox1 = bit::MaskFromRange(8);
    /// Mailbox 1 - Hardware updates this bit after each transmission attempt
    static constexpr auto kTransmissionOkMailbox1 = bit::MaskFromRange(9);
    /// Mailbox 1 - Set when the previous TX failed due to arbitration lost
    static constexpr auto kArbitrationLostMailbox1 = bit::MaskFromRange(10);
    /// Mailbox 1 - Set when the previous TX failed due to an error
    static constexpr auto kTransmissionErrorMailbox1 = bit::MaskFromRange(11);
    /// Mailbox 1 - Set by software to abort the transmission for the mailbox
    static constexpr auto kAbortRequestMailbox1 = bit::MaskFromRange(15);
    /// Mailbox 2 - Set by hardware when the last request (transmit or abort)
    /// has been completed
    static constexpr auto kRequestCompletedMailbox2 = bit::MaskFromRange(16);
    /// Mailbox 2 - Hardware updates this bit after each transmission attempt
    static constexpr auto kTransmissionOkMailbox2 = bit::MaskFromRange(17);
    /// Mailbox 2 - Set when the previous TX failed due to arbitration lost
    static constexpr auto kArbitrationLostMailbox2 = bit::MaskFromRange(18);
    /// Mailbox 2 - Set when the previous TX failed due to an error
    static constexpr auto kTransmissionErrorMailbox2 = bit::MaskFromRange(19);
    /// Mailbox 2 - Set by software to abort the transmission for the mailbox
    static constexpr auto kAbortRequestMailbox2 = bit::MaskFromRange(23);
    /// Number of empty mailboxes
    static constexpr auto kMailboxCode = bit::MaskFromRange(24, 25);
    /// Mailbox 0 - Set by hardware to indicate empty
    static constexpr auto kTransmitMailbox0Empty = bit::MaskFromRange(26);
    /// Mailbox 1 - Set by hardware to indicate empty
    static constexpr auto kTransmitMailbox1Empty = bit::MaskFromRange(27);
    /// Mailbox 2 - Set by hardware to indicate empty
    static constexpr auto kTransmitMailbox2Empty = bit::MaskFromRange(28);
    /// Set by hardware when more than one mailbox is pending and mailbox 0 has
    /// lower priority
    static constexpr auto kLowestPriorityFlagMailbox0 = bit::MaskFromRange(29);
    /// Set by hardware when more than one mailbox is pending and mailbox 1 has
    /// lower priority
    static constexpr auto kLowestPriorityFlagMailbox1 = bit::MaskFromRange(30);
    /// Set by hardware when more than one mailbox is pending and mailbox 2 has
    /// lower priority
    static constexpr auto kLowestPriorityFlagMailbox2 = bit::MaskFromRange(31);
  };

  /// This struct holds the bitmap for enabling CANx interrupts.
  /// It is HW mapped to a 32-bit register: TSR (pg. 680).
  struct InterruptEnableRegister  // NOLINT
  {
    /// Transmit mailbox empty interrupt enable
    static constexpr auto kTransmitMailboxEmpty = bit::MaskFromRange(0);
    /// FIFO 0 message pending interrupt enable
    static constexpr auto kFIFO0MessagePending = bit::MaskFromRange(1);
    /// FIFO 0 full interrupt enable
    static constexpr auto kFIFO0Full = bit::MaskFromRange(2);
    /// FIFO 0 overrun interrupt enable
    static constexpr auto kFIFO0OverRun = bit::MaskFromRange(3);
    /// FIFO 1 message pending interrupt enable
    static constexpr auto kFIFO1MessagePending = bit::MaskFromRange(4);
    /// FIFO 1 full interrupt enable
    static constexpr auto kFIFO1Full = bit::MaskFromRange(5);
    /// FIFO 1 overrun interrupt enable
    static constexpr auto kFIFO1OverRun = bit::MaskFromRange(6);
    /// Error warning interrupt enable
    static constexpr auto kErrorWarning = bit::MaskFromRange(8);
    /// Error passive interrupt enable
    static constexpr auto kErrorPassive = bit::MaskFromRange(9);
    /// Bus-off interrupt enable
    static constexpr auto kBusOff = bit::MaskFromRange(10);
    /// Last error code interrupt enable
    static constexpr auto kLastErrorCode = bit::MaskFromRange(11);
    /// Error interrupt enable
    static constexpr auto kErrorInterrupt = bit::MaskFromRange(15);
    /// Wakeup interrupt enable
    static constexpr auto kWakeup = bit::MaskFromRange(16);
    /// Sleep interrupt enable
    static constexpr auto kSleep = bit::MaskFromRange(17);
  };

  /// This struct holds the bitmap for the mailbox identifier.
  /// It is represents 32-bit register: CAN_TIxR(0 - 2) (pg. 685).
  /// It is represents 32-bit register: CAN_RIxR(0 - 1) (pg. 688).
  struct MailboxIdentifier  // NOLINT
  {
    /// Transmit
    static constexpr auto kTransmitMailboxRequest = bit::MaskFromRange(0);
    /// Receive/Transmit
    static constexpr auto kRemoteRequest = bit::MaskFromRange(1);
    /// Receive/Transmit
    static constexpr auto kIdentifierType = bit::MaskFromRange(2);
    /// Receive/Transmit
    static constexpr auto kStandardIdentifier = bit::MaskFromRange(21, 31);
    /// Receive/Transmit
    static constexpr auto kExtendedIdentifier = bit::MaskFromRange(3, 31);
  };

  /// This struct holds the bitmap for data length control and time stamp.
  /// It is represents 32-bit register: CAN_TDTxR(0 - 2) (pg. 686).
  /// It is represents 32-bit register: CAN_RDTxR(0 - 1) (pg. 689).
  struct FrameLengthAndInfo  // NOLINT
  {
    /// Receive/Transmit
    static constexpr auto kDataLengthCode = bit::MaskFromRange(3, 0);
    /// Transmit
    static constexpr auto kkTransmitGlobalTime = bit::MaskFromRange(8);
    /// Receive
    static constexpr auto kFilterMatchIndex = bit::MaskFromRange(8, 15);
    /// Receive/Transmit
    static constexpr auto kMessageTimeStamp = bit::MaskFromRange(16, 31);
  };

  /// This struct holds the bitmap for the FIFOx Status
  /// It is represents 32-bit register: CAN_RFxR(0 - 1) (pg. 680).
  struct FIFOStatus  // NOLINT
  {
    /// Indicates how many messages are pending in the receive FIFO
    static constexpr auto kMessagesPending = bit::MaskFromRange(0, 1);
    /// Set by hardware when three messages are stored in the FIFO.
    static constexpr auto kIsFIFOFull = bit::MaskFromRange(3);
    /// Set by hardware when a new message has been released and passed the
    /// filter while the FIFO is full.
    static constexpr auto kIsFIFOOverrun = bit::MaskFromRange(4);
    /// Release the output mailbox of the FIFO.
    static constexpr auto kReleaseOutputMailbox = bit::MaskFromRange(5);
  };

  /// This struct holds the bitmap for the filter master control.
  /// It is represents 32-bit register: CAN_FMR (pg. 691).
  struct FilterMaster
  {
    /// Initialization mode for filter banks
    static constexpr auto kInitializationMode = bit::MaskFromRange(0);
    /// Defines the start bank for CAN2
    static constexpr auto kCan2StartBank = bit::MaskFromRange(8, 13);
  };

  /// This enumeration labels the initialization state of a filter.
  /// Used with CAN Filter Master Register (CAN_FMR) (pg. 691).
  enum class FilterBankMasterControl : int
  {
    /// Active filters state
    kActive = 0,
    /// Initialization state for the filter
    kInitialization = 1
  };

  /// This enumeration labels the mode of a filter
  /// Used with CAN Filter Mode Register (CAN_FM1R) (pg. 692)
  enum class FilterType : int
  {
    /// Mask what bits in the identifier to accept
    kMask = 0,
    /// List the identifier to accept
    kList = 1
  };

  /// This enumeration labels the scale of a filter
  /// Used with CAN Filter Scale Register (CAN_FS1R) (pg. 692)
  enum class FilterScale : int
  {
    /// Use two 16 bit identifiers
    kDual16BitScale = 0,
    /// Use one 32 bit identifier
    kSingle32BitScale = 1
  };

  /// This enumeration lables the selectded FIFO.
  /// Used with CAN FIFO Assignment Register (CAN_FFA1R) (pg. 693).
  enum class FIFOAssignment : int
  {
    /// FIFO 1
    kFIFO1 = 0,
    /// FIFO 2
    kFIFO2 = 1,
    /// No FIFO selected
    kFIFONone = 4
  };

  /// This enumeration labels the activation state of a filter
  /// Used with CAN Filter Activation Register (CAN_FFA1R) (pg. 693)
  enum class FilterActivation : int
  {
    /// Disable filter
    kNotActive = 0,
    /// Enable fIlter
    kActive = 1
  };

  /// Contains all of the information for to control and configure a CANBUS bus
  /// on the STM32F10x platform.
  struct Port_t  // NOLINT
  {
    /// Reference to transmit pin object
    sjsu::Pin & td_pin;

    /// Pin function code for transmit
    uint8_t td_function_code;

    /// Reference to read pin object
    sjsu::Pin & rd_pin;

    /// Pin function code for receive
    uint8_t rd_function_code;

    /// Resistor state
    PinSettings_t::Resistor rd_resistor_state;

    /// Pointer to the STM32F1 CAN peripheral in memory
    CAN_TypeDef * can;

    /// Peripheral's ID
    sjsu::SystemController::ResourceID id;
  };

  /// Container for the STM32F10x CANBUS registers
  struct StmDataRegisters_t
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

  /// @param channel - Which CANBUS channel to use
  explicit constexpr Can(const Port_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  {
    /// Power on CANBUS peripheral=
    auto & platform = sjsu::SystemController::GetPlatformController();
    platform.PowerUpPeripheral(channel_.id);  // checked

    /// Configure pins

    // Setup TX as output with alternative control (TXD)
    channel_.td_pin.settings.function = 1;
    channel_.td_pin.settings.Floating();

    // Setup RX as Input with Pullup
    channel_.rd_pin.settings.function = 2;  // Input Floating
    channel_.rd_pin.settings.PullUp();

    channel_.td_pin.Initialize();
    channel_.rd_pin.Initialize();

    // Enter Initialization mode in order to write to CAN registers.
    SetMasterMode(MasterControl::kSleepModeRequest, false);
    SetMasterMode(MasterControl::kInitializationRequest, true);
    SetMasterMode(MasterControl::kNoAutomaticRetransmission, true);
    SetMasterMode(MasterControl::kAutomaticBussOffManagement, true);

    // Wait to enter Initialization mode
    while (!GetMasterStatus(MasterStatus::kInitializationAcknowledge))
    {
    }

    ConfigureBaudRate();
    ConfigureReceiveHandler();

    EnableAcceptanceFilter();

    // Leave Initialization mode
    SetMasterMode(MasterControl::kInitializationRequest, false);

    // Wait to leave Initialization mode
    while (GetMasterStatus(MasterStatus::kInitializationAcknowledge))
    {
    }

    return;
  }

  /// Converts desired message to the CANx registers
  StmDataRegisters_t ConvertMessageToRegisters(const Message_t & message) const
  {
    StmDataRegisters_t registers;

    uint32_t frame_info =
        bit::Value(0)
            // .Insert(message.uptime, FrameLengthAndInfo::kMessageTimeStamp)
            .Insert(message.length, FrameLengthAndInfo::kDataLengthCode);

    uint32_t frame_id = 0;

    if (message.format == Message_t::Format::kStandard)
    {
      frame_id =
          bit::Value(0)
              .Insert(true, MailboxIdentifier::kTransmitMailboxRequest)
              .Insert(message.is_remote_request,
                      MailboxIdentifier::kRemoteRequest)
              .Insert(Value(message.format), MailboxIdentifier::kIdentifierType)
              .Insert(message.id, MailboxIdentifier::kStandardIdentifier);
    }
    else if (message.format == Message_t::Format::kExtended)
    {
      frame_id =
          bit::Value(0)
              .Insert(true, MailboxIdentifier::kTransmitMailboxRequest)
              .Insert(message.is_remote_request,
                      MailboxIdentifier::kRemoteRequest)
              .Insert(Value(message.format), MailboxIdentifier::kIdentifierType)
              .Insert(message.id, MailboxIdentifier::kExtendedIdentifier);
    }

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
    registers.id     = frame_id;
    registers.data_a = data_a;
    registers.data_b = data_b;

    return registers;
  }

  void Send(const Message_t & message) override
  {
    StmDataRegisters_t registers = ConvertMessageToRegisters(message);
    bool sent                    = false;
    while (!sent)
    {
      uint32_t status_register = channel_.can->TSR;
      // Check if any buffer is available.
      if (bit::Read(status_register, TransmitStatus::kTransmitMailbox0Empty))
      {
        channel_.can->sTxMailBox[0].TDTR &= ~(0xF);
        channel_.can->sTxMailBox[0].TDTR |= message.length & 0xF;
        channel_.can->sTxMailBox[0].TDLR = registers.data_a;
        channel_.can->sTxMailBox[0].TDHR = registers.data_b;
        channel_.can->sTxMailBox[0].TIR  = registers.id;
        sent                             = true;
      }
      else if (bit::Read(status_register,
                         TransmitStatus::kTransmitMailbox1Empty))
      {
        channel_.can->sTxMailBox[1].TDTR &= ~(0xF);
        channel_.can->sTxMailBox[1].TDTR |= message.length & 0xF;
        channel_.can->sTxMailBox[1].TDLR = registers.data_a;
        channel_.can->sTxMailBox[1].TDHR = registers.data_b;
        channel_.can->sTxMailBox[1].TIR  = registers.id;
        sent                             = true;
      }
      else if (bit::Read(status_register,
                         TransmitStatus::kTransmitMailbox2Empty))
      {
        channel_.can->sTxMailBox[2].TDTR &= ~(0xF);
        channel_.can->sTxMailBox[2].TDTR |= message.length & 0xF;
        channel_.can->sTxMailBox[2].TDLR = registers.data_a;
        channel_.can->sTxMailBox[2].TDHR = registers.data_b;
        channel_.can->sTxMailBox[2].TIR  = registers.id;
        sent                             = true;
      }
    }
  }

  bool HasData() override
  {
    uint32_t fifo0_status = channel_.can->RF0R;
    uint32_t fifo1_status = channel_.can->RF1R;
    if (bit::Read(fifo0_status, FIFOStatus::kMessagesPending))
    {
      return true;
    }
    if (bit::Read(fifo1_status, FIFOStatus::kMessagesPending))
    {
      return true;
    }
    return false;
  }

  Message_t Receive() override
  {
    Message_t message;

    uint32_t fifo0_status      = channel_.can->RF0R;
    uint32_t fifo1_status      = channel_.can->RF1R;
    FIFOAssignment fifo_select = FIFOAssignment::kFIFONone;

    if (bit::Read(fifo0_status, FIFOStatus::kMessagesPending))
    {
      fifo_select = FIFOAssignment::kFIFO1;
    }
    else if (bit::Read(fifo1_status, FIFOStatus::kMessagesPending))
    {
      fifo_select = FIFOAssignment::kFIFO2;
    }
    else
    {
      // Error, tried to receive when there were no pending messages.
      return message;
    }

    uint32_t frame = channel_.can->sFIFOMailBox[Value(fifo_select)].RDTR;
    uint32_t id    = channel_.can->sFIFOMailBox[Value(fifo_select)].RIR;

    // Extract all of the information from the message frame
    bool is_remote_request =
        bit::Extract(id, MailboxIdentifier::kRemoteRequest);
    uint32_t length = bit::Extract(frame, FrameLengthAndInfo::kDataLengthCode);
    uint32_t format = bit::Extract(id, MailboxIdentifier::kIdentifierType);

    message.is_remote_request = is_remote_request;
    message.length            = static_cast<uint8_t>(length);
    message.format            = static_cast<Message_t::Format>(format);

    // Get the frame ID
    if (message.format == Message_t::Format::kExtended)
    {
      message.id = bit::Extract(id, MailboxIdentifier::kExtendedIdentifier);
    }
    else
    {
      message.id = bit::Extract(id, MailboxIdentifier::kStandardIdentifier);
    }

    // Pull the bytes from RDL into the payload array
    message.payload[0] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDLR >> (0 * 8)) & 0xFF;
    message.payload[1] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDLR >> (1 * 8)) & 0xFF;
    message.payload[2] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDLR >> (2 * 8)) & 0xFF;
    message.payload[3] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDLR >> (3 * 8)) & 0xFF;

    // Pull the bytes from RDH into the payload array
    message.payload[4] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDHR >> (0 * 8)) & 0xFF;
    message.payload[5] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDHR >> (1 * 8)) & 0xFF;
    message.payload[6] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDHR >> (2 * 8)) & 0xFF;
    message.payload[7] =
        (channel_.can->sFIFOMailBox[Value(fifo_select)].RDHR >> (3 * 8)) & 0xFF;

    // Release the RX buffer and allow another buffer to be read.
    if (fifo_select == FIFOAssignment::kFIFO1)
    {
      channel_.can->RF0R =
          bit::Set(channel_.can->RF0R, FIFOStatus::kReleaseOutputMailbox);
    }
    else if (fifo_select == FIFOAssignment::kFIFO2)
    {
      channel_.can->RF1R =
          bit::Set(channel_.can->RF1R, FIFOStatus::kReleaseOutputMailbox);
    }

    return message;
  }

  bool SelfTest(uint32_t id) override
  {
    Message_t test_message;
    test_message.id = id;

    // Enable self-test mode
    DisableInterrupts();
    SetLoopback();

    Send(test_message);

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
    ClearLoopback();
    EnableInterrupts();

    return true;
  }

  bool IsBusOff() override
  {
    // True = Bus is in sleep mode
    // False = Bus has left sleep mode.
    return bit::Read(channel_.can->MCR, MasterStatus::kSleepAcknowledge);
  }

  /// Sets CANx to receive what it transmits
  void SetLoopback()
  {
    SetMasterMode(MasterControl::kInitializationRequest, true);
    channel_.can->BTR = bit::Set(channel_.can->BTR, BusTiming::kLoopBackMode);
    SetMasterMode(MasterControl::kInitializationRequest, false);
  }

  /// Disabled CANx from receiving what it transmits
  void ClearLoopback()
  {
    SetMasterMode(MasterControl::kInitializationRequest, true);
    channel_.can->BTR = bit::Clear(channel_.can->BTR, BusTiming::kLoopBackMode);
    SetMasterMode(MasterControl::kInitializationRequest, false);
  }

  /// Enable/Disable controller modes
  ///
  /// @param mode - which mode to enable/disable
  /// @param enable_mode - true if you want to enable the mode. False otherwise.
  void SetMasterMode(bit::Mask mode, bool enable_mode)
  {
    channel_.can->MCR = bit::Insert(channel_.can->MCR, enable_mode, mode);
  }

 private:
  void ConfigureBaudRate()
  {
    constexpr int tseg1     = 4;
    constexpr int tseg2     = 1;
    constexpr int sync_jump = 0;

    constexpr uint32_t clocks_per_bit = tseg1 + tseg2 + sync_jump + 3;

    auto & system         = sjsu::SystemController::GetPlatformController();
    const auto frequency  = system.GetClockRate(channel_.id);
    const auto clock_rate = settings.baud_rate * clocks_per_bit;
    uint32_t prescaler =
        (frequency.to<uint32_t>() / (clock_rate.to<uint32_t>()) - 1);

    channel_.can->BTR =
        bit::Insert(channel_.can->BTR, prescaler, BusTiming::kPrescalar);
    channel_.can->BTR =
        bit::Insert(channel_.can->BTR, tseg1, BusTiming::kTimeSegment1);
    channel_.can->BTR =
        bit::Insert(channel_.can->BTR, tseg2, BusTiming::kTimeSegment2);
    channel_.can->BTR =
        bit::Insert(channel_.can->BTR, sync_jump, BusTiming::kSyncJumpWidth);
  }

  void ConfigureReceiveHandler()
  {
    if (settings.handler)
    {
      InterruptController::GetPlatformController().Enable({
          .interrupt_request_number = stm32f10x::CAN1_RX0_IRQn,
          .interrupt_handler        = [this]() { settings.handler(*this); },
      });

      bit::Register(&channel_.can->IER)
          .Set(InterruptEnableRegister::kFIFO0MessagePending)
          .Save();
      bit::Register(&channel_.can->IER)
          .Set(InterruptEnableRegister::kFIFO1MessagePending)
          .Save();
    }
    else
    {
      bit::Register(&channel_.can->IER)
          .Clear(InterruptEnableRegister::kFIFO0MessagePending)
          .Save();
      bit::Register(&channel_.can->IER)
          .Clear(InterruptEnableRegister::kFIFO1MessagePending)
          .Save();
    }
  }

  void EnableInterrupts()
  {
    bit::Register(&channel_.can->IER)
        .Set(InterruptEnableRegister::kFIFO0MessagePending)
        .Save();
    bit::Register(&channel_.can->IER)
        .Set(InterruptEnableRegister::kFIFO1MessagePending)
        .Save();
  }
  void DisableInterrupts()
  {
    bit::Register(&channel_.can->IER)
        .Clear(InterruptEnableRegister::kFIFO0MessagePending)
        .Save();
    bit::Register(&channel_.can->IER)
        .Clear(InterruptEnableRegister::kFIFO1MessagePending)
        .Save();
  }

  bool ConfigureFilter([[maybe_unused]] uint32_t id,
                       [[maybe_unused]] uint32_t mask,
                       [[maybe_unused]] bool is_extended = false)
  {
    return false;
  }

  void EnableAcceptanceFilter()
  {
    // Activate filter initialization mode (Set bit)
    SetFilterBankMode(FilterBankMasterControl::kInitialization);

    // Deactivate filter 0 (Clear bit)
    SetFilterActivationState<0>(FilterActivation::kNotActive);

    // Configure filter 0 to single 32-bit scale configuration (Set bit)
    SetFilterScale<0>(FilterScale::kSingle32BitScale);

    // Clear filter 0 registers to accept all messages.
    channel_.can->sFilterRegister[0].FR1 = 0;
    channel_.can->sFilterRegister[0].FR2 = 0;

    // Set filter to mask mode
    SetFilterType<0>(FilterType::kMask);

    // Assign filter 0 to FIFO 0 (Clear bit)
    SetFilterFIFOAssignment<0>(FIFOAssignment::kFIFO1);

    // Activate filter 0 (Set bit)
    SetFilterActivationState<0>(FilterActivation::kActive);

    // Deactivate filter initialization mode (clear bit)
    SetFilterBankMode(FilterBankMasterControl::kActive);
  }

  void SetFilterBankMode(FilterBankMasterControl mode)
  {
    bit::Register(&channel_.can->FMR)
        .Insert(Value(mode), FilterMaster::kInitializationMode)
        .Save();
  }

  template <uint32_t filter>
  void SetFilterType(FilterType filtertype)
  {
    static_assert((filter >= 0 && filter <= 27),
                  "There are only 28 filters available 0 - 27");
    bit::Register(&channel_.can->FM1R)
        .Insert(Value(filtertype), bit::MaskFromRange(filter))
        .Save();
  }

  template <uint32_t filter>
  void SetFilterScale(FilterScale scale)
  {
    static_assert((filter >= 0 && filter <= 27),
                  "There are only 28 filters available 0 - 27");
    bit::Register(&channel_.can->FS1R)
        .Insert(Value(scale), bit::MaskFromRange(filter))
        .Save();
  }

  template <uint32_t filter>
  void SetFilterFIFOAssignment(FIFOAssignment fifo)
  {
    static_assert((filter >= 0 && filter <= 27),
                  "There are only 28 filters available 0 - 27");
    bit::Register(&channel_.can->FFA1R)
        .Insert(Value(fifo), bit::MaskFromRange(filter))
        .Save();
  }

  template <uint32_t filter>
  void SetFilterActivationState(FilterActivation state)
  {
    static_assert((filter >= 0 && filter <= 27),
                  "There are only 28 filters available 0 - 27");
    bit::Register(&channel_.can->FA1R)
        .Insert(Value(state), bit::MaskFromRange(filter))
        .Save();
  }

  bool GetMasterStatus(bit::Mask mode) const
  {
    return bit::Read(CAN1->MSR, mode);
  }

  const Port_t & channel_;
};

inline Can & GetCan()
{
  static auto & port1_transmit_pin = GetPin<'A', 12>();
  static auto & port1_read_pin     = GetPin<'A', 11>();

  /// Predefined definition for CAN1
  static const Can::Port_t kCan1 = {
    .td_pin = port1_transmit_pin,
    .rd_pin = port1_read_pin,
    .can    = stm32f10x::CAN1,
    .id     = sjsu::stm32f10x::SystemController::Peripherals::kCan1,
  };

  static Can can1(kCan1);
  return can1;
}

}  // namespace stm32f10x
}  // namespace sjsu
