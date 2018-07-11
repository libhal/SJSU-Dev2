#include "UART1.hpp"

bool UART1::init(unsigned int baud, uint32_t queue_size)
{
	//Configure Pins for UART 1
	//WIll defaultly use P2.0 - 2.8
	//0=Tx, 1=Rx, 2=CTS, 3=DCD, 4=DSR, 5=DTR, 6=RI, 7=RTS
	
	//For now the extra control flow functions aren't enabled
	//When minimum product has been completed they will 
	//be worked upon

	//Reset
	LPC_IOCON -> P2_0 &= ~(7 << 0);//Tx
	LPC_IOCON -> P2_1 &= ~(7 << 0);//Rx
	// LPC_IOCON -> P2_2 &= ~(7 << 0);//CTS
	// LPC_IOCON -> P2_3 &= ~(7 << 0);//DCD
	// LPC_IOCON -> P2_4 &= ~(7 << 0);//DSR
	// LPC_IOCON -> P2_5 &= ~(7 << 0);//DTR
	// LPC_IOCON -> P2_6 &= ~(7 << 0);//RI
	// LPC_IOCON -> P2_7 &= ~(7 << 0);//RTS

	//Set
	LPC_IOCON -> P2_0 |= (2 << 0);//Tx
	LPC_IOCON -> P2_1 |= (2 << 0);//Rx
	// LPC_IOCON -> P2_2 |= (2 << 0);//CTS
	// LPC_IOCON -> P2_3 |= (2 << 0);//DCD
	// LPC_IOCON -> P2_4 |= (2 << 0);//DSR
	// LPC_IOCON -> P2_5 |= (2 << 0);//DTR
	// LPC_IOCON -> P2_6 |= (2 << 0);//RI
	// LPC_IOCON -> P2_7 |= (2 << 0);//RTS
	
	return UART_Base::init(baud, queue_size);
}

UART1::UART1() : UART_Base((unsigned int*) LPC_UART1_BASE)
{
	//Handing off LPC_UART1_BASE address to parent class
}
UART1::~UART1()
{
	//Nothing for now
}