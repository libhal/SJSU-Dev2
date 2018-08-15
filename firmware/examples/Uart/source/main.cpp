// #include <project_config.hpp>

// #include <cstdint>
// #include <cstdio>

// #include "L0_LowLevel/delay.hpp"
// #include "L0_LowLevel/LPC40xx.h"
// #include "L1_Drivers/uart.hpp"
// #include "L2_Utilities/debug_print.hpp"

// int main(void)
// {
//     // This example code uses a loopback test. Please connect the
//     // relevant TxD and RxD pins together. For this example please
//     // connect the TxD2 and RxD2 pins together

//     Uart test;
//     char receive;
//     char input2 [] = "Hello World";

//     // Calls the initialize function and checks if the
//     // function fails due to wrong input.
//     if (test.Initialize(38400, 2) == 0)
//     {
//         printf("Fail!");
//     }

//     // While loop to constantly send the array of data
//     while (1)
//     {
//         // This first loop sends all the data within the array
//         for (int i=0; input2[i] != '\0'; i++) test.Send(input2[i]);

//         // This second loop receives all the sent data and prints it out
//         for (int j=0; input2[j] != '\0'; j++)  // 12
//         {
//             receive = test.Receive();
//             printf("%c", receive);
//         }
//             printf("\n");
//         Delay(1000);
//     }
//     return 0;
// }
