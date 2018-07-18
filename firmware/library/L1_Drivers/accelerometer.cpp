/*
 * accelerometer.cpp
 *
 *  Created on: Jun 20, 2018
 *      Author: Andrew Javier
 */

#include "accelerometer.hpp"

bool accelerometer :: Init()
{
    RegisterMap_t whoAmI = RegisterMap_t::who_am_i;
    uint8_t returnedValue = readReg((unsigned char)whoAmI);
    return (returnedValue == whoAmIExpectedValue);
}
int16_t accelerometer :: GetX()
{
    RegisterMap_t X = RegisterMap_t::x;
    return (int16_t)get16BitRegister((unsigned char)X) / dataOffset;
}

int16_t accelerometer :: GetY()
{
    RegisterMap_t Y = RegisterMap_t::y;
    return (int16_t)get16BitRegister((unsigned char)Y) / dataOffset;
}

int16_t accelerometer :: GetZ()
{
    RegisterMap_t Z = RegisterMap_t::z;
    return (int16_t)get16BitRegister((unsigned char)Z) / dataOffset;
}

float accelerometer :: GetRoll()
{
    int y = GetY();
    int z = GetZ();
    return (atan2(y, z) * radiansToDegree);
}

float accelerometer :: GetPitch()
{
    int x = GetX();
    int y = GetY();
    int z = GetZ();
    float pitchNumerator = x * -1;
    float pitchDenominator = sqrt((y * y) + (z * z));
    return (atan2(pitchNumerator, pitchDenominator) * radiansToDegree);
}
uint8_t accelerometer :: GetFullScaleRange()
{
    RegisterMap_t dataConfig = RegisterMap_t::data_config;
    uint8_t configReg = readReg((unsigned char)dataConfig);
    uint8_t G;
    configReg &= 0x03;
    if (configReg && 0x01)
    {
        G = (configReg & 0x02) ? 0 : 4;
    }
    else
    {
        G = (configReg & 0x02) ? 8 : 2;
    }
    return G;
}

void accelerometer :: SetFullScaleRange(uint8_t range_value)
{
    RegisterMap_t dataConfig = RegisterMap_t::data_config;
    unsigned char configReg = readReg((unsigned char)dataConfig);
    configReg &= ~0x03; //clear the Full Scale Range field
    if (range_value <= 2)
    {
        writeReg((unsigned char)dataConfig, configReg);
    }
    else if (range_value >=3 && range_value < 8)
    {
        configReg |= 0x01;
        writeReg((unsigned char)dataConfig, configReg);
    }
    else
    {
        configReg |= 0x02;
        writeReg((unsigned char)dataConfig, configReg);
    }
}
