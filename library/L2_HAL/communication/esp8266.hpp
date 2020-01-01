#pragma once

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "config.hpp"
#include "L1_Peripheral/uart.hpp"
#include "utility/status.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

namespace sjsu
{
class Esp8266
{
 public:
  struct Buffer_t
  {
    size_t size;
    uint8_t * address;
  };
  struct ReadUntil_t
  {
    uint8_t * buffer;
    size_t length;
    const char * end;
    uint32_t buffer_position = 0;
    uint32_t end_position    = 0;
    bool success             = false;
  };
  struct ModuleVersion_t
  {
    static constexpr size_t kModuleInfoLength = 128;
    uint8_t module_info[kModuleInfoLength];
  };
  struct NetworkConnection_t
  {
    static constexpr size_t kConnectionInfoLength = 68;
    uint8_t connection_info[kConnectionInfoLength];
  };
  enum ConnectionType : uint8_t
  {
    kTcp = 0,
    kUdp = 1
  };
  static constexpr const char * kConnectionType[] =
  {
    "TCP",
    "UDP"
  };
  static constexpr uint32_t kBaudRate = 115200;
  // Confirmation responses received from the ESP8266 module
  static constexpr char kOk[] = "\r\nOK\r\n";
  static constexpr char kAPConfirmationResponse[] = "WIFI GOT IP\r\n\r\nOK\r\n";
  static constexpr char kSendRequestConfirmationResponse[] = "\r\nSEND OK";
  // Commands being sent to the ESP8266 module
  static constexpr char kTestCommand[] = "AT\r\n";
  static constexpr char kDisableEchoCommand[] = "ATE0\r\n";
  static constexpr char kClientModeCommand[] = "AT+CWMODE=1\r\n";
  static constexpr char kResetCommand[] = "AT+RST\r\n";
  static constexpr char kDeepSleepCommand[] = "AT+GSLP=%" PRIu32 "\r\n";
  static constexpr char kGetModuleVersionCommand[] = "AT+GMR\r\n";
  static constexpr char kGetNetworkConnectionInfoCommand[] =
    "AT+CIPSTATUS\r\n";
  static constexpr char kConnectToAccessPointCommand[] =
    "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n";
  static constexpr char kDisconnectFromAccessPointCommand[] =
    "AT+CWQAP\r\n";
  static constexpr char kConnectToServerCommand[] =
    "AT+CIPSTART=\"%s\",\"%s\",%" PRIu16 "\r\n";
  static constexpr char kDisconnectFromServerCommand[] = "AT+CIPCLOSE\r\n";
  static constexpr char kSendDataCommand[] = "AT+CIPSEND=%" PRIu32 "\r\n";
  // GET request format
  static constexpr char kGetRequest[] =
    "GET %s HTTP/1.1\r\n"
    "Host: %.*s\r\n"
    "\r\n";
  // POST request format
  static constexpr char kPostRequest[] =
    "POST %s HTTP/1.1\r\n"
    "Host: %.*s\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: %" PRIu32 "\r\n"
    "\r\n"
    "%s\r\n"
    "\r\n";
  // Default timeout for uart read
  static constexpr std::chrono::milliseconds kDefaultTimeout = 500ms;

