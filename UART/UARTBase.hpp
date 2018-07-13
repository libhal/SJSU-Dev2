/*
 * UARTBase.hpp
 *
 *  Created on: Jun 23, 2018
 *      Author: Jeremy
 */
#ifndef UART_BASE_HPP_
#define UART_BASE_HPP_

#include "queue.h"
#include "LPC40xx.h"
#include "uart0_min.hpp"

class UART_Base
{
    public:
        void setBaud(unsigned int baudrate);
        bool init(unsigned int baud, uint32_t queue_size = 16);
        void IntHandle();
        void send(char out, unsigned int time_limit);
        bool receive(char* CharInput, unsigned int time_limit);
    private:
        UART_Base();
        QueueHandle_t Rx;
        LPC_UART_TypeDef* UARTBaseReg;
};

#endif /* UART_BASE_HPP_ */