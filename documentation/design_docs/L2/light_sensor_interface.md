# Light Sensor

- [Light Sensor](#light-sensor)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Initialization](#initialization)
  - [Obtaining Sensor Readings](#obtaining-sensor-readings)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location
`devices`

# Type
Interface

# Background
Light sensors typically use photo-detectors to measure the amount of light in a
environment. The SI unit used is lux.

# Overview
The interface should be inherited by light sensing drivers.

# Detailed Design
## API
```c++
namespace sjsu
{
class LightSensor
{
 public:
  virtual Status Initialize() const                           = 0;
  virtual units::illuminance::lux_t GetIlluminance() const    = 0;
  virtual float GetPercentageBrightness() const               = 0;
  units::illuminance::lux_t GetMaxIlluminance() const = 0;
};
}  // namespace sjsu
```

## Initialization
```c++
virtual Status Initialize() const
```
Performs the necessary initialization sequence to initializes the driver for use
and returns the initialization status.

## Obtaining Sensor Readings
```c++
virtual units::illuminance::lux_t GetIlluminance() const
```
Returns the sensor reading in units of lux.

```c++
virtual units::illuminance::lux_t GetMaxIlluminance() const
```
Returns the maximum illuminance that can be detected by the device.

```c++
float GetPercentageBrightness() const
```
Returns the detected brightness as a percentage ranging from 0.0f to 1.0f.
The percentage is determined with the following equation:

percentage = value ÷ (max illuminance)

# Caveats
N/A

# Future Advancements
N/A
