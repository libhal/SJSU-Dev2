#include <algorithm>

#include "L1_Peripheral/lpc40xx/can.hpp"
#include "utility/log.hpp"

int main(void)
{
  sjsu::lpc40xx::Can can1(sjsu::lpc40xx::Can::Channel::kCan1);
  sjsu::lpc40xx::Can can2(sjsu::lpc40xx::Can::Channel::kCan2);

  sjsu::LogInfo("CAN application starting...");
  sjsu::LogInfo(
      "The first part of this example will perform a local self-test. A local");
  sjsu::LogInfo(
      "self-test only requires one SJ2 board and a CAN transceiver with a 120");
  sjsu::LogInfo(
      "Ohm termination resistor. If the self-test passes, then your setup is");
  sjsu::LogInfo(
      "ready to be connected to other CAN nodes. The second part of this");
  sjsu::LogInfo(
      "example will transmit and receive messages on CAN 1 and CAN 2. If");
  sjsu::LogInfo(
      "any two of the controllers enter a BUS-OFF error state and become");
  sjsu::LogInfo(
      "disabled, then the frame error location will be printed to the console");
  sjsu::LogInfo("and the controller will be re-enabled.");

  sjsu::LogInfo("Initializing CAN 1 with default bit rate of 100 kBit/s...");
  can1.Initialize();
  sjsu::LogInfo("Initializing CAN 2 with default bit rate of 100 kBit/s...");
  can2.Initialize();

  sjsu::LogInfo("Enabling CAN 1 & CAN 2...");
  can1.Enable();
  can2.Enable();

  sjsu::LogInfo("Configuring CAN 1 & CAN 2 baud rates (setting to 100kHz)...");
  can1.ConfigureBaudRate(100_kHz);
  can1.ConfigureBaudRate(100_kHz);

  sjsu::LogInfo("Starting local self-test for CAN 1...");
  if (can1.SelfTest(146))
  {
    sjsu::LogInfo("CAN 1 self-test" SJ2_HI_BOLD_GREEN " passed!");
  }
  else
  {
    sjsu::LogError("CAN 1 self-test" SJ2_HI_BOLD_RED " failed!");
  }

  sjsu::LogInfo("Starting local self-test for CAN 2...");
  if (can1.SelfTest(244))
  {
    sjsu::LogInfo("CAN 2 self-test" SJ2_HI_BOLD_GREEN " passed!");
  }
  else
  {
    sjsu::LogError("CAN 2 self-test" SJ2_HI_BOLD_RED " failed!");
  }

  while (true)
  {
    // Demonstrate using array literals
    can1.Send(326, { 1, 2, 3, 4, 5, 6, 7, 8 });
    sjsu::LogInfo(
        "Sent message 0x146 with a data length of 8 bytes from CAN 1...");

    // See the array appear back on the device
    if (can2.HasData())
    {
      sjsu::Can::Message_t message = can2.Receive();
      sjsu::LogInfo("CAN 2 received a message!");
      sjsu::LogInfo("ID: 0x%0" PRIX32, message.id);
      for (uint8_t i = 0; i < message.length; i++)
      {
        sjsu::LogInfo("Data[%i]: 0x%x", i, message.payload[i]);
      }
    }

    // Demonstrate constructing a CAN message from scratch
    const std::array<uint8_t, 4> kMessagePayload = { 1, 3, 3, 7 };

    sjsu::Can::Message_t tx_message;
    tx_message.id     = 0x244;
    tx_message.length = sizeof(kMessagePayload);
    tx_message.SetPayload(kMessagePayload);

    can2.Send(tx_message);

    sjsu::LogInfo(
        "Sent message 0x244 with a data length of 4 bytes from CAN 2...");

    if (can1.HasData())
    {
      sjsu::Can::Message_t message = can1.Receive();
      sjsu::LogInfo("CAN 1 received a message!");
      sjsu::LogInfo("ID: 0x%0" PRIx32, message.id);
      for (uint8_t i = 0; i < message.length; i++)
      {
        sjsu::LogInfo("Data[%i]: 0x%x", i, message.payload[i]);
      }
    }

    if (can1.IsBusOff())
    {
      sjsu::LogInfo("CAN 1 is in a BUS-OFF error state and is disabled!");
      sjsu::LogInfo("Re-enabling CAN 1...");
      can1.Enable();
    }

    if (can2.IsBusOff())
    {
      sjsu::LogInfo("CAN 2 is in a BUS-OFF error state and is disabled!");
      sjsu::LogInfo("Re-enabling CAN 2...");
      can2.Enable();
    }

    sjsu::Delay(1s);
  }

  return 0;
}
