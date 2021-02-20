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

#include "peripherals/uart.hpp"
#include "devices/communication/internet_socket.hpp"
#include "config.hpp"
#include "module.hpp"
#include "utility/debug.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"
#include "utility/time/timeout_timer.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Serial driver for the ESP8266 wifi device that implements the WiFi and
/// InternetSocket interfaces.
/// Can be used to connect to a WiFi host device.
/// Can be used to communicate over the internet using this driver.
class Esp8266 : public Module<>
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

  /// ESP8266's Implementation of the sjsu::InternetSocket interface/protocol
  class Esp8266InternetSocket : public InternetSocket
  {
   public:
    /// Constructor for Esp8266 InternetSocket implementation
    ///
    /// @param esp - a reference to the main esp8266 object
    explicit constexpr Esp8266InternetSocket(Esp8266 & esp) : esp_(esp) {}

    void ModuleInitialize() override
    {
      esp_.Initialize();
    }

    bool Connect(Protocol protocol,
                 std::string_view address,
                 uint16_t port,
                 std::chrono::nanoseconds timeout) override
    {
      return esp_.Connect(protocol, address, port, timeout);
    }

    void Write(std::span<const uint8_t> data,
               std::chrono::nanoseconds timeout) override
    {
      esp_.Write(data, timeout);
    }

    size_t Read(std::span<uint8_t> data,
                std::chrono::nanoseconds timeout) override
    {
      return esp_.Read(data, timeout);
    }

    void Close() override
    {
      esp_.Close();
    }

   private:
    Esp8266 & esp_;
  };

  /// ESP8266's Implementation of the sjsu::Wifi interface
  class Esp8266WiFi : public WiFi
  {
   public:
    /// Constructor for ESP8266 WiFi implementation
    ///
    /// @param esp - a reference to the main esp8266 object
    explicit constexpr Esp8266WiFi(Esp8266 & esp) : esp_(esp) {}

    void ModuleInitialize() override
    {
      esp_.Initialize();
    }

    bool ConnectToAccessPoint(std::string_view ssid,
                              std::string_view password,
                              std::chrono::nanoseconds read_timeout) override
    {
      return esp_.ConnectToAccessPoint(ssid, password, read_timeout);
    }

    void DisconnectFromAccessPoint() override
    {
      esp_.DisconnectFromAccessPoint();
    }

    bool IsConnected() override
    {
      return esp_.IsConnected();
    }

    NetworkConnection_t GetNetworkConnectionInfo() override
    {
      return NetworkConnection_t{};
    }

   private:
    Esp8266 & esp_;
  };

  /// @param port - uart port to use to communicate with the ESP8266 device
  /// @param baud_rate - operating baudrate of the uart device.
  explicit constexpr Esp8266(sjsu::Uart & port,
                             uint32_t baud_rate = kDefaultBaudRate)
      : uart_port_(port), kBaudRate(baud_rate), socket_(*this), wifi_(*this)
  {
  }

  void ModuleInitialize() override
  {
    // Set baudrate with all other UART settings set to default
    const UartSettings_t kDefaultSettings = { .baud_rate = kBaudRate };
    uart_port_.settings                   = kDefaultSettings;
    uart_port_.Initialize();
    uart_port_.Flush();

    try
    {
      LogDebug("Reseting module...");
      WifiWrite("+++");
      Reset();

      // Disable echo
      LogDebug("Disabling echo back...");
      WifiWrite("ATE0\r\n");
      ReadUntil(kOk, kDefaultTimeout, true);

      // Enable simple IP client mode
      LogDebug("Set device to simple IP client mode...");
      WifiWrite("AT+CWMODE=1\r\n");
      ReadUntil(kOk, kDefaultTimeout, true);

      LogDebug("Testing that module is ready...");
      TestModule();
    }
    catch (sjsu::Exception & e)
    {
      if (e.GetCode() == std::errc::timed_out)
      {
        throw sjsu::Exception(
            std::errc::no_such_device,
            "Enabling ESP8266 driver failed! The baud rate could be "
            "incorrect. The device may not be connected, or connected "
            "properly, or is not responding to input (possibly broken).");
      }
    }
  }

  /// @return InternetSocket& - A reference to a InternetSocket client object
  /// that uses the ESP8266. Use this to
  InternetSocket & GetInternetSocket()
  {
    return socket_;
  }

  /// @return WiFi& - A reference to a WiFi client object that uses the ESP8266
  WiFi & GetWiFi()
  {
    return wifi_;
  }

  /// Tests that the ESP8266 can respond to commands.
  ///
  /// @throw std::errc::timed_out exception if the ESP8266 fails to respond in
  /// time.
  void TestModule()
  {
    WifiWrite("AT\r\n");
    ReadUntil(kOk, kDefaultTimeout, true);
  }

  /// Resets ESP8266 using serial command.
  void Reset()
  {
    WifiWrite("AT+RST\r\n");
    ReadUntil<1024>("\r\nready\r\n", kDefaultLongTimeout, true);
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

    int length = ReadUntil(buffer, "OK\r\n", 3s);

    debug::HexdumpDebug(buffer.data(), length);

    IpAddress_t ip;
    std::array<uint32_t, 4> temp_ip;
    sscanf(reinterpret_cast<const char *>(buffer.data()),
           "%lu.%lu.%lu.%lu,",
           &temp_ip[0],
           &temp_ip[1],
           &temp_ip[2],
           &temp_ip[3]);

    ip.data[0] = static_cast<uint8_t>(temp_ip[0]);
    ip.data[1] = static_cast<uint8_t>(temp_ip[1]);
    ip.data[2] = static_cast<uint8_t>(temp_ip[2]);
    ip.data[3] = static_cast<uint8_t>(temp_ip[3]);

    return ip;
  }

 private:
  // ===========================================================================
  // WiFi
  // ===========================================================================

  bool ConnectToAccessPoint(
      std::string_view ssid,
      std::string_view password,
      std::chrono::nanoseconds read_timeout = kDefaultLongTimeout)
  {
    std::array<char, 128> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          R"(AT+CWJAP_CUR="%s","%s")"
                          "\r\n",
                          ssid.data(),
                          password.data());

    std::span payload(command_buffer.data(), length);
    uart_port_.Write(std::as_bytes(payload));

    int confirmation_location =
        ReadUntil("WIFI GOT IP\r\n\r\nOK\r\n", read_timeout);

    return (confirmation_location != -1);
  }

  void DisconnectFromAccessPoint()
  {
    WifiWrite("AT+CWQAP\r\n");
    ReadUntil(kOk);
  }

  /// @param mode - Which mode to put the ESP8266 WiFi module into.
  void SetMode(WiFi::Mode mode)
  {
    std::array<char, 32> command_buffer;

    int length = snprintf(command_buffer.data(),
                          command_buffer.size(),
                          R"(AT+CWMODE=%u)"
                          "\r\n",
                          Value(mode));

    std::span payload(command_buffer.data(), length);
    uart_port_.Write(std::as_bytes(payload));

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
                            WiFi::AccessPointSecurity security)
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

    std::span payload(command_buffer.data(), length);
    uart_port_.Write(std::as_bytes(payload));

    ReadUntil("OK\r\n", 100ms);
  }

  // ===========================================================================
  // InternetProtocol
  // ===========================================================================

  bool Connect(InternetSocket::Protocol protocol,
               std::string_view address,
               uint16_t port,
               std::chrono::nanoseconds timeout)
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

    std::span payload(command_buffer.data(), length);
    uart_port_.Write(std::as_bytes(payload));

    if (ReadUntil(kOk, timer.GetTimeLeft(), true) == -1)
    {
      return false;
    }

    return true;
  }

  void Bind(uint16_t port = 333)
  {
    WifiWrite("AT+CIPMUX=1\r\n");
    ReadUntil(kOk);

    std::array<char, 64> command_buffer;
    static constexpr char kServerStart[] = "AT+CIPSERVER=1,%" PRIu16 "\r\n";

    int length = snprintf(
        command_buffer.data(), command_buffer.size(), kServerStart, port);

    std::span payload(command_buffer.data(), length);
    uart_port_.Write(std::as_bytes(payload));

    ReadUntil(kOk, kDefaultLongTimeout, true);
  }

  // TODO(kammce): Fix this!!
  bool IsConnected()
  {
    return false;
  }

  void Write(std::span<const uint8_t> data, std::chrono::nanoseconds timeout)
  {
    TimeoutTimer timer(timeout);

    std::array<char, 32> data_count_buffer;
    int send_count_length = snprintf(data_count_buffer.data(),
                                     data_count_buffer.size(),
                                     "AT+CIPSEND=%zu\r\n",
                                     data.size());

    // Send payload length
    std::span payload(data_count_buffer.data(), send_count_length);
    uart_port_.Write(std::as_bytes(payload));

    // Wait for an OK!
    if (ReadUntil(kOk, timer.GetTimeLeft()) == -1)
    {
      LogDebug("CIPSEND");
      throw Exception(std::errc::io_error,
                      "ESP8266 did not accept requestion to send data");
    }

    // Now write the rest of the data
    uart_port_.Write(data);

    // Wait for an OK!
    if (ReadUntil("\r\nSEND OK\r\n\r\n", timer.GetTimeLeft()) == -1)
    {
      LogDebug("SEND OK");
      throw Exception(std::errc::io_error, "Attempt to write to server failed");
    }
  }

  size_t Read(std::span<uint8_t> data, std::chrono::nanoseconds timeout)
  {
    TimeoutTimer timer(timeout);
    size_t received_bytes = GetReceiveLength(timer);

    if (received_bytes == 0)
    {
      return 0;
    }

    auto data_subspan = data.first(std::min(data.size(), received_bytes));

    return uart_port_.Read(data_subspan, timer.GetTimeLeft());
  }

  void Close()
  {
    WifiWrite("AT+CIPCLOSE\r\n");
    ReadUntil(kOk);
  }

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

    auto receive_length = reinterpret_cast<const char *>(total_buffer.data());

    sscanf(receive_length, "%zu:", &incoming_bytes);
    return incoming_bytes;
  }

  // Writes command array to Esp8266
  template <size_t kLength>
  void WifiWrite(const char (&str)[kLength])
  {
    const auto * ptr = reinterpret_cast<const uint8_t *>(str);
    uart_port_.Write(std::span(ptr, kLength - 1));
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
      debug::HexdumpDebug(buffer.data(), buffer.size());
      throw Exception(std::errc::timed_out,
                      "Did not get the expected response from device.");
    }

    return -1;
  }

  /// Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const char * end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool throw_on_failure            = false)
  {
    std::array<uint8_t, kBufferSize> buffer = { 0 };
    return ReadUntil(buffer, end, timeout, throw_on_failure);
  }

  Uart & uart_port_;
  const uint32_t kBaudRate;
  Esp8266InternetSocket socket_;
  Esp8266WiFi wifi_;
};  // namespace sjsu
}  // namespace sjsu
