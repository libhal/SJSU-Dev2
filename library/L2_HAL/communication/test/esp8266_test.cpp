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
        .Using(Esp8266::kResetCommand, sizeof(Esp8266::kResetCommand)) +
      // Checks if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }
  SECTION("Initialize")
  {
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
  }
  SECTION("Get Network Connection")
  {
  }
  SECTION("Connect to Access Point")
  {
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
        sizeof(Esp8266::kDisconnectFromAccessPointCommand)) +
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }
  SECTION("Is Connected to Access Point")
  {
  }
  SECTION("Connect to Server")
  {
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
        sizeof(Esp8266::kDisconnectFromServerCommand)) +
      // Checks to see if uart read was called once
      ConstOverloadedMethod(mock_uart, Read, Status(uint8_t *, size_t,
        std::chrono::microseconds)));
  }
  SECTION("Is Connected to Server")
  {
  }
  SECTION("Send Get Request")
  {
  }
  SECTION("Send Post Request")
  {
  }
}
}  // namespace sjsu
