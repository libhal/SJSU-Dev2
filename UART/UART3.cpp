#include "UART3.hpp"

bool UART3::init(unsigned int baud, uint32_t queue_size)
{
	//Configure Pins for UART 0
	//Can use P4.28 and P4.29
	//28 for Tx and 29 for Rx

	//Reset then set
	LPC_IOCON -> P4_28 &= ~(7 << 0);
	LPC_IOCON -> P4_29 &= ~(7 << 0);

	LPC_IOCON -> P4_28 |= (2 << 0);
	LPC_IOCON -> P4_29 |= (2 << 0);
	return UART_Base::init(baud, queue_size);
}

UART3::UART3() : UART_Base((unsigned int*) LPC_UART3_BASE)
{
	//Handing off LPC_UART0_BASE address to parent class
}
UART3::~UART3()
{
	//Nothing for now
}