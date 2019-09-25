#include <inttypes.h>
#include "L1_Peripheral/lpc40xx/can.hpp"
#include "utility/log.hpp"

int main(void)
{
  // Using default constructor for CAN 1
  sjsu::lpc40xx::Can can1;
  // Using parameterized constructor for CAN 2
  sjsu::lpc40xx::Can can2(sjsu::lpc40xx::Can::Controllers::kCan2,
                          sjsu::lpc40xx::Can::BaudRates::kBaud100Kbps,
                          sjsu::lpc40xx::Can::kPort2TransmitPin,
                          sjsu::lpc40xx::Can::kPort2ReadPin);
  // Tx Messages
  sjsu::lpc40xx::Can::TxMessage_t tx_message1;
  memset(&tx_message1, 0, sizeof(sjsu::lpc40xx::Can::TxMessage_t));
  sjsu::lpc40xx::Can::TxMessage_t tx_message2;
  memset(&tx_message2, 0, sizeof(sjsu::lpc40xx::Can::TxMessage_t));

  // Rx Messages
  sjsu::lpc40xx::Can::RxMessage_t rx_message1;
  memset(&rx_message1, 0, sizeof(sjsu::lpc40xx::Can::RxMessage_t));
  sjsu::lpc40xx::Can::RxMessage_t rx_message2;
  memset(&rx_message2, 0, sizeof(sjsu::lpc40xx::Can::RxMessage_t));

  constexpr uint8_t kTxQueueSize       = 10;
  constexpr uint8_t kRxQueueSize       = 10;
  constexpr uint16_t kMessageId        = 580;
  constexpr uint8_t kMessagePayload[4] = { 1, 3, 3, 7 };
  constexpr size_t kMessagePayloadSize = 4;

  LOG_INFO("CAN application starting...");
  LOG_INFO(
      "The first part of this example will perform a local self-test. A local");
  LOG_INFO(
      "self-test only requires one SJ2 board and a CAN transceiver with a 120");
  LOG_INFO(
      "Ohm termination resistor. If the self-test passes, then your setup is");
  LOG_INFO("ready to be connected to other CAN nodes. The second part of this");
  LOG_INFO(
      "example will transmit and receive messages on CAN 1 and CAN 2. If");
  LOG_INFO("any two of the controllers enter a BUS-OFF error state and become");
  LOG_INFO(
      "disabled, then the frame error location will be printed to the console");
  LOG_INFO("and the controller will be re-enabled.");

  LOG_INFO("Initializing CAN 1 with a bit rate of 100 kBit/s...");
  sjsu::lpc40xx::Can::StaticQueueConfig_t<kTxQueueSize, kRxQueueSize> can1_q;
  can1.CreateStaticQueues(can1_q);
  can1.Initialize();

  LOG_INFO("Starting local self-test for CAN 1...");
  if (can1.SelfTestBus(146) == true)
  {
    LOG_INFO("CAN 1 self-test passed!");
  }
  else
  {
    LOG_INFO("CAN 1 self-test failed!");
  }

  LOG_INFO("Initializing CAN 2 with a bit rate of 100 kBit/s...");
  sjsu::lpc40xx::Can::StaticQueueConfig_t<kTxQueueSize, kRxQueueSize> can2_q;
  can2.CreateStaticQueues(can2_q);
  can2.Initialize();

  LOG_INFO("Starting local self-test for CAN 2...");
  if (can2.SelfTestBus(244) == true)
  {
    LOG_INFO("CAN 2 self-test passed!");
    // For this example, if CAN 1 is on the same network,
    // then it will have received the self-test
    // message and stored it in its receive buffer. As a result, we call
    // Receive() here to "dump" that message.
    can1.Receive(&rx_message1);
  }
  else
  {
    LOG_INFO("CAN 2 self-test failed!");
  }

  while (true)
  {
    if (can1.Send(&tx_message1, 326, { 1, 2, 3, 4, 5, 6, 7, 8 }) == true)
    {
      LOG_INFO(
          "Sent message 0x146 with a data length of 8 bytes from CAN 1...");
    }

    if (can2.Receive(&rx_message1) == true)
    {
      LOG_INFO("CAN 2 received a message!");
      LOG_INFO("ID: 0x%0" PRIx32, rx_message1.id);
      for (uint8_t i = 0; i < rx_message1.frame_data.data_length; i++)
      {
        LOG_INFO("Data[%i]: 0x%x", i, rx_message1.data.bytes[i]);
      }
    }

    if (can2.Send(&tx_message2, kMessageId, kMessagePayload,
                  kMessagePayloadSize) == true)
    {
      LOG_INFO(
          "Sent message 0x244 with a data length of 4 bytes from CAN 2...");
    }

    if (can1.Receive(&rx_message2) == true)
    {
      LOG_INFO("CAN 1 received a message!");
      LOG_INFO("ID: 0x%0" PRIx32, rx_message2.id);
      for (uint8_t i = 0; i < rx_message2.frame_data.data_length; i++)
      {
        LOG_INFO("Data[%i]: 0x%x", i, rx_message2.data.bytes[i]);
      }
    }

    if (can1.IsBusOff() == true)
    {
      const char * error_message;
      LOG_INFO("CAN 1 is in a BUS-OFF error state and is disabled!");
      if (can1.GetFrameErrorLocation(error_message) == true)
      {
        LOG_INFO("Retrieved frame error location: %s", error_message);
      }
      LOG_INFO("Re-enabling CAN 1...");
      can1.EnableBus();
    }

    if (can2.IsBusOff() == true)
    {
      const char * error_message;
      LOG_INFO("CAN 2 is in a BUS-OFF error state and is disabled!");
      if (can2.GetFrameErrorLocation(error_message) == true)
      {
        LOG_INFO("Retrieved frame error location: %s", error_message);
      }
      LOG_INFO("Re-enabling CAN 2...");
      can2.EnableBus();
    }
    sjsu::Delay(1s);
  }

  return 0;
}
