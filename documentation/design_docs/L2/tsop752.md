# Tsop752 Infrared Receiver Design Document

- [Tsop752 Infrared Receiver Design Document](#tsop752-infrared-receiver-design-document)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
    - [Initialization](#initialization)
    - [Capturing The De-modulated IR signal](#capturing-the-de-modulated-ir-signal)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
  - [Demonstration Project](#demonstration-project)

# Location
`L2 HAL`

# Type
Implementation

# Background
The Tsop752 is an infrared (IR) receiver module for IR remote control systems.
The device detects modulated IR signals and outputs a de-modulated signal to be
processed by a micro-controller.

The device data-sheet can be found
[here](https://www.vishay.com/docs/82494/tsop752.pdf).

# Overview
The Tsop752 outputs IR data frames which consist of a de-modulated pulse train.
The `Capture` module is used to capture incoming pulses of each frame from the
device. When a rising and/or falling edge of a signal is detected, a timestamp
in milliseconds is recorded. The logic level of data in the received frame is
determined by examining the period of each pulse.

In addition to the capture module, the `Timer` module will be utilized to
determine the end of transmission frame when no consecutive pulse is received
within a timeout period after the last received pulse.

For consumer IR applications, such as IR remote control systems, two of the
commonly used IR transmission protocols include RC-5 and NEC. An example of a
typical IR transmission frame conforming to the NEC protocol can be found
[here](https://techdocs.altium.com/sites/default/files/wiki_attachments/296329/NECMessageFrame.png).

# Detailed Design
## API
```C++
namespace sjsu
{
class Tsop752 final : public InfraredReceiver
{
 public:
  constexpr explicit Tsop752(sjsu::Capture & capture,
                             sjsu::Timer & timer);

  Status Initialize() const override;
  void SetInterruptCallback(DataReceivedHandler handler) const override;

 private:
  static void HandlePulseCaptured();
  static void HandleEndOfDataFrame();

  inline static DataFrame_t data_frame;
  inline static DataReceivedHandler interrupt_handler;

  const sjsu::Capture & capture_;
  const sjsu::Timer & timer_;
};
}  // namespace sjsu
```

### Initialization
```C++
constexpr explicit Tsop752(sjsu::Capture & capture,
                           sjsu::Timer & timer)

Status Initialize() const override
```
The following sequence is performed to initialize the driver:
1. Initialize and configure `capture_`.
2. Initialize and configure `timer_`.
3. Return `Status::kSuccess` if both drivers are initialize successfully,
   Otherwise return the failure `Status`.

### Capturing The De-modulated IR signal
```C++
void SetInterruptCallback(DataReceivedHandler handler) const override
```
`SetInterruptCallback` sets the `interrupt_handler` that is invoked when a data
frame is successfully received.

```C++
static void HandlePulseCaptured()
```
The capture module is configured to increment at 1MHz (1µs).
The `HandlePulseCaptured` is invoked when a pulse edge is detected and the
value of Capture Register, CRn, will serve as a timestamp for when the edge was
detected. The timestamp is appended to the `pulse_buffer` of `data_frame`.
Additionally, upon capturing a pulse edge, the timer is reset.

```C++
static void HandleEndOfDataFrame()
```
The `HandleEndOfDataFrame` is invoked when the timeout period of the time is
reached after receiving after a pulse edge is received. This signifies the end
of the transmission frame. The `interrupt_handler` is invoked to allow the user
to process the received data.

# Caveats
N/A

# Future Advancements
N/A

# Testing Plan
## Unit Testing Scheme
The `Capture` and `Timer` modules shall be mocked and stubbed to provide test
inputs for the `Tsop752` driver.

The following functions shall be tested:
- `Initialize`
  - Should return the failure `Status` when `Initialize` of either `Capture`
    or `Timer` are stubbed to not return `Status::kSuccess`.
  - Should return `Status::kSuccess` when `Initialize` of both `Capture` and
    `Timer` are stubbed to return `Status::kSuccess`.

## Demonstration Project
A remote control utilizing the NEC will be used to transmit IR signals. The
program shall use a interrupt handler function that is invoked when a button on
the remote is pressed. The handler function shall decode the received data frame
and output the received IR code.
