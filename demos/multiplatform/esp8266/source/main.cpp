#include <cinttypes>
#include <cstdint>
#include <string_view>

#include "peripherals/lpc40xx/uart.hpp"
#include "peripherals/stm32f10x/uart.hpp"
#include "devices/communication/esp8266.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"

std::string_view host = "www.example.com";
std::string_view get_request_example =
    "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "\r\n";

int main()
{
  sjsu::LogInfo("ESP8266 Application Starting...");

  // Phase #1:
  // Define all of the peripheral you plan to use as pointers. Pointers must be
  // used in order to do the next step
  sjsu::Uart * uart = &sjsu::GetInactive<sjsu::Uart>();

  // Phase #2:
  // Use "constexpr if" to instantiate the peripherals for each platform
  if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::stm32f10x)
  {
    sjsu::LogInfo("Current Platform STM32F10x...");
    // Giving UART a massive 1kB receive buffer to make we don't lose any data.
    static auto & uart2 = sjsu::stm32f10x::GetUart<2, 1024>();
    uart                = &uart2;
  }
  else if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc40xx)
  {
    sjsu::LogInfo("Current Platform LPC40xx...");
    static auto & uart3 = sjsu::lpc40xx::GetUart<3>();
    uart                = &uart3;
  }
  else
  {
    sjsu::LogError("Invalid platform for this application!");
    return -1;
  }

  sjsu::Esp8266 esp(*uart);

  auto & socket = esp.GetInternetSocket();
  auto & wifi   = esp.GetWiFi();

  // NOTE: there is no need to initialize wifi or socket as they both simply
  // initialize and enable the ESP module.
  sjsu::LogInfo("Initializing Esp8266 module...");
  esp.Initialize();

  while (true)
  {
    sjsu::LogInfo("Connecting to WiFi...");
    if (wifi.ConnectToAccessPoint("KAMMCE-PHONE", "roverteam", 10s))
    {
      break;
    }
    sjsu::LogWarning("Failed to connect to WiFi... Retrying ...");
    wifi.DisconnectFromAccessPoint();
  }

  sjsu::LogInfo("Connected to WiFi!!");
  sjsu::LogInfo("Connecting to server (%s)...", host.data());

  socket.Connect(sjsu::InternetSocket::Protocol::kTCP, host, 80, 5s);

  sjsu::LogInfo("Writing to server (%s)...", host.data());

  std::span write_payload(
      reinterpret_cast<const uint8_t *>(get_request_example.data()),
      get_request_example.size());

  socket.Write(write_payload, 5s);

  sjsu::LogInfo("Reading back response from server (%s)...", host.data());

  std::array<uint8_t, 1024 * 2> response;
  size_t read_back = socket.Read(response, 10s);

  sjsu::LogInfo("Printing Server Response:");
  printf("%.*s\n", read_back, response.data());
  puts("===================================================================");

  socket.Close();

  wifi.DisconnectFromAccessPoint();

  return 0;
}
