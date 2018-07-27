/* Interface used to obtain the temperature of the system.*/


#ifndef TEMPERATURE_SENSOR_HPP_

#define TEMPERATURE_SENSOR_HPP_

#pragma once

#include "i2c2_device.hpp"

class TemperatureSensor : private i2c2_device

{

    public:

        TemperatureSensor(char address) : i2c2_device(address) {}                            // Access address

    	unsigned char getTemperatureByte();						     // Read from register 0x00 and return byte.

        float getCelsius();                                                                  // Converting bits from register 0x00 to float

        float getFahrenheit();                                                               // Converting Celsius to Fahrenheit


};

