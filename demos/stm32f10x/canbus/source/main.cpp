#include <cstdint>

#include "platforms/targets/stm32f10x/stm32f10x.h"

#include "utility/log.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
namespace stm32f10x
{

enum BITRATE{CAN_50KBPS, 
             CAN_100KBPS, 
             CAN_125KBPS, 
             CAN_250KBPS, 
             CAN_500KBPS, 
             CAN_1000KBPS};

typedef struct
{
	uint16_t id;
	uint8_t  data[8];
	uint8_t  len;
} CAN_msg_t;

typedef const struct
{
	uint8_t TS2;
	uint8_t TS1;
	uint8_t BRP;
} CAN_bit_timing_t;

CAN_bit_timing_t CAN_bit_timing[6] = 
  {{2, 13, 45}, 
   {2, 15, 20}, 
   {2, 13, 18}, 
   {2, 13, 9}, 
   {2, 15, 4}, 
   {2, 15, 2}};

void CANInit(BITRATE bitrate)
{
	 sjsu::LogInfo("Starting Initialization"); 
	// Enable CAN clock (CAN = b'25)
	RCC->APB1ENR |= (1 << 25);

	// Enable GPIOB clock (GPIOB = b'3)
	RCC->APB2ENR |= (1 << 3);

	// Enable AFIO clock (AFIO = b'0)
	RCC->APB2ENR |= (1 << 0);

	// Reset CAN remap (CAN_REMAP = b'14:13)
	// Set CAN remap (CAN_REMAP = b'14:13) 
		// b'00 = Rx:PA11 Tx:PA12 
		// b'10 = Rx:PB8  Tx:PB9
		// b'11 = Rx:PD0  Tx:PD1
	AFIO->MAPR   &= ~((1 << 13) | (1 << 14));
	AFIO->MAPR   |= (1 << 14);   	

	// Set Pin Configuration CRH[3:0]
		// MODE[1:0]
			// 00: Input Mode
			// 01: Output Mode 10MHz
			// 10: Output Mode 2MHz
			// 11: Output Mode 50MHz
		// CNF[3:2]
			// Input Mode
				// 00 Analog Mode
				// 01 Floating Input
				// 10 Input with pull-up / pull-down
				// 11 Reserved
			// Output Mode
				// 00 General Purpose Output push-pull
				// 01 General Purpose Output Open-Drain
				// 10 Alternate Function Push-Pull
				// 11 Alternate Function Open-Drain

	// Clear Bits Pins PB9 & PB8 [7:0]
	GPIOB->CRH   &= ~(0xFFUL);

	// Set PB8 as Rx Input with pull-up / pull-down CRH[3:0]
	// Set PB8 with a pull up resistor. ODR[8]
		// 0 Pull-Down
		// 1 Pull-Up
	GPIOB->CRH   |= (0b1000 << 3);
	GPIOB->ODR   |= (1 << 8);

	// Set PB9 as Tx Output Mode 50Mz with Alternate Function Push/Pull CRH[7:4]
	GPIOB->CRH   |= (0b1011 << 4);
	  
	// Set CAN to initialization mode
		// Bit 0 - IRQN Initialization request
		// Bit 1 - SLEEP Sleep Mode Request
		// Bit 2 - TXFP Transmit FIFO Priority
		// Bit 3 - RFLM Recieve FIFO Locked Mode
		// Bit 4 - NART No Automatic retransmission
		// Bit 5 - AWUM Automatic Wakeup Mode
		// Bit 6 - ABOM Automatic bus-off management
		// Bit 7 - ABOM Automatic Bus-Off Management
		// Bit 15 - RESET bxCAN Sotware Master Reset
		// Bit 16 - DBF Debug Freeze
	// Clear all Bits
	CAN1->MCR &= ~((0b11 << 15) | 0b11111111);
	// Set IRQN NART ABOM
  	CAN1->MCR = 0x51UL;			    

	// Set the Canbus Bit Timings
		// BPR [9:0] Baud rate Pre-scale
		// TS1 [19:16] Time Segment 1
		// TS2 [22:20] Time Segment 2
		// SJW [25:24] Resynchronized Jump Width
		// LBKM[30]    Loop back Mode
		// SLIM [31]   Silent Mode (debug)
	// Clear all bits
	CAN1->BTR &=  ~((0x1FF) |
					(0x0F << 16) | 
					(0x07 << 20) | 
					(0x03 << 24) |
					(0x01 << 30) | 
					(0x01 << 31)); 

	// Set BPR TS1 TS2 LBKM
	CAN1->BTR |=  ((CAN_bit_timing[bitrate].BRP-1) & 0x1FF) |
				  (((CAN_bit_timing[bitrate].TS1-1) & 0x0F) << 16) | 
				  (((CAN_bit_timing[bitrate].TS2-1) & 0x07) << 20) |
				  (0x01 << 30);

// Set Filters to accept all messages by seting filter 1 mask to 0x00

	// Registers
		// FMR (Filter Mask Register)
			// [0] Filter init Mode
		// FM1R (Filter Mode Register)
			// [27:0] 0: Mask Mode | 1: List mode
		// FS1R (Filter Scale Register)
			// [27:0] 0: Dual 16-bit scale | 1: Single 32-bit scale
		// FFA1R (FIFO Assignment Register)
			// [27:0] 0: Assign FIFO 0 | 1: Assign FIFO 1
		// FA1R (Filter Activation Register)
			// [27:0] 0: Filter Not Active | 1: Filter Active 
	 sjsu::LogInfo("Starting Filter Configuration"); 

	// Configure Filters to default values
	// 8 - 12 as 11 1000
	// Assign all filters to CAN1
	CAN1->FM1R |= 0x1C << 8;              

	// Set to filter initialization mode
	CAN1->FMR  |=   0x1UL;   
				
	// Clear bit 0 to deactivate filter 0
	CAN1->FA1R &= ~(0x1UL);            

	// Set bit 0 to set filter 0 to single 32 bit configuration
	CAN1->FS1R |=   0x1UL;               

	// Clear filters registers to 0
	CAN1->sFilterRegister[0].FR1 = 0x0UL; 
	CAN1->sFilterRegister[0].FR2 = 0x0UL;

	// Set filter to mask mode
	CAN1->FM1R &= ~(0x1UL);               

	// Clear bit 0 to assign filter 0 to FIFO 0
	CAN1->FFA1R &= ~(0x1UL);			   

	// Activate filter 0       
	CAN1->FA1R  |=   0x1UL;          

	// Deactivate initialization mode
	CAN1->FMR   &= ~(0x1UL);			
	// Set CAN to normal mode         
	CAN1->MCR   &= ~(0x1UL);              

	// Wait for CAN to leave initialization mode
	while (!(CAN1->MSR & 0x1UL)); 

	sjsu::LogInfo("Initialization Complete"); 
}

