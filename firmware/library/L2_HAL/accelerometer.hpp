#pragma once

#include <cstdint>
#include <cstdio>

#include "L1_Drivers/i2c.hpp"

class AccelerometerInterface
{
 public:
    virtual bool Init() = 0;
    virtual int16_t GetX() = 0;
    virtual int16_t GetY() = 0;
    virtual int16_t GetZ() = 0;
    virtual float GetPitch() = 0;
    virtual float GetRoll() = 0;
    virtual uint8_t GetFullScaleRange() = 0;
    virtual void SetFullScaleRange(uint8_t range_value) = 0;
};

class Accelerometer : public AccelerometerInterface
{
 public:
    uint16_t const kDataOffset = 16;
    uint8_t const kAccelerometerAddress = 0x1c;
    float const kRadiansToDegree = 180/3.14;
    uint8_t const kWhoAmIExpectedValue = 0x2a;
    uint8_t const kMsbShift = 8;
    size_t const kEightBitLength = 1;
    size_t const kSixteenBitLength = 2;
    uint32_t const kDefaultTimeout = 1000;

    enum class RegisterMap_t : uint8_t
      {
        status = 0x00,
        x = 0x01,
        y = 0x03,
        z = 0x05,
        who_am_i = 0x0d,
        data_config = 0x0e
      };

    Accelerometer()
    {
    }
    bool Init() override
    {
        I2c Initialization(I2c::Port::kI2c2);
        Initialization.Initialize();
        uint8_t WhoAmIReceivedValue;
        RegisterMap_t IdentityRegister = RegisterMap_t::who_am_i;
        uint8_t WhoAmIRegister;
        WhoAmIRegister = static_cast <unsigned char> (IdentityRegister);
        Initialization.WriteThenRead(kAccelerometerAddress, &WhoAmIRegister,
                                     kEightBitLength, &WhoAmIReceivedValue,
                                     kEightBitLength, kDefaultTimeout);
        return WhoAmIReceivedValue == kWhoAmIExpectedValue;
    }
    int16_t GetX() override
    {
        I2c XTransaction(I2c::Port::kI2c2);
        XTransaction.Initialize();
        uint8_t kXVal[2];
        RegisterMap_t XReg = RegisterMap_t::x;
        uint8_t kXReg;
        kXReg = static_cast <unsigned char> (XReg);
        XTransaction.WriteThenRead(kAccelerometerAddress, &kXReg,
                                   kEightBitLength, kXVal,
                                   kSixteenBitLength, kDefaultTimeout);
        printf("MSB %x, LSB %x\n", kXVal[0], kXVal[1]);
        return (int16_t)((kXVal[0] << kMsbShift) |
                 kXVal[1])/kDataOffset;  // Data arrives MSB then LSB
    }
    int16_t GetY() override
    {
        I2c YTransaction(I2c::Port::kI2c2);
        YTransaction.Initialize();
        uint8_t kYVal[2];
        RegisterMap_t YReg = RegisterMap_t::y;
        uint8_t kYReg;
        kYReg = static_cast <unsigned char> (YReg);
        YTransaction.WriteThenRead(kAccelerometerAddress, &kYReg,
                                   kEightBitLength, kYVal,
                                   kSixteenBitLength, kDefaultTimeout);
        return (int16_t)((kYVal[0] << kMsbShift) |
                 kYVal[1])/kDataOffset;  // Data arrives MSB then LSB
    }
    int16_t GetZ() override
    {
        I2c ZTransaction(I2c::Port::kI2c2);
        ZTransaction.Initialize();
        uint8_t kZVal[2];
        RegisterMap_t ZReg = RegisterMap_t::z;
        uint8_t kZReg;
        kZReg = static_cast <unsigned char> (ZReg);
        ZTransaction.WriteThenRead(kAccelerometerAddress, &kZReg,
                                   kEightBitLength, kZVal,
                                   kSixteenBitLength, kDefaultTimeout);
        return (int16_t)((kZVal[0] << kMsbShift) |
                 kZVal[1])/kDataOffset;  // Data arrives MSB then LSB
    }
    float GetPitch() override
    {
        int x = GetX();
        int y = GetY();
        int z = GetZ();
        float kPitchNumerator = x * -1;
        float kPitchDenominator = sqrt((y * y) + (z * z));
        float pitch = atan2(kPitchNumerator, kPitchDenominator) * kRadiansToDegree;
        printf("%f\n", pitch);
        return pitch;
    }
    float GetRoll() override
    {
        int y = GetY();
        int z = GetZ();
        return (atan2(y, z) * kRadiansToDegree);
    }
    uint8_t GetFullScaleRange() override
    {
        I2c FullScaleRange(I2c::Port::kI2c2);
        FullScaleRange.Initialize();
        uint8_t fullScaleRangeValue;
        RegisterMap_t dataConfig = RegisterMap_t::data_config;
        uint8_t configReg;
        configReg = (unsigned char) dataConfig;
        uint8_t FullScaleValue;
        FullScaleRange.WriteThenRead(kAccelerometerAddress, &configReg,
                                     kEightBitLength, &FullScaleValue,
                                     kEightBitLength, kDefaultTimeout);
        FullScaleValue &= 0x01;
        switch (FullScaleValue)
        {
            case 0: FullScaleValue = 2;
            case 1: FullScaleValue = 4;
            case 2: FullScaleValue = 8;
        }
        return FullScaleValue;
    }
    void SetFullScaleRange(uint8_t range_value) override
    {
        printf("range_value is %x\n", range_value);
        I2c FullScaleRange(I2c::Port::kI2c2);
        FullScaleRange.Initialize();
        RegisterMap_t dataConfig = RegisterMap_t::data_config;
        uint8_t configReg;
        configReg = static_cast <unsigned char> (dataConfig);
        uint8_t sendRange;
        if (range_value == 2)
        {
            sendRange = 0;
        }
        else if (range_value == 4)
        {
            sendRange = 1;
        }
        else if (range_value == 8)
        {
            sendRange = 2;
        }
        else
        {
            sendRange = 0;
        }
        printf("send range is %x\n", sendRange);
        uint8_t fullScaleRangeWriteBuffer [2] = {configReg, sendRange};
        FullScaleRange.Write(kAccelerometerAddress, fullScaleRangeWriteBuffer,
                             kSixteenBitLength, kDefaultTimeout);
    }
    virtual ~Accelerometer()
    {
    }
};
