#include <cstdio>
#include <iterator>

#include "L1_Drivers/uart.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  LOG_INFO("Uart application starting.");
  LOG_INFO(
      "The aplication uses a loopback test. The test will be constantly "
      "sending data between the Tx and Rx pins."
      "Please connect pins P2.8 and P2.9 together using a wire for this "
      "test.");
  Uart uart2(Uart::Channels::kUart2);
  LOG_INFO(
      "Initializing UART port 2 with a baud rate of 38400 bits per second");
  uart2.Initialize(38400);
  while (true)
  {
    LOG_INFO(
        "A char array containing the characters 'Hello World' will"
        "now be sent from the Tx pin to the Rx pin");
    char message[] = "Hello World!";
    for (size_t i = 0; message[i] != '\0'; i++)
    {
      // This first loop sends all the data within the array
      LOG_INFO("Sending characters now.");
      uart2.Send(message[i]);
    }
    LOG_INFO("Recieving characters that arrived at Rx pin and printing them");
    // This second loop receives all the sent data and prints it out
    for (size_t i = 0; i < std::size(message) - 1; i++)
    {
      char receive = uart2.Receive();
      putchar(receive);
    }

    putchar('\n');
    Delay(1000);
  }
  return 0;
}
