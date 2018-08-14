#pragma once

#include <cmath>
#include <cstdint>

#include "L1_Drivers/i2c.hpp"

class AccelerometerInterface
{
 public:
    virtual bool Init();
    virtual int16_t GetX();
    virtual int16_t GetY();
    virtual int16_t GetZ();
    virtual float GetPitch();
    virtual float GetRoll();
    virtual uint8_t GetFullScaleRange();
    virtual void SetFullScaleRange(uint8_t range_value);
};

class Accelerometer : AccelerometerInterface
{
 public:
    uint16_t const dataOffset = 16;
    uint8_t const accelerometerAddress = 0x38;
    float const radiansToDegree = 180/3.14;
    uint8_t const whoAmIExpectedValue = 0x2a;
    size_t const 8BitLength = 1;
    size_t const 16BitLength = 2;

    enum class RegisterMap_t
      {
        status = 0x00,
        x = 0x01,
        y = 0x03,
        z = 0x05,
        who_am_i = 0x0d,
        data_config = 0x0e
      };
    Accelerometer();
    bool Init() override;
    int16_t GetX() override
    {
        I2c XTransaction(kI2c2);
        uint16_t kXVal;
        RegisterMap_t kXReg = RegisterMap_t::x;
        XTransaction.WriteThenRead(accelerometerAddress, &kXReg, 
                                   8BitLength, kXVal, 16BitLength);
        return kXVal/dataOffset;
    };
    int16_t GetY() override;
    int16_t GetZ() override;
    float GetPitch() override;
    float GetRoll() override;
    uint8_t GetFullScaleRange() override;
    void SetFullScaleRange(uint8_t range_value) override;
    ~Accelerometer();
};
