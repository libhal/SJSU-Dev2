#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"

class UARTInterface
{
	    virtual void setBaud(unsigned int baudrate) = 0;
        virtual bool init(unsigned int baud, uint32_t queue_size = 16) = 0;
        virtual void IntHandle() = 0;
        virtual void send(char out, unsigned int time_limit) = 0;
        virtual bool receive(char* CharInput, unsigned int time_limit) = 0;
};