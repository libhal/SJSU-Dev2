# Accelerometer

- [Accelerometer](#accelerometer)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
- [Using the Sensor](#using-the-sensor)
  - [Initialization](#initialization)
  - [GetX(), GetY(), GetX()](#getx-gety-getx)
  - [Configuring the Sensor using SetFullScale()](#configuring-the-sensor-using-setfullscale)
- [GetPitch() and GetRoll()](#getpitch-and-getroll)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location

`L2 Sensor`

# Type

Interface & Implementation

# Background

An accelerometer is a device that


 that communicates using the I2C
communication protocol. It can be read from and written to through the I2C
communication line with the SJTwo microcontroller.

The MMA452Q communicates over I2C.

# Overview

The purpose of the accelerometer interface is to allow any microcontroller to
communicate with an accelerometers like the MM8452QA or MPU6050. At the base,
the interface obtains data of the X, Y, and Z axes independently. In addition,
the interface includes processing sensor data to determine device pitch and roll.

# Detailed Design

```C++
class Accelerometer
{
 public:
    virtual bool Initialize() = 0;
    virtual int32_t GetX() = 0;
    virtual int32_t GetY() = 0;
    virtual int32_t GetZ() = 0;
    virtual uint8_t GetFullScaleRange() = 0;
    virtual void SetFullScaleRange(uint8_t range_value) = 0;

    // Utility methods

    float GetPitch();
    float GetRoll();
};
```

# Using the Sensor

## Initialization
Initialize hardware necessary to communicate with external accelerometer.

## GetX(), GetY(), GetX()

These methods should retrieve the necessary accelerometer data for each of their
respective axes.

## Configuring the Sensor using SetFullScale()

Many accelerometers have the capability to have their acceleration scaled
limited. This allows for greater precision in the acceleration range you expect
your application to be in.

If you choose too low of an acceleration, then your acceleration value will
saturate at its numeric limit, and the information about the acceleration at
that level will be lost. For example, if you set the maximum scale value to be
1g with a 10-bit accelerometer but your application will encounter accelerations
of 1.2g, then any reading from the device will show +511 as long as the
acceleration is above 1g.

If you choose too high of an acceleration, then the precision of the data will
be lower, If you choose for example 5g as your scale to max but you only
encounter 1g. For a 10-bit accelerometer, you will only see data between 511/5
which is equal to -102 to +102. This means that you are missing out on 80% of
the precision you could be getting if the scale was closer to the expected
scale.

# GetPitch() and GetRoll()

These utility methods calculate pitch and roll based on the GetX(), GetY(), and
GetZ() methods.

# Caveats

GetPitch() and GetRoll() do not have information about the orientation of the
accelerometer and assume an orientation which may not match with the actual
device.

# Future Advancements

Consider combining accelerometer data into 1 single X, Y, and Z structure.
