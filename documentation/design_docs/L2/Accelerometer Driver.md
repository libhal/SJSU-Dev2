# Accelerometer Driver

- [Accelerometer Driver](#accelerometer-driver)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview (all)](#overview-all)
- [Detailed Design (API Interface)](#detailed-design-api-interface)
- [Using the Sensor](#using-the-sensor)
  - [Initialization](#Initialization)
  - [Obtaining Data from Sensor](#obtaining-data-from-sensor)
  - [Configuring the Sensor](#configuring-the-sensor)
- [Caveats](#caveats)
- [Future Advancements (optional)](#future-advancements-optional)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
  - [Integration Testing](#integration-testing)
  - [Demonstration Project](#demonstration-project)

# Location

`L2 Sensor`.

# Type

Interface that can be implemented.

# Background

The MMA8452Q accelerometer is a device that communicates using the I2C communication protocol. It can be read from and written to through the I2C communication line with the SJTwo microcontroller.

The accelerometer driver requires an I2C API to function.

# Overview (all)

The purpose of the accelerometer driver is to allow the SJTwo microcontroller to communicate with the MM8452QA accelerometer. At the base, the driver obtains data of the X, Y, and Z axes independently. In addition, the driver includes processing sensor data to determine device pitch and roll.

# Detailed Design (API Interface)

```C++
class ApiTemplate
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

```

# Using the Sensor

## Initialization

1. Run initialization function from the I2C class.
2. Check the “WHO_AM_I” register to ensure that communication is working properly and return the status

## Obtaining Data from Sensor

Data is obtained from the sensor by using the WriteThenRead function from the I2C class. This function writes the appropriate register address into the acceleration sensor and then reads back the next 16 bits from the slave. Since data from the sensor is only 12 bits, the value returned from the sensor is divided by 16 to remove this offset and maintain the polarity. Furthermore, the pitch and roll functions fetch relevant axis values through the get functions for calculations.

## Configuring the Sensor

The full scale range of the sensor can be configured through the “CFG” register. The user is given the option of setting the full scale range to 2G, 4G, or 8G.

# Caveats

N/A

# Future Advancements (optional)

N/A

# Testing Plan

N/A

## Unit Testing Scheme

N/A

## Integration Testing

The accelerometer driver will be tested with the I2C driver of the SJOne board until one is developed for the SJTwo board. When that happens, this driver will be ported over.

## Demonstration Project

During demonstration, the driver should be able to successfully fetch sensor data, utilize the data in the functions, and configure the sensitivity settings. The demonstration should also yield results consistent with the specifications given in the datasheet.
