#include "UART4.hpp"

bool UART4::init(unsigned int baud, uint32_t queue_size)
{
	//Configure Pins for UART 4
	//Can use P1.29 and P2.9
	//1.29 for Tx and 2.9 for Rx
		
	//Reset then set
	LPC_IOCON -> P1_29 &= ~(7 << 0);
	LPC_IOCON -> P2_9 &= ~(7 << 0);

	LPC_IOCON -> P1_29 |= (5 << 0);
	LPC_IOCON -> P2_9 |= (3 << 0);

	return UART_Base::init(baud, queue_size);
}

UART4::UART4() : UART_Base((unsigned int*) LPC_UART4_BASE)
{
	//Handing off LPC_UART4_BASE address to parent class
}
UART4::~UART4()
{
	//Nothing for now
}