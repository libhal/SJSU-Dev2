/*
 * UART2.hpp
 *
 *  Created on: Jun 30, 2018
 *      Author: Jeremy
 */
#ifndef UART_2_HPP_
#define UART_2_HPP_

#include "UART_Base.hpp" //Base class

class UART2: public UART_Base
{
	public:
		bool init(unsigned int baud, uint32_t queue_size = 16);
	private:
		UART2();
		~UART2();
}


#endif /* UART_2_HPP_ */