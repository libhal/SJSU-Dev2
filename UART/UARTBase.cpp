#include "UARTBase.hpp"

void UART_Base::setBaud(unsigned int baudrate)
{
	uint32_t pclk = get_PCLK();

	uint32_t baud = (pclk/(16*baudrate)) + 0.5;
	// Added the 0.5 to ensure rounding is done right as numbers like 
	// 0.9 would get round down to 0 instead of up to 1
	UARTBaseReg -> LCR = (1 << 7); //DLAB Enable
	UARTBaseReg -> DLM = (baud >> 8);
	UARTBaseReg -> DLL = baud;
	UARTBaseReg -> LCR = 7; //DLAB Disable, 8 bit package, 2 stop bits
}
bool UART_Base::init(uint32_t baud, uint32_t queue_size)
{
	
	//Power up and enable the appropriate UART and NVIC Interrupt
	if(LPC_UART0_BASE == (unsigned int) UARTBaseReg)
	{
		LPC_SC -> PCONP |= (1 << 3);
		NVIC_EnableIRQ(UART0_IRQn);
	}
	else if(LPC_UART2_BASE == (unsigned int) UARTBaseReg)
	{
		LPC_SC -> PCONP |= (1 << 24);
		NVIC_EnableIRQ(UART2_IRQn);
	}
	else if(LPC_UART3_BASE == (unsigned int) UARTBaseReg)
	{
		LPC_SC -> PCONP |= (1 << 25);
		NVIC_EnableIRQ(UART3_IRQn);
	}
	else
		return false;

	setBaud(baud);//User defined baudrate

	//Enable FIFO and set Rx trigger level to have an 8 char timeout
	UARTBaseReg -> FCR = (1 << 0) | (0x2 << 6);
	//Reset Rx and Tx FIFO
	UARTBaseReg -> FCR |= (1 << 1) | (1 << 2);
	
	//Enable RBR, THRE. and Rx Line Interrupts
	UARTBaseReg -> IER &= ~(7 << 0);
	UARTBaseReg -> IER |= (7 << 0);

	if(!Rx) Rx = xQueueCreate(queue_size, sizeof(char));
}
void UART_Base::IntHandle()
{
	char data;
	while(!UARTBaseReg -> LSR & (1 <<0));
	data = UARTBaseReg -> RBR;
	if(!QueueSendFromISR(Rx, &data, NULL))
		uart0_puts("Failed to send");
}
void UART_Base::send(char out)
{
	UARTBaseReg -> THR = out;
    while(UARTBaseReg -> LSR & (1 << 5));
}
bool UART_Base::receive(char* CharInput, unsigned int time_limit)
{
	if(!CharInput || !Rx)
		return false;
	else
		while(!QueueReceive(Rx, &CharInput, time_limit));
	return true;
}