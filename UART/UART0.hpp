/*
 * UART0.hpp
 *
 *  Created on: Jun 25, 2018
 *      Author: Jeremy
 */
#ifndef UART_0_HPP_
#define UART_0_HPP_

#include "UART_Base.hpp" //Base class

class UART0: public UART_Base
{
	public:
		bool init(unsigned int baud, uint32_t queue_size = 16);
	private:
		UART0();
		~UART0();
}


#endif /* UART_0_HPP_ */