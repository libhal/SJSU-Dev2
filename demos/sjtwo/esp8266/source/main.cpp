#include <cstdint>

#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "L2_HAL/communication/esp8266.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  static constexpr char ssid[] = "ATTci78Na2";
  static constexpr char password[] = "unhwm\%dnq3\%e";

  static constexpr char server[] = "www.example.com/";
  static constexpr uint16_t port = 80;

  static constexpr char url[] = "www.example.com/";
  static constexpr size_t BUFFER_SIZE = 1300;
  uint8_t buffer[BUFFER_SIZE];
  
  sjsu::lpc40xx::Uart uart3(sjsu::lpc40xx::Uart::Port::kUart3);
  sjsu::Esp8266 Wifi(uart3);

  bool done = false;

  LOG_INFO("Staring Wifi Module Application");
  while (true)
  {
    if (!done)
    {
      if (Wifi.Initialize())
      {
        if (Wifi.ConnectToAccessPoint(ssid, password, 2000ms)) // Add timeout
        {
          if (Wifi.ConnectToServer(server, port, 2000ms)) // Add timeout
          {
            Wifi.SendGetRequest(url, {.size=BUFFER_SIZE, .address=buffer}, 2000ms); // Add timeout
            Wifi.DisconnectFromServer();
            Wifi.DisconnectFromAccessPoint();
            printf("-----Printing Buffer------\n");
            printf("%s", reinterpret_cast<char*>(buffer));
          }
          else
          {
            LOG_INFO("Failed to Connect to Server");
            Wifi.DisconnectFromServer();
            Wifi.DisconnectFromAccessPoint();
          }
        }
        else
        {
          LOG_INFO("Failed to Connect to AP");
          Wifi.DisconnectFromAccessPoint();
        }
      }
      else
      {
        LOG_INFO("Failed to initialize");
      }

      done = true;
    }
  }

  return -1;
}
