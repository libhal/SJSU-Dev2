#pragma once

#include "L1_Peripheral/gpio.hpp"

#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace lpc17xx
{
/// GPIO implementation for the lpc17xx platform
class Gpio final : public sjsu::Gpio
{
 public:
  /// Specifies number of ports and pins that can be used with gpio interrupts.
  static constexpr uint8_t kNumberOfPins = 32;
  /// Specifies the number of ports that can have interrupts.
  static constexpr uint8_t kNumberOfInterruptPorts = 2;
  /// Specifies the number of ports that the LPC17xx has.
  static constexpr uint8_t kNumberOfPorts = 5;
  /// Mode zero is the GPIO function for all pins.
  static constexpr uint8_t kGpioFunction = 0;
  /// Structure containing kNumberOfPorts of gpio peripherals. In the LPC17xx
  /// hardware, the addresses between LPC_GPIO peripherals are contiguous, so we
  /// can make a map of them by making an array of LPC_GPIO_TypeDefs.
  struct GpioPort_t
  {
    /// List of LPC_GPIO peripherals
    LPC_GPIO_TypeDef port[kNumberOfPorts];
  };
  /// Map a pointer to a GpioPort_t to the gpio.ports actual address.
  inline static GpioPort_t * gpio = reinterpret_cast<GpioPort_t *>(0x2009'C000);
  /// Lookup table that holds developer gpio interrupt handelers.
  inline static IsrPointer interrupthandlers[kNumberOfInterruptPorts]
                                            [kNumberOfPins];
  /// Holds gpio interrupt port specific registers. Used to make the code more
  /// readable.
  struct GpioInterruptRegisterMap_t
  {
    /// Pointer to the register that holds the rising edge event status. So if
    /// pin 5 had a rising edge interrupt cause it, then there will be a 1 in
    /// the 5th position.
    volatile uint32_t * rising_edge_status = nullptr;
    /// Pointer to the register that holds the falling edge event status. So if
    /// pin 5 had a falling edge interrupt cause it, then there will be a 1 in
    /// the 5th position.
    volatile uint32_t * falling_edge_status = nullptr;
    /// Pointer to the register that clears gpio interrupts.
    volatile uint32_t * clear = nullptr;
    /// Pointer to the register that enables/disables rising edge interrupts.
    volatile uint32_t * enable_rising_edge = nullptr;
    /// Pointer to the register that enables/disables falling edge interrupts.
    volatile uint32_t * enable_falling_edge = nullptr;
  };

  /// An array that contains all the port specific gpio interrupt registers.
  inline static GpioInterruptRegisterMap_t interrupt[kNumberOfInterruptPorts] =
      { { .rising_edge_status  = &(LPC_GPIOINT->IO0IntStatR),
          .falling_edge_status = &(LPC_GPIOINT->IO0IntStatF),
          .clear               = &(LPC_GPIOINT->IO0IntClr),
          .enable_rising_edge  = &(LPC_GPIOINT->IO0IntEnR),
          .enable_falling_edge = &(LPC_GPIOINT->IO0IntEnF) },
        { .rising_edge_status  = &(LPC_GPIOINT->IO2IntStatR),
          .falling_edge_status = &(LPC_GPIOINT->IO2IntStatF),
          .clear               = &(LPC_GPIOINT->IO2IntClr),
          .enable_rising_edge  = &(LPC_GPIOINT->IO2IntEnR),
          .enable_falling_edge = &(LPC_GPIOINT->IO2IntEnF) } };
  /// Register that points to the port status register.
  inline static volatile uint32_t * port_status = &(LPC_GPIOINT->IntStatus);
  /// Get a ARM cortex interrupt controller object
  static constexpr sjsu::cortex::InterruptController kInterruptController =
      sjsu::cortex::InterruptController();
  /// Converts the port into the appropriate index in the lookup table.
  ///
  /// @param port - port to convert to a lookup table index.
  constexpr uint8_t ConvertPortToInterruptPortIndex(uint8_t port)
  {
    return (port == 2) ? 1 : 0;
  }
  /// For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr Gpio(uint8_t port,
                 uint8_t pin,
                 const sjsu::InterruptController & interrupt_controller =
                     kInterruptController)
      : kInteruptPort(ConvertPortToInterruptPortIndex(port)),
        kPin(port, pin),
        interrupt_controller_(interrupt_controller)
  {
  }
  void SetDirection(Direction direction) const override
  {
    kPin.SetPinFunction(kGpioFunction);
    volatile uint32_t * dir = &gpio->port[kPin.GetPort()].FIODIR;

    if (direction == Direction::kInput)
    {
      *dir = bit::Clear(*dir, kPin.GetPin());
    }
    else
    {
      *dir = bit::Set(*dir, kPin.GetPin());
    }
  }
  void Set(State output = kHigh) const override
  {
    if (output == State::kHigh)
    {
      gpio->port[kPin.GetPort()].FIOSET = (1 << kPin.GetPin());
    }
    else
    {
      gpio->port[kPin.GetPort()].FIOCLR = (1 << kPin.GetPin());
    }
  }
  void Toggle() const override
  {
    gpio->port[kPin.GetPort()].FIOPIN ^= (1 << kPin.GetPin());
  }
  bool Read() const override
  {
    return bit::Read(gpio->port[kPin.GetPort()].FIOPIN, kPin.GetPin());
  }
  const sjsu::Pin & GetPin() const override
  {
    return kPin;
  }
  /// Checks if the selected gpio port is valid for external interrupts.
  bool ValidPortCheck() const
  {
    bool is_valid = (kInteruptPort <= 1);
    SJ2_ASSERT_WARNING(is_valid,
                       "Port %d cannot be used for External Interrupts. Need "
                       "to use GPIO on Port 0 or 2.",
                       kPin.GetPort());
    return is_valid;
  }
  /// Assigns the developer's ISR function to the port/pin gpio instance.
  void SetInterruptRoutine(IsrPointer function) const
  {
    ValidPortCheck();
    interrupthandlers[kInteruptPort][kPin.GetPin()] = function;
  }
  /// Clears the developers ISR function from the port/pin gio instance.
  void ClearInterruptRoutine() const
  {
    interrupthandlers[kInteruptPort][kPin.GetPin()] = nullptr;
  }
  /// Sets the selected edge that the gpio interrupt will be triggered on.
  void SetInterruptEdge(Edge edge) const
  {
    ValidPortCheck();
    switch (edge)
    {
      case Edge::kEdgeRising:
      {
        SetEdgeRising();
        break;
      }
      case Edge::kEdgeFalling:
      {
        SetEdgeFalling();
        break;
      }
      case Edge::kEdgeBoth:
      {
        SetEdgeRising();
        SetEdgeFalling();
        break;
      }
      default:
      {
        LOG_WARNING(
            "Edge %d cannot be used for External Interrupts."
            "Need to use a rising, falling, or both configuration.",
            Value(edge));
      }
    }
  }
  /// Clears the selected edge of the gpio interrupt from being triggered.
  void ClearInterruptEdge(Edge edge)
  {
    ValidPortCheck();
    switch (edge)
    {
      case Edge::kEdgeRising:
      {
        ClearEdgeRising();
        break;
      }
      case Edge::kEdgeFalling:
      {
        ClearEdgeFalling();
        break;
      }
      case Edge::kEdgeBoth:
      {
        ClearEdgeRising();
        ClearEdgeFalling();
        break;
      }
      default:
      {
        LOG_WARNING(
            "Edge %d cannot be used for External Interrupts."
            "Need to use a rising, falling, or both configuration.",
            Value(edge));
      }
    }
  }
  /// Assign the developer's ISR and sets the selected edge that the gpio
  /// interrupt will be triggered on.
  void AttachInterrupt(IsrPointer function, Edge edge) const override
  {
    ValidPortCheck();
    SetInterruptRoutine(function);
    SetInterruptEdge(edge);
  }
  /// Removes the developer's ISR and clears the selected edge of the gpio
  /// interrupt from being triggered.
  void DetachInterrupt() const override
  {
    ValidPortCheck();
    ClearInterruptRoutine();
    ClearEdgeRising();
    ClearEdgeFalling();
  }
  /// Enables all gpio interrupts by putting the gpio internal ISR on the
  /// Interrupt Vector Table.
  void EnableInterrupts()
  {
    // GPIO interrupts is shared with the EINT3 channel
    interrupt_controller_.Register({
        .interrupt_request_number  = EINT3_IRQn,
        .interrupt_service_routine = InterruptHandler,
    });
  }
  /// Disables all interrupts by removing the gpio internal ISR from the
  /// Interrupt Vector Table.
  void DisableInterrupts()
  {
    interrupt_controller_.Deregister(EINT3_IRQn);
  }
  /// The gpio internal ISR that calls the developer's ISR's.
  static void InterruptHandler()
  {
    int triggered_port = (*port_status >> 2);
    int triggered_pin =
        __builtin_ctz(*interrupt[triggered_port].rising_edge_status |
                      *interrupt[triggered_port].falling_edge_status);
    interrupthandlers[triggered_port][triggered_pin]();
    *interrupt[triggered_port].clear |= (1 << triggered_pin);
  }

 private:
  /// Sets the gpio interrupt to trigger on a rising edge.
  void SetEdgeRising() const
  {
    *interrupt[kInteruptPort].enable_rising_edge |= (1 << kPin.GetPin());
  }
  /// Sets the gpio interrupt to trigger on a falling edge.
  void SetEdgeFalling() const
  {
    *interrupt[kInteruptPort].enable_falling_edge |= (1 << kPin.GetPin());
  }
  /// Clears the gpio interrupt to no longer trigger on a rising edge.
  void ClearEdgeRising() const
  {
    *interrupt[kInteruptPort].enable_rising_edge &= ~(1 << kPin.GetPin());
  }
  /// Clears the gpio interrupt to no longer trigger on a falling edge.
  void ClearEdgeFalling() const
  {
    *interrupt[kInteruptPort].enable_falling_edge &= ~(1 << kPin.GetPin());
  }
  /// Cache of the port index in the lookup table.
  const uint8_t kInteruptPort;
  /// Internal pin object.
  const Pin kPin;
  /// Internal reference to an interrupt controller.
  const sjsu::InterruptController & interrupt_controller_;
};
}  // namespace lpc17xx
}  // namespace sjsu
