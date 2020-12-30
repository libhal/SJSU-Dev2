#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"

#include "utility/log.hpp"
#include "utility/time.hpp"

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
  // Enable CAN clock (CAN1 = b'25) (CAN2 = b'26)
	RCC->APB1ENR |= (1 << 25);
  
  // Enable AFIO clock (AFIO = b'0)
	RCC->APB2ENR |= (1 << 0);

  // reset CAN remap (CAN_REMAP = b'13-14)
  AFIO->MAPR   &= ~((1 << 13) | (1 << 14));

  // Set CAN remap (CAN_REMAP = b'13-14) to use PB8, PB9
  AFIO->MAPR   |= (1 << 14);   	

  // Enable GPIOB clock (GPIOB = b'3)
  RCC->APB2ENR |= (1 << 3);

  // Configure PB8 and PB9		
  // Clear Bits 0 - 7	
	GPIOB->CRH   &= ~(0xFFUL);

  // Set pin 8 as Input Mode (MODE = b'0 = 00)
  // Set Pin 8 Function as Input with Pull-up / pull-down (CNFy = b'2 = 11)
  // Set Pin 8 with a pull up resistor.
  GPIOB->CRH   |= (1 << 3);
	GPIOB->ODR   |= (1 << 8);

  // Set pin 9 as Output Mode at 50Mz (MODE = b'4 = 11)
  // Set pin 9 as Alternate Function Push/Pull (CNFy = b'6 = 10)
  GPIOB->CRH   |= (3 << 4);
  GPIOB->CRH   |= (1 << 7);
  
  // Set CAN to initialization mode
    // Bit 0 - IRQN Initialization request
	  // Bit 4 - NART No Automatic retransmission
	  // Bit 6 - ABOM Automatic bus-off management
  CAN1->MCR = 0x51UL;			    

// Clear the register
  // Bits 9:0 Baud rate Pre-scale
  // Bits 19:16 TS1 Time Segment 1
  // Bits 22:20 TS2 Time Segment 2
  // Bits 25:24 SJW Resynchronized Jump Width
  // Bits 30 LBKM Loop back Mode
  // Bits 31 SILM Silent Mode
  CAN1->BTR &= ~((0x03 << 24) | 
                 (0x07 << 20) | 
                 (0x0F << 16) | 
                 (0x1FF) |
                 (0x01 << 30)); 

	CAN1->BTR |=  (((CAN_bit_timing[bitrate].TS2-1) & 0x07) << 20) | 
                (((CAN_bit_timing[bitrate].TS1-1) & 0x0F) << 16) | 
                ((CAN_bit_timing[bitrate].BRP-1) & 0x1FF);
 
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
	while (CAN1->MSR & 0x1UL); 
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

static void filterInit (int num, int id =0, int mask =0) 
{
  CAN1->FA1R &= ~(1 << num); // Disable filter
  CAN1->FS1R |= (1 << num);  // Set single 32 bit register

    MMIO32(fr1 + 8 * num) = id;
    MMIO32(fr2 + 8 * num) = mask;

  CAN1->FA1R |= (1 << num); // Enable Filter
}


int main()
{
  sjsu::LogInfo("Starting Hello World Application");
  CAN_msg_t CAN_rx_msg;  // Holds receiving CAN messages
  CAN_msg_t CAN_tx_msg;  // Holds transmitted CAN messagess

  while (true)
  {


  sjsu::LogInfo("Sending Message"); 
	CAN_tx_msg.id      = 0;
	CAN_tx_msg.data[0] = 0xAA;
	CAN_tx_msg.data[1] = 0xBB;
	CAN_tx_msg.data[2] = 0xCC;
	CAN_tx_msg.data[3] = 0xDD;
	CAN_tx_msg.len     = 4;

  while(!(CAN1->sTxMailBox[0].TIR & 0x1UL))
  {}
  CAN1->sTxMailBox[0].TIR &= ~(0x1UL);

  CANSend(&CAN_tx_msg);


    for (uint8_t i = 0; i < 10; i++)
    {
      sjsu::LogInfo("Waiting for message");
      sjsu::Delay(100ms);

      if(CANMsgAvail())
      {
        CANReceive(&CAN_rx_msg);
        sjsu::LogInfo("%#04x\n %#04x\n %#04x\n %#04x\n",
          CAN_rx_msg.data[0],
          CAN_rx_msg.data[1],
          CAN_rx_msg.data[2],
          CAN_rx_msg.data[3]);
        break;
      }

    }

    sjsu::Delay(1000ms);
  }
  return -1;
}


} // stm32f10x
} // sjsu