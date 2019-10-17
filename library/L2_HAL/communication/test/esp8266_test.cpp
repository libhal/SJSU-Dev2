#include "L2_HAL/communication/esp8266.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Esp8266);
TEST_CASE("Esp8266 Test", "[esp8266]")
{
  Mock<Uart> mock_uart;
  Fake(Method(mock_uart, Initialize),
       ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)),
       ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
         std::chrono::microseconds)));

  const Uart & uart = mock_uart.get();
  Esp8266 test_wifi_module(uart);

  SECTION("Test Module")
  {
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static constexpr uint8_t kConfirmationResponse[] = "\r\nOK";
      static uint8_t count = 0;

      // After writing the test command to the wifi module, the microcontroller
      // is supposed to read a confirmation response
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == (sizeof(kConfirmationResponse) - 1));

        for (size_t i = 0; i < size; i++)
        {
          data[i] = kConfirmationResponse[i];
        }
        count++;
      }
      // The second uart read is used to flush the serial buffer
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    CHECK(test_wifi_module.TestModule() == true);
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(Esp8266::kTestCommand, (sizeof(Esp8266::kTestCommand)-1)));
  }

  SECTION("Reset Module")
  {
    When(ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds)))
      .AlwaysReturn(Status::kTimedOut);
    test_wifi_module.ResetModule();
    Verify(
      // Check to see if reset command was writen to the esp8266 module
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kResetCommand, (sizeof(Esp8266::kResetCommand)-1)) +
      // Checks if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }

  SECTION("Initialize")
  {
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static constexpr uint8_t kConfirmationResponse[] = "\r\nOK";
      static uint8_t count = 0;

      // After writing the test command to the wifi module, the microcontroller
      // is supposed to read a confirmation response
      if (count == 2)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == (sizeof(kConfirmationResponse) - 1));

        for (size_t i = 0; i < size; i++)
        {
          data[i] = kConfirmationResponse[i];
        }
        count++;
      }
      // This part is primarily used for the FlushSerialBuffer method
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
        count++;
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    CHECK(test_wifi_module.Initialize() == true);
    Verify(
      // Check to see if Initalize function was called with the baud rate
      // specified in the esp8266 class
      Method(mock_uart, Initialize).Using(Esp8266::kBaudRate) +
      // Check to see if kDisableEchoCommand was written to esp8266 module
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kDisableEchoCommand,
          (sizeof(Esp8266::kDisableEchoCommand)-1)) +
      // Check to see if kResetCommand was written to esp8266 module
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kResetCommand, (sizeof(Esp8266::kResetCommand)-1)) +
      // Checks if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)) +
      // Check to see if kClientModeCommand was written to esp8266 module
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kClientModeCommand,
          (sizeof(Esp8266::kClientModeCommand)-1)) +
      // Checks if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)) +
      // Checks to see if kTestCommand was written to esp8266 module
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kTestCommand, (sizeof(Esp8266::kTestCommand)-1)));
  }

  SECTION("Deep Sleep")
  {
    static constexpr uint8_t kExpectedWrite[] = "AT+GSLP=27\r\n";
    // Used to check every uart write call and compare it with kExpectedWrite
    auto verify_write = [](const uint8_t * data, size_t size) {
      static uint8_t index = 0;

      CHECK(*data == kExpectedWrite[index]);
      CHECK(size == 1);
      index++;
    };
    When(ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
      .AlwaysDo(verify_write);
    When(ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds)))
      .AlwaysReturn(Status::kTimedOut);
    // Called DeepSleep() with 27 as the parameter
    test_wifi_module.DeepSleep(27);
    Verify(
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds))).Once();
  }

  SECTION("Get Module Version")
  {
    static constexpr uint8_t kTest[] = "test\r\nOK";
    // Used to read kTest to buffer when using uart read
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      // Reads kTest to buffer and checks if timeout and size are correct
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 128);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }

        count++;
      }
      // Checks FlushSerialBuffer has right timeout and size
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    Esp8266::ModuleVersion_t version = test_wifi_module.GetModuleVersion();
    // Checks to see if module_info in ModuleVersion_t struct was written to
    for (size_t i = 0; i < sizeof(kTest); i++)
    {
      CHECK(version.module_info[i] == kTest[i]);
    }
    // Checks to see if GetModuleVersionCommand was written to the esp8266
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(Esp8266::kGetModuleVersionCommand,
              (sizeof(Esp8266::kGetModuleVersionCommand)-1)));
  }

  SECTION("Get Network Connection")
  {
    static constexpr uint8_t kTest[] = "test\r\nOK";
    // Used to read kTest to buffer when using uart read
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      // Reads kTest to buffer and checks if timeout and size are correct
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 68);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }

        count++;
      }
      // Checks FlushSerialBuffer has right timeout and size
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    Esp8266::NetworkConnection_t network =
      test_wifi_module.GetNetworkConnection();
    // Checks to see if connection_info in NetworkConnection_t struct was
    // written to
    for (size_t i = 0; i < sizeof(kTest); i++)
    {
      CHECK(network.connection_info[i] == kTest[i]);
    }
    // Checks to see if GetNetworkConnectionInfoCommand was written to the
    // esp8266
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(Esp8266::kGetNetworkConnectionInfoCommand,
              (sizeof(Esp8266::kGetNetworkConnectionInfoCommand)-1)));
  }

  SECTION("Connect to Access Point")
  {
    static constexpr uint8_t kExpectedWrite[] =
        "AT+CWJAP=\"ssid_test\",\"password_test\"\r\n";
    static constexpr char kSsid[] = "ssid_test";
    static constexpr char kPassword[] = "password_test";
    auto verify_write = [](const uint8_t * data, size_t size) {
      static uint8_t index = 0;

      CHECK(*data == kExpectedWrite[index]);
      CHECK(size == 1);
      index++;
    };
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static constexpr uint8_t kConfirmationResponse[] = "\r\nOK";
      static uint8_t count = 0;

      // After writing the test command to the wifi module, the microcontroller
      // is supposed to read a confirmation response
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == (sizeof(kConfirmationResponse) - 1));

        for (size_t i = 0; i < size; i++)
        {
          data[i] = kConfirmationResponse[i];
        }
        count++;
      }
      // The second uart read is used to flush the serial buffer
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t)))
      .AlwaysDo(verify_write);
    When(ConstOverloadedMethod(
         mock_uart,
         Read,
         Status(uint8_t *, size_t, std::chrono::microseconds)))
      .AlwaysDo(uart_response);
    CHECK(test_wifi_module.ConnectToAccessPoint(kSsid, kPassword,
          Esp8266::kReadTimeout) == true);
    Verify(
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds))).Exactly(2);
  }

  SECTION("Disconnect from Access Point")
  {
    When(ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds)))
      .AlwaysReturn(Status::kTimedOut);
    test_wifi_module.DisconnectFromAccessPoint();
    Verify(
      // Checks to see that uart Write is called with the "disconnect from
      // access point" command
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kDisconnectFromAccessPointCommand,
        (sizeof(Esp8266::kDisconnectFromAccessPointCommand)-1)) +
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }

  SECTION("Is Connected to Access Point")
  {
    static constexpr uint8_t kTest[] = "STATUS:5";
    // Used to read kTest to buffer when using uart read
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      // Reads kTest to buffer and checks if timeout and size are correct
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 68);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }

        count++;
      }
      // Checks FlushSerialBuffer has right timeout and size
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    // Checks to see if GetNetworkConnectionInfoCommand was written to the
    // esp8266
    CHECK(test_wifi_module.IsConnectedToAccessPoint() == false);
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(Esp8266::kGetNetworkConnectionInfoCommand,
              (sizeof(Esp8266::kGetNetworkConnectionInfoCommand)-1)));
  }

  SECTION("Connect to Server")
  {
    static constexpr uint8_t kExpectedWrite[] =
        "AT+CIPSTART=\"UDP\",\"server_test\",1000\r\n";
    static constexpr char kServer[] = "server_test";
    auto verify_write = [](const uint8_t * data, size_t size) {
      static uint8_t index = 0;

      CHECK(*data == kExpectedWrite[index]);
      CHECK(size == 1);
      index++;
    };
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static constexpr uint8_t kErrorResponse[] = "\r\nERROR";
      static uint8_t count = 0;

      // After writing the test command to the wifi module, the microcontroller
      // is supposed to read a confirmation response
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == (sizeof(kErrorResponse) - 1));

        for (size_t i = 0; i < size; i++)
        {
          data[i] = kErrorResponse[i];
        }
        count++;
      }
      // The second uart read is used to flush the serial buffer
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(mock_uart, Write,
        void(const uint8_t *, size_t)))
      .AlwaysDo(verify_write);
    When(ConstOverloadedMethod(
         mock_uart,
         Read,
         Status(uint8_t *, size_t, std::chrono::microseconds)))
      .AlwaysDo(uart_response);
    CHECK(test_wifi_module.ConnectToServer(kServer, 1000,
          Esp8266::kReadTimeout, Esp8266::ConnectionType::kUdp) == false);
    Verify(
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds))).Exactly(2);
  }

  SECTION("Disconnect From Server")
  {
    When(ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
      std::chrono::microseconds)))
      .Return(Status::kSuccess, Status::kSuccess, Status::kTimedOut);
    test_wifi_module.DisconnectFromServer();
    Verify(
      // Checks to see that uart Write is called with the "disconnect from
      // server" command
      ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
        .Using(Esp8266::kDisconnectFromServerCommand,
        (sizeof(Esp8266::kDisconnectFromServerCommand)-1)) +
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }

  SECTION("Is Connected to Server")
  {
    static constexpr uint8_t kTest[] = "STATUS:7";
    // Used to read kTest to buffer when using uart read
    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      // Reads kTest to buffer and checks if timeout and size are correct
      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 68);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }

        count++;
      }
      // Checks FlushSerialBuffer has right timeout and size
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    // Checks to see if GetNetworkConnectionInfoCommand was written to the
    // esp8266
    CHECK(test_wifi_module.IsConnectedToAccessPoint() == true);
    Verify(
        ConstOverloadedMethod(mock_uart, Write, void(const uint8_t *, size_t))
            .Using(Esp8266::kGetNetworkConnectionInfoCommand,
              (sizeof(Esp8266::kGetNetworkConnectionInfoCommand)-1)));
  }

  SECTION("Send Get Request")
  {
    static constexpr uint8_t kCheckSendDataCommand[] = "AT+CIPSEND=39\r\n";
    static constexpr uint8_t kCheckGetRequest[] =
      "GET /test/ HTTP/1.1\r\n"
      "Host: test.com\r\n"
      "\r\n";
    static constexpr uint8_t kTest[] = "Test OK\r\n";
    static constexpr char kUrl[] = "test.com/test/";
    auto verify_write = [](const uint8_t * data, size_t size) {
      static uint8_t send_data_index = 0;
      static uint8_t get_request_index = 0;
      static uint8_t count = 0;

      if (count < (sizeof(kCheckSendDataCommand)-1))
      {
        CHECK(*data == kCheckSendDataCommand[send_data_index]);
        CHECK(size == 1);
        send_data_index++;
        count++;
      }
      else
      {
        CHECK(*data == kCheckGetRequest[get_request_index]);
        CHECK(size == 1);
        get_request_index++;
      }
    };

    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);

        data[0] = '>';

        count++;

        return Status::kSuccess;
      }
      else if (count == 1)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 9);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }
        count++;
      }
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(mock_uart, Write,
        void(const uint8_t *, size_t)))
      .AlwaysDo(verify_write);
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    uint8_t buffer[9];
    CHECK(test_wifi_module.SendGetRequest(kUrl,
          {.size=sizeof(buffer), .address=buffer}) == 0);
    for (int i = 0; i < 9; i++)
    {
      CHECK(buffer[i] == kTest[i]);
    }
  }

  SECTION("Send Post Request")
  {
    static constexpr uint8_t kCheckSendDataCommand[] = "AT+CIPSEND=119\r\n";
    static constexpr uint8_t kCheckPostRequest[] =
      "POST /test/ HTTP/1.1\r\n"
      "Host: test.com\r\n"
      "Content-Type: application/x-www-form-urlencoded\r\n"
      "Content-Length: 7\r\n"
      "\r\n"
      "test=OK\r\n"
      "\r\n";
    static constexpr uint8_t kTest[] = "Test OK\r\n";
    static constexpr char kUrl[] = "test.com/test/";
    static constexpr char kPayload[] = "test=OK";
    auto verify_write = [](const uint8_t * data, size_t size) {
      static uint8_t send_data_index = 0;
      static uint8_t post_request_index = 0;
      static uint8_t count = 0;

      if (count < (sizeof(kCheckSendDataCommand)-1))
      {
        CHECK(*data == kCheckSendDataCommand[send_data_index]);
        CHECK(size == 1);
        send_data_index++;
        count++;
      }
      else
      {
        CHECK(*data == kCheckPostRequest[post_request_index]);
        CHECK(size == 1);
        post_request_index++;
      }
    };

    auto uart_response =
        [](uint8_t * data,
           size_t size,
           std::chrono::microseconds timeout) -> Status {
      static uint8_t count = 0;

      if (count == 0)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);

        data[0] = '>';

        count++;

        return Status::kSuccess;
      }
      else if (count == 1)
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 9);

        for (size_t i = 0; i < (sizeof(kTest)-1); i++)
        {
          data[i] = kTest[i];
        }
        count++;
      }
      else
      {
        CHECK(timeout == Esp8266::kReadTimeout);
        CHECK(size == 1);
      }

      return Status::kTimedOut;
    };
    When(ConstOverloadedMethod(mock_uart, Write,
        void(const uint8_t *, size_t)))
      .AlwaysDo(verify_write);
    When(ConstOverloadedMethod(
             mock_uart,
             Read,
             Status(uint8_t *, size_t, std::chrono::microseconds)))
        .AlwaysDo(uart_response);
    uint8_t buffer[9];
    CHECK(test_wifi_module.SendPostRequest(kUrl, kPayload,
          {.size=sizeof(buffer), .address=buffer}) == 0);
    for (int i = 0; i < 9; i++)
    {
      CHECK(buffer[i] == kTest[i]);
    }
  }
}
}  // namespace sjsu