  // Pass in a Uart channel
  explicit constexpr Esp8266(const Uart & port)
    : uart_port_(port)
  {
  }
  // Sends the test command to Esp8266; returns true if ESP8266 confirmation
  // response is received
  virtual bool TestModule()
  {
    WifiWrite(kTestCommand);

    return (ReadUntil(kOk, kDefaultTimeout) > -1) ? true : false;
  }
  // Sends reset command to ESP8266
  virtual void ResetModule()
  {
    WifiWrite(kResetCommand);
    ReadUntil<1024>("\r\nready\r\n", 10s);
  }
  // Initializes Uart driver and resets the wifi module. Returns true if
  // TestCommand method returns true.
  virtual bool Initialize()
  {
    uart_port_.Initialize(kBaudRate);

    while (uart_port_.HasData())
    {
      uart_port_.Read();
    }

    WifiWrite("+++");
    ResetModule();
    WifiWrite(kDisableEchoCommand);
    ReadUntil(kOk, kDefaultTimeout);
    WifiWrite(kClientModeCommand);
    ReadUntil(kOk, kDefaultTimeout);

    return TestModule();
  }
  // Puts ESP8266 into deep sleep. Time is in microseconds
  virtual void DeepSleep(uint32_t time)
  {
    if (time != 0)
    {
      char command_buffer[20];

      // Write full command with time to char buffer
      snprintf(command_buffer, sizeof(command_buffer), kDeepSleepCommand,
               time);

      WifiWrite(command_buffer);
      ReadUntil(kOk, kDefaultTimeout);
    }
  }
  // Gets module version and writes it to default buffer
  virtual ModuleVersion_t GetModuleVersion()
  {
    ModuleVersion_t version;

    WifiWrite(kGetModuleVersionCommand);
    ReadUntil(version.module_info, sizeof(version.module_info),
              kDefaultTimeout, kOk);

    return version;
  }
  // Gets network connection info and writes it to default buffer
  virtual NetworkConnection_t GetNetworkConnection()
  {
    NetworkConnection_t network;

    WifiWrite(kGetNetworkConnectionInfoCommand);
    ReadUntil(network.connection_info, sizeof(network.connection_info),
              kDefaultTimeout, kOk);

    return network;
  }
  // Connect to access point. Returns true if connection is established.
  virtual bool ConnectToAccessPoint(const char * ssid,
                                    const char * password,
                                    std::chrono::milliseconds read_timeout =
                                      10s)
  {
    char command_buffer[84];
    snprintf(command_buffer, sizeof(command_buffer),
             kConnectToAccessPointCommand, ssid, password);

    WifiWrite(command_buffer);
    return (ReadUntil(kAPConfirmationResponse, read_timeout) > -1) ?
      true : false;
  }
  // Disconnects ESP8266 from access point
  virtual void DisconnectFromAccessPoint()
  {
    WifiWrite(kDisconnectFromAccessPointCommand);
    ReadUntil(kOk, kDefaultTimeout);
  }
  // Returns true if connected to an access point
  virtual bool IsConnectedToAccessPoint()
  {
    const uint8_t kStatus = 7;
    NetworkConnection_t network = GetNetworkConnection();

    return (network.connection_info[kStatus] != '5');
  }
  // Connects ESP8266 to hostname. Returns true if connection is established.
  virtual bool ConnectToServer(const char * server,
                               uint16_t port,
                               std::chrono::milliseconds read_timeout =
                                 kDefaultTimeout,
                               ConnectionType type = ConnectionType::kTcp)
  {
    char command_buffer[43];
    snprintf(command_buffer, sizeof(command_buffer), kConnectToServerCommand,
             kConnectionType[type], server, port);

    WifiWrite(command_buffer);
    ReadUntil(kOk, read_timeout);
    return (ReadUntil(kOk, read_timeout) > -1) ? true : false;
  }
  // Disconnect from server
  virtual void DisconnectFromServer()
  {
    WifiWrite(kDisconnectFromServerCommand);
    ReadUntil(kOk, kDefaultTimeout);
  }
  // Returns true if ESP8266 is connected to hostname
  virtual bool IsConnectedToServer()
  {
    const uint8_t kStatus = 7;
    NetworkConnection_t network = GetNetworkConnection();

    return (network.connection_info[kStatus] == '3');
  }
  // Sends get request to server
  // url example format: sjsu.edu/ or sjsu.edu/parkingtransportationmaps/
  // Returns length of data in buffer
  // Stores request response in buffer
  virtual int32_t SendGetRequest(const char * url,
                                 Buffer_t buffer,
                                 std::chrono::milliseconds read_timeout =
                                   kDefaultTimeout)
  {
    size_t end_of_hostname = 0;
    size_t url_size = strlen(url);
    uint32_t get_request_size = 25 + static_cast<uint32_t>(url_size);
    char send_data_command_buffer[23];
    char get_request_buffer[128];

    snprintf(send_data_command_buffer, sizeof(send_data_command_buffer),
             kSendDataCommand, get_request_size);

    for (uint32_t i = 0; i < url_size; i++)
    {
      if (url[end_of_hostname] != '/')
      {
        end_of_hostname++;
      }
      else
      {
        break;
      }
    }

    snprintf(get_request_buffer, sizeof(get_request_buffer), kGetRequest,
             &url[end_of_hostname], static_cast<int>(end_of_hostname), url);

    WifiWrite(send_data_command_buffer);
    if (ReadUntil(">", 10s) == -1)
    {
      return -1;
    }

    WifiWrite(get_request_buffer);
    return FlushBuffer(buffer.address, (buffer.size-1), read_timeout);
  }
  // Sends post request to server
  // url example format: sjsu.edu/ or sjsu.edu/parkingtransportationmaps/
  // payload example format: stuff=4 or stuff=12&things=13
  // Returns true if info was sent
  // Stores request response in buffer
  virtual int32_t SendPostRequest(const char * url,
                                  const char * payload,
                                  Buffer_t buffer,
                                  std::chrono::milliseconds read_timeout =
                                    kDefaultTimeout)
  {
    size_t end_of_hostname = 0;
    size_t url_size = strlen(url);
    uint32_t payload_size = static_cast<uint32_t>(strlen(payload));
    int temp_payload_size = payload_size;
    uint32_t post_request_size = 97 + payload_size +
      static_cast<uint32_t>(url_size);
    char send_data_command_buffer[23];
    char post_request_buffer[256];

    while (temp_payload_size != 0)
    {
      temp_payload_size /= 10;
      post_request_size++;
    }

    snprintf(send_data_command_buffer, sizeof(send_data_command_buffer),
             kSendDataCommand, post_request_size);

    for (uint32_t i = 0; i < url_size; i++)
    {
      if (url[end_of_hostname] != '/')
      {
        end_of_hostname++;
      }
      else
      {
        break;
      }
    }

    snprintf(post_request_buffer, sizeof(post_request_buffer), kPostRequest,
             &url[end_of_hostname], static_cast<int>(end_of_hostname), url,
             payload_size, payload);

    WifiWrite(send_data_command_buffer);
    if (ReadUntil(">", 10s) == -1)
    {
      return -1;
    }

    WifiWrite(post_request_buffer);
    return FlushBuffer(buffer.address, (buffer.size-1), read_timeout);
  }

