extern "C"
{
	#include "CAN.h"
}
#include <unistd.h>
#include "gtest/gtest.h"
#include <thread>

using namespace std;

// Register definitions
AFIO_Mock* AFIO  = new AFIO_Mock;
RCC_Mock*  RCC   = new RCC_Mock;
GPIO_Mock* GPIOB = new GPIO_Mock;
CAN_Mock*  CAN1  = new CAN_Mock;

// Message structs
CAN_msg_t CAN_rx_msg;
CAN_msg_t CAN_tx_msg;

// Hardware mocks are not used in this test, since there is 
// no point in having an object populate the CAN registers
// based on the contents of a CAN struct, just to test functions
// that perform the exact instructions but in reverse. Instead,
//  expected register values will be written directly to the registers.

/**
 * Test the CANReceive() function.
 */
TEST(CAN, Receive)
{
	// Test with ID: 0, payload {0,0,0,0,0,0,0,0}, length: 8
	CAN1->sFIFOMailBox[0].RIR  = 0x00000000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000008;
	CAN1->sFIFOMailBox[0].RDLR = 0x00000000;
	CAN1->sFIFOMailBox[0].RDHR = 0x00000000;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 0);
	EXPECT_EQ(CAN_rx_msg.data[0], 0);
	EXPECT_EQ(CAN_rx_msg.data[1], 0);
	EXPECT_EQ(CAN_rx_msg.data[2], 0);
	EXPECT_EQ(CAN_rx_msg.data[3], 0);
	EXPECT_EQ(CAN_rx_msg.data[4], 0);
	EXPECT_EQ(CAN_rx_msg.data[5], 0);
	EXPECT_EQ(CAN_rx_msg.data[6], 0);
	EXPECT_EQ(CAN_rx_msg.data[7], 0);
	EXPECT_EQ(CAN_rx_msg.len    , 8);
	
	// Test with ID: 1, payload {AA,BB,CC,DD}, length: 4
	CAN1->sFIFOMailBox[0].RIR  = 0x00200000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000004;
	CAN1->sFIFOMailBox[0].RDLR = 0xDDCCBBAA;
	CAN1->sFIFOMailBox[0].RDHR = 0x00000000;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 1);
	EXPECT_EQ(CAN_rx_msg.data[0], 0xAA);
	EXPECT_EQ(CAN_rx_msg.data[1], 0xBB);
	EXPECT_EQ(CAN_rx_msg.data[2], 0xCC);
	EXPECT_EQ(CAN_rx_msg.data[3], 0xDD);
	EXPECT_EQ(CAN_rx_msg.len    , 4);
	
	// Test with ID: 3, payload {1,0,3,0,5,0,7,0}, length: 8
	CAN1->sFIFOMailBox[0].RIR  = 0x00600000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000008;
	CAN1->sFIFOMailBox[0].RDLR = 0x00030001;
	CAN1->sFIFOMailBox[0].RDHR = 0x00070005;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 3);
	EXPECT_EQ(CAN_rx_msg.data[0], 1);
	EXPECT_EQ(CAN_rx_msg.data[1], 0);
	EXPECT_EQ(CAN_rx_msg.data[2], 3);
	EXPECT_EQ(CAN_rx_msg.data[3], 0);
	EXPECT_EQ(CAN_rx_msg.data[4], 5);
	EXPECT_EQ(CAN_rx_msg.data[5], 0);
	EXPECT_EQ(CAN_rx_msg.data[6], 7);
	EXPECT_EQ(CAN_rx_msg.data[7], 0);
	EXPECT_EQ(CAN_rx_msg.len    , 8);
	
	// Test with ID: 4, payload {0,2,0,4,0,6,0,8}, length: 8
	CAN1->sFIFOMailBox[0].RIR  = 0x00800000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000008;
	CAN1->sFIFOMailBox[0].RDLR = 0x04000200;
	CAN1->sFIFOMailBox[0].RDHR = 0x08000600;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 4);
	EXPECT_EQ(CAN_rx_msg.data[0], 0);
	EXPECT_EQ(CAN_rx_msg.data[1], 2);
	EXPECT_EQ(CAN_rx_msg.data[2], 0);
	EXPECT_EQ(CAN_rx_msg.data[3], 4);
	EXPECT_EQ(CAN_rx_msg.data[4], 0);
	EXPECT_EQ(CAN_rx_msg.data[5], 6);
	EXPECT_EQ(CAN_rx_msg.data[6], 0);
	EXPECT_EQ(CAN_rx_msg.data[7], 8);
	EXPECT_EQ(CAN_rx_msg.len    , 8);
	
	// Test with ID: 240, payload {01,23,45,67,89,AB,CD,EF}, length: 8
	CAN1->sFIFOMailBox[0].RIR  = 0x1E000000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000008;
	CAN1->sFIFOMailBox[0].RDLR = 0x67452301;
	CAN1->sFIFOMailBox[0].RDHR = 0xEFCDAB89;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 0xF0);
	EXPECT_EQ(CAN_rx_msg.data[0], 0x01);
	EXPECT_EQ(CAN_rx_msg.data[1], 0x23);
	EXPECT_EQ(CAN_rx_msg.data[2], 0x45);
	EXPECT_EQ(CAN_rx_msg.data[3], 0x67);
	EXPECT_EQ(CAN_rx_msg.data[4], 0x89);
	EXPECT_EQ(CAN_rx_msg.data[5], 0xAB);
	EXPECT_EQ(CAN_rx_msg.data[6], 0xCD);
	EXPECT_EQ(CAN_rx_msg.data[7], 0xEF);
	EXPECT_EQ(CAN_rx_msg.len    , 8);
	
	// Test with ID: 2047, payload {FF,FF,FF,FF,FF,FF,FF,FF}, length: 8
	CAN1->sFIFOMailBox[0].RIR  = 0xFFE00000;
	CAN1->sFIFOMailBox[0].RDTR = 0x00000008;
	CAN1->sFIFOMailBox[0].RDLR = 0xFFFFFFFF;
	CAN1->sFIFOMailBox[0].RDHR = 0xFFFFFFFF;
	
	CANReceive(&CAN_rx_msg);
	
	EXPECT_EQ(CAN_rx_msg.id     , 0x7FF);
	EXPECT_EQ(CAN_rx_msg.data[0], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[1], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[2], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[3], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[4], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[5], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[6], 0xFF);
	EXPECT_EQ(CAN_rx_msg.data[7], 0xFF);
	EXPECT_EQ(CAN_rx_msg.len    , 8);
}

