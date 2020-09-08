// Still an incomplete driver for the esp8266.
// Should not be used in production software.
#pragma once

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>

#include "config.hpp"
#include "L1_Peripheral/uart.hpp"
#include "L2_HAL/communication/internet_socket.hpp"
#include "utility/debug.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"
#include "utility/timeout_timer.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Serial driver for the ESP8266 wifi device that implements the WiFi and
/// InternetSocket interfaces.
/// Can be used to connect to a WiFi host device.
/// Can be used to communicate over the internet using this driver.
class Esp8266 : public InternetSocket, public WiFi
{
 public:
  /// Default baud rate for ESP8266
  static constexpr uint32_t kDefaultBaudRate = 115200;
  /// Default timeout for system operations
  static constexpr std::chrono::nanoseconds kDefaultTimeout = 2s;
  /// Default timeout for long operations
  static constexpr std::chrono::nanoseconds kDefaultLongTimeout = 10s;
  /// Confirmation responses received from the ESP8266 module
  static constexpr char kOk[] = "\r\nOK\r\n";

  /// @param port - uart port to use to communicate with the ESP8266 device
  /// @param baud_rate - operating baudrate of the uart device.
  explicit constexpr Esp8266(const sjsu::Uart & port,
                             uint32_t baud_rate = kDefaultBaudRate)
      : uart_port_(port), baud_rate_(baud_rate)
  {
  }

  /// Tests that the ESP8266 can respond to commands.
  /// @return std::errc::timed_out if the ESP8266 fails to respond in time.
  void TestModule()
  {
    WifiWrite("AT\r\n");
    ReadUntil(kOk);
  }

  // ===========================================================================
  // WiFi
  // ===========================================================================

  /// Resets ESP8266 using serial command.
  void Reset()
  {
    WifiWrite("AT+RST\r\n");
    ReadUntil<1024>("\r\n\r\nready\r\n", kDefaultLongTimeout);
  }

  void Initialize() override
  {
    uart_port_.Initialize(baud_rate_);
    uart_port_.Flush();

    WifiWrite("+++");
    Reset();

    // Disable echo
    WifiWrite("ATE0\r\n");
    ReadUntil(kOk);
    // Enable simple IP client mode
    WifiWrite("AT+CWMODE=1\r\n");
    ReadUntil(kOk);

    return TestModule();
  }

  bool ConnectToAccessPoint(
      std::string_view ssid,
      std::string_view password,
      std::chrono::nanoseconds read_timeout = kDefaultLongTimeout) override
  {
    std::array<char, 128> command_buffer;

    int length = snprintf(command_buffer.data(), command_buffer.size(),
                          R"(AT+CWJAP_CUR="%s","%s")"
                          "\r\n",
                          ssid.data(), password.data());

    uart_port_.Write(command_buffer.data(), length);

    int confirmation_location =
        ReadUntil("WIFI GOT IP\r\n\r\nOK\r\n", read_timeout);

    return (confirmation_location != -1);
  }

  void DisconnectFromAccessPoint() override
  {
    WifiWrite("AT+CWQAP\r\n");
    ReadUntil(kOk);
  }

  NetworkConnection_t GetNetworkConnectionInfo() override
  {
    return NetworkConnection_t{};
  }

  InternetSocket & GetInternetSocket() override
  {
    return *this;
  }

  /// @param mode - Which mode to put the ESP8266 WiFi module into.
  void SetMode(WifiMode mode)
  {
    std::array<char, 32> command_buffer;

    int length = snprintf(command_buffer.data(), command_buffer.size(),
                          R"(AT+CWMODE=%u)"
                          "\r\n",
                          Value(mode));

    uart_port_.Write(command_buffer.data(), length);

    ReadUntil("OK\r\n", 100ms);
  }

