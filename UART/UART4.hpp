/*
 * UART4.hpp
 *
 *  Created on: Jul 5, 2018
 *      Author: Jeremy
 */
#ifndef UART_4_HPP_
#define UART_4_HPP_

#include "UART_Base.hpp" //Base class

class UART4: public UART_Base
{
	public:
		bool init(unsigned int baud, uint32_t queue_size = 16);
	private:
		UART4();
		~UART4();
}


#endif /* UART_4_HPP_ */