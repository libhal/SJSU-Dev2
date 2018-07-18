/*
 * accelerometer.hpp
 *
 *  Created on: Jun 20, 2018
 *      Author: Andrew Javier
 */

#ifndef ACCELEROMETER_HPP_
#define ACCELEROMETER_HPP_
#include "i2c2_device.hpp"  // I2C Device Base Class
#include "math.h"
#include <stdint.h>

class accelerometer : private i2c2_device
{
public:
    uint16_t const dataOffset = 16;
    uint8_t const accelerometerAddress = 0x38;
    float const radiansToDegree = 180/3.14;
    uint8_t const whoAmIExpectedValue = 0x2a;

    enum class RegisterMap_t
        {
            status = 0x00,
            x = 0x01,
            y = 0x03,
            z = 0x05,
            who_am_i = 0x0d,
            data_config = 0x0e
        };

    accelerometer() : i2c2_device(I2CAddr_AccelerationSensor) //I2CAddr_AccelerationSensor = 0x38
    {
    }
    bool Init();
    int16_t GetX();
    int16_t GetY();
    int16_t GetZ();
    float GetPitch();
    float GetRoll();
    uint8_t GetFullScaleRange();
    void SetFullScaleRange(uint8_t range_value);
    ~accelerometer();

};



#endif /* ACCELEROMETER_HPP_ */
