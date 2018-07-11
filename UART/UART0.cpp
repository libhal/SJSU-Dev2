#include "UART0.hpp"

bool UART0::init(unsigned int baud, uint32_t queue_size)
{
	//Configure Pins for UART 0
	//Can use P0.2 and P0.3
	//2 for Tx and 3 for Rx
		
	//Reset then set
	LPC_IOCON -> P0_2 &= ~(7 << 0);
	LPC_IOCON -> P0_3 &= ~(7 << 0);

	LPC_IOCON -> P0_2 |= (4 << 0);
	LPC_IOCON -> P0_3 |= (4 << 0);
	return UART_Base::init(baud, queue_size);
}

UART0::UART0() : UART_Base((unsigned int*) LPC_UART0_BASE)
{
	//Handing off LPC_UART0_BASE address to parent class
}
UART0::~UART0()
{
	//Nothing for now
}