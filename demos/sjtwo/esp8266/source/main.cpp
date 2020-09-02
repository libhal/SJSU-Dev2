#include <cinttypes>
#include <cstdint>
#include <string_view>

#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "L2_HAL/communication/esp8266.hpp"
#include "utility/log.hpp"
#include "utility/debug.hpp"

constexpr std::string_view kHost = "www.example.com";
constexpr std::string_view kGetRequestExample =
    "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "\r\n";

int main()
{
  sjsu::LogInfo("ESP8266 Application Starting...");
  sjsu::lpc40xx::Uart uart3(sjsu::lpc40xx::Uart::Port::kUart3);
  sjsu::Esp8266 wifi(uart3);

  sjsu::LogInfo("Initializing Esp8266 module...");
  if (auto result = wifi.Initialize(); !result)
  {
    result.error()->Print();
  }

  while (true)
  {
    sjsu::LogInfo("Connecting to WiFi...");
    if (wifi.ConnectToAccessPoint("ssid", "password"))
    {
      break;
    }
    sjsu::LogWarning("Failed to connect to WiFi... Retrying ...");
    wifi.DisconnectFromAccessPoint();
  }

  sjsu::LogInfo("Connected to WiFi!!");

  sjsu::LogInfo("Connecting to server (%s)...", kHost.data());

  if (auto result =
          wifi.Connect(sjsu::InternetSocket::Protocol::kTCP, kHost, 9000, 5s);
      !result)
  {
    result.error()->Print();
  }

  if (auto result =
          wifi.Write(kGetRequestExample.data(), kGetRequestExample.size(), 5s);
      !result)
  {
    result.error()->Print();
  }

  std::array<char, 2048> response;
  size_t read_back = 0;
  read_back += SJ2_RETURN_VALUE_ON_ERROR(
      wifi.Read(&response[read_back], response.size(), 10s), -1);
  read_back += SJ2_RETURN_VALUE_ON_ERROR(
      wifi.Read(&response[read_back], response.size() - read_back, 10s), -1);

  sjsu::LogInfo("Printing Server Response:");
  printf("%.*s\n", read_back, response.data());
  puts("===================================================================");

  wifi.Close();
  wifi.DisconnectFromAccessPoint();

  return 0;
}