  /// Connect to an access point
  ///
  /// @param ssid - ssid of the acces point
  /// @param password - password of the access point
  /// @param channel_id - which wifi channel to use
  /// @param security - password security of the access point
  /// @return Status
  void ConfigureAccessPoint(std::string_view ssid,
                            std::string_view password,
                            uint8_t channel_id,
                            AccessPointSecurity security)
  {
    std::array<char, 256> command_buffer;

    int length =
        snprintf(command_buffer.data(), command_buffer.size(),
                 R"(AT+CWSAP="%s","%s",%u,%u)"
                 "\r\n",
                 ssid.data(), password.data(), channel_id, Value(security));

    uart_port_.Write(command_buffer.data(), length);

    ReadUntil("OK\r\n", 100ms);
  }

  /// Contains IPv4 address information.
  struct IpAddress_t
  {
    /// Contains each digit of the IPv4 address
    std::array<uint8_t, 4> data;

    /// Converts IP Address numeric representation into a std::array string.
    std::array<char, 16> ToString()
    {
      std::array<char, 16> ip_string;

      snprintf(ip_string.data(), ip_string.size(), "%u.%u.%u.%u", data[0],
               data[1], data[2], data[3]);

      return ip_string;
    }
  };

  /// @return the first IP address in the list of connected devices
  IpAddress_t ListConnections()
  {
    WifiWrite("AT+CWLIF\r\n");

    std::array<uint8_t, 256> buffer = { 0 };

    int length = ReadUntil(buffer, "OK\r\n", 3s);

    debug::Hexdump(buffer.data(), length);

    IpAddress_t ip;
    std::array<uint32_t, 4> temp_ip;
    sscanf(reinterpret_cast<const char *>(buffer.data()), "%lu.%lu.%lu.%lu,",
           &temp_ip[0], &temp_ip[1], &temp_ip[2], &temp_ip[3]);

    ip.data[0] = static_cast<uint8_t>(temp_ip[0]);
    ip.data[1] = static_cast<uint8_t>(temp_ip[1]);
    ip.data[2] = static_cast<uint8_t>(temp_ip[2]);
    ip.data[3] = static_cast<uint8_t>(temp_ip[3]);

    return ip;
  }

  // ===========================================================================
  // InternetProtocol
  // ===========================================================================
  bool Connect(Protocol protocol,
               std::string_view address,
               uint16_t port,
               std::chrono::nanoseconds timeout) override
  {
    static constexpr const char * kConnectionType[] = { "TCP", "UDP" };
    static constexpr char kConnectToServerCommand[] =
        R"(AT+CIPSTART="%s","%s",%)" PRIu16 "\r\n";

    TimeoutTimer timer(timeout);

    std::array<char, 128> command_buffer;

    int length = snprintf(
        command_buffer.data(), command_buffer.size(), kConnectToServerCommand,
        kConnectionType[Value(protocol)], address.data(), port);

    uart_port_.Write(command_buffer.data(), length);

    if (ReadUntil(kOk, timer.GetTimeLeft(), true) == -1)
    {
      return false;
    }

    return true;
  }

  /// Starts a web server which allows the esp8266 to be connected to, retrieve
  /// HTTP requests and send back responses.
  ///
  /// @param port - which port should be used for the server
  void Bind(uint16_t port = 333)
  {
    WifiWrite("AT+CIPMUX=1\r\n");
    ReadUntil(kOk);

    std::array<char, 64> command_buffer;
    static constexpr char kServerStart[] = "AT+CIPSERVER=1,%" PRIu16 "\r\n";

    int length = snprintf(command_buffer.data(), command_buffer.size(),
                          kServerStart, port);
    uart_port_.Write(command_buffer.data(), length);

    ReadUntil(kOk, kDefaultLongTimeout, true);
  }

  /// TODO(kammce): Fix this!!
  bool IsConnected() override
  {
    return false;
  }

