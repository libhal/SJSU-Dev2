# Tsop752 Infrared Receiver (UART) Design Document

- [Tsop752 Infrared Receiver (UART) Design Document](#tsop752-infrared-receiver-uart-design-document)
  - [Objective](#objective)
  - [Background](#background)
  - [Overview](#overview)
  - [Detailed Design](#detailed-design)
    - [InfraredReceiver Interface](#infraredreceiver-interface)
    - [API](#api)
    - [Initialization](#initialization)
    - [Receiving IR Signals (Polling)](#receiving-ir-signals-polling)
    - [Receiving IR Signals (Interrupts)](#receiving-ir-signals-interrupts)
  - [Caveats](#caveats)
  - [Test Plan](#test-plan)
    - [Unit Testing Scheme](#unit-testing-scheme)
    - [Integration Testing](#integration-testing)
    - [Demonstration Project](#demonstration-project)

## Objective
To develop a driver and library API for the Tsop752 Infrared Receiving using UART.

## Background
The Tsop752 is an infrared (IR) receiver module for IR remote control systems. The device detects IR signals and outputs the demodulated signal to be processed by a micro-controller.

The device data-sheet can be found [here](https://www.vishay.com/docs/82494/tsop752.pdf).

## Overview
The demodulated signals from the Tsop752 will be received using UART with IrDA support. IrDA is a set of communication protocols for wireless infrared communication. The signals are buffered and obtained from the UART'S Rx FIFO register.

## Detailed Design

### InfraredReceiver Interface
```c++
namespace sjsu
{
class InfraredReceiver
{
 public:
  virtual Status Initialize() const              = 0;
  virtual bool IsNewSignalReceived() const       = 0;
  virtual uint32_t GetLastReceivedSignal() const = 0;
};
}
```

### API
```c++
namespace sjsu
{
template <typename T>
class Tsop752;

template <>
class Tsop752<sjsu::Uart> final : public InfraredReceiver
{
public:
  constexpr explicit Tsop752(Uart & uart, uint32_t baud_rate);

  Status Initialize() const override;
  bool IsNewSignalReceived() const override;
  uint32_t GetLastReceivedSignal() const override;

private:
  const Uart & uart_;
  const uint32_t kBaudRate;
};
}
```

### Initialization
```c++
Status Initialize() const override;
```
The following sequence is performed to initialize the driver:
1. Initialize the UART port with the specified `kBaudRate`.
2. Enable IrDA for the UART port.
3. Return the initialization `Status`.

### Receiving IR Signals (Polling)
```c++
bool IsNewSignalReceived() const override;
```
`IsNewSignalReceived` returns `true` if the UART Rx FIFO is not empty, indicating a new signal is buffered and waiting to be read.

```c++
uint32_t GetLastReceivedSignal() const override;
```
If `IsNewSignalReceived` returns `true`, `GetLastReceivedSignal` will fetch the signal data from the UART Rx FIFO and return the received IR signal.

```c++
sjsu::Tsop752<sjsu::Uart> sensor;

while (true)
{
  if (sensor.IsNewSignalReceived())
  {
    uint32_t signal = sensor.GetLastReceivedSignal();
    // do something with IR signal...
  }
}
```

### Receiving IR Signals (Interrupts)
To be added...

## Caveats
The UART port used by the driver must support IrDA for the driver to function properly. For example, the LPC176x/5x series MCUs have four UART ports from 0-3; however, only ports 0, 2, and 3 support IrDA.

## Test Plan

### Unit Testing Scheme
The UART driver shall be mocked and stubbed to inject test inputs for the Tsop752 driver.

|  Function             |  Expected Result                                    |
|:---------------------:|:---------------------------------------------------:|
| IsNewSignalReceived   | Should return `true` when UART Rx FIFO is not empty |
| GetLastReceivedSignal | Should return `0` if there are no previous received signal.<br>Otherwise should return the previous received signal. |

### Integration Testing
To be added...

### Demonstration Project
To be added...
