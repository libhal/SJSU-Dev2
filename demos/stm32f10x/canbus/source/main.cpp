#include <cstdint>

#include "peripherals/stm32f10x/can.hpp"
#include "platforms/targets/stm32f10x/stm32f10x.h"

#include "utility/log.hpp"
#include "utility/time/time.hpp"


#include <initializer_list>
#include <scope>
#include <string_view>

int main(void)
{
  sjsu::stm32f10x::Can & can1 = sjsu::stm32f10x::GetCan<1>();
  
  sjsu::LogInfo("Initializing CAN 1 with default bit rate of 100 kBit/s...");
  can1.ModuleInitialize();
  
  using namespace sjsu;
  using namespace stm32f10x;
  CAN_msg_t CAN_rx_msg;  // Holds receiving CAN messages
  CAN_msg_t CAN_tx_msg;  // Holds transmitted CAN messagess
  int test_id = 0;
  
  if (can1.SelfTest(42))
  {
    sjsu::LogInfo("CAN 2 self-test" SJ2_HI_BOLD_GREEN " passed!");
  }
  else
  {
    sjsu::LogError("CAN 2 self-test" SJ2_HI_BOLD_RED " failed!");
  }


  while (true)
  { 
    sjsu::LogInfo("Sending Message with ID: %d", test_id);
    sjsu::Can::Message_t test_message;
    test_message.id = test_id;
    test_message.length = 8;
    test_message.payload[0] = 0x88 + test_id;
    test_message.payload[1] = 0x99 + test_id;
    test_message.payload[2] = 0xAA + test_id;
    test_message.payload[3] = 0xBB + test_id;
    test_message.payload[4] = 0xBB + test_id;
    test_message.payload[5] = 0xCC + test_id;
    test_message.payload[6] = 0xDD + test_id;
    test_message.payload[7] = 0xFF + test_id;
    can1.Send(test_message);

    for (uint8_t i = 0; i < 40; i++)
    {
      if(can1.HasData())
      {
        auto received_message = can1.Receive();
        sjsu::LogInfo("%d | %#04x %#04x %#04x %#04x %#04x %#04x %#04x %#04x\n",
              received_message.id,
              received_message.payload[0],
              received_message.payload[1],
              received_message.payload[2],
              received_message.payload[3],
              received_message.payload[4],
              received_message.payload[5],
              received_message.payload[6],
              received_message.payload[7]);
        break;
      }
      else
      {
         sjsu::Delay(250ms);
      }

    }
    test_id++;
    sjsu::Delay(1000ms);
  }

  return 0;
}


