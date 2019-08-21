#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"

#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Gpio final : public sjsu::Gpio
{
 public:
  // Specifies number of ports and pins that can be used with gpio interrupts.
  static constexpr uint8_t kNumberOfPins  = 32;
  static constexpr uint8_t kNumberOfPorts = 2;
  // Mode zero is the GPIO function for all pins.
  static constexpr uint8_t kGpioFunction = 0;
  // Table of GPIO ports located in LPC memory map
  inline static LPC_GPIO_TypeDef * gpio_port[6] = {
    LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5
  };

  // Lookup table that holds developer gpio interrupt handelers.
  inline static IsrPointer interrupthandlers[kNumberOfPorts][kNumberOfPins];

  //  Structure that holds gpio tnterrupt port specpefic registers,
  struct GpioInterruptRegisterMap_t
  {
    volatile uint32_t * rising_edge_status  = nullptr;
    volatile uint32_t * falling_edge_status = nullptr;
    volatile uint32_t * clear               = nullptr;
    volatile uint32_t * enable_rising_edge  = nullptr;
    volatile uint32_t * enable_falling_edge = nullptr;
  };

  // An array that contains all the port specific gpio interrupt registers.
  inline static GpioInterruptRegisterMap_t interrupt[kNumberOfPorts] = {
    { .rising_edge_status  = &(LPC_GPIOINT->IO0IntStatR),
      .falling_edge_status = &(LPC_GPIOINT->IO0IntStatF),
      .clear               = &(LPC_GPIOINT->IO0IntClr),
      .enable_rising_edge  = &(LPC_GPIOINT->IO0IntEnR),
      .enable_falling_edge = &(LPC_GPIOINT->IO0IntEnF) },
    { .rising_edge_status  = &(LPC_GPIOINT->IO2IntStatR),
      .falling_edge_status = &(LPC_GPIOINT->IO2IntStatF),
      .clear               = &(LPC_GPIOINT->IO2IntClr),
      .enable_rising_edge  = &(LPC_GPIOINT->IO2IntEnR),
      .enable_falling_edge = &(LPC_GPIOINT->IO2IntEnF) }
  };

  inline static volatile uint32_t * port_status = &(LPC_GPIOINT->IntStatus);
  static constexpr sjsu::cortex::InterruptController kInterruptController =
      sjsu::cortex::InterruptController();

  // For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr Gpio(uint8_t port_number,
                 uint8_t pin_number,
                 const sjsu::InterruptController & interrupt_controller =
                     kInterruptController)

      : interupt_port_(0),
        pin_(port_number, pin_number),
        interrupt_controller_(interrupt_controller)
  {
    interupt_port_ = (port_number == 2) ? 1 : 0;
  }
  // Sets the GPIO pin direction as output or input depending on the
  // Direction enum parameter
  void SetDirection(Direction direction) const override
  {
    pin_.SetPinFunction(kGpioFunction);
    volatile uint32_t * dir_register = &gpio_port[pin_.GetPort()]->DIR;

    if (direction == Direction::kInput)
    {
      *dir_register = bit::Clear(*dir_register, pin_.GetPin());
    }
    else
    {
      *dir_register = bit::Set(*dir_register, pin_.GetPin());
    }
  }
  // Sets the GPIO output pin to high or low depending on the State enum
  // parameter
  void Set(State output = kHigh) const override
  {
    if (output == State::kHigh)
    {
      gpio_port[pin_.GetPort()]->SET = (1 << pin_.GetPin());
    }
    else
    {
      gpio_port[pin_.GetPort()]->CLR = (1 << pin_.GetPin());
    }
  }
  // Toggle the output of a GPIO output pin
  void Toggle() const override
  {
    gpio_port[pin_.GetPort()]->PIN ^= (1 << pin_.GetPin());
  }
  // Returns the current State state of the pin
  bool Read() const override
  {
    return bit::Read(gpio_port[pin_.GetPort()]->PIN, pin_.GetPin());
  }
  const sjsu::Pin & GetPin() const override
  {
    return pin_;
  }

  // Checks if the selected gpio port is valid for external interrupts.
  bool ValidPortCheck() const
  {
    bool is_valid = (interupt_port_ <= 1);
    SJ2_ASSERT_WARNING(is_valid,
                       "Port %d cannot be used for External Interrupts. Need "
                       "to use GPIO on Port 0 or 2.",
                       pin_.GetPort());
    return is_valid;
  }

  // Assigns the developer's ISR function to the port/pin gpio instance.
  void SetInterruptRoutine(IsrPointer function) const
  {
    ValidPortCheck();
    interrupthandlers[interupt_port_][pin_.GetPin()] = function;
  }

  // Clears the developers ISR function from the port/pin gio instance.
  void ClearInterruptRoutine() const
  {
    interrupthandlers[interupt_port_][pin_.GetPin()] = nullptr;
  }

  // Sets the selected edge that the gpio interrupt will be triggered on.
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
            util::Value(edge));
      }
    }
  }

  // Clears the seleted edge of the gpio interrupt from being triggered.
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
            util::Value(edge));
      }
    }
  }

  // Assign the developer's ISR and sets the selected edge that the gpio
  // interrupt will be triggered on.
  void AttachInterrupt(IsrPointer function, Edge edge) const override
  {
    ValidPortCheck();
    SetInterruptRoutine(function);
    SetInterruptEdge(edge);
  }

  // Removes the developer's ISR and clears the selected edge of the gpio
  // interrupt from being triggered.
  void DetachInterrupt() const override
  {
    ValidPortCheck();
    ClearInterruptRoutine();
    ClearEdgeRising();
    ClearEdgeFalling();
  }

  // Enables all gpio interrupts by putting the gpio internal ISR on the
  // Interrupt Vector Table.
  void EnableInterrupts()
  {
    interrupt_controller_.Register({
        .interrupt_request_number  = GPIO_IRQn,
        .interrupt_service_routine = InterruptHandler,
    });
  }

  // Disables all interrupts by removing the gpio internal ISR from the
  // Interrupt Vector Table.
  void DisableInterrupts()
  {
     interrupt_controller_.Deregister(GPIO_IRQn);
  }

  // The gpio internal ISR that calls the developer's ISR's.
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
  // Sets the gpio interrupt to trigger on a rising edge.
  void SetEdgeRising() const
  {
    *interrupt[interupt_port_].enable_rising_edge |= (1 << pin_.GetPin());
  }

  // Sets the gpio interrupt to trigger on a falling edge.
  void SetEdgeFalling() const
  {
    *interrupt[interupt_port_].enable_falling_edge |= (1 << pin_.GetPin());
  }

  // Clears the gpio interrupt to no longer trigger on a rising edge.
  void ClearEdgeRising() const
  {
    *interrupt[interupt_port_].enable_rising_edge &= ~(1 << pin_.GetPin());
  }

  // Clears the gpio interrupt to no longer trigger on a falling edge.
  void ClearEdgeFalling() const
  {
    *interrupt[interupt_port_].enable_falling_edge &= ~(1 << pin_.GetPin());
  }

  uint8_t interupt_port_;
  sjsu::lpc40xx::Pin pin_;
  const sjsu::InterruptController & interrupt_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
