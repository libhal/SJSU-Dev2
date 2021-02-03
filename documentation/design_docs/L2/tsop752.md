# Tsop752 Infrared Receiver

- [Tsop752 Infrared Receiver](#tsop752-infrared-receiver)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Initialization](#initialization)
  - [Capturing and Handling the De-modulated IR signal](#capturing-and-handling-the-de-modulated-ir-signal)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
  - [Demonstration Project](#demonstration-project)

# Location
`devices`

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
The `PulseCapture` module is used to capture incoming pulses of each frame from
the device. When a rising or falling edge of a signal is detected, a timestamp
in microseconds is recorded. The logic levels of the data in the received frame
is determined by examining the period of each pulse.

In addition to the capture module, the `Timer` module will be utilized to
determine the end of transmission frame when no consecutive pulse is received
within a timeout period after the last received pulse.

# Detailed Design
## API
```C++
namespace sjsu
{
class Tsop752 final : public InfraredReceiver
{
 public:
  explicit Tsop752(sjsu::PulseCapture & capture,
                   sjsu::Timer & timer,
                   std::chrono::microseconds timeout);

  Status Initialize() override;
  void SetInterruptCallback(DataReceivedHandler handler) override;
  void HandlePulseCaptured(PulseCapture::CaptureStatus_t status);
  void HandleEndOfFrame();

 private:
  const sjsu::PulseCapture & capture_;
  const sjsu::Timer & timer_;
  const std::chrono::microseconds kTimeout;
  DataFrame_t data_frame_;
  bool is_start_of_new_frame_;
  uint32_t last_received_timestamp_;
  DataReceivedHandler user_callback_ = nullptr;
};
}  // namespace sjsu
```

## Initialization
```C++
Status Initialize() const override
```
The following sequence is performed to initialize the driver:
1. Initialize and configure `capture_`. If the peripheral fails to initialize,
   the failure status shall be returned.
2. Initialize and configure `timer_`.
3. Return `Status::kSuccess` if both drivers are initialize successfully,
   Otherwise return the failure `Status`.

## Capturing and Handling the De-modulated IR signal
```C++
void SetInterruptCallback(DataReceivedHandler handler) const override
```
`SetInterruptCallback` sets the `user_callback_` that is invoked when a data
frame is successfully received.

```C++
void HandlePulseCaptured(PulseCapture::CaptureStatus_t status)
```
The capture module is configured to increment at 1MHz (1µs).
`HandlePulseCaptured` is invoked when a pulse edge is detected and the
value of Capture Register, CRn, will serve as a timestamp for when the edge was
detected. The duration of the pulse is determined by calculating the difference
of the previously received timestamp and the current timestamp. Additionally,
upon capturing a pulse edge, the timer is reset and restarted.

```C++
void HandleEndOfDataFrame()
```
`HandleEndOfDataFrame` is invoked when one of the following conditions is met:
1. The timeout period of the timer is reached after receiving a pulse edge.
2. The data_frame's `pulse_buffer_length` reaches the `kMaxPulseBufferSize`.

This signifies the end of the transmission frame and the `user_callback_` is
invoked to allow the received data frame to be further processed.

# Caveats
For the LPC17xx and LPC40xx series MCU, the `PulseCapture` and `Timer`
peripherals must not share the same channels. Each channel for both peripherals
share the same NVIC. Due to the implementation of both peripherals, initializing
the peripherals with the same channel will cause the interrupt callback to be
overwritten. For example, suppose `PulseCapture` and `Timer` are initialized
using the same channel with interrupts enabled. If the `Timer` was last to be
initialized, only the interrupt callbacks of the `Timer` peripheral will be
invoked.

# Future Advancements
N/A

# Testing Plan
## Unit Testing Scheme
The `PulseCapture` and `Timer` modules shall be mocked and stubbed to provide
test inputs for the `Tsop752` driver.

The following functions shall be tested:
1. `Initialize()`
   - Should return the failure `Status` when `Initialize` of `PulseCapture` is
     stubbed to return a `Status` that is not `Status::kSuccess`.
   - Should return the failure `Status` when `Initialize` of`Timer` is stubbed
     to return a `Status` that is not `Status::kSuccess`.
   - Should return `Status::kSuccess` when `Initialize` of both `Capture` and
     `Timer` are stubbed to return `Status::kSuccess`.
2. `HandlePulseCaptured()`
   - Test input timestamps: `{ 0, 9'000, 13'500, 14'060, 15'750, 16'310 }` shall
     be used.
   - At each iteration, the method should:
     - Invoke the `Reset` and `Start` methods of `Timer` once.
3. `HandleEndOfFrame()`
   - Test input timestamps: `{ 0, 9'000, 13'500, 14'060, 15'750, 16'310 }` shall
     be used.
   - The method should:
     - Invoke the `Stop` method of `Timer` once.
     - Invoke the mocked user callback.
       - The resulting pulse buffer should be equivalent to
         `{ 9'000, 4'500, 560, 1'690, 560 }`
       - The data frame's `pulse_buffer_length` should be `5`.

## Demonstration Project
A demonstration project using the LPC17xx platform to receive remote control
signals from a NEC remote can be found
[here](/demos/sjone/ir_receiver/source/main.cpp).
