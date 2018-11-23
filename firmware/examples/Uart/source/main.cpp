#include <project_config.hpp>

#include <cstdio>
#include <iterator>

#include "L1_Drivers/uart.hpp"
#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"

int main(void)
{
    // This example code uses a loopback test Please connect the relevant TxD
    // and RxD pins together. For this example please connect the TxD2 and RxD2
    // pins together.
    Uart uart2(Uart::Channels::kUart2);
    // Calls the initialize function and checks if the
    // function fails due to wrong input.
    uart2.Initialize(38400);
    // While loop to constantly send the array of data
    while (true)
    {
        char message[] = "Hello World!";
        // This first loop sends all the data within the array
        for (size_t i = 0; message[i] != '\0'; i++)
        {
          uart2.Send(message[i]);
        }
        // This second loop receives all the sent data and prints it out
        for (size_t i = 0; i < std::size(message)-1; i++)
        {
            char receive = uart2.Receive();
            putchar(receive);
        }
        putchar('\n');
        Delay(1000);
    }
    return 0;
}
