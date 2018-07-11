/*
 * UART1.hpp
 *
 *  Created on: Jul 5, 2018
 *      Author: Jeremy
 */
#ifndef UART_1_HPP_
#define UART_1_HPP_

#include "UART_Base.hpp" //Base class

class UART1: public UART_Base
{
	public:
		bool init(unsigned int baud, uint32_t queue_size = 16);
	private:
		UART1();
		~UART1();
}


#endif /* UART_1_HPP_ */