 private:
  // Writes command array to Esp8266
  template<size_t kLength>
  void WifiWrite(const char (&str)[kLength])
  {
    uart_port_.Write(reinterpret_cast<const uint8_t *>(str), kLength - 1);
  }
  // Reads to provided buffer and checks to see if the end of the read matches
  // the end char array.  Returns size of read or -1 if end char array doesn't
  // match the read char array.
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

    return (until.success) ? until.buffer_position : -1;
  }
  // Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const char * end,
                std::chrono::milliseconds timeout)
  {
    uint8_t buffer[kBufferSize] = { 0 };
    int length = ReadUntil(buffer, sizeof(buffer), timeout, end);

    return length;
  }
  // Writes responses to ESP8266 from buffer
  virtual uint32_t FlushBuffer(uint8_t * buffer,
                                     std::size_t length,
                                     std::chrono::milliseconds timeout)
  {
    uint8_t total_buffer[64] = {0};
    ReadUntil(total_buffer, sizeof(total_buffer), timeout, "+IPD,");
    ReadUntil(total_buffer, sizeof(total_buffer), timeout, ":");

    uint32_t total_bytes = 0;
    sscanf(reinterpret_cast<const char *>(total_buffer), "%lu:", &total_bytes);
    total_bytes = std::min(static_cast<uint32_t>(length), total_bytes);

    uint32_t position = 0;
    sjsu::Wait(timeout, [this, buffer, &position, total_bytes]() {
      if (position >= total_bytes)
      {
        return true;
      }
      if (!uart_port_.HasData())
      {
        return false;
      }
      buffer[position++] = uart_port_.Read();
      return false;
    });
    return position;
  }

  const Uart & uart_port_;
};
}  // namespace sjsu
