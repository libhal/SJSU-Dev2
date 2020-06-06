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
      wifi.Connect(sjsu::InternetSocket::Protocol::kTCP, kHost, 9000, 5s));

  LOG_ON_FAILURE(
      wifi.Write(kGetRequestExample.data(), kGetRequestExample.size(), 5s));

  std::array<char, 2048> response;
  size_t read_back = 0;
  read_back += wifi.Read(&response[read_back], response.size(), 10s);
  read_back +=
      wifi.Read(&response[read_back], response.size() - read_back, 10s);

  sjsu::LogInfo("Printing Server Response:");
  printf("%.*s\n", read_back, response.data());
  puts("===================================================================");

  wifi.Close();
  wifi.DisconnectFromAccessPoint();

  return 0;
}
