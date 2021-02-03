# Temperature Sensor

- [Temperature Sensor](#temperature-sensor)
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
An abstract interface for temperature sensing device drivers.

# Overview
The interface should be inherited by device drivers that are responsible for
obtaining temperature readings.

# Detailed Design
## API
```c++
namespace sjsu
{
class TemperatureSensor
{
 public:
  virtual Status Initialize() const = 0;
  virtual Status GetTemperature(
      units::temperature::celsius_t * temperature) const = 0;
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
virtual Status GetTemperature(
    units::temperature::celsius_t * temperature) const
```
Retrieves the temperature reading and writes the value to the designated memory
address.

# Caveats
N/A

# Future Advancements
N/A