/**
 * Test the CANSend() function.
 */
TEST(CAN, Send)
{
	CAN1->sTxMailBox[0].TIR = 0x0;
	
	// Test with ID: 0, payload {0,0,0,0,0,0,0,0}, length: 8
	CAN_tx_msg.id      = 0;
	CAN_tx_msg.data[0] = 0;
	CAN_tx_msg.data[1] = 0;
	CAN_tx_msg.data[2] = 0;
	CAN_tx_msg.data[3] = 0;
	CAN_tx_msg.data[4] = 0;
	CAN_tx_msg.data[5] = 0;
	CAN_tx_msg.data[6] = 0;
	CAN_tx_msg.data[7] = 0;
	CAN_tx_msg.len     = 8;
	
	std::thread t([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t.detach();
	
	CANSend(&CAN_tx_msg);
	
	EXPECT_EQ(CAN1->sTxMailBox[0].TIR , 0x00000000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDTR, 0x00000008);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDLR, 0x00000000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDHR, 0x00000000);

	// Test with ID: 1, payload {AA,BB,CC,DD}, length: 4
	CAN_tx_msg.id      = 1;
	CAN_tx_msg.data[0] = 0xAA;
	CAN_tx_msg.data[1] = 0xBB;
	CAN_tx_msg.data[2] = 0xCC;
	CAN_tx_msg.data[3] = 0xDD;
	CAN_tx_msg.len     = 4;
	
	std::thread t0([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t0.detach();
	
	CANSend(&CAN_tx_msg);
	
	EXPECT_EQ(CAN1->sTxMailBox[0].TIR , 0x00200000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDTR, 0x00000004);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDLR, 0xDDCCBBAA);

	// Test with ID: 3, payload {1,0,3,0,5,0,7,0}, length: 8
	CAN_tx_msg.id      = 3;
	CAN_tx_msg.data[0] = 1;
	CAN_tx_msg.data[1] = 0;
	CAN_tx_msg.data[2] = 3;
	CAN_tx_msg.data[3] = 0;
	CAN_tx_msg.data[4] = 5;
	CAN_tx_msg.data[5] = 0;
	CAN_tx_msg.data[6] = 7;
	CAN_tx_msg.data[7] = 0;
	CAN_tx_msg.len     = 8;

	std::thread t1([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t1.detach();
	
	CANSend(&CAN_tx_msg);
	
	EXPECT_EQ(CAN1->sTxMailBox[0].TIR , 0x00600000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDTR, 0x00000008);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDLR, 0x00030001);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDHR, 0x00070005);
	
	// Test with ID: 4, payload {0,2,0,4,0,6,0,8}, length: 8
	CAN_tx_msg.id      = 4;
	CAN_tx_msg.data[0] = 0;
	CAN_tx_msg.data[1] = 2;
	CAN_tx_msg.data[2] = 0;
	CAN_tx_msg.data[3] = 4;
	CAN_tx_msg.data[4] = 0;
	CAN_tx_msg.data[5] = 6;
	CAN_tx_msg.data[6] = 0;
	CAN_tx_msg.data[7] = 8;
	CAN_tx_msg.len     = 8;
	
	std::thread t2([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t2.detach();
	
	CANSend(&CAN_tx_msg);
	
	EXPECT_EQ(CAN1->sTxMailBox[0].TIR , 0x00800000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDTR, 0x00000008);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDLR, 0x04000200);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDHR, 0x08000600);

	// Test with ID: 240, payload {01,23,45,67,89,AB,CD,EF}, length: 8
	CAN_tx_msg.id      = 0xF0;
	CAN_tx_msg.data[0] = 0x01;
	CAN_tx_msg.data[1] = 0x23;
	CAN_tx_msg.data[2] = 0x45;
	CAN_tx_msg.data[3] = 0x67;
	CAN_tx_msg.data[4] = 0x89;
	CAN_tx_msg.data[5] = 0xAB;
	CAN_tx_msg.data[6] = 0xCD;
	CAN_tx_msg.data[7] = 0xEF;
	CAN_tx_msg.len     = 8;
	
	std::thread t3([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t3.detach();
	
	CANSend(&CAN_tx_msg);
	
	EXPECT_EQ(CAN1->sTxMailBox[0].TIR , 0x1E000000);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDTR, 0x00000008);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDLR, 0x67452301);
	EXPECT_EQ(CAN1->sTxMailBox[0].TDHR, 0xEFCDAB89);

	// Test with ID: 2047, payload {FF,FF,FF,FF,FF,FF,FF,FF}, length: 8
	CAN_tx_msg.id      = 0x7FF;
	CAN_tx_msg.data[0] = 0xFF;
	CAN_tx_msg.data[1] = 0xFF;
	CAN_tx_msg.data[2] = 0xFF;
	CAN_tx_msg.data[3] = 0xFF;
	CAN_tx_msg.data[4] = 0xFF;
	CAN_tx_msg.data[5] = 0xFF;
	CAN_tx_msg.data[6] = 0xFF;
	CAN_tx_msg.data[7] = 0xFF;
	CAN_tx_msg.len     = 8;
	
	std::thread t4([] {
		while(!(CAN1->sTxMailBox[0].TIR & 0x1UL));
		CAN1->sTxMailBox[0].TIR &= ~(0x1UL);
	});
	t4.detach();
	
	CANReceive(&CAN_tx_msg);
	
	CAN1->sTxMailBox[0].TIR  = 0xFFE00000;
	CAN1->sTxMailBox[0].TDTR = 0x00000008;
	CAN1->sTxMailBox[0].TDLR = 0xFFFFFFFF;
	CAN1->sTxMailBox[0].TDHR = 0xFFFFFFFF;
}