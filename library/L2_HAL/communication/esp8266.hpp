#pragma once

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
  // Confirmation responses recieved from the ESP8266 module
  static constexpr uint8_t kConfirmationResponse[] = "\r\nOK";
  static constexpr uint8_t kAPConfirmationResponse[] = "WIFI";
  static constexpr uint8_t kSendRequestConfirmationResponse[] = "\r\nSEND OK";
  // Error response recieved from ESP8266 module
  static constexpr uint8_t kErrorResponse[] = "\r\nERROR";
  // Commands being sent to the ESP8266 module
  static constexpr uint8_t kTestCommand[] = "AT\r\n";
  static constexpr uint8_t kDisableEchoCommand[] = "ATE0\r\n";
  static constexpr uint8_t kClientModeCommand[] = "AT+CWMODE=1\r\n";
  static constexpr uint8_t kResetCommand[] = "AT+RST\r\n";
  static constexpr char kDeepSleepCommand[] = "AT+GSLP=%" PRIu32 "\r\n";
  static constexpr uint8_t kGetModuleVersionCommand[] = "AT+GMR\r\n";
  static constexpr uint8_t kGetNetworkConnectionInfoCommand[] =
    "AT+CIPSTATUS\r\n";
  static constexpr char kConnectToAccessPointCommand[] =
    "AT+CWJAP=\"%s\",\"%s\"\r\n";
  static constexpr uint8_t kDisconnectFromAccessPointCommand[] =
    "AT+CWQAP\r\n";
  static constexpr char kConnectToServerCommand[] =
    "AT+CIPSTART=\"%s\",\"%s\",%" PRIu16 "\r\n";
  static constexpr uint8_t kDisconnectFromServerCommand[] = "AT+CIPCLOSE\r\n";
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
  static constexpr std::chrono::microseconds kDefaultTimeout = 5000ms;

  // Pass in a Uart channel
  explicit constexpr Esp8266(const Uart & port)
    : uart_port_(port)
  {
  }
  // Sends the test command to Esp8266; returns true if ESP8266 confirmation
  // response is received
  virtual bool TestModule()
  {
    uint8_t receive_buffer[5];
    receive_buffer[4] = '\0';

    Write(kTestCommand);

    uart_port_.Read(receive_buffer, sizeof(receive_buffer-1), kDefaultTimeout);
    FlushSerialBuffer();

    printf("--Test--\n");
    printf("Expected: %s, Actual: %s\n", kConfirmationResponse, receive_buffer);

    return CompareResponse(kConfirmationResponse,
                           (sizeof(kConfirmationResponse)-1),
                           receive_buffer);
  }
  // Sends reset command to ESP8266
  virtual void ResetModule()
  {
    printf("--Reset--\n");
    Write(kResetCommand);
    Wait(200ms);
    Write(kDisconnectFromServerCommand);
    Wait(200ms);
    FlushSerialBuffer();
    Write(kDisconnectFromAccessPointCommand);
    Wait(200ms);
    FlushSerialBuffer();
  }
  // Initializes Uart driver and resets the wifi module. Returns true if
  // TestCommand method returns true.
  virtual bool Initialize()
  {
    printf("--Initialize--\n");
    uart_port_.Initialize(kBaudRate);

    FlushSerialBuffer();
    ResetModule();
    Write(kDisableEchoCommand);
    FlushSerialBuffer();
    Write(kClientModeCommand);
    FlushSerialBuffer();

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

      WriteFromNullTerminatedBuffer(reinterpret_cast<uint8_t*>(command_buffer));
      FlushSerialBuffer();
    }
  }
  // Gets module version and writes it to default buffer
  virtual ModuleVersion_t GetModuleVersion()
  {
    ModuleVersion_t version;

    Write(kGetModuleVersionCommand);
    uart_port_.Read(version.module_info, sizeof(version.module_info),
                    kDefaultTimeout);
    FlushSerialBuffer();

    AppendNullToResponse(version.module_info, sizeof(version.module_info));

    return version;
  }
  // Gets network connection info and writes it to default buffer
  virtual NetworkConnection_t GetNetworkConnection()
  {
    NetworkConnection_t network;

    Write(kGetNetworkConnectionInfoCommand);
    uart_port_.Read(network.connection_info,
                    sizeof(network.connection_info), kDefaultTimeout);
    FlushSerialBuffer();

    AppendNullToResponse(network.connection_info,
                         sizeof(network.connection_info));

    return network;
  }
  // Connect to access point. Returns true if connection is established.
  virtual bool ConnectToAccessPoint(const char * ssid,
                                    const char * password,
                                    std::chrono::microseconds read_timeout =
                                      kDefaultTimeout)
  {
    char command_buffer[80];
    uint8_t receive_buffer[16];
    receive_buffer[15] = '\0';

    snprintf(command_buffer, sizeof(command_buffer),
             kConnectToAccessPointCommand, ssid, password);

    printf("%s", command_buffer);

    WriteFromNullTerminatedBuffer(reinterpret_cast<uint8_t*>(command_buffer));
    uart_port_.Read(receive_buffer, sizeof(receive_buffer-1), read_timeout);
    printf("Afte Wifi: %s", receive_buffer[5]);
    Wait(200ms);
    FlushSerialBuffer();

    printf("--Connect to ap--\n");
    printf("Expected: %s, Actual: %s\n", kAPConfirmationResponse, receive_buffer);

    return CompareResponse(kAPConfirmationResponse,
                           (sizeof(kAPConfirmationResponse)-1),
                           receive_buffer);
  }
  // Disconnects ESP8266 from access point
  virtual void DisconnectFromAccessPoint()
  {
    Write(kDisconnectFromAccessPointCommand);
    FlushSerialBuffer();
  }
  // Returns true if connected to an access point
  virtual bool IsConnectedToAccessPoint()
  {
    const uint8_t kStatus = 7;
    NetworkConnection_t network = GetNetworkConnection();

    return (network.connection_info[kStatus] != '5');
  }
  // Connects ESP8266 to hostname. Returns true if connection is estabilshed.
  virtual bool ConnectToServer(const char * server,
                               uint16_t port,
                               std::chrono::microseconds read_timeout =
                                 kDefaultTimeout,
                               ConnectionType type = ConnectionType::kTcp)
  {
    char command_buffer[43];
    uint8_t receive_buffer[7];

    snprintf(command_buffer, sizeof(command_buffer), kConnectToServerCommand,
             kConnectionType[type], server, port);

    WriteFromNullTerminatedBuffer(reinterpret_cast<uint8_t*>(command_buffer));

    uart_port_.Read(receive_buffer, sizeof(receive_buffer), read_timeout);
    FlushSerialBuffer();

    printf("--Server connect--\n");
    printf("Not Expected: %s, Actual: %s\n", kErrorResponse, receive_buffer);

    return !(CompareResponse(kErrorResponse, (sizeof(kErrorResponse)-1),
                             receive_buffer));
  }
  // Disconnect from server
  virtual void DisconnectFromServer()
  {
    Write(kDisconnectFromServerCommand);
    FlushSerialBuffer();
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
  // Returns number of characters not written to buffer
  // Stores request response in buffer
  virtual int32_t SendGetRequest(const char * url,
                                 Buffer_t buffer,
                                 std::chrono::microseconds read_timeout =
                                   kDefaultTimeout)
  {
    size_t end_of_hostname = 0;
    size_t url_size = strlen(url);
    uint32_t get_request_size = 25 + static_cast<uint32_t>(url_size);
    char send_data_command_buffer[23];
    char get_request_buffer[128];
    uint8_t dummy_buffer;

    for (uint32_t k = 0; k < buffer.size; k++)
    {
      buffer.address[k] = '\0';
    }

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
    printf("%s", send_data_command_buffer);
    WriteFromNullTerminatedBuffer(
      reinterpret_cast<uint8_t*>(send_data_command_buffer));

    if (uart_port_.Read(&dummy_buffer, 1, kDefaultTimeout) == Status::kTimedOut)
    {
      return -1;
    }

    printf("dummy buffer: %c", dummy_buffer);

    if (dummy_buffer == '>')
    {
      int16_t missed_char = 0;
      printf("%s", get_request_buffer);
      WriteFromNullTerminatedBuffer(
        reinterpret_cast<uint8_t*>(get_request_buffer));

      uart_port_.Read(buffer.address, (buffer.size-1), read_timeout);

      for (uint32_t j = 0; j < 500; j++)
      {
        if (uart_port_.Read(&dummy_buffer, 1, read_timeout) !=
          Status::kTimedOut)
        {
          missed_char++;
        }
        else
        {
          break;
        }
      }

      return missed_char;
    }
    else
    {
      return -1;
    }
  }
  // Sends post request to server
  // url example format: sjsu.edu/ or sjsu.edu/parkingtransportationmaps/
  // payload example format: stuff=4 or stuff=12&things=13
  // Returns true if info was sent
  // Stores request response in buffer
  virtual int32_t SendPostRequest(const char * url,
                                  const char * payload,
                                  Buffer_t buffer,
                                  std::chrono::microseconds read_timeout =
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
    uint8_t dummy_buffer;

    for (uint32_t k = 0; k < buffer.size; k++)
    {
      buffer.address[k] = '\0';
    }

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

    WriteFromNullTerminatedBuffer(
      reinterpret_cast<uint8_t*>(send_data_command_buffer));

    if (uart_port_.Read(&dummy_buffer, 1, kDefaultTimeout) == Status::kTimedOut)
    {
      return -1;
    }

    if (dummy_buffer == '>')
    {
      int16_t missed_char = 0;

      WriteFromNullTerminatedBuffer(
        reinterpret_cast<uint8_t*>(post_request_buffer));

      uart_port_.Read(buffer.address, (buffer.size-1), read_timeout);

      for (uint32_t j = 0; j < 500; j++)
      {
        if (uart_port_.Read(&dummy_buffer, 1, read_timeout) !=
          Status::kTimedOut)
        {
          missed_char++;
        }
        else
        {
          break;
        }
      }

      return missed_char;
    }
    else
    {
      return -1;
    }
  }

 private:
  // Writes command array to Esp8266 and flushes buffer
  template<typename T, size_t size>
  void Write(const T (&array)[size])
  {
    uart_port_.Write(array, (size-1));
  }
  // Compares expected responses with the actual response from Esp8266
  virtual bool CompareResponse(const uint8_t * expected_response,
                               size_t expected_response_size,
                               const uint8_t * actual_response)
  {
    bool match = true;

    for (uint16_t i = 0; i < expected_response_size; i++)
    {
      if (expected_response[i] != actual_response[i])
      {
        printf("Iteration: %i\n", i);
        printf("Expected: %c\n", expected_response[i]);
        printf("Actual: %c\n", actual_response[i]);
        match = false;
        break;
      }
    }

    return match;
  }
  // Writes responses to ESP8266 from buffer
  virtual void FlushSerialBuffer()
  {
    printf("--Buffer Flush--\n");
    constexpr uint8_t kFlushCount = 100;
    uint8_t buffer;

    for (uint8_t i = 0; i < kFlushCount; i++)
    {
      if (uart_port_.Read(&buffer, 1, kDefaultTimeout) != Status::kTimedOut)
      {
        printf("%c", buffer);
        continue;
      }
      else
      {
        break;
      }
    }
  }
  virtual void AppendNullToResponse(uint8_t * buffer, size_t size)
  {
    for (size_t i = 0; i < (size-5); i++)
    {
      if ((buffer[i] == '\r') && (buffer[i+1] == '\n') && (buffer[i+2] == 'O')
          && (buffer[i+3] == 'K'))
      {
        buffer[i+4] = '\0';
        break;
      }
    }

    buffer[size-1] = '\0';
  }
  virtual void WriteFromNullTerminatedBuffer(const uint8_t * buffer)
  {
    uint32_t index = 0;
    while (buffer[index] != '\0')
    {
      uart_port_.Write(&(buffer[index]), 1);
      index++;
    }
  }

  const Uart & uart_port_;
};
}  // namespace sjsu
