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
#include "utility/debug.hpp"
#include "utility/enum.hpp"
#include "utility/status.hpp"
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
  /// @return Status::kTimeout if it could not.
  Status TestModule()
  {
    WifiWrite("AT\r\n");
    return ConvertReadUntilToStatus(ReadUntil(kOk));
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

  Status Initialize() override
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

  Status ConnectToAccessPoint(
      std::string_view ssid,
      std::string_view password,
      std::chrono::nanoseconds read_timeout = kDefaultLongTimeout) override
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
        ReadUntil("WIFI GOT IP\r\n\r\nOK\r\n", read_timeout));
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

  /// @param mode - Which mode to put the ESP8266 WiFi module into.
  Status SetMode(WifiMode mode)
  {
    std::array<char, 32> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          R"(AT+CWMODE=%u)"
                          "\r\n",
                          Value(mode));

    uart_port_.Write(command_buffer.data(), length);

    return ConvertReadUntilToStatus(ReadUntil("OK\r\n", 100ms));
  }

  /// Connect to an access point
  ///
  /// @param ssid - ssid of the acces point
  /// @param password - password of the access point
  /// @param channel_id - which wifi channel to use
  /// @param security - password security of the access point
  /// @return Status
  Status ConfigureAccessPoint(std::string_view ssid,
                              std::string_view password,
                              uint8_t channel_id,
                              AccessPointSecurity security)
  {
    std::array<char, 256> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          R"(AT+CWSAP="%s","%s",%u,%u)"
                          "\r\n",
                          ssid.data(),
                          password.data(),
                          channel_id,
                          Value(security));

    uart_port_.Write(command_buffer.data(), length);

    return ConvertReadUntilToStatus(ReadUntil("OK\r\n", 100ms));
  }

  /// Contains IPv4 address information.
  struct IpAddress_t
  {
    /// A status indicating if the IP Address is valid.
    Status status = Status::kDeviceNotFound;

    /// Contains each digit of the IPv4 address
    std::array<uint8_t, 4> data;

    /// Converts IP Address numeric representation into a std::array string.
    std::array<char, 16> ToString()
    {
      std::array<char, 16> ip_string;

      snprintf(ip_string.data(),
               ip_string.size(),
               "%u.%u.%u.%u",
               data[0],
               data[1],
               data[2],
               data[3]);

      return ip_string;
    }
  };

  /// @return the first IP address in the list of connected devices
  IpAddress_t ListConnections()
  {
    WifiWrite("AT+CWLIF\r\n");

    std::array<uint8_t, 256> buffer = { 0 };
    int length = ReadUntil(buffer.data(), buffer.size(), "OK\r\n", 3s);

    debug::Hexdump(buffer.data(), length);

    IpAddress_t ip;
    std::array<uint32_t, 4> temp_ip;
    int arguments = sscanf(reinterpret_cast<const char *>(buffer.data()),
                           "%lu.%lu.%lu.%lu,",
                           &temp_ip[0],
                           &temp_ip[1],
                           &temp_ip[2],
                           &temp_ip[3]);

    if (arguments == 4)
    {
      ip.status = Status::kSuccess;
    }

    ip.data[0] = static_cast<uint8_t>(temp_ip[0]);
    ip.data[1] = static_cast<uint8_t>(temp_ip[1]);
    ip.data[2] = static_cast<uint8_t>(temp_ip[2]);
    ip.data[3] = static_cast<uint8_t>(temp_ip[3]);

    return ip;
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

    TimeoutTimer timer(timeout);

    std::array<char, 128> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          kConnectToServerCommand,
                          kConnectionType[Value(protocol)],
                          address.data(),
                          port);

    uart_port_.Write(command_buffer.data(), length);

    return ConvertReadUntilToStatus(ReadUntil(kOk, timer.GetTimeLeft(), true));
  }

  /// Starts a web server which allows the esp8266 to be connected to, retrieve
  /// HTTP requests and send back responses.
  ///
  /// @param port - which port should be used for the server
  Status Bind(uint16_t port = 333)
  {
    WifiWrite("AT+CIPMUX=1\r\n");
    ReadUntil(kOk);

    std::array<char, 64> command_buffer;
    static constexpr char kServerStart[] = "AT+CIPSERVER=1,%" PRIu16 "\r\n";

    int length = snprintf(
        command_buffer.data(), command_buffer.size(), kServerStart, port);
    uart_port_.Write(command_buffer.data(), length);

    return ConvertReadUntilToStatus(ReadUntil(kOk, kDefaultLongTimeout, true));
  }

  /// TODO(kammce): Fix this!!
  bool IsConnected() override
  {
    return false;
  }

  Status Write(const void * data,
               size_t size,
               std::chrono::nanoseconds timeout) override
  {
    TimeoutTimer timer(timeout);

    std::array<char, 32> data_count_buffer;
    int send_count_length = snprintf(data_count_buffer.data(),
                                     data_count_buffer.size(),
                                     "AT+CIPSEND=%zu\r\n",
                                     size);
    // Send payload length
    uart_port_.Write(data_count_buffer.data(), send_count_length);

    // Wait for an OK!
    if (ReadUntil(kOk, timer.GetTimeLeft()) == -1)
    {
      LogDebug("CIPSEND");
      return Status::kBusError;
    }

    // Now write the rest of the data
    uart_port_.Write(data, size);

    // Wait for an OK!
    if (ReadUntil("\r\nSEND OK\r\n\r\n", timer.GetTimeLeft()) == -1)
    {
      LogDebug("SEND OK");
      return Status::kBusError;
    }

    return Status::kSuccess;
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

    Status status = uart_port_.Read(
        &byte_buffer[position], bytes_to_read, timer.GetTimeLeft());

    if (!IsOk(status))
    {
      return 0;
    }

    position += bytes_to_read;

    return position;
  }

  Status Close() override
  {
    WifiWrite("AT+CIPCLOSE\r\n");
    return ConvertReadUntilToStatus(ReadUntil(kOk));
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

    ReadUntil(
        total_buffer.data(), total_buffer.size(), "+IPD,", timer.GetTimeLeft());
    ReadUntil(
        total_buffer.data(), total_buffer.size(), ":", timer.GetTimeLeft());

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
  int ReadUntil(uint8_t * buffer,
                size_t length,
                const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool hexdump                     = false)
  {
    struct ReadUntil_t
    {
      uint8_t * buffer;
      size_t length;
      const char * end;
      size_t string_length     = 0;
      uint32_t buffer_position = 0;
      uint32_t end_position    = 0;
      bool success             = false;
    };

    memset(buffer, 0, length);
    ReadUntil_t until = {
      .buffer        = buffer,
      .length        = length,
      .end           = end,
      .string_length = strlen(end),
    };

    sjsu::Wait(timeout, [this, &until]() {
      if (until.end_position >= until.string_length)
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

    if (hexdump)
    {
      debug::Hexdump(buffer, until.buffer_position);
    }

    return (until.success) ? until.buffer_position : -1;
  }

  // Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool hexdump                     = false)
  {
    std::array<uint8_t, kBufferSize> buffer = { 0 };
    int length = ReadUntil(buffer.data(), buffer.size(), end, timeout, hexdump);
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
};  // namespace sjsu
}  // namespace sjsu