 void CANSend(CAN_msg_t* CAN_tx_msg)
 {
	volatile int count = 0;
	 
	CAN1->sTxMailBox[0].TIR   = (CAN_tx_msg->id) << 21;
	
	CAN1->sTxMailBox[0].TDTR &= ~(0xF);
	CAN1->sTxMailBox[0].TDTR |= CAN_tx_msg->len & 0xFUL;
	
	CAN1->sTxMailBox[0].TDLR  = 
                (static_cast<uint32_t>(CAN_tx_msg->data[3]) << 24) |
								(static_cast<uint32_t>(CAN_tx_msg->data[2]) << 16) |
								(static_cast<uint32_t>(CAN_tx_msg->data[1]) <<  8) |
								(static_cast<uint32_t>(CAN_tx_msg->data[0]) );
	CAN1->sTxMailBox[0].TDHR  = 
                (static_cast<uint32_t>(CAN_tx_msg->data[7]) << 24) |
								(static_cast<uint32_t>(CAN_tx_msg->data[6]) << 16) |
								(static_cast<uint32_t>(CAN_tx_msg->data[5]) <<  8) |
								(static_cast<uint32_t>(CAN_tx_msg->data[4]) );

	CAN1->sTxMailBox[0].TIR  |= 0x1UL;
	while(CAN1->sTxMailBox[0].TIR & 0x1UL && count++ < 1000000);
	 
	 if (!(CAN1->sTxMailBox[0].TIR & 0x1UL)) return;
	 
	 //Sends error log to screen
	//  while (CAN1->sTxMailBox[0].TIR & 0x1UL)
	//  {
	// 	 SendInt(CAN1->ESR);
	// 	 SendLine();
	// 	 SendInt(CAN1->MSR);
	// 	 SendLine();
	// 	 SendInt(CAN1->TSR);
	// 	 SendLine();
	// 	 SendLine();
	//  }
 }

