// #include "L0_LowLevel/LPC40xx.h"
// #include "L0_LowLevel/SystemFiles/system_LPC407x_8x_177x_8x.h"
// #include "L1_Drivers/uart.hpp"
// #include "L5_Testing/testing_frameworks.hpp"

// TEST_CASE("Testing Uart", "[Uart]")
// {
//     // Simulated local version of LPC_UART2 to verify registers
//     LPC_UART2_TypeDef local_uart;

//     Uart test(2);
//     test.Initialize(38400);

//     SECTION("Initialization")
//     {
//         // Checking the FCR register that it is properly set
//         // It enables the FIFO and resets the Tx and Rx FIFO
//         // Check page 501 of the user manual for register description
//         constexpr uint8_t FIFOEnableAndReset = 0b111;

//         CHECK(FIFOEnableandReset == (local_uart -> FCR));
        
//     }
//     SECTION("Baud Rate")
//     {
//         // Standard UART is 8 bit packages, with 1 stop bit and no parity enabled
//         // Check page 502 of the user manual for regiser description
//         constexpr uint8_t StandardUart = 0b011;

//         // Divisor values to be checked
//         uint32_t div =
//             static_cast<uint32_t>(OSC_CLK / (16.0f * baudrate) + 0.5f);
//         uint8_t high = static_cast<uint8_t>(div >> 8);
//         uint8_t low = static_cast<uint8_t>(div >> 0);

//         // Unlock DLAB to check baud rate registers
//         local_uart -> LCR |= (1 << 7);

//         CHECK(high == (local_uart -> DLM));
//         CHECK(low == (local_uart -> DLL));

//         local_uart -> LCR &= ~(1 << 7);

//         CHECK(StandardUart == (local_uart -> LCR));
//     }
//     SECTION("Send")
//     {
//         // We are sending data through the send function
//         // We are gonna test whether or not the data makes it into
//         // the THR register that outputs data onto the Tx pin
//         constexpr uint8_t SendData = 0b1000;
//         test.Send(8);
//         CHECK(SendData == (local_uart -> THR));
//     }
//     SECTION("Recieve")
//     {
//         constexpr uint8_t Data = 0b101;

//         local_uart -> RBR = Data;
//         uint8_t Din = test.Recieve();

//         CHECK(Data == Din);
//     }
// }
