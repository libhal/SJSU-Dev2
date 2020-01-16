#include <cstdint>
#include <cstring>

#include "L1_Peripheral/lpc40xx/uart.hpp"
// #include "L2_HAL/communication/esp8266.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"
#include "utility/debug.hpp"

sjsu::lpc40xx::Uart uart3(sjsu::lpc40xx::Uart::Port::kUart3);

template <size_t kLength>
void WifiWrite(const char (&str)[kLength])
{
  printf(SJ2_BACKGROUND_GREEN "\nSending > %s\n" SJ2_COLOR_RESET, str);
  uart3.Write(reinterpret_cast<const uint8_t *>(str), kLength - 1);
}

struct ReadUntil_t
{
  uint8_t * buffer;
  size_t length;
  const char * end;
  uint32_t buffer_position = 0;
  uint32_t end_position    = 0;
  bool success             = false;
};

int ReadUntil(uint8_t * buffer,
              size_t length,
              std::chrono::milliseconds timeout,
              const char * end)
{
  memset(buffer, 0, length);
  ReadUntil_t until = {
    .buffer = buffer,
    .length = length,
    .end    = end,
  };

  sjsu::Wait(timeout, [&until]() {
    if (until.end[until.end_position] == '\0')
    {
      until.success = true;
      return true;
    }
    if (!uart3.HasData())
    {
      return false;
    }

    uint32_t buf_pos      = until.buffer_position % until.length;
    until.buffer[buf_pos] = uart3.Read();

    if (until.buffer[buf_pos] == until.end[until.end_position])
    {
      until.end_position++;
    }
    else if (until.buffer[buf_pos] == until.end[0])
    {
      until.end_position = 1;
    }
    else
    {
      until.end_position = 0;
    }

    until.buffer_position++;
    return false;
  });

  return (until.success) ? until.buffer_position : -1;
}

template <size_t kBufferSize = 64>
int ReadUntil(const char * end,
              std::chrono::milliseconds timeout,
              bool print_hexdump = false)
{
  uint8_t buffer[kBufferSize] = { 0 };
  int length                  = ReadUntil(buffer, sizeof(buffer), timeout, end);

  if (length != -1)
  {
    printf(SJ2_HI_BACKGROUND_GREEN "COMMAND SUCCESSFUL!\n" SJ2_COLOR_RESET);
  }
  else
  {
    printf(SJ2_BACKGROUND_RED "COMMAND STATUS FAILURE\n" SJ2_COLOR_RESET);
  }

  if (print_hexdump)
  {
    sjsu::debug::Hexdump(buffer, kBufferSize);
  }
  return length;
}

uint32_t FlushBuffer(uint8_t * buffer,
                     uint32_t length,
                     std::chrono::milliseconds timeout)
{
  uint8_t total_buffer[64] = {0};
  ReadUntil(total_buffer, sizeof(total_buffer), timeout, "+IPD,");
  ReadUntil(total_buffer, sizeof(total_buffer), timeout, ":");

  uint32_t total_bytes = 0;
  sscanf(reinterpret_cast<const char *>(total_buffer), "%lu:", &total_bytes);
  total_bytes = std::min(length, total_bytes);
  printf("TOTAL BYTES = %lu\n", total_bytes);

  uint32_t position = 0;
  // Need total_bytes to consider the length parameter
  sjsu::Wait(timeout, [buffer, &position, total_bytes]() {
    if (position >= total_bytes)
    {
      return true;
    }
    if (!uart3.HasData())
    {
      return false;
    }
    buffer[position++] = uart3.Read();
    return false;
  });
  return position;
}

constexpr char kGetRequest[] =
    "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "Connection: keep-alive\r\n\r\n";

int main()
{
  static constexpr char kOk[] = "\r\nOK\r\n";

  [[maybe_unused]] constexpr size_t kGetRequestSize = sizeof(kGetRequest);
  LOG_INFO("Staring Wifi Module Application...");

  uart3.Initialize(115200);

  while (uart3.HasData())
  {
    uart3.Read();
  }

  WifiWrite("+++");
  WifiWrite("AT+RST\r\n");
  ReadUntil<1024>("\r\nready\r\n", 10s, true);

  WifiWrite("ATE0\r\n");
  ReadUntil(kOk, 1s, true);

  WifiWrite("AT+CWMODE=3\r\n");
  ReadUntil(kOk, 1s, true);

  WifiWrite("AT+CWJAP_CUR=\"KAMMCE-PHONE\",\"roverteam\"\r\n");
  ReadUntil<128>("WIFI GOT IP\r\n\r\nOK\r\n", 10s, true);

  WifiWrite("AT+CIPSTART=\"TCP\",\"www.example.com\",80\r\n");
  ReadUntil(kOk, 5s, true);

  for (int i = 0; i < 3; i++)
  {
    WifiWrite("AT+CIPSEND=65\r\n");
    ReadUntil(kOk, 100ms);
    uint8_t buffer[1024 * 2] = { 0 };
    LOG_INFO("ITERATION > %d", i);
    WifiWrite(kGetRequest);
    auto len = FlushBuffer(buffer, sizeof(buffer), 10s);
    printf(SJ2_BACKGROUND_GREEN "\nWiFi Received > " SJ2_COLOR_RESET "\n%s\n",
           buffer);
    sjsu::debug::Hexdump(buffer, len);
  }

  WifiWrite("AT+CIPCLOSE\r\n");
  ReadUntil(kOk, 1s);
  WifiWrite("AT+CWQAP\r\n");
  ReadUntil(kOk, 1s);

  return -1;
}