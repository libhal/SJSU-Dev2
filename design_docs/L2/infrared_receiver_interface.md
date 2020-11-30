# Infrared Receiver

- [Infrared Receiver](#infrared-receiver)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location
`L2_HAL`

# Type
Interface

# Background
Infrared (IR) receivers typically use a photo-diode to convert IR light into an
electrical signal that can be processed by a micro-controller. Common
applications include continuous data communication and consumer remote control
systems.

IrDA is the standard used for continuous data transmission applications;
however, the receiver/transceiver device must be IrDA compatible. The standard
mode for IrDA, serial Infrared (SIR), allows serial data to be transmitted or
received through standard UART.

For consumer IR remote applications, commonly used data formats include:
1. Bi-phase Encoding (ie.
   [RC-5](https://www.sbprojects.net/knowledge/ir/rc5.php),
   [RC-6](https://www.sbprojects.net/knowledge/ir/rc6.php))
2. Pulse Distance Encoding (ie.
   [NEC](https://www.sbprojects.net/knowledge/ir/nec.php))
3. Pulse Length Encoding (ie.
   [SIRC](https://www.sbprojects.net/knowledge/ir/sirc.php))

More information regarding these three data formats can be found
[here](https://www.vishay.com/docs/80071/dataform.pdf). Additionally
[LIRC](http://www.lirc.org/) contains a large database of remote control codes.

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
  using DataReceivedHandler = std::function<void(const DataFrame_t *)>;

  virtual Status Initialize()                                    const = 0;
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
