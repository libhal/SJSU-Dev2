// Still an incomplete driver for the esp8266.
// Should not be used in production software.
#pragma once

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "config.hpp"
#include "L1_Peripheral/uart.hpp"
#include "L2_HAL/communication/internet_socket.hpp"
#include "utility/status.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
class Esp8266 : public InternetSocket, public WiFi
{
 public:
  /// Default baud rate for ESP8266
  static constexpr uint32_t kDefaultBaudRate = 115200;
  /// Default timeout for uart read
  static constexpr std::chrono::nanoseconds kDefaultTimeout = 2s;
  /// Confirmation responses received from the ESP8266 module
  static constexpr char kOk[] = "\r\nOK\r\n";

  explicit constexpr Esp8266(const Uart & port,
                             uint32_t baud_rate = kDefaultBaudRate)
      : uart_port_(port), baud_rate_(baud_rate), incoming_bytes_(0)
  {
  }

  bool TestModule()
  {
    WifiWrite("AT\r\n");
    return (ReadUntil(kOk) > -1) ? true : false;
  }

  // ===========================================================================
  // WiFi
  // ===========================================================================

  /// Resets ESP8266 using serial command.
  void Reset()
  {
    WifiWrite("AT+RST\r\n");
    ReadUntil<1024>("\r\n\r\nready\r\n", 10s);
  }

  Status Initialize() override
  {
    static constexpr char kDisableEchoCommand[] = "ATE0\r\n";
    static constexpr char kClientModeCommand[]  = "AT+CWMODE=1\r\n";

    uart_port_.Initialize(baud_rate_);
    uart_port_.Flush();

    WifiWrite("+++");
    Reset();

    WifiWrite(kDisableEchoCommand);
    ReadUntil(kOk);
    WifiWrite(kClientModeCommand);
    ReadUntil(kOk);

    return TestModule() ? Status::kSuccess : Status::kNotReadyYet;
  }

  Status ConnectToAccessPoint(
      std::string_view ssid,
      std::string_view password,
      std::chrono::nanoseconds read_timeout = 10s) override
  {
    std::array<char, 128> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          R"(AT+CWJAP_CUR="%s","%s")"
                          "\r\n",
                          ssid.data(),
                          password.data());

    uart_port_.Write(command_buffer.data(), length);

    return ConvertReadUntilToStatus(
        ReadUntil<1024>("WIFI GOT IP\r\n\r\nOK\r\n", read_timeout));
  }

  Status DisconnectFromAccessPoint() override
  {
    WifiWrite("AT+CWQAP\r\n");
    return ConvertReadUntilToStatus(ReadUntil(kOk));
  }

  NetworkConnection_t GetNetworkConnectionInfo() override
  {
    return NetworkConnection_t{};
  }

  InternetSocket & GetInternetSocket() override
  {
    return *this;
  }

  // ===========================================================================
  // InternetProtocol
  // ===========================================================================
  Status Connect(Protocol protocol,
                 std::string_view address,
                 uint16_t port,
                 std::chrono::nanoseconds timeout) override
  {
    static constexpr const char * kConnectionType[] = { "TCP", "UDP" };
    static constexpr char kConnectToServerCommand[] =
        R"(AT+CIPSTART="%s","%s",%)" PRIu16 "\r\n";
    std::array<char, 128> command_buffer;

    snprintf(command_buffer.data(),
             command_buffer.size(),
             kConnectToServerCommand,
             kConnectionType[Value(protocol)],
             address.data(),
             port);

    uart_port_.Write(command_buffer.data(), command_buffer.size());

    return ConvertReadUntilToStatus(ReadUntil(kOk, timeout));
  }

  /// TODO(kammce): Fix this!!
  bool IsConnected() override
  {
    return false;
  }

  Status Write(const void * data,
               size_t size,
               std::chrono::nanoseconds) override
  {
    std::array<char, 32> data_count_buffer;
    int send_count_length = snprintf(data_count_buffer.data(),
                                     data_count_buffer.size(),
                                     "AT+CIPSEND=%zu\r\n",
                                     size);
    // Send payload length
    uart_port_.Write(data_count_buffer.data(), send_count_length);

    // Wait for an OK!
    if (ReadUntil(kOk, 100ms) == -1)
    {
      return Status::kBusError;
    }

    // Now write the rest of the data
    uart_port_.Write(data, size);

    return Status::kSuccess;
  }

  size_t GetReceiveLength()
  {
    size_t incoming_bytes = 0;
    std::array<uint8_t, 64> total_buffer;
    ReadUntil(total_buffer.data(), total_buffer.size(), "+IPD,", 100ms);
    ReadUntil(total_buffer.data(), total_buffer.size(), ":", 100ms);
    const char * receive_length =
        reinterpret_cast<const char *>(total_buffer.data());

    sscanf(receive_length, "%zu:", &incoming_bytes);

    return incoming_bytes;
  }

  size_t Read(void * buffer,
              size_t size,
              std::chrono::nanoseconds timeout) override
  {
    size_t received_bytes = 0;
    size_t position       = 0;
    size_t bytes_to_read  = 0;
    do
    {
      received_bytes = GetReceiveLength();

      if (received_bytes == 0)
      {
        break;
      }

      uint8_t * byte_buffer = reinterpret_cast<uint8_t *>(buffer);
      bytes_to_read         = std::min(size - position, received_bytes);

      Status status =
          uart_port_.Read(&byte_buffer[position], bytes_to_read, timeout);

      if (!IsOk(status))
      {
        break;
      }

      position += bytes_to_read;
    } while (received_bytes != 0 && bytes_to_read < size);

    return position;
  }

  Status Close() override
  {
    WifiWrite("AT+CIPCLOSE\r\n");
    return ConvertReadUntilToStatus(ReadUntil(kOk));
  }

 private:
  // Writes command array to Esp8266
  template <size_t kLength>
  void WifiWrite(const char (&str)[kLength])
  {
    uart_port_.Write(reinterpret_cast<const uint8_t *>(str), kLength - 1);
  }

  // Reads to provided buffer and checks to see if the end of the read matches
  // the end char array.  Returns size of read or -1 if end char array doesn't
  // match the read char array.
  int ReadUntil(uint8_t * buffer,
                size_t length,
                const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout)
  {
    struct ReadUntil_t
    {
      uint8_t * buffer;
      size_t length;
      const char * end;
      uint32_t buffer_position = 0;
      uint32_t end_position    = 0;
      bool success             = false;
    };

    memset(buffer, 0, length);
    ReadUntil_t until = {
      .buffer = buffer,
      .length = length,
      .end    = end,
    };

    sjsu::Wait(timeout, [this, &until]() {
      if (until.end[until.end_position] == '\0')
      {
        until.success = true;
        return true;
      }
      if (!uart_port_.HasData())
      {
        return false;
      }

      uint32_t buf_pos      = until.buffer_position % until.length;
      until.buffer[buf_pos] = uart_port_.Read();

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

    // debug::Hexdump(buffer, until.buffer_position);

    return (until.success) ? until.buffer_position : -1;
  }

  // Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout)
  {
    uint8_t buffer[kBufferSize] = { 0 };
    int length = ReadUntil(buffer, sizeof(buffer), end, timeout);
    return length;
  }

  Status ConvertReadUntilToStatus(int result)
  {
    if (result != -1)
    {
      return Status::kSuccess;
    }
    else
    {
      return Status::kTimedOut;
    }
  }

  const Uart & uart_port_;
  uint32_t baud_rate_;
  size_t incoming_bytes_;
};
}  // namespace sjsu
