// Note: Refer to i2c2_device function used from https://github.com/kammce/SJSU-Dev/blob/master/firmware/lib/L4_IO/i2c2_device.hpp

#ifndef TEMPERATURE_SENSOR_HPP_

#define TEMPERATURE_SENSOR_HPP_

#include "i2c2_device.hpp"

class Temperature_I2C : private i2c2_device

{

    public:

        Temperature_I2C(char address) : i2c2_device(address) {}                              // Access address

    	unsigned char getTemperatureByte();						     // Read from register 0x00 and return byte.

        float getCelsius();                                                                  // Converting bits from register 0x00 to float

        float getFahrenheit();                                                               // Converting Celsius to Fahrenheit


};

