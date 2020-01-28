#include <cinttypes>
#include <cstdint>
#include <string_view>

#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "L2_HAL/communication/esp8266.hpp"
#include "utility/log.hpp"
#include "utility/debug.hpp"

constexpr char kGetRequestExample[] =
    "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n\r\n";

#define LOG_IF_NOT_SUCCESSFUL(expression)                \
  {                                                      \
    sjsu::Status log_no_success_status = (expression);   \
    if (log_no_success_status != sjsu::Status::kSuccess) \
    {                                                    \
      LOG_WARNING("Expression Failed: %s", #expression); \
    }                                                    \
  }

int main()
{
  LOG_INFO("ESP8266 Application Starting...");
  sjsu::lpc40xx::Uart uart3(sjsu::lpc40xx::Uart::Port::kUart3);
  sjsu::Esp8266 wifi(uart3);

  LOG_INFO("Initializing Wifi...");
  LOG_IF_NOT_SUCCESSFUL(wifi.Initialize());

  while (true)
  {
    LOG_INFO("Attempting to connect...");
    auto status = wifi.ConnectToAccessPoint("KAMMCE-PHONE", "roverteam");
    if (status == sjsu::Status::kSuccess)
    {
      break;
    }
    LOG_INFO("Failed to connect... Retrying ...");
    wifi.DisconnectFromAccessPoint();
  }

  LOG_INFO("Connected to Wifi");

  for (int i = 0; i < 2; i++)
  {
    LOG_INFO("Connecting to server...");
    LOG_IF_NOT_SUCCESSFUL(wifi.Connect(
        sjsu::InternetSocket::Protocol::kTCP, "www.example.com", 80, 5s));

    LOG_INFO("Send Write Request...");
    LOG_IF_NOT_SUCCESSFUL(
        wifi.Write(kGetRequestExample, sizeof(kGetRequestExample), 100ms));

    LOG_INFO("Read Back Response...");
    char response[1024 * 4] = {};
    size_t read_back        = wifi.Read(response, sizeof(response), 10s);

    LOG_INFO("Hexdumping Response...");
    sjsu::debug::Hexdump(response, read_back);

    LOG_INFO("Direct Print...");
    printf("%.*s\r", read_back, response);

    wifi.Close();
    sjsu::Delay(5s);
  }

  wifi.DisconnectFromAccessPoint();

  return 0;
}
