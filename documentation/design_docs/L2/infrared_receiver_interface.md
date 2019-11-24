# Infrared Receiver Interface Design Document

- [Infrared Receiver Interface Design Document](#infrared-receiver-interface-design-document)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location
`L2_HAL`.

# Type
Interface

# Background
Infrared (IR) receivers typically use a photo-diode to convert IR light into an
electrical signal that can be processed by a micro-controller. One of the common
applications of infrared communication is remote control systems.

# Overview
The `InfraredReceiver` interface should be inherited by communication drivers
responsible for receiving IR data.

# Detailed Design
## API
```c++
namespace sjsu
{
class InfraredReceiver
{
 public:
  struct DataFrame_t
  {
    inline static constexpr uint32_t kMaxPulseBufferSize = 100;
    uint32_t pulse_buffer[kMaxPulseBufferSize];
    uint32_t number_of_pulses;
  };

  using DataReceivedHandler = std::function<void(DataFrame_t *)>;

  virtual Status Initialize()                                   const = 0;
  virtual void SetInterruptCallback(DataReceivedHandler handler) const = 0;
};
}
```

```c++
Status Initialize() const;
```
`Initialize` performs the necessary initialization sequence to initialize the
device and returns `Status::kSuccess` upon successful initialization. If the
device fails to be initialized, the appropriate failure `Status` is returned.

```c++
virtual void SetInterruptCallback(DataReceivedHandler handler) const = 0;
```
`SetInterruptCallback` sets the interrupt callback handler that is invoked when
a new data is successfully received.

# Caveats
N/A

# Future Advancements
N/A
