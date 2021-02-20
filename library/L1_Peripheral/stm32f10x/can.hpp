#pragma once

#include <initializer_list>
#include <scope>
#include <string_view>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/can.hpp"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"


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


  /// CANBUS FIFO Status (pg. 680)
  struct FIFOStatus  // NOLINT
  {
    static constexpr bit::Mask kInitializationRequest           = bit::MaskFromRange(0, 1);
    static constexpr bit::Mask kInitializationRequest           = bit::MaskFromRange(3);
    static constexpr bit::Mask kInitializationRequest           = bit::MaskFromRange(4);
    static constexpr bit::Mask kInitializationRequest           = bit::MaskFromRange(5);
  };

  /// CANBus frame bit masks for the TFM and RFM registers
  struct FrameInfo  // NOLINT
  {

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
  /// on the LPC40xx platform.
  struct Channel_t
  {

  };

  /// Container for the LPC40xx CANBUS registers
  struct LpcRegisters_t
  {

  };

  /// List of supported CANBUS channels
  struct Channel  // NOLINT
  {

  };

  /// Pointer to the LPC CANBUS acceptance filter peripheral in memory
  inline static LPC_CANAF_TypeDef * can_acceptance_filter_register = LPC_CANAF;

  /// @param channel - Which CANBUS channel to use
  explicit constexpr Can(const Channel_t & channel) : channel_(channel) {}

  void ModuleInitialize() override
  void ModuleEnable(bool enable = true) override
  void ConfigureBaudRate(units::frequency::hertz_t baud) override
  bool ConfigureFilter([[maybe_unused]] uint32_t id,
                       [[maybe_unused]] uint32_t mask,
                       [[maybe_unused]] bool is_extended = false) override
  void ConfigureAcceptanceFilter(bool enable) override
  void Send(const Message_t & message) override
  bool HasData([[maybe_unused]] uint32_t id = 0) override
  Message_t Receive([[maybe_unused]] uint32_t id = 0) override
  bool SelfTest(uint32_t id) override
  bool IsBusOff() override

 private:

}
}  // namespace lpc40xx
}  // namespace sjsu