  void Write(const void * data,
             size_t size,
             std::chrono::nanoseconds timeout) override
  {
    TimeoutTimer timer(timeout);

    std::array<char, 32> data_count_buffer;
    int send_count_length =
        snprintf(data_count_buffer.data(), data_count_buffer.size(),
                 "AT+CIPSEND=%zu\r\n", size);
    // Send payload length
    uart_port_.Write(data_count_buffer.data(), send_count_length);

    // Wait for an OK!
    if (ReadUntil(kOk, timer.GetTimeLeft()) == -1)
    {
      LogDebug("CIPSEND");
      throw Exception(std::errc::io_error,
                      "ESP8266 did not accept requestion to send data");
    }

    // Now write the rest of the data
    uart_port_.Write(data, size);

    // Wait for an OK!
    if (ReadUntil("\r\nSEND OK\r\n\r\n", timer.GetTimeLeft()) == -1)
    {
      LogDebug("SEND OK");
      throw Exception(std::errc::io_error, "Attempt to write to server failed");
    }
  }

  size_t Read(void * buffer,
              size_t size,
              std::chrono::nanoseconds timeout) override
  {
    TimeoutTimer timer(timeout);
    size_t position      = 0;
    size_t bytes_to_read = 0;

    size_t received_bytes = GetReceiveLength(timer);

    if (received_bytes == 0)
    {
      return 0;
    }

    uint8_t * byte_buffer = reinterpret_cast<uint8_t *>(buffer);
    bytes_to_read         = std::min(size - position, received_bytes);

    uart_port_.Read(&byte_buffer[position], bytes_to_read, timer.GetTimeLeft());

    position += bytes_to_read;

    return position;
  }

  void Close() override
  {
    WifiWrite("AT+CIPCLOSE\r\n");
    ReadUntil(kOk);
  }

 private:
  /// Gets the number of incoming bytes coming from a network connection.
  ///
  /// @param timer - timeout timer reference with the remaining amount of time
  ///                left to perform this operation.
  /// @return size_t number of bytes to return
  size_t GetReceiveLength(TimeoutTimer & timer)
  {
    size_t incoming_bytes = 0;
    std::array<uint8_t, 64> total_buffer;

    ReadUntil(total_buffer, "+IPD,", timer.GetTimeLeft());
    ReadUntil(total_buffer, ":", timer.GetTimeLeft());

    const char * receive_length =
        reinterpret_cast<const char *>(total_buffer.data());

    sscanf(receive_length, "%zu:", &incoming_bytes);

    return incoming_bytes;
  }

  // Writes command array to Esp8266
  template <size_t kLength>
  void WifiWrite(const char (&str)[kLength])
  {
    uart_port_.Write(reinterpret_cast<const uint8_t *>(str), kLength - 1);
  }

  // Reads to provided buffer and checks to see if the end of the read matches
  // the end char array.  Returns size of read or -1 if end char array doesn't
  // match the read char array.
  int ReadUntil(std::span<uint8_t> buffer,
                const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool throw_on_failure            = true)
  {
    struct ReadUntil_t
    {
      std::span<uint8_t> buffer;
      const char * end;
      size_t string_length     = 0;
      uint32_t buffer_position = 0;
      uint32_t end_position    = 0;
      bool success             = false;
    };

    std::fill(buffer.begin(), buffer.end(), 0);

    ReadUntil_t until = {
      .buffer        = buffer,
      .end           = end,
      .string_length = strlen(end),
    };

    Wait(timeout, [this, &until]() {
      if (until.end_position >= until.string_length)
      {
        until.success = true;
        return true;
      }
      if (!uart_port_.HasData())
      {
        return false;
      }

      uint32_t buf_pos      = until.buffer_position % until.buffer.size();
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

    if (until.success)
    {
      return until.buffer_position;
    }

    if (throw_on_failure)
    {
      throw Exception(std::errc::timed_out, "Could not find ");
    }

    return -1;
  }

  // Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool throw_on_failure            = false)
  {
    std::array<uint8_t, kBufferSize> buffer = { 0 };
    return ReadUntil(buffer, end, timeout, throw_on_failure);
  }

  const Uart & uart_port_;
  uint32_t baud_rate_;
};  // namespace sjsu
}  // namespace sjsu
