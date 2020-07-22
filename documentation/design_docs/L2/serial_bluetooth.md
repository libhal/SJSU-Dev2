# Serial Bluetooth Interface

- [Serial Bluetooth Interface](#serial-bluetooth-interface)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [bool GetState() const](#bool-getstate-const)
  - [void Disconnect() const](#void-disconnect-const)
  - [void SendCommand(const char * command, const std::initializer_list<uint8_t> parameters, std::chrono::nanoseconds timeout) const](#void-sendcommandconst-char--command-const-stdinitializer_listuint8_t-parameters-stdchrononanoseconds-timeout-const)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location

`L2_HAL`

# Type

Interface

# Background

Bluetooth Low Energy (BLE) is a wireless personal area network (WPAN) that
provides the ability for devices to communicate using 2.4 GHz radio frequencies.

# Overview

The interface should be implemented by bluetooth peripherals that utilizes
serial communication such as SPI or UART for the transferring of data.

Examples of common serial bluetooth modules include:

- HM-10
- ZS-040
- MLT-BT05
- CC41

# Detailed Design

```c++
class SerialBluetooth
{
 public:
  virtual bool GetState() const = 0;
  virtual void Disconnect() const = 0;

  virtual void SendCommand(const char * command,
                           const std::initializer_list<uint8_t> parameters = {},
                           std::chrono::nanoseconds timeout = 10ms) const = 0;
};
```

## bool GetState() const
Should return the state of the device. This may vary by device depending on
whether the pin is configured to show the connection state or the mode of the
device.

## void Disconnect() const
Should send a command or toggle a pin to disconnect from any active connection.

## void SendCommand(const char * command, const std::initializer_list<uint8_t> parameters, std::chrono::nanoseconds timeout) const
Should send the desired command to the device. For example, sending a basic 2
char command: `"AT"`.

# Caveats

N/A

# Future Advancements

N/A
