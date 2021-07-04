#pragma once

#include <initializer_list>
#include <scope>
#include <string_view>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "peripherals/can.hpp"
#include "peripherals/cortex/interrupt.hpp"
#include "peripherals/inactive.hpp"
#include "peripherals/stm32f10x/pin.hpp"
#include "peripherals/stm32f10x/system_controller.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/macros.hpp"


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
    static constexpr bit::Mask kPrescalar = bit::MaskFromRange(0, 9);
    /// The delay from the nominal Sync point to the sample point is (this value
    /// plus one) CAN clocks.
    static constexpr bit::Mask kTimeSegment1 = bit::MaskFromRange(16, 19);
    /// The delay from the sample point to the next nominal sync point isCAN
    /// clocks. The nominal CAN bit time is (this value plus the value in
    /// kTimeSegment1 plus 3) CAN clocks.
    static constexpr bit::Mask kTimeSegment2 = bit::MaskFromRange(20, 22);
    /// Used to compensate for positive and negative edge phase errors
    static constexpr bit::Mask kSyncJumpWidth = bit::MaskFromRange(24, 25);
    /// bxCAN treats its own transmitted messages as received messages
    /// and stores them (if they pass acceptance filtering) in a Receive 
    /// mailbox. (pg. 659)
    static constexpr bit::Mask kLoopBackMode = bit::MaskFromRange(30);
    /// The bXCAN is able to recieve valid data frames and valid remote 
    /// frames, but it sends only recessive bits on the CAN bus amd it cannot 
    /// start a transmission. (pg. 658)
    static constexpr bit::Mask kSilentMode = bit::MaskFromRange(31);
  };
  ////////////////////////////////////////
  //////// INTERRUPTS FOR LATER //////////
  ////////////////////////////////////////

  /// This struct holds interrupt flags and capture flag status. It is HW mapped
  /// to a 16-bit register: ICR (pg. 557)
  struct Interrupts  // NOLINT
  {

  };

  /// This struct holds CAN controller Master status information.
  /// It is HW mapped to a 32-bit register: MSR (pg. 676)
  struct MasterStatus  // NOLINT
  {
    /// Indicates to the software that the CAN hardware is now in initialization mode
    static constexpr bit::Mask kInitializationAcknowledge = bit::MaskFromRange(0);
    /// Indicates to the software that the CAN hardware is now in Sleep mode.
    static constexpr bit::Mask kSleepAcknowledge          = bit::MaskFromRange(1);
    static constexpr bit::Mask kErrorInterrupt            = bit::MaskFromRange(2);
    static constexpr bit::Mask kWakeupInterrupt           = bit::MaskFromRange(3);
    static constexpr bit::Mask kSleepAcknowledgeInterrupt = bit::MaskFromRange(4);
    /// Indicates if the CAN is a Transmitter
    static constexpr bit::Mask kTransmitMode              = bit::MaskFromRange(8);
    /// Indicates if the CAN is a receiver
    static constexpr bit::Mask kRecieveMode               = bit::MaskFromRange(9);
    /// Holds the last value of Rx
    static constexpr bit::Mask kLastSamplePoint           = bit::MaskFromRange(10);
    /// Moniters the actual value of the CAN_Rx pin.
    static constexpr bit::Mask kCANRxSignal               = bit::MaskFromRange(11);
  };

  /// This struct holds CAN transmit status information. It is HW mapped to a
  /// 32-bit register: TSR (pg. 677).
  struct TransmitStatus  // NOLINT
  {
    static constexpr bit::Mask kRequestCompletedMailbox0    = bit::MaskFromRange(0);
    static constexpr bit::Mask kTransmissionOkMailbox0      = bit::MaskFromRange(1);
    static constexpr bit::Mask kArbitrationLostMailbox0     = bit::MaskFromRange(2);
    static constexpr bit::Mask kTransmissionErrorMailbox0   = bit::MaskFromRange(3);
    static constexpr bit::Mask kAbortRequestMailbox0        = bit::MaskFromRange(7);
    static constexpr bit::Mask kRequestCompletedMailbox1    = bit::MaskFromRange(8);
    static constexpr bit::Mask kTransmissionOkMailbox1      = bit::MaskFromRange(9);
    static constexpr bit::Mask kArbitrationLostMailbox1     = bit::MaskFromRange(10);
    static constexpr bit::Mask kTransmissionErrorMailbox1   = bit::MaskFromRange(11);
    static constexpr bit::Mask kAbortRequestMailbox1        = bit::MaskFromRange(15);
    static constexpr bit::Mask kRequestCompletedMailbox2    = bit::MaskFromRange(16);
    static constexpr bit::Mask kTransmissionOkMailbox2      = bit::MaskFromRange(17);
    static constexpr bit::Mask kArbitrationLostMailbox2     = bit::MaskFromRange(18);
    static constexpr bit::Mask kTransmissionErrorMailbox2   = bit::MaskFromRange(19);
    static constexpr bit::Mask kAbortRequestMailbox2        = bit::MaskFromRange(23);
    static constexpr bit::Mask kMailboxCode                 = bit::MaskFromRange(24, 25);
    static constexpr bit::Mask kTransmitMailbox0Empty       = bit::MaskFromRange(26);
    static constexpr bit::Mask kTransmitMailbox1Empty       = bit::MaskFromRange(27);
    static constexpr bit::Mask kTransmitMailbox2Empty       = bit::MaskFromRange(28);
    static constexpr bit::Mask kLowestPriorityFlagMailbox0  = bit::MaskFromRange(29);
    static constexpr bit::Mask kLowestPriorityFlagMailbox1  = bit::MaskFromRange(30);
    static constexpr bit::Mask kLowestPriorityFlagMailbox2  = bit::MaskFromRange(31);
  };

  /// CANBUS modes
  struct Mode  // NOLINT
  {
    static constexpr bit::Mask kInitializationRequest           = bit::MaskFromRange(0);
    static constexpr bit::Mask kSleepModeRequest                = bit::MaskFromRange(1);
    static constexpr bit::Mask kTransimtFIFOPriority            = bit::MaskFromRange(2);
    static constexpr bit::Mask kRecieveFIFOPriority             = bit::MaskFromRange(3);
    static constexpr bit::Mask kNoAutomaticRetransmission       = bit::MaskFromRange(4);
    static constexpr bit::Mask kAutomaticWakeupMode             = bit::MaskFromRange(5);
    static constexpr bit::Mask kAutomaticBussOffManagement      = bit::MaskFromRange(6);
    static constexpr bit::Mask kTimeTriggeredCommunicationMode  = bit::MaskFromRange(7);
    static constexpr bit::Mask kCanMasterReset                  = bit::MaskFromRange(15);
    static constexpr bit::Mask kDebugFreeze                     = bit::MaskFromRange(16);
  };

  struct FrameIdentifier  // NOLINT
  {
    //Transmit
    static constexpr bit::Mask kTransmitMailboxRequest    = bit::MaskFromRange(0);
    //Both
    static constexpr bit::Mask kRemoteRequest = bit::MaskFromRange(1);
    static constexpr bit::Mask kIdentifierType            = bit::MaskFromRange(2);
    static constexpr bit::Mask kStandardIdentifier        = bit::MaskFromRange(20,3);
    static constexpr bit::Mask kExtendedIdentifier        = bit::MaskFromRange(31,3);
  };

    struct FrameInfo  // NOLINT
  {
    //Both
    static constexpr bit::Mask kDataLengthCode = bit::MaskFromRange(3,0);
    //Transmit
    static constexpr bit::Mask kkTransmitGlobalTime = bit::MaskFromRange(8);
    //Recieve
    static constexpr bit::Mask kFilterMatchIndex = bit::MaskFromRange(15,8);
    //Both
    static constexpr bit::Mask kMessageTimeStamp = bit::MaskFromRange(31,16);
  };
  
  /// CANBUS FIFO Status (pg. 680)
  struct FIFOStatus  // NOLINT
  {
    // Indicates how many messages are pending in the recieve FIFO
    static constexpr bit::Mask kMessagesPending         = bit::MaskFromRange(0, 1);
    // Set by hardware when three messages are stored in the FIFO.
    static constexpr bit::Mask kIsFIFOFull              = bit::MaskFromRange(3);
    // Set by hardware when a new message has been released and passed the filter while the FIFO is full.
    static constexpr bit::Mask kIsFIFOOverrun           = bit::MaskFromRange(4);
    // Release the output mailbox of the FIFO.
    static constexpr bit::Mask kReleaseOutputMailbox    = bit::MaskFromRange(5);
  };

   enum class FIFOSelect : int
   {
    kFIFO1 = 0,
    kFIFO2 = 1,
    kFIFONone = 4
   };



  ////////////////////////////////////////
  //////// Filter Registers     //////////
  ////////////////////////////////////////
  
  /// CAN Filter Master Register
  struct FilterMaster
  {
    static constexpr bit::Mask kInitializationMode            = bit::MaskFromRange(0);
    static constexpr bit::Mask kCan2StartBank                 = bit::MaskFromRange(8, 13);
  };

  /// CAN Filter Bank Mode
   enum class FilterBankMode : int
  {
    kActive                        = 0,
    kInitialization                = 1
  };

  /// CAN Filter Mode Register (CAN_FM1R)
  enum class FilterMode : int
  {
    kMaskMode                      =0,
    kListMode                      =1
  };

  /// CAN Filter Scale Register (CAN_FS1R)
  enum class FilterScale : int
  {
    kDual16BitScale                =0,
    kSingle32BitScale              =1
  };

    /// CAN Filter Activation Register (CAN_FFA1R)
   enum class  FilterActivation  : int
  {
    kNotActive                      =0,
    kActive                         =1
  };

  struct Filter
  {
    static constexpr bit::Mask k0                      = bit::MaskFromRange(0);
    static constexpr bit::Mask k1                      = bit::MaskFromRange(1);
    static constexpr bit::Mask k2                      = bit::MaskFromRange(2);
    static constexpr bit::Mask k3                      = bit::MaskFromRange(3);
    static constexpr bit::Mask k4                      = bit::MaskFromRange(4);
    static constexpr bit::Mask k5                      = bit::MaskFromRange(5);
    static constexpr bit::Mask k6                      = bit::MaskFromRange(6);
    static constexpr bit::Mask k7                      = bit::MaskFromRange(7);
    static constexpr bit::Mask k8                      = bit::MaskFromRange(8);
    static constexpr bit::Mask k9                      = bit::MaskFromRange(9);
    static constexpr bit::Mask k10                     = bit::MaskFromRange(10);
    static constexpr bit::Mask k11                     = bit::MaskFromRange(11);
    static constexpr bit::Mask k12                     = bit::MaskFromRange(12);
    static constexpr bit::Mask k13                     = bit::MaskFromRange(13);
    static constexpr bit::Mask k14                     = bit::MaskFromRange(14);
    static constexpr bit::Mask k15                     = bit::MaskFromRange(15);
    static constexpr bit::Mask k16                     = bit::MaskFromRange(16);
    static constexpr bit::Mask k17                     = bit::MaskFromRange(17);
    static constexpr bit::Mask k18                     = bit::MaskFromRange(18);
    static constexpr bit::Mask k19                     = bit::MaskFromRange(19);
    static constexpr bit::Mask k20                     = bit::MaskFromRange(20);
    static constexpr bit::Mask k21                     = bit::MaskFromRange(21);
    static constexpr bit::Mask k22                     = bit::MaskFromRange(22);
    static constexpr bit::Mask k23                     = bit::MaskFromRange(23);
    static constexpr bit::Mask k24                     = bit::MaskFromRange(24);
    static constexpr bit::Mask k25                     = bit::MaskFromRange(25);
    static constexpr bit::Mask k26                     = bit::MaskFromRange(26);
    static constexpr bit::Mask k27                     = bit::MaskFromRange(27);
  };





  /// CANBUS Mailbox Register
  struct MailboxIdentifier
  {
    static constexpr bit::Mask kTransmitMailboxRequest        = bit::MaskFromRange(0);
    static constexpr bit::Mask kRemoteTransmissionRequest     = bit::MaskFromRange(1);
    static constexpr bit::Mask kIdentifierExtension           = bit::MaskFromRange(2);
    static constexpr bit::Mask kStandardId                    = bit::MaskFromRange(21, 31);
    static constexpr bit::Mask kExtenedId                     = bit::MaskFromRange(3, 31);
  };


  /// https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 554)
  enum class Commands : uint32_t
  {

  };

  /// https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 560)
  /// CAN frame format: https://goo.gl/images/XLjzn5
  enum FrameErrorCodes : uint8_t
  {

  };

  /// Frame Error container with error codes and error messages.
  struct FrameError_t
  {

  };

  /// Lookup table with all of the CANBUS FrameErrors.
  static constexpr std::array<FrameError_t, 19> kFrameErrorTable = {
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

    /// Pointer to the STM32F1 CAN peripheral in memory
    CAN_TypeDef * registers;

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

 

  /// Pointer to the LPC CANBUS acceptance filter peripheral in memory
  // inline static LPC_CANAF_TypeDef * can_acceptance_filter_register = LPC_CANAF;

  /// @param channel - Which CANBUS channel to use
  explicit constexpr Can(const Port_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  {
    /// Power on CANBUS peripheral=
    auto & platform = sjsu::SystemController::GetPlatformController();
    platform.PowerUpPeripheral(channel_.id);

    /// Configure pins
    channel_.td_pin.settings.function = channel_.td_function_code;
    channel_.rd_pin.settings.function = channel_.rd_function_code;

    channel_.td_pin.Initialize();
    channel_.rd_pin.Initialize();

    // Enter Initalization mode in order to write to CAN registers.
    SetMode(Mode::kInitializationRequest, true);
    SetMode(Mode::kNoAutomaticRetransmission, true);
    SetMode(Mode::kAutomaticBussOffManagement, true);
    // Wait to enter Initialization mode
    while (!VerifyStatus(MasterStatus::kInitializationAcknowledge, true)){}

    ConfigureBaudRate();
    // ConfigureReceiveHandler();
    EnableAcceptanceFilter();

    // Leave Initialization mode
    SetMode(Mode::kInitializationRequest, false);
    // Wait to leave Initialization mode
    while (!VerifyStatus(MasterStatus::kInitializationAcknowledge, false)){}
    return;
  }

  void Send(const Message_t & message)
  {
    StmDataRegisters_t registers = ConvertMessageToRegisters(message);

    bool sent = false;
    while (!sent)
    {
      uint32_t status_register = channel_.registers->TSR;
      // Check if any buffer is available.
      if (bit::Read(status_register, TransmitStatus::kTransmitMailbox0Empty))
      {
        channel_.registers->sTxMailBox[0].TDTR = registers.frame;
        channel_.registers->sTxMailBox[0].TDLR = registers.data_a;
        channel_.registers->sTxMailBox[0].TDHR = registers.data_b;
        channel_.registers->sTxMailBox[0].TIR = registers.id;
        sent = true;
      }
      else if (bit::Read(status_register, TransmitStatus::kTransmitMailbox1Empty))
      {
        channel_.registers->sTxMailBox[1].TDTR = registers.frame;
        channel_.registers->sTxMailBox[1].TDLR = registers.data_a;
        channel_.registers->sTxMailBox[1].TDHR = registers.data_b;
        channel_.registers->sTxMailBox[1].TIR = registers.id;
        sent = true;
      }
      else if (bit::Read(status_register, TransmitStatus::kTransmitMailbox2Empty))
      {
        channel_.registers->sTxMailBox[2].TDTR = registers.frame;
        channel_.registers->sTxMailBox[2].TDLR = registers.data_a;
        channel_.registers->sTxMailBox[2].TDHR = registers.data_b;
        channel_.registers->sTxMailBox[2].TIR = registers.id;
        sent = true;
      }
    }
  }

  bool HasData()
  {
    uint32_t fifo0_status = channel_.registers->RF0R;
    uint32_t fifo1_status = channel_.registers->RF1R;
    if (bit::Read(fifo0_status, FIFOStatus::kMessagesPending))
    {
      return true;
    }
    if(bit::Read(fifo1_status, FIFOStatus::kMessagesPending))
    {
      return true;
    }
    return false;
  }
  
  Message_t Receive()
  {
    Message_t message;

    uint32_t fifo0_status = channel_.registers->RF0R;
    uint32_t fifo1_status = channel_.registers->RF1R;
    FIFOSelect fifo_select = FIFOSelect::kFIFONone;

    if (bit::Read(fifo0_status, FIFOStatus::kMessagesPending))
    {
      fifo_select = FIFOSelect::kFIFO1;
    }
    else if(bit::Read(fifo1_status, FIFOStatus::kMessagesPending))
    {
      fifo_select = FIFOSelect::kFIFO2;
    }
    // else
    // {
    //   // Error, tried to recieve when there were no pending messages.
    //   return;
    // }

    uint32_t frame = channel_.registers->sFIFOMailBox[Value(fifo_select)].RDTR;
    uint32_t id = channel_.registers->sFIFOMailBox[Value(fifo_select)].RIR;

    // Extract all of the information from the message frame
    bool is_remote_request = bit::Extract(id,     FrameIdentifier::kRemoteRequest);
    uint32_t length        = bit::Extract(frame,  FrameInfo::kDataLengthCode);
    uint32_t format        = bit::Extract(id,     FrameIdentifier::kIdentifierType);

    message.is_remote_request = is_remote_request;
    message.length            = static_cast<uint8_t>(length);
    message.format            = static_cast<Message_t::Format>(format);

    // Get the frame ID
    if (message.format == Message_t::Format::kExtended)
    {
      message.id = bit::Extract(id, FrameIdentifier::kExtendedIdentifier);
    }
    else
    {
      message.id = bit::Extract(id, FrameIdentifier::kStandardIdentifier);
    }

    // Pull the bytes from RDA into the payload array
    message.payload[0] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDLR >> (0 * 8)) & 0xFF;
    message.payload[1] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDLR >> (1 * 8)) & 0xFF;
    message.payload[2] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDLR >> (2 * 8)) & 0xFF;
    message.payload[3] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDLR >> (3 * 8)) & 0xFF;

    // Pull the bytes from RDB into the payload array
    message.payload[4] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDHR >> (0 * 8)) & 0xFF;
    message.payload[5] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDHR >> (1 * 8)) & 0xFF;
    message.payload[6] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDHR >> (2 * 8)) & 0xFF;
    message.payload[7] = (channel_.registers->sFIFOMailBox[Value(fifo_select)].RDHR >> (3 * 8)) & 0xFF;

    // Release the RX buffer and allow another buffer to be read.
    if (fifo_select == FIFOSelect::kFIFO1)
    {
      channel_.registers->RF0R = bit::Set(channel_.registers->RF0R, FIFOStatus::kReleaseOutputMailbox);
    }
    else if (fifo_select == FIFOSelect::kFIFO2)
    {
      channel_.registers->RF1R = bit::Set(channel_.registers->RF1R, FIFOStatus::kReleaseOutputMailbox);
    }

    return message;
  }

  // bool SelfTest(uint32_t id)
  // {
  //   Message_t test_message;
  //   test_message.id = id;

  //   // Enable reset mode in order to write to registers
  //   SetMode(Mode::kInitializationRequest, true);
  //   // Enable self-test mode
  //   SetLoopback(true);
  //   // Disable reset mode
  //   SetMode(Mode::kInitializationRequest, false);

  //   // Write test message to tx buffer 1
  //   StmDataRegisters_t registers = ConvertMessageToRegisters(test_message);

  //   // Set self-test request and send buffer 1
  //   while (true)
  //   {
  //     uint32_t status_register = channel_.registers->SR;
  //     // Check if any buffer is available.
  //     if (bit::Read(status_register, BufferStatus::kTx1Released))
  //     {
  //       channel_.registers->TFI1 = registers.frame;
  //       channel_.registers->TID1 = registers.id;
  //       channel_.registers->TDA1 = 0;
  //       channel_.registers->TDB1 = 0;
  //       channel_.registers->CMR  = Value(Commands::kSelfReceptionSendTxBuffer1);
  //       break;
  //     }
  //   }

  //   // Allow time for RX to fire
  //   Wait(100ms, [this]() { return HasData(); });

  //   // Read the message from the rx buffer and enqueue it into the rx queue.
  //   auto received_message = Receive();

  //   // Check if the received message matches the one we sent
  //   if (received_message.id != test_message.id)
  //   {
  //     return false;
  //   }

  //   // Disable self-test mode
  //   SetMode(Mode::kReset, true);
  //   SetMode(Mode::kSelfTest, false);
  //   SetMode(Mode::kReset, false);

  //   return true;
  // }
  bool IsBusOff() override
  {}
  ~Can(){}


 private:
  void ConfigureBaudRate()
  {
    constexpr int kTseg1               = 0;
    constexpr int kTseg2               = 0;
    constexpr int kSyncJump            = 0;
    constexpr uint32_t kBaudRateAdjust = kTseg1 + kTseg2 + kSyncJump + 3;

    auto & system         = sjsu::SystemController::GetPlatformController();
    const auto kFrequency = system.GetClockRate(channel_.id);

    // Must be between 8 - 25 time Quanta

    // Clock    = 10 Mhz
    // Tq       = 1/10Mhz = 100 ns
    // bit rate = 100 Khz
    // bit time = 1/100Khz = 10 us
    // # of tq = bit time / tq = 10000 ns  / 100 ns = 100 
    // Tsync = 1
    // TSync + TSeg1 / TSync + TSeg1 + TSeg2 = 80%
    // 1 + Tseg / 100 = 80% so TSeg1 = 79 TSeg2 = 21 

    uint32_t prescaler = ((kFrequency / settings.baud_rate) - 1);

    uint32_t prescaler =
    // uint32_t tq = (Prescaler + 1) * (1 / kFrequency); // Length of time quanta

          // enum BITRATE{CAN_50KBPS,
          //              CAN_100KBPS,
          //              CAN_125KBPS,
          //              CAN_250KBPS,
          //              CAN_500KBPS,
          //              CAN_1000KBPS};

          // CAN_bit_timing_t CAN_bit_timing[6] = 
          // {{2, 13, 45}, 
          //  {2, 15, 20}, 
          //  {2, 13, 18}, 
          //  {2, 13, 9}, 
          //  {2, 15, 4}, 
          //  {2, 15, 2}};
      channel_.registers->BTR =
        bit::Insert(channel_.registers->BTR, 0, BusTiming::kPrescalar);
      channel_.registers->BTR =
        bit::Insert(channel_.registers->BTR, 14, BusTiming::kTimeSegment1);
      channel_.registers->BTR =
        bit::Insert(channel_.registers->BTR, 4, BusTiming::kTimeSegment2);
      channel_.registers->BTR =
        bit::Insert(channel_.registers->BTR, 1, BusTiming::kLoopBackMode);
    

    // sjsu::LogDebug(
    //     "freq = %f :: prescale = %lu", kFrequency.to<double>(), prescaler);
  }
  void ConfigureReceiveHandler(){}
  StmDataRegisters_t ConvertMessageToRegisters(const Message_t & message) const
  {
    StmDataRegisters_t registers;

    uint32_t frame_info =
        bit::Value()
            .Insert(message.length, FrameInfo::kDataLengthCode)
            .Insert(Value(message.format), FrameIdentifier::kIdentifierType)
            .Insert(message.is_remote_request, FrameIdentifier::kRemoteRequest);


    uint32_t frame_id =
        bit:value()
            .Insert(message.is_remote_request, FrameIdentifier::kRemoteRequest)
            .Set(FrameIdentifier::kTransmitMailboxRequest);

    if(message.format == Message_t::Format::kStandard)
    {
      frame_id = 
         bit:value(frame_id)
            .Insert(message.id, FrameIdentifier::kStandardIdentifier);

    }
    else if(message.format == Message_t::Format::kStandard)
    {
        frame_id = 
         bit:value(frame_id)
            .Insert(message.id, FrameIdentifier::kExtendedIdentifier);
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


  
  bool ConfigureFilter([[maybe_unused]] uint32_t id,
                        [[maybe_unused]] uint32_t mask,
                        [[maybe_unused]] bool is_extended = false)
                        {}
  void EnableAcceptanceFilter()
  {
    // (Dont think needed)
    // Configure Filters (8-9 as Mask Mode) (10 - 12 as List Mode)
    // 8 - 12 as 1 1100
    // CAN1->FM1R |= 0x1C << 8;              

    // Activate filter initialization mode (Set bit 0) 
    channel_.registers->FMR  |=   0x1UL;
    SetFilterBankMode(FilterBankMode::kInitialization);
   
    // Deactivate filter 0 (Clear bit 0) 
    channel_.registers->FA1R &= ~(0x1UL);
    ActivateFilter(Filter::k0, FilterActivation::kNotActive);    

    // Configure filter 0 to single 32-bit scale configuration (Set bit 0)
    channel_.registers->FS1R |=   0x1UL;
    SetFilterScale(Filter::k0, FilterScale::kSingle32BitScale);             

    // Clear filters registers to 0
    channel_.registers->sFilterRegister[0].FR1 = 0x0UL; 
    channel_.registers->sFilterRegister[0].FR2 = 0x0UL;

    // Set filter to mask mode (Clear bit 0)
    channel_.registers->FM1R &= ~(0x1UL);       
    SetFilterFilteringMode(Filter::k0, FilterMode::kMaskMode);      

    // Assign filter 0 to FIFO 0 (Clear bit 0) 
    channel_.registers->FFA1R &= ~(0x1UL);	
    SetFilterFIFOSelect(Filter::k0, FilterFIFOSelect::kFIFO0);		   

    // Activate filter 0 (Set bit 0) 
    channel_.registers->FA1R  |=   0x1UL;
    ActivateFilter(Filter::k0, FilterActivation::kActive);        

    // Deactivate filter initialization mode (clear bit 0)
    channel_.registers->FMR   &= ~(0x1UL);
    SetFilterBankMode(FilterBankMode::kActive);	       
  }

  void SetFilterBankMode(FilterBankMode mode)
  {
     channel_.registers->FMR =
        bit::Insert(channel_.registers->FMR, Value(mode), FilterMaster::kInitializationMode);
  }

  void SetFilterFilteringMode(bit::Mask filter, FilterMode mode)
  {
    channel_.registers->FM1R =
        bit::Insert(channel_.registers->FM1R, Value(mode), filter);
  }

  void SetFilterScale(bit::Mask filter, FilterScale scale)
  {
    channel_.registers->FS1R =
        bit::Insert(channel_.registers->FS1R, Value(scale), filter);
  }

  void SetFilterFIFOSelect(bit::Mask filter, FIFOSelect fifo)
  {
    channel_.registers->FFA1R =
        bit::Insert(channel_.registers->FFA1R, Value(fifo), filter);
  }

  void ActivateFilter(bit::Mask filter, FilterActivation state)
  {
    channel_.registers->FA1R =
        bit::Insert(channel_.registers->FA1R, Value(state), filter);
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

  void SetLoopback(bool enable_mode) const
  {
     bit::Insert(channel_.registers->BTR, kLoopBackMode, mode);
  }

    /// Verify controller modess
  bool VerifyStatus(bit::Mask status, bool state) const
  {
    return bit::Read(channel_.registers->MSR, status);
  }
   const Port_t & channel_;
};

template <int port>
inline Can & GetCan()
{
  if constexpr (port == 1)
  {
    static auto & port1_transmit_pin = GetPin<0, 12>();
    static auto & port1_read_pin     = GetPin<0, 11>();

    /// Predefined definition for CAN1
    static const Can::Port_t kCan1 = {
      .td_pin           = port1_transmit_pin,
      .td_function_code = 8,
      .rd_pin           = port1_read_pin,
      .rd_function_code = 2,
      .registers        = stm32f10x::CAN1,
      .id               = sjsu::stm32f10x::SystemController::Peripherals::kCan1,
    };

    static Can can1(kCan1);
    return can1;
  }
  // else if constexpr (port == 2)
  // {
  //   static auto & port2_transmit_pin = GetPin<2, 8>();
  //   static auto & port2_read_pin     = GetPin<2, 7>();

  //   /// Predefined definition for CAN2
  //   static const Can::Port_t kCan2 = {
  //     .td_pin           = port2_transmit_pin,
  //     .td_function_code = 8,
  //     .rd_pin           = port2_read_pin,
  //     .rd_function_code = 2,
  //     .registers        = stm32f10x::CAN2,
  //     .id               = sjsu::stm32f10x::SystemController::Peripherals::kCan2,
  //   };

  //   static Can can2(kCan2);
  //   return can2;
  // }
  else
  {
    static_assert(InvalidOption<port>,
                  SJ2_ERROR_MESSAGE_DECORATOR(
                      "Support CAN ports for LPC40xx are CAN1, CAN2"));
    return GetCan<0>();
  }
}
}  // namespace lpc40xx
}  // namespace sjsu