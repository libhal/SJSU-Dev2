






#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"

class UARTInterface
{
	    virtual void setBaud(unsigned int baudrate);
        virtual bool init(unsigned int baud, uint32_t queue_size = 16);
        virtual void IntHandle();
        virtual void send(char out, unsigned int time_limit);
        virtual bool receive(char* CharInput, unsigned int time_limit);
};