  void CANReceive(CAN_msg_t* CAN_rx_msg)
 {
	CAN_rx_msg->id  = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FFUL;
	CAN_rx_msg->len = (CAN1->sFIFOMailBox[0].RDTR) & 0xFUL;
	
	CAN_rx_msg->data[0] = 0xFFUL &  CAN1->sFIFOMailBox[0].RDLR;
	CAN_rx_msg->data[1] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 8);
	CAN_rx_msg->data[2] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 16);
	CAN_rx_msg->data[3] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDLR >> 24);
	CAN_rx_msg->data[4] = 0xFFUL &  CAN1->sFIFOMailBox[0].RDHR;
	CAN_rx_msg->data[5] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 8);
	CAN_rx_msg->data[6] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 16);
	CAN_rx_msg->data[7] = 0xFFUL & (CAN1->sFIFOMailBox[0].RDHR >> 24);
	
	CAN1->RF0R |= 0x20UL;
 }
 

  uint8_t CANMsgAvail(void)
 {
	 return CAN1->RF0R & 0x3UL;
}

void CANSetFilter(uint16_t id)
 {
	 static uint32_t filterID = 0;
	 
	 if (filterID == 112)
	 {
		 return;
	 }
	 
	 CAN1->FMR  |=   0x1UL;                // Set to filter initialization mode
	 
	 switch(filterID%4)
	 {
		 case 0:
				// if we need another filter bank, initialize it
				CAN1->FA1R |= 0x1UL <<(filterID/4);
				CAN1->FM1R |= 0x1UL << (filterID/4);
		    CAN1->FS1R &= ~(0x1UL << (filterID/4)); 
				
				CAN1->sFilterRegister[filterID/4].FR1 = (id << 5) | (id << 21);
		    CAN1->sFilterRegister[filterID/4].FR2 = (id << 5) | (id << 21);
				break;
		 case 1:
			  CAN1->sFilterRegister[filterID/4].FR1 &= 0x0000FFFF;
				CAN1->sFilterRegister[filterID/4].FR1 |= id << 21;
			  break;
		 case 2:
				CAN1->sFilterRegister[filterID/4].FR2 = (id << 5) | (id << 21);
		    break;
		 case 3:
			  CAN1->sFilterRegister[filterID/4].FR2 &= 0x0000FFFF;
				CAN1->sFilterRegister[filterID/4].FR2 |= id << 21;
				break;
	 }
	 filterID++;
	 CAN1->FMR   &= ~(0x1UL);			  // Deactivate initialization mode
 }

// static void filterInit (int num, int id =0, int mask =0) 
// {
//   CAN1->FA1R &= ~(1 << num); // Disable filter
//   CAN1->FS1R |= (1 << num);  // Set single 32 bit register

//     MMIO32(fr1 + 8 * num) = id;
//     MMIO32(fr2 + 8 * num) = mask;

//   CAN1->FA1R |= (1 << num); // Enable Filter
// }

} // stm32f10x
} // sjsu

int main()
{

using namespace sjsu;
using namespace stm32f10x;
  sjsu::LogInfo("Starting Hello World Application");
  CAN_msg_t CAN_rx_msg;  // Holds receiving CAN messages
  CAN_msg_t CAN_tx_msg;  // Holds transmitted CAN messagess

  CANInit(CAN_1000KBPS);
//   CANSetFilter(0x01);

  while (true)
  {


  sjsu::LogInfo("Sending Message"); 
	CAN_tx_msg.id      = 0;
	CAN_tx_msg.data[0] = 0xAa;
	CAN_tx_msg.data[1] = 0xBb;
	CAN_tx_msg.data[2] = 0xCc;
	CAN_tx_msg.data[3] = 0xDd;
	CAN_tx_msg.data[4] = 0xEe;
	CAN_tx_msg.data[5] = 0xFf;
	CAN_tx_msg.data[6] = 0xAb;
	CAN_tx_msg.data[7] = 0xAc;
	CAN_tx_msg.len     = 8;

  // while(!(CAN1->sTxMailBox[0].TIR & 0x1UL))
  // {}
  // CAN1->sTxMailBox[0].TIR &= ~(0x1UL);

  	CANSend(&CAN_tx_msg);


    for (uint8_t i = 0; i < 20; i++)
    {
      sjsu::LogInfo("Waiting for message");
      sjsu::Delay(250ms);

      if(CANMsgAvail())
      {
        CANReceive(&CAN_rx_msg);
        sjsu::LogInfo("%#04x %#04x %#04x %#04x %#04x %#04x %#04x %#04x\n",
          CAN_rx_msg.data[0],
          CAN_rx_msg.data[1],
          CAN_rx_msg.data[2],
          CAN_rx_msg.data[3],
		  CAN_rx_msg.data[4],
		  CAN_rx_msg.data[5],
		  CAN_rx_msg.data[6],
		  CAN_rx_msg.data[7]);
        break;
      }

    }

    sjsu::Delay(1000ms);
  }
  return -1;
}


