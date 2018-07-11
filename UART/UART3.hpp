/*
 * UART3.hpp
 *
 *  Created on: Jun 30, 2018
 *      Author: Jeremy
 */
#ifndef UART_3_HPP_
#define UART_3_HPP_

#include "UART_Base.hpp" //Base class

class UART3: public UART_Base
{
	public:
		bool init(unsigned int baud, uint32_t queue_size = 16);
	private:
		UART3();
		~UART3();
}


#endif /* UART_3_HPP_ */