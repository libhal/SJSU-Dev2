#pragma once

#include <FreeRTOS.h>
#include <queue.h>

#include <initializer_list>
#include <cstring>

#include "L1_Peripheral/can.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/macros.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Can final : public sjsu::Can
{
 public:
  // Adding this so Send() with the std::initializer_list is within the scope of
  // this class.
  using sjsu::Can::Send;

  // This struct holds bit timing values. It is used to configure the CAN bus
  // clock. It is HW mapped to a 32-bit register: BTR (pg. 562)
  struct [[gnu::packed]] BusTiming_t
  {
    union {
      uint32_t BTR;
      struct
      {
        // The peripheral bus clock is divided by this value
        uint16_t baud_rate_prescaler : 10;
        uint8_t reserved_1 : 4;
        // Used to compensate for positive and negative edge phase errors
        uint8_t sync_jump_width : 2;
        // Determines the location of the sample point
        uint8_t time_segment_1 : 4;
        uint8_t time_segment_2 : 3;
        // How many times the bus is sampled; 0 == once, 1 == 3 times
        uint8_t sampling : 1;
        uint8_t reserved_2 : 8;
      } values;
    };
  };
  // This struct holds interrupt flags and capture flag status. It is HW mapped
  // to a 16-bit register: ICR (pg. 557)
  struct [[gnu::packed]] Interrupts_t
  {
    union {
      // ICR - Interrupt and Capture Register
      // NOTE: Bits 1-10 are cleared by the CAN controller
      //       as soon as they are read.
      //       Bits 16-23 & 24-31 are released by the CAN
      //       controller as soon as they are read.
      uint32_t ICR;
      struct
      {
        uint8_t rx_buffer_full : 1;
        uint8_t tx_buffer_1_ready : 1;
        uint8_t error_warning : 1;
        uint8_t data_overrun : 1;
        uint8_t wake_up : 1;
        uint8_t error_passive : 1;
        uint8_t arbitration_lost : 1;
        uint8_t bus_error : 1;
        uint8_t identifier_ready : 1;
        uint8_t tx_buffer_2_ready : 1;
        uint8_t tx_buffer_3_ready : 1;
        uint8_t reserved : 5;
        uint8_t error_code_location : 5;
        uint8_t error_code_direction : 1;
        uint8_t error_code_type : 2;
        uint8_t arbitration_lost_location : 8;
      } flags;
    };
  };
  // This struct holds CAN controller global status information.
  // It is a condensed version of the status register.
  // It is HW mapped to a 32-bit register: GSR (pg. 555)
  struct [[gnu::packed]] GlobalStatus_t
  {
    union {
      // GSR - Global Status Register
      uint32_t GSR;
      struct
      {
        uint8_t rx_buffer_full : 1;
        uint8_t data_overrun : 1;
        uint8_t tx_buffer_ready : 1;
        uint8_t tx_complete : 1;
        uint8_t receiving_message : 1;
        uint8_t transmitting_message : 1;
        uint8_t bus_error : 1;
        uint8_t bus_status : 1;
        uint8_t reserved : 8;
        uint8_t rx_error_count : 8;
        uint8_t tx_error_count : 8;
      } flags;
    };
  };
  // This struct holds a R/W value. This value is compared against
  // the Tx and Rx error counts
  // It is HW mapped to a 32-bit register: EWL (pg. 563)
  struct [[gnu::packed]] ErrorWarningLimit_t
  {
    union {
      // EWL - Error Warning Limit Register
      uint32_t EWL;
      struct
      {
        uint8_t error_warning_limit : 8;
        uint32_t reserved : 24;
      } error;
    };
  };
  // This struct holds CAN controller status information. It is HW mapped to a
  // 32-bit register: SR (pg. 564) Each "not_used_x" is already covered in the
  // GSR (global status register).
  struct [[gnu::packed]] Status_t
  {
    union {
      // SR - Status Register
      uint32_t SR;
      struct
      {
        uint8_t not_used_1 : 1;
        uint8_t not_used_2 : 1;
        uint8_t tx_buffer_1_released : 1;
        uint8_t tx_buffer_1_complete : 1;
        uint8_t not_used_3 : 1;
        uint8_t tx_buffer_1_transmitting : 1;
        uint8_t bus_error : 1;
        uint8_t not_used_4 : 1;
        uint8_t not_used_5 : 1;
        uint8_t not_used_6 : 1;
        uint8_t tx_buffer_2_released : 1;
        uint8_t tx_buffer_2_complete : 1;
        uint8_t not_used_7 : 1;
        uint8_t tx_buffer_2_transmitting : 1;
        uint8_t not_used_8 : 1;
        uint8_t not_used_9 : 1;
        uint8_t not_used_10 : 1;
        uint8_t not_used_11 : 1;
        uint8_t tx_buffer_3_released : 1;
        uint8_t tx_buffer_3_complete : 1;
        uint8_t not_used_12 : 1;
        uint8_t tx_buffer_3_transmitting : 1;
        uint8_t not_used_13 : 1;
        uint8_t not_used_14 : 1;
        uint8_t reserved : 8;
      } flags;
    };
  };

  enum Controllers : uint8_t
  {
    kCan1 = 0,
    kCan2 = 1,
    kNumberOfControllers
  };

  inline static bool is_controller_initialized[kNumberOfControllers] = {
    [kCan1] = false,
    [kCan2] = false
  };

  inline static Interrupts_t interrupts[kNumberOfControllers];
  inline static Status_t status[kNumberOfControllers];
  inline static GlobalStatus_t global_status[kNumberOfControllers];

  // Registers
  inline static LPC_CAN_TypeDef * can_registers[kNumberOfControllers] = {
    [kCan1] = LPC_CAN1, [kCan2] = LPC_CAN2
  };

  inline static LPC_CANAF_TypeDef * can_acceptance_filter_register = LPC_CANAF;

  // Queue transmit handles
  inline static QueueHandle_t transmit_queue[kNumberOfControllers] = {
    [kCan1] = NULL,
    [kCan2] = NULL
  };

  // Queue receive handles
  inline static QueueHandle_t receive_queue[kNumberOfControllers] = {
    [kCan1] = NULL,
    [kCan2] = NULL
  };

  // Templated struct the user can configure and then pass to
  // CreateStaticQueues()
  template <size_t kTxLength, size_t kRxLength>
  struct StaticQueueConfig_t
  {
    // Queue data structures
    inline static StaticQueue_t tx_static_queue;
    inline static StaticQueue_t rx_static_queue;

    // Queue lengths
    inline static constexpr uint8_t kTxQueueLength = kTxLength;
    inline static constexpr uint8_t kRxQueueLength = kRxLength;

    // Queue item sizes
    inline static constexpr uint8_t kTxQueueItemSize = sizeof(TxMessage_t *);
    inline static constexpr uint8_t kRxQueueItemSize = sizeof(RxMessage_t);

    // Queue storage area
    inline static uint8_t
        tx_queue_storage_area[kTxQueueLength * kTxQueueItemSize];
    inline static uint8_t
        rx_queue_storage_area[kRxQueueLength * kRxQueueItemSize];
  };

  enum PinFunctions : uint8_t
  {
    kRd1FunctionBit = 1,
    kTd1FunctionBit = 1,
    kRd2FunctionBit = 1,
    kTd2FunctionBit = 1
  };

  enum Interrupts : uint8_t
  {
    kRxBufferIntEnableBit  = 0,
    kTxBuffer1IntEnableBit = 1,
    kTxBuffer2IntEnableBit = 9,
    kTxBuffer3IntEnableBit = 10
  };

  enum Modes : uint8_t
  {
    kReset      = 0,
    kListenOnly = 1,
    kSelfTest   = 2,
    kTxPriority = 3,
    kSleepMode  = 4,
    kRxPolarity = 5,
    kTest       = 7
  };

  enum BaudRates : uint8_t
  {
    kBaud100Kbps = 100
  };

  enum TxBuffers : uint8_t
  {
    kBuffer1 = 0,
    kBuffer2 = 1,
    kBuffer3 = 2
  };

  // https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 554)
  enum Commands : uint8_t
  {
    kReleaseRxBuffer            = 0x04,
    kSendTxBuffer1              = 0x21,
    kSendTxBuffer2              = 0x41,
    kSendTxBuffer3              = 0x81,
    kSelfReceptionSendTxBuffer1 = 0x30,
    kAcceptAllMessages          = 0x02
  };

  // https://www.nxp.com/docs/en/user-guide/UM10562.pdf (pg. 560)
  // CAN frame format: https://goo.gl/images/XLjzn5
  enum FrameErrorCodes : uint8_t
  {
    kStartOfFrame = 0x03,
    kID28toID21 = 0x02,
    kID20toID18 = 0x06,
    kSrtrBit = 0x04,
    kIdeBit = 0x05,
    kID17toID13 = 0x07,
    kID12toID5 = 0x0F,
    kID4toID0 = 0x0E,
    kRtrBit = 0x0C,
    kReservedBit1 = 0x0D,
    kReservedBit0 = 0x09,
    kDataLengthCode = 0x0B,
    kDataField = 0x0A,
    kCrcSequence = 0x08,
    kCrcDelimiter = 0x18,
    kAcknowledgeSlot = 0x19,
    kAcknowledgeDelimiter = 0x1B,
    kEndOfFrame = 0x1A,
    kIntermission = 0x12
  };

  struct FrameError_t
  {
    uint8_t errorCode;
    const char * errorMessage;
  };

  inline static FrameError_t frame_error_table[19] =
  {
    {kStartOfFrame, "Start of Frame"},
    {kID28toID21, "ID28 ... ID21"},
    {kID20toID18, "ID20 ... ID18"},
    {kSrtrBit, "SRTR Bit"},
    {kIdeBit, "IDE Bit"},
    {kID17toID13, "ID17 ... ID13"},
    {kID12toID5, "ID12 ... ID5"},
    {kID4toID0, "ID4 ... ID0"},
    {kRtrBit, "RTR Bit"},
    {kReservedBit1, "Reserved Bit 1"},
    {kReservedBit0, "Reserved Bit 0"},
    {kDataLengthCode, "Data Length Code"},
    {kDataField, "Data Field"},
    {kCrcSequence, "CRC Sequence"},
    {kCrcDelimiter, "CRC Delimiter"},
    {kAcknowledgeSlot, "Acknowledge Slot"},
    {kAcknowledgeDelimiter, "Acknowledge Delimiter"},
    {kEndOfFrame, "End of Frame"},
    {kIntermission, "Intermission"}
  };

  static void ProcessIrq()
  {
    TxMessage_t * message_ptr;
    bool transmit_interrupt, receive_interrupt;

    for (uint8_t controller = 0; controller < kNumberOfControllers;
         controller++)
    {
      if (is_controller_initialized[controller] == true)
      {
        // Capture interrupt register
        // Bits are cleared when you read the ICR register
        interrupts[controller].ICR = can_registers[controller]->ICR;

        transmit_interrupt = interrupts[controller].flags.tx_buffer_1_ready ||
                             interrupts[controller].flags.tx_buffer_2_ready ||
                             interrupts[controller].flags.tx_buffer_3_ready;

        receive_interrupt = interrupts[controller].flags.rx_buffer_full;

        if (transmit_interrupt == true)
        {
          if (xQueueIsQueueEmptyFromISR(transmit_queue[controller]) == pdFALSE)
          {
            // dequeue message pointer
            xQueueReceiveFromISR(transmit_queue[controller], &message_ptr, 0);
            // send it to the available buffer
            if (interrupts[controller].flags.tx_buffer_1_ready)
            {
              WriteMessageToBuffer(
                  message_ptr, kBuffer1, static_cast<Controllers>(controller));
            }
            else if (interrupts[controller].flags.tx_buffer_2_ready)
            {
              WriteMessageToBuffer(
                  message_ptr, kBuffer2, static_cast<Controllers>(controller));
            }
            else if (interrupts[controller].flags.tx_buffer_3_ready)
            {
              WriteMessageToBuffer(
                  message_ptr, kBuffer3, static_cast<Controllers>(controller));
            }
            else
            {
              // will never occur
            }
          }
        }

        if (receive_interrupt == true)
        {
          ReadMessageFromBuffer(static_cast<Controllers>(controller));
        }
      }
    }
  }

  static void CanIrqHandler()
  {
    ProcessIrq();
  }

  inline static const InterruptController::RegistrationInfo_t
      kCanInterruptInfo = {
        .interrupt_request_number  = CAN_IRQn,
        .interrupt_service_routine = &CanIrqHandler,
        .enable_interrupt          = true,
        .priority                  = 5,
      };

  inline static const sjsu::lpc40xx::SystemController
      kDefaultSystemController = sjsu::lpc40xx::SystemController();
  inline static const sjsu::cortex::InterruptController
      kCortexInterruptController = sjsu::cortex::InterruptController();

  inline static const lpc40xx::Pin kPort1ReadPin = Pin(0, 0);
  inline static const lpc40xx::Pin kPort1TransmitPin = Pin(0, 1);
  inline static const lpc40xx::Pin kPort2ReadPin = Pin(2, 7);
  inline static const lpc40xx::Pin kPort2TransmitPin = Pin(2, 8);

  // Default constructor that defaults to CAN 1
  constexpr Can()
      : controller_(kCan1),
        baud_rate_(BaudRates::kBaud100Kbps),
        rd_(kPort1ReadPin),
        td_(kPort1TransmitPin),
        system_controller_(kDefaultSystemController),
        interrupt_controller_(kCortexInterruptController)
  {
  }

  constexpr Can(
      Controllers controller,
      BaudRates baud_rate,
      const sjsu::Pin & td_pin = GetInactive<sjsu::Pin>(),
      const sjsu::Pin & rd_pin = GetInactive<sjsu::Pin>(),
      const sjsu::lpc40xx::SystemController & system_controller =
          kDefaultSystemController,
      const sjsu::cortex::InterruptController & = kCortexInterruptController)
      : controller_(controller),
        baud_rate_(baud_rate),
        rd_(td_pin),
        td_(rd_pin),
        system_controller_(system_controller),
        interrupt_controller_(kCortexInterruptController)
  {
  }

  Status Initialize() const override
  {
    Status status = Status::kSuccess;
    if (controller_ > kNumberOfControllers)
    {
      status = Status::kDeviceNotFound;
    }
    else
    {
      SJ2_ASSERT_FATAL(
          !(transmit_queue[controller_] == NULL),
          "CAN message queues have not been created! Please call "
          "CreateQueues() from this class before calling Initialize().");
      EnablePower();
      ConfigurePins();
      // Enable reset mode in order to write to CAN registers.
      SetControllerMode(kReset, true);
      // Enable local buffer priority mode.
      SetControllerMode(kTxPriority, true);
      // CAN bus clock (on the wire)
      SetBaudRate();
      // Accept all messages
      // TODO(#343): Allow the user to configure their own filter.
      EnableAcceptanceFilter();
      EnableInterrupts();
      // Enable core interrupt
      interrupt_controller_.Register(kCanInterruptInfo);
      // Disable reset mode and enter operating mode.
      SetControllerMode(kReset, false);
      is_controller_initialized[controller_] = true;
    }
    return status;
  }

  // TODO(#344):
  // If this Send() function gets a pointer that only has scope within a
  // function call, then once that function goes out of scope, the pointer will
  // become a dangling pointer (pointer to invalid data). Afterwards, when
  // WriteMessageToBuffer() tries to access the data to write it to the HW
  // buffers, it will get corrupt data. This problem can arise if the user wants
  // to send a 1 time message. However, if the user is sending cyclic messages,
  // it is assumed that global message handles exist somewhere in the user's app
  // layer since they need to be updated periodically.
  bool Send(TxMessage_t * const kMessage,
            uint32_t id,
            const uint8_t * const kPayload,
            size_t length) const override
  {
    bool success = false;

    // Capture status flag register
    status[controller_].SR = can_registers[controller_]->SR;

    // Assemble the CAN message with the user provided data.
    //
    // Since this memory location is written to by this function and is read by
    // the ISR (to eventually write the data at this memory location into the
    // transmit buffer), we disable the CAN interrupt to ensure that complete
    // and valid data is written before it is accessed by the ISR.
    interrupt_controller_.Deregister(CAN_IRQn);
    kMessage->id                           = id;
    kMessage->frame_data.data_length       = (length & 0xF);
    kMessage->frame_data.remote_tx_request = 0;
    for (uint8_t i = 0; i < length; i++)
    {
      kMessage->data.bytes[i] = kPayload[i];
    }
    interrupt_controller_.Register(kCanInterruptInfo);

    // Check if any buffer is available.
    if (status[controller_].flags.tx_buffer_1_released)
    {
      WriteMessageToBuffer(kMessage, kBuffer1, controller_);
      success = true;
    }
    else if (status[controller_].flags.tx_buffer_2_released)
    {
      WriteMessageToBuffer(kMessage, kBuffer2, controller_);
      success = true;
    }
    else if (status[controller_].flags.tx_buffer_3_released)
    {
      WriteMessageToBuffer(kMessage, kBuffer3, controller_);
      success = true;
    }
    else
    {
      // All buffers busy, enqueue the message pointer.
      // TODO(#345): Ideally this should be a priority queue based on message
      // ID.
      if (xQueueSend(transmit_queue[controller_], &kMessage, 0) == pdPASS)
      {
        success = true;
      }
    }
    return success;
  }

  [[gnu::always_inline]] bool Receive(
      RxMessage_t * const kMessage) const override {
    bool success = false;
    if (xQueueReceive(receive_queue[controller_], kMessage, 0) == pdPASS)
    {
      success = true;
    }
    return success;
  }

  bool SelfTestBus(uint32_t id) const override
  {
    bool success = false;

    TxMessage_t test_message_tx;
    RxMessage_t test_message_rx;

    memset(&test_message_tx, 0, sizeof(TxMessage_t));
    memset(&test_message_rx, 0, sizeof(RxMessage_t));

    test_message_tx.id = id;

    // Enable reset mode in order to write to registers
    SetControllerMode(kReset, true);
    // Enable self-test mode
    SetControllerMode(kSelfTest, true);
    // Disable reset mode
    SetControllerMode(kReset, false);
    // Write test message to tx buffer 1
    WriteMessageToBuffer(&test_message_tx, kBuffer1, controller_);
    // Set self-test request and send buffer 1
    can_registers[controller_]->CMR = kSelfReceptionSendTxBuffer1;

    // Allow time for RX interrupt to fire
    sjsu::Delay(2ms);

    // Get the message; the ISR (interrupt service routine)
    // will read the message from the rx buffer
    // and enqeue it into the rx queue.
    Receive(&test_message_rx);

    // Check if the received message matches the one we sent
    if (test_message_rx.id == test_message_tx.id)
    {
      success = true;
    }

    // Disable self-test mode
    SetControllerMode(kReset, true);
    SetControllerMode(kSelfTest, false);
    SetControllerMode(kReset, false);

    return success;
  }

  bool IsBusOff() const override
  {
    status[controller_].SR = can_registers[controller_]->SR;
    return status[controller_].flags.bus_error;
  }

  bool GetFrameErrorLocation(const char * &error_message) const override
  {
    bool success = false;
    interrupts[controller_].ICR = can_registers[controller_]->ICR;

    for (uint8_t i = 0; i < 19; i++)
    {
      if (frame_error_table[i].errorCode ==
          interrupts[controller_].flags.error_code_location)
      {
        error_message = frame_error_table[i].errorMessage;
        success = true;
      }
    }
    return success;
  }

  void EnableBus() const override
  {
    SetControllerMode(kReset, false);
  }

  template <typename T>
  void CreateStaticQueues(T static_queue_config)
  {
    transmit_queue[controller_] =
        xQueueCreateStatic(static_queue_config.kTxQueueLength,
                           static_queue_config.kTxQueueItemSize,
                           static_queue_config.tx_queue_storage_area,
                           &static_queue_config.tx_static_queue);
    receive_queue[controller_] =
        xQueueCreateStatic(static_queue_config.kRxQueueLength,
                           static_queue_config.kRxQueueItemSize,
                           static_queue_config.rx_queue_storage_area,
                           &static_queue_config.rx_static_queue);
  }

 private:
  [[gnu::always_inline]] static void WriteMessageToBuffer(
      const TxMessage_t * const kMessage,
      TxBuffers buffer_number,
      Controllers controller)
  {
    volatile TxMessage_t * ptr_to_buffer_offset;
    Commands command;

    switch (buffer_number)
    {
      case kBuffer1:
      {
        ptr_to_buffer_offset = reinterpret_cast<volatile TxMessage_t *>(
            &can_registers[controller]->TFI1);
        command = kSendTxBuffer1;
      }
      break;
      case kBuffer2:
      {
        ptr_to_buffer_offset = reinterpret_cast<volatile TxMessage_t *>(
            &can_registers[controller]->TFI2);
        command = kSendTxBuffer2;
      }
      break;
      case kBuffer3:
      {
        ptr_to_buffer_offset = reinterpret_cast<volatile TxMessage_t *>(
            &can_registers[controller]->TFI3);
        command = kSendTxBuffer3;
      }
      break;
      default:
        // should never occur
        break;
    }

    kCortexInterruptController.Deregister(CAN_IRQn);
    ptr_to_buffer_offset->TFI        = kMessage->TFI;
    ptr_to_buffer_offset->id         = kMessage->id;
    ptr_to_buffer_offset->data.qword = kMessage->data.qword;
    can_registers[controller]->CMR   = command;
    kCortexInterruptController.Register(kCanInterruptInfo);
  }

  [[gnu::always_inline]] static void ReadMessageFromBuffer(
      Controllers controller)
  {
    RxMessage_t message;
    memset(&message, 0, sizeof(RxMessage_t));

    volatile RxMessage_t * ptr_to_buffer_offset =
        reinterpret_cast<volatile RxMessage_t *>(
            &can_registers[controller]->RFS);

    kCortexInterruptController.Deregister(CAN_IRQn);
    message.RFS        = ptr_to_buffer_offset->RFS;
    message.id         = ptr_to_buffer_offset->id;
    message.data.qword = ptr_to_buffer_offset->data.qword;
    kCortexInterruptController.Register(kCanInterruptInfo);

    xQueueSendFromISR(receive_queue[controller], &message, 0);
    can_registers[controller]->CMR = kReleaseRxBuffer;
  }

  [[gnu::always_inline]] void CaptureRegisterData() {
    interrupts[controller_].ICR    = can_registers[controller_]->ICR;
    status[controller_].SR         = can_registers[controller_]->SR;
    global_status[controller_].GSR = can_registers[controller_]->GSR;
  }

  void SetControllerMode(const Modes kModeBitPos, bool enable_mode) const
  {
    if (enable_mode == true)
    {
      can_registers[controller_]->MOD |= (1 << kModeBitPos);
    }
    else
    {
      can_registers[controller_]->MOD &= ~(1 << kModeBitPos);
    }
  }

  void EnablePower() const
  {
    if (controller_ == kCan1)
    {
      system_controller_.PowerUpPeripheral(
          sjsu::lpc40xx::SystemController::Peripherals::kCan1);
    }
    else
    {
      system_controller_.PowerUpPeripheral(
          sjsu::lpc40xx::SystemController::Peripherals::kCan2);
    }
  }

  void ConfigurePins() const
  {
    // Configure internal pin MUX to map board I/O pins to controller
    //                       _
    //                      / |<--x-->[GPIOx]
    //                     /  |<--x-->[SPIx]
    // [Board I/O Pin]<-->|Mux|<----->[CANx]
    //                     \  |<--x-->[UARTx]
    //                      \_|<--x-->[I2Cx]
    //
    if (controller_ == kCan1)
    {
      rd_.SetPinFunction(kRd1FunctionBit);
      td_.SetPinFunction(kTd1FunctionBit);
    }
    else
    {
      rd_.SetPinFunction(kRd2FunctionBit);
      td_.SetPinFunction(kTd2FunctionBit);
    }
  }

  void EnableInterrupts() const
  {
    can_registers[controller_]->IER |= (1 << kRxBufferIntEnableBit);
    can_registers[controller_]->IER |= (1 << kTxBuffer1IntEnableBit);
    can_registers[controller_]->IER |= (1 << kTxBuffer2IntEnableBit);
    can_registers[controller_]->IER |= (1 << kTxBuffer3IntEnableBit);
  }

  // TODO(#346): Add more baud rates and look into dynamically calculating baud
  // rates for different peripheral bus clock speeds.
  // TODO(#346): Make this function public so the user can set their own baud
  // rate
  // TODO(#346): This function should use GetPeripheralFrequency() to calculate
  // baud rates.
  // TODO(#347): Add details about these calculations in documentation.
  void SetBaudRate() const
  {
    BusTiming_t bus_timing;
    memset(&bus_timing, 0, sizeof(BusTiming_t));
    // Assumes 48Mhz peripheral bus clock
    if (baud_rate_ == kBaud100Kbps)
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

      // Scaled down to 1Mhz
      bus_timing.values.baud_rate_prescaler = 47;
      // Used to compensate for positive and negative edge phase errors. Defines
      // how much the sample point can be shifted.
      bus_timing.values.sync_jump_width = 3;
      // These time segments determine the location of the "sample point".
      bus_timing.values.time_segment_1 = 6;
      bus_timing.values.time_segment_2 = 1;
      // The bus is sampled 3 times (recommended for low speeds).
      bus_timing.values.sampling = 0;

      can_registers[controller_]->BTR = bus_timing.BTR;
    }
  }

  static void EnableAcceptanceFilter()
  {
    can_acceptance_filter_register->AFMR = kAcceptAllMessages;
  }

  Controllers controller_;
  BaudRates baud_rate_;
  const sjsu::Pin & rd_;
  const sjsu::Pin & td_;
  const sjsu::lpc40xx::SystemController & system_controller_;
  const sjsu::cortex::InterruptController & interrupt_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
