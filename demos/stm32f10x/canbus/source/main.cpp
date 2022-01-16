#include <cstdint>
#include <initializer_list>
#include <scope>
#include <string_view>

#include "peripherals/stm32f10x/can.hpp"
#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main(void)
{
  sjsu::stm32f10x::Can & can1 = sjsu::stm32f10x::GetCan();

  sjsu::LogInfo("CAN application starting...");
  sjsu::LogInfo(
      "The first part of this example will perform a local self-test.");
  sjsu::LogInfo(
      "If the self-test passes, then your setup is ready to be connected");
  sjsu::LogInfo(
      "to a CAN transceiver and other can nodes. The second part of this");
  sjsu::LogInfo("example will transmit and receive messages on CAN 1.");
  sjsu::LogInfo("Initializing CAN 1 with default bit rate of 100 kBit/s...");
  sjsu::LogInfo("Can::CanSettings_t::baud_rate = kFastdBaudRate reach 1Mhz");

  // Set the CanBus handler to recieve incomming messages.
  // STM32F10x requires recieving an incomming message to clear the interrupt
  // for it.
  can1.settings.handler = [](sjsu::Can & can)
  {
    if (can.HasData())
    {
      auto received_message = can.Receive();
      sjsu::LogInfo(
          "Recieved Message -  %d | %#04x %#04x %#04x %#04x %#04x %#04x %#04x "
          "%#04x",
          received_message.id,
          received_message.payload[0],
          received_message.payload[1],
          received_message.payload[2],
          received_message.payload[3],
          received_message.payload[4],
          received_message.payload[5],
          received_message.payload[6],
          received_message.payload[7]);
    }
  };

  can1.ModuleInitialize();
  unsigned int test_id = 0;

  sjsu::LogInfo("Starting local self-test for CAN 1...");
  if (can1.SelfTest(42))
  {
    sjsu::LogInfo("CAN 2 self-test" SJ2_HI_BOLD_GREEN " passed!");
  }
  else
  {
    sjsu::LogError("CAN 2 self-test" SJ2_HI_BOLD_RED " failed!");
  }

  unsigned int count = 0;
  while (true)
  {
    // Send new message every 3 seconds.
    if (count == 30)
    {
      // Sending:
      // Incrementing ID to verify validity of message.
      // Array of 1's to check frequency with logic analyzer.
      sjsu::LogInfo("Sending Message - ID: %d", test_id);
      sjsu::Can::Message_t test_message;
      test_message.id         = test_id;
      test_message.length     = 8;
      test_message.payload[0] = static_cast<unsigned char>(0x01);
      test_message.payload[1] = static_cast<unsigned char>(0x01+test_id);
      test_message.payload[2] = static_cast<unsigned char>(0x02+test_id);
      test_message.payload[3] = static_cast<unsigned char>(0x03+test_id);
      test_message.payload[4] = static_cast<unsigned char>(0x04+test_id);
      test_message.payload[5] = static_cast<unsigned char>(0x05+test_id);
      test_message.payload[6] = static_cast<unsigned char>(0x06+test_id);
      test_message.payload[7] = static_cast<unsigned char>(0x07+test_id);
      can1.Send(test_message);
      count = 0;
      test_id++;
    }

    count++;
    sjsu::Delay(100ms);
  }

  return 0;
}
