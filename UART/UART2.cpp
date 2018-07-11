#include "UART2.hpp"

bool UART2::init(unsigned int baud, uint32_t queue_size)
{
	//Configure Pins for UART 2
	//Can use P2.8 and P2.9
	//8 for Tx and 9 for Rx
		
	//Reset then set
	LPC_IOCON -> P2_8 &= ~(7 << 0);
	LPC_IOCON -> P2_9 &= ~(7 << 0);

	LPC_IOCON -> P2_8 |= (2 << 0);
	LPC_IOCON -> P2_9 |= (2 << 0);
	return UART_Base::init(baud, queue_size);
}

UART2::UART2() : UART_Base((unsigned int*) LPC_UART2_BASE)
{
	//Handing off LPC_UART0_BASE address to parent class
}
UART2::~UART2()
{
	//Nothing for now
}