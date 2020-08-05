#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc17xx/uart.hpp"
#include "L2_HAL/communication/bluetooth/zs040.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting Bluetooth (UART) Example...");

  sjsu::lpc17xx::Uart uart2(sjsu::lpc17xx::UartPort::kUart2);
  sjsu::lpc17xx::Gpio state_pin(0, 0);
  sjsu::lpc17xx::Gpio key_pin(0, 1);
  sjsu::bluetooth::Zs040 zs040(uart2, key_pin, state_pin);

  zs040.Initialize();
  zs040.Enable();

  sjsu::LogInfo("Version:     %.*s", zs040.GetVersion());
  sjsu::LogInfo("Baud:        %c", sjsu::Value(zs040.GetBaudRate()));
  sjsu::LogInfo("Role:        %c", sjsu::Value(zs040.GetRole()));
  // sjsu::LogInfo("Role:        %c",
  //               sjsu::Value(zs040.SetRole(bluetooth::Zs040::Role::kSlave)));
  sjsu::LogInfo("MAC Address: %.*s", zs040.GetMacAddress());
  sjsu::LogInfo("Set Name:    %.*s", zs040.SetDeviceName("Some Device"));
  sjsu::LogInfo("Device Name: %.*s", zs040.GetDeviceName());
  sjsu::LogInfo("UUID:        %.*s", zs040.GetUuid());
  sjsu::LogInfo("State:       %.*s", zs040.SendCommand("+STATE"));

  zs040.Scan();

  sjsu::Halt();

  // zs040.Scan();

  // zs040.SendCommand("AT");
  // // zs040.SendCommand("AT+RESET", 550ms);
  // zs040.SendCommand("AT+VERSION");
  // zs040.SendCommand("AT+NAME");
  // zs040.SendCommand("AT+BAUD");
  // zs040.SendCommand("AT+STATE");
  // zs040.SendCommand("AT+UUID");
  // zs040.SendCommand("AT+CHAR", 15ms);

  // // zs040.SendCommand("AT+HELP", {}, 5s);

  // constexpr uint8_t kMasterMode =
  //     sjsu::Value(sjsu::bluetooth::Zs040::Role::kMaster);
  // constexpr bool kEnableSearch = true;

  // zs040.SendCommand("AT+ROLE1");     // enter master mode
  // zs040.SendCommand("AT+INQ1", 5s);  // search for devices
  // // +INQS
  // // +INQ:1 0x882583F117DF -36
  // // +INQE
  // // Devices Found 1
  // zs040.SendCommand("AT+CONN1", 1s);  // connect to device #1
  // // +Connecting  0x882583F117DF
  // // +Connected  0x882583F117DF

  // while (true)
  // {
  //   if (zs040.HasData())
  //   {
  //     printf("%c", reinterpret_cast<sjsu::Uart &>(zs040).Read());
  //   }
  // }

  return 0;
}
