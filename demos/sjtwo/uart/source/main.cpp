#include <cstdio>
#include <iterator>

#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Uart Loopback Application starting...");
  sjsu::LogInfo("Connect jumper between P2[8] and P2[9].");
  sjsu::lpc40xx::Uart uart2(sjsu::lpc40xx::Uart::Port::kUart2);

  sjsu::LogInfo("Initializing UART port 2");
  uart2.Initialize();

  sjsu::LogInfo("Setting standard uart frame format.");
  uart2.ConfigureFormat();

  sjsu::LogInfo("Setting Baud rate to 38400");
  uart2.ConfigureBaudRate(38400);

  sjsu::LogInfo("Enabling UART!");
  uart2.Enable();

  while (true)
  {
    std::string_view payload = "SJSU-DEV2";
    sjsu::LogInfo("Transmitting \"%s\" ...", payload.data());
    uart2.Write(payload);

    sjsu::LogInfo("Receiving characters...");

    // Create buffer to hold the bytes received from the UART peripheral
    std::array<uint8_t, 32> receive_buffer;

    // Fill buffer with zeros
    receive_buffer.fill(0);

    // Read bytes into buffer until either the buffer is full or the UART
    // peripheral no longer has any more bytes left.
    // Store bytes into the kReadBytes variable.
    const size_t kReadBytes = uart2.Read(receive_buffer);

    // Print out the number of bytes read and the actual buffer
    sjsu::LogInfo("(%zu) Bytes received: %.*s",
                  kReadBytes,
                  kReadBytes,
                  receive_buffer.data());

    sjsu::Delay(1s);
  }
  return 0;
}
