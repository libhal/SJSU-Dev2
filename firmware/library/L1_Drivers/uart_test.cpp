// #include "L0_LowLevel/LPC40xx.h"
// #include "L0_LowLevel/SystemFiles/system_LPC407x_8x_177x_8x.h"
// #include "L1_Drivers/uart.hpp"
// #include "L5_Testing/testing_frameworks.hpp"

// TEST_CASE("Testing Uart", "[Uart]")
// {
//     // Char to be used in multiple tests
//     constexpr char T = 'T';

//     // Local object of LPC_UART_TypeDef to
//     // observe changes in registers of Uart1
//     LPC_UART1_TypeDef Local_Uart1

//     Uart test

//     // Calling the Initialization of UART 1 so it can be used for all tests
//     // Called to be at 9600 bps

//     test.Initialize(9600, 1);

//     SECTION("Initialize UART 1")
//     {
//         constexpr uint8_t DLAB_bit = 7;
//         constexpr uint32_t kFCR = 0b111;
//         constexpr uint32_t kDiv =
//             static_cast<uint32_t>(OSC_CLK / (16.0f * 9600.0f) + 0.5f);
//         constexpr uint8_t kDLM = static_cast<uint8_t>(kDiv >> 8);
//         constexpr uint8_t kDLL = static_cast<uint8_t>(kDiv >> 0);
//         constexpr uint32_t kLCR = 0b11;

//         // Check first 3 bits of FCR
//         CHECK(kFCR == ((Local_Uart1.FCR) & 0b111));

//         // Unlock DLAB
//         Local_Uart1.LCR |= (1 << DLAB_bit);

//         // Check DLM bits
//         CHECK(kDLM == Local_Uart1.DLM);

//         // Check DLL bits
//         CHECK(kDLL == Local_Uart1.DLL);

//         // Lock DLAB
//         Local_Uart1.LCR &= ~(1 << DLAB_bit);

//         CHECK(kLCR == (Local_Uart1.LCR) & 0b11);
//     }
//     SECTION("Sending Data")
//     {
//         constexpr uint8_t TER_bit = 7;
//         // Disable Tx enable so test can check bits in register
//         Local_Uart1.TER &= ~(1 << TER_bit);

//         // Send the letter T
//         test.Send('T');

//         // Check the transmit holding register if it is holding the letter T
//         CHECK(T == Local_Uart1.THR);

//         // Enable the FIFO to send the data
//         Local_Uart1.TER |= (1 << TER_bit);
//     }
//     SECTION("Reading Data")
//     {
//         char output;
//         // Inject a T char into the receive buffer
//         Local_Uart1.RBR = T;

//         // Check the output of the receive function
//         CHECK(T == (output = test.Receive()));
//     }
// }
