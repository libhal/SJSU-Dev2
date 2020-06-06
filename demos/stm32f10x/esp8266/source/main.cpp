#include <string_view>

#include "L1_Peripheral/stm32f10x/uart.hpp"
#include "L2_HAL/communication/esp8266.hpp"
#include "utility/log.hpp"

constexpr std::string_view kHost = "www.example.com";
constexpr std::string_view kGetRequestExample =
    "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "\r\n";

int main()
{
  sjsu::LogInfo("ESP8266 Application Starting...");

  // Giving UART a massive 1kB receive buffer to make we don't lose any data.
  sjsu::stm32f10x::Uart<1024> uart2(sjsu::stm32f10x::UartBase::Port::kUart2);
  sjsu::Esp8266 wifi(uart2);

  sjsu::LogInfo("Initializing Esp8266 module...");
  LOG_ON_FAILURE(wifi.Initialize());

  while (true)
  {
    sjsu::LogInfo("Connecting to WiFi...");
    auto status = wifi.ConnectToAccessPoint("ssid", "password");
    if (status == sjsu::Status::kSuccess)
    {
      break;
    }
    sjsu::LogWarning("Failed to connect to WiFi... Retrying ...");
    wifi.DisconnectFromAccessPoint();
  }

  sjsu::LogInfo("Connected to WiFi!!");

  sjsu::LogInfo("Connecting to server (%s)...", kHost.data());
  LOG_ON_FAILURE(
      wifi.Connect(sjsu::InternetSocket::Protocol::kTCP, kHost, 80, 5s));

  LOG_ON_FAILURE(
      wifi.Write(kGetRequestExample.data(), kGetRequestExample.size(), 5s));

  std::array<char, 2048> response;
  size_t read_back = 0;
  read_back += wifi.Read(&response[read_back], response.size(), 10s);
  read_back +=
      wifi.Read(&response[read_back], response.size() - read_back, 10s);

  sjsu::LogInfo("Printing Response From Server:");
  printf("%.*s\n", read_back, response.data());
  puts("===================================================================");

  wifi.Close();
  wifi.DisconnectFromAccessPoint();

  return 0;
}
