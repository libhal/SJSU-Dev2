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

  /// This struct holds bit timing values. It is used to configure the CAN bus
  /// clock. It is HW mapped to a 32-bit register: BTR (pg. 683)
  struct BusTiming  // NOLINT
  {
    /// The peripheral bus clock is divided by this value
    static constexpr auto kPrescalar = bit::MaskFromRange(0, 9);
    /// The delay from the nominal Sync point to the sample point is (this value
    /// plus one) CAN clocks.
    static constexpr auto kTimeSegment1 = bit::MaskFromRange(16, 19);
    /// The delay from the sample point to the next nominal sync point isCAN
    /// clocks. The nominal CAN bit time is (this value plus the value in
    /// kTimeSegment1 plus 3) CAN clocks.
    static constexpr auto kTimeSegment2 = bit::MaskFromRange(20, 22);
    /// Used to compensate for positive and negative edge phase errors
    static constexpr auto kSyncJumpWidth = bit::MaskFromRange(24, 25);
    /// bxCAN treats its own transmitted messages as received messages
    /// and stores them (if they pass acceptance filtering) in a Receive
    /// mailbox. (pg. 659)
    static constexpr auto kLoopBackMode = bit::MaskFromRange(30);
    /// The bXCAN is able to recieve valid data frames and valid remote
    /// frames, but it sends only recessive bits on the CAN bus amd it cannot
    /// start a transmission. (pg. 658)
    static constexpr auto kSilentMode = bit::MaskFromRange(31);
  };

  /// CANBUS modes
  struct MasterControl  // NOLINT
  {
    static constexpr auto kInitializationRequest      = bit::MaskFromRange(0);
    static constexpr auto kSleepModeRequest           = bit::MaskFromRange(1);
    static constexpr auto kTransimtFIFOPriority       = bit::MaskFromRange(2);
    static constexpr auto kRecieveFIFOPriority        = bit::MaskFromRange(3);
    static constexpr auto kNoAutomaticRetransmission  = bit::MaskFromRange(4);
    static constexpr auto kAutomaticWakeupMode        = bit::MaskFromRange(5);
    static constexpr auto kAutomaticBussOffManagement = bit::MaskFromRange(6);
    static constexpr auto kTimeTriggeredCommMode      = bit::MaskFromRange(7);
    static constexpr auto kCanMasterReset             = bit::MaskFromRange(15);
    static constexpr auto kDebugFreeze                = bit::MaskFromRange(16);
  };

  /// This struct holds CAN controller Master status information.
  /// It is HW mapped to a 32-bit register: MSR (pg. 676)
  struct MasterStatus  // NOLINT
  {
    /// Indicates to the software that the CAN hardware is now in initialization
    /// mode
    static constexpr auto kInitializationAcknowledge = bit::MaskFromRange(0);
    /// Indicates to the software that the CAN hardware is now in Sleep mode.
    static constexpr auto kSleepAcknowledge          = bit::MaskFromRange(1);
    static constexpr auto kErrorInterrupt            = bit::MaskFromRange(2);
    static constexpr auto kWakeupInterrupt           = bit::MaskFromRange(3);
    static constexpr auto kSleepAcknowledgeInterrupt = bit::MaskFromRange(4);
    /// Indicates if the CAN is a Transmitter
    static constexpr auto kTransmitMode = bit::MaskFromRange(8);
    /// Indicates if the CAN is a receiver
    static constexpr auto kRecieveMode = bit::MaskFromRange(9);
    /// Holds the last value of Rx
    static constexpr auto kLastSamplePoint = bit::MaskFromRange(10);
    /// Moniters the actual value of the CAN_Rx pin.
    static constexpr auto kCANRxSignal = bit::MaskFromRange(11);
  };

  /// This struct holds CAN transmit status information. It is HW mapped to a
  /// 32-bit register: TSR (pg. 677).
  struct TransmitStatus  // NOLINT
  {
    static constexpr auto kRequestCompletedMailbox0  = bit::MaskFromRange(0);
    static constexpr auto kTransmissionOkMailbox0    = bit::MaskFromRange(1);
    static constexpr auto kArbitrationLostMailbox0   = bit::MaskFromRange(2);
    static constexpr auto kTransmissionErrorMailbox0 = bit::MaskFromRange(3);
    static constexpr auto kAbortRequestMailbox0      = bit::MaskFromRange(7);
    static constexpr auto kRequestCompletedMailbox1  = bit::MaskFromRange(8);
    static constexpr auto kTransmissionOkMailbox1    = bit::MaskFromRange(9);
    static constexpr auto kArbitrationLostMailbox1   = bit::MaskFromRange(10);
    static constexpr auto kTransmissionErrorMailbox1 = bit::MaskFromRange(11);
    static constexpr auto kAbortRequestMailbox1      = bit::MaskFromRange(15);
    static constexpr auto kRequestCompletedMailbox2  = bit::MaskFromRange(16);
    static constexpr auto kTransmissionOkMailbox2    = bit::MaskFromRange(17);
    static constexpr auto kArbitrationLostMailbox2   = bit::MaskFromRange(18);
    static constexpr auto kTransmissionErrorMailbox2 = bit::MaskFromRange(19);
    static constexpr auto kAbortRequestMailbox2      = bit::MaskFromRange(23);
    static constexpr auto kMailboxCode           = bit::MaskFromRange(24, 25);
    static constexpr auto kTransmitMailbox0Empty = bit::MaskFromRange(26);
    static constexpr auto kTransmitMailbox1Empty = bit::MaskFromRange(27);
    static constexpr auto kTransmitMailbox2Empty = bit::MaskFromRange(28);
    static constexpr auto kLowestPriorityFlagMailbox0 = bit::MaskFromRange(29);
    static constexpr auto kLowestPriorityFlagMailbox1 = bit::MaskFromRange(30);
    static constexpr auto kLowestPriorityFlagMailbox2 = bit::MaskFromRange(31);
  };

  struct InterruptEnableRegister  // NOLINT
  {
    static constexpr auto kTransmitMailboxEmpty = bit::MaskFromRange(0);
    static constexpr auto kFIFO0MessagePending  = bit::MaskFromRange(1);
    static constexpr auto kFIFO0Full            = bit::MaskFromRange(2);
    static constexpr auto kFIFO0OverRun         = bit::MaskFromRange(3);
    static constexpr auto kFIFO1MessagePending  = bit::MaskFromRange(4);
    static constexpr auto kFIFO1Full            = bit::MaskFromRange(5);
    static constexpr auto kFIFO1OverRun         = bit::MaskFromRange(6);
    static constexpr auto kErrorWarning         = bit::MaskFromRange(8);
    static constexpr auto kErrorPassive         = bit::MaskFromRange(9);
    static constexpr auto kBusOff               = bit::MaskFromRange(10);
    static constexpr auto kLastErrorCode        = bit::MaskFromRange(11);
    static constexpr auto kErrorInterrupt       = bit::MaskFromRange(15);
    static constexpr auto kWakeup               = bit::MaskFromRange(16);
    static constexpr auto kSleep                = bit::MaskFromRange(17);
  };

  struct MaillboxIdentifier  // NOLINT
  {
    // Transmit
    static constexpr auto kTransmitMailboxRequest = bit::MaskFromRange(0);
    // Recieve/Transmit
    static constexpr auto kRemoteRequest      = bit::MaskFromRange(1);
    static constexpr auto kIdentifierType     = bit::MaskFromRange(2);
    static constexpr auto kStandardIdentifier = bit::MaskFromRange(21, 31);
    static constexpr auto kExtendedIdentifier = bit::MaskFromRange(3, 31);
  };

  struct FrameLengthAndInfo  // NOLINT
  {
    // Recieve/Transmit
    static constexpr auto kDataLengthCode = bit::MaskFromRange(3, 0);
    // Transmit
    static constexpr auto kkTransmitGlobalTime = bit::MaskFromRange(8);
    // Recieve
    static constexpr auto kFilterMatchIndex = bit::MaskFromRange(8, 15);
    // Recieve/Transmit
    static constexpr auto kMessageTimeStamp = bit::MaskFromRange(16, 31);
  };

  /// CANBUS FIFO Status (pg. 680)
  struct FIFOStatus  // NOLINT
  {
    // Indicates how many messages are pending in the recieve FIFO
    static constexpr auto kMessagesPending = bit::MaskFromRange(0, 1);
    // Set by hardware when three messages are stored in the FIFO.
    static constexpr auto kIsFIFOFull = bit::MaskFromRange(3);
    // Set by hardware when a new message has been released and passed the
    // filter while the FIFO is full.
    static constexpr auto kIsFIFOOverrun = bit::MaskFromRange(4);
    // Release the output mailbox of the FIFO.
    static constexpr auto kReleaseOutputMailbox = bit::MaskFromRange(5);
  };

  enum class FIFOAssignment : int
  {
    kFIFO1    = 0,
    kFIFO2    = 1,
    kFIFONone = 4
  };

  /// CAN Filter Master Register
  struct FilterMaster
  {
    static constexpr auto kInitializationMode = bit::MaskFromRange(0);
    static constexpr auto kCan2StartBank      = bit::MaskFromRange(8, 13);
  };

  /// CAN Filter Bank Mode
  enum class FilterBankMasterControl : int
  {
    kActive         = 0,
    kInitialization = 1
  };

  /// CAN Filter Mode Register (CAN_FM1R)
  enum class FilterType : int
  {
    kMask = 0,
    kList = 1
  };

  /// CAN Filter Scale Register (CAN_FS1R)
  enum class FilterScale : int
  {
    kDual16BitScale   = 0,
    kSingle32BitScale = 1
  };

  /// CAN Filter Activation Register (CAN_FFA1R)
  enum class FilterActivation : int
  {
    kNotActive = 0,
    kActive    = 1
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

    // Enter Initalization mode in order to write to CAN registers.
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
              .Insert(true, MaillboxIdentifier::kTransmitMailboxRequest)
              .Insert(message.is_remote_request,
                      MaillboxIdentifier::kRemoteRequest)
              .Insert(Value(message.format),
                      MaillboxIdentifier::kIdentifierType)
              .Insert(message.id, MaillboxIdentifier::kStandardIdentifier);
    }
    else if (message.format == Message_t::Format::kExtended)
    {
      frame_id =
          bit::Value(0)
              .Insert(true, MaillboxIdentifier::kTransmitMailboxRequest)
              .Insert(message.is_remote_request,
                      MaillboxIdentifier::kRemoteRequest)
              .Insert(Value(message.format),
                      MaillboxIdentifier::kIdentifierType)
              .Insert(message.id, MaillboxIdentifier::kExtendedIdentifier);
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
      // Error, tried to recieve when there were no pending messages.
      return message;
    }

    uint32_t frame = channel_.can->sFIFOMailBox[Value(fifo_select)].RDTR;
    uint32_t id    = channel_.can->sFIFOMailBox[Value(fifo_select)].RIR;

    // Extract all of the information from the message frame
    bool is_remote_request =
        bit::Extract(id, MaillboxIdentifier::kRemoteRequest);
    uint32_t length = bit::Extract(frame, FrameLengthAndInfo::kDataLengthCode);
    uint32_t format = bit::Extract(id, MaillboxIdentifier::kIdentifierType);

    message.is_remote_request = is_remote_request;
    message.length            = static_cast<uint8_t>(length);
    message.format            = static_cast<Message_t::Format>(format);

    // Get the frame ID
    if (message.format == Message_t::Format::kExtended)
    {
      message.id = bit::Extract(id, MaillboxIdentifier::kExtendedIdentifier);
    }
    else
    {
      message.id = bit::Extract(id, MaillboxIdentifier::kStandardIdentifier);
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

  void SetLoopback()
  {
    SetMasterMode(MasterControl::kInitializationRequest, true);
    channel_.can->BTR = bit::Set(channel_.can->BTR, BusTiming::kLoopBackMode);
    SetMasterMode(MasterControl::kInitializationRequest, false);
  }
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
  struct Filter
  {
    static constexpr auto k0  = bit::MaskFromRange(0);
    static constexpr auto k1  = bit::MaskFromRange(1);
    static constexpr auto k2  = bit::MaskFromRange(2);
    static constexpr auto k3  = bit::MaskFromRange(3);
    static constexpr auto k4  = bit::MaskFromRange(4);
    static constexpr auto k5  = bit::MaskFromRange(5);
    static constexpr auto k6  = bit::MaskFromRange(6);
    static constexpr auto k7  = bit::MaskFromRange(7);
    static constexpr auto k8  = bit::MaskFromRange(8);
    static constexpr auto k9  = bit::MaskFromRange(9);
    static constexpr auto k10 = bit::MaskFromRange(10);
    static constexpr auto k11 = bit::MaskFromRange(11);
    static constexpr auto k12 = bit::MaskFromRange(12);
    static constexpr auto k13 = bit::MaskFromRange(13);
    static constexpr auto k14 = bit::MaskFromRange(14);
    static constexpr auto k15 = bit::MaskFromRange(15);
    static constexpr auto k16 = bit::MaskFromRange(16);
    static constexpr auto k17 = bit::MaskFromRange(17);
    static constexpr auto k18 = bit::MaskFromRange(18);
    static constexpr auto k19 = bit::MaskFromRange(19);
    static constexpr auto k20 = bit::MaskFromRange(20);
    static constexpr auto k21 = bit::MaskFromRange(21);
    static constexpr auto k22 = bit::MaskFromRange(22);
    static constexpr auto k23 = bit::MaskFromRange(23);
    static constexpr auto k24 = bit::MaskFromRange(24);
    static constexpr auto k25 = bit::MaskFromRange(25);
    static constexpr auto k26 = bit::MaskFromRange(26);
    static constexpr auto k27 = bit::MaskFromRange(27);
  };

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
