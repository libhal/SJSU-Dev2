#pragma once

#include <cstdint>

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

class GpioInterface
{
 public:
  enum Direction : uint8_t
  {
    kInput  = 0,
    kOutput = 1
  };
  enum State : uint8_t
  {
    kLow  = 0,
    kHigh = 1
  };
  enum class Edge : uint8_t
  {
    kEdgeRising  = 0,
    kEdgeFalling = 1,
    kEdgeBoth    = 2
  };
  virtual void SetAsInput()                                    = 0;
  virtual void SetAsOutput()                                   = 0;
  virtual void SetDirection(Direction direction)               = 0;
  virtual void SetHigh()                                       = 0;
  virtual void SetLow()                                        = 0;
  virtual void Set(State output = kHigh)                       = 0;
  virtual void Toggle()                                        = 0;
  virtual State ReadState()                                    = 0;
  virtual bool Read()                                          = 0;
  virtual PinInterface & GetPin()                              = 0;
  virtual void AttachInterrupt(IsrPointer function, Edge edge) = 0;
  virtual void DetachInterrupt()                               = 0;
  virtual void SetInterruptRoutine(IsrPointer function)        = 0;
  virtual void ClearInterruptRoutine()                         = 0;
  virtual void SetInterruptEdge(Edge edge)                     = 0;
  virtual void ClearInterruptEdge(Edge edge)                   = 0;
};

class Gpio final : public GpioInterface
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

  // For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr Gpio(uint8_t port_number, uint8_t pin_number)
      : interupt_port_(false),
        pin_(&lpc40xx_pin_),
        lpc40xx_pin_(port_number, pin_number)
  {
    interupt_port_ = (port_number == 2) ? 1 : 0;
  }
  // For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  constexpr explicit Gpio(PinInterface * pin)
      : interupt_port_(false), pin_(pin), lpc40xx_pin_(Pin::CreateInactivePin())
  {
    interupt_port_ = pin_->GetPort();
  }
  // Sets the GPIO pin direction as input
  void SetAsInput(void) override
  {
    pin_->SetPinFunction(kGpioFunction);
    gpio_port[pin_->GetPort()]->DIR &= ~(1 << pin_->GetPin());
  }
  // Sets the GPIO pin direction as output
  void SetAsOutput(void) override
  {
    pin_->SetPinFunction(kGpioFunction);
    gpio_port[pin_->GetPort()]->DIR |= (1 << pin_->GetPin());
  }
  // Sets the GPIO pin direction as output or input depending on the
  // Direction enum parameter
  inline void SetDirection(Direction direction) override
  {
    (direction) ? SetAsOutput() : SetAsInput();
  }
  // Sets the GPIO output pin to high
  void SetHigh(void) override
  {
    gpio_port[pin_->GetPort()]->SET = (1 << pin_->GetPin());
  }
  // Sets the GPIO output pin to low
  void SetLow(void) override
  {
    gpio_port[pin_->GetPort()]->CLR = (1 << pin_->GetPin());
  }
  // Sets the GPIO output pin to high or low depending on the State enum
  // parameter
  void Set(State output = kHigh) override
  {
    (output) ? SetHigh() : SetLow();
  }
  // Toggle the output of a GPIO output pin
  void Toggle() override
  {
    gpio_port[pin_->GetPort()]->PIN ^= (1 << pin_->GetPin());
  }
  // Returns the current State state of the pin
  State ReadState(void) override
  {
    bool state = (gpio_port[pin_->GetPort()]->PIN >> pin_->GetPin()) & 1;
    return static_cast<State>(state);
  }
  // Returns true if input or output pin is high
  bool Read(void) override
  {
    return (gpio_port[pin_->GetPort()]->PIN >> pin_->GetPin()) & 1;
  }
  PinInterface & GetPin() override
  {
    return *pin_;
  }

  // Checks if the selected gpio port is valid for external interrupts.
  bool ValidPortCheck()
  {
    bool is_valid = (interupt_port_ <= 1);
    SJ2_ASSERT_WARNING(is_valid,
                       "Port %d cannot be used for External Interrupts. Need "
                       "to use GPIO on Port 0 or 2.",
                       pin_->GetPort());
    return is_valid;
  }

  // Assigns the developer's ISR function to the port/pin gpio instance.
  void SetInterruptRoutine(IsrPointer function) override
  {
    ValidPortCheck();
    interrupthandlers[interupt_port_][pin_->GetPin()] = function;
  }

  // Clears the developers ISR function from the port/pin gio instance.
  void ClearInterruptRoutine() override
  {
    interrupthandlers[interupt_port_][pin_->GetPin()] = nullptr;
  }

  // Sets the selected edge that the gpio interrupt will be triggered on.
  void SetInterruptEdge(Edge edge) override
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
  void ClearInterruptEdge(Edge edge) override
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
  void AttachInterrupt(IsrPointer function, Edge edge) override
  {
    ValidPortCheck();
    SetInterruptRoutine(function);
    SetInterruptEdge(edge);
  }

  // Removes the developer's ISR and clears the selected edge of the gpio
  // interrupt from being triggered.
  void DetachInterrupt() override
  {
    ValidPortCheck();
    ClearInterruptRoutine();
    ClearEdgeRising();
    ClearEdgeFalling();
  }

  // Enables all gpio interrupts by putting the gpio internal ISR on the
  // Interrupt Vector Table.
  static void EnableInterrupts()
  {
    RegisterIsr(GPIO_IRQn, InterruptHandler);
  }

  // Disables all interrupts by removing the gpio internal ISR from the
  // Interrupt Vector Table.
  static void DisableInterrupts()
  {
    DeregisterIsr(GPIO_IRQn);
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
  void SetEdgeRising()
  {
    *interrupt[interupt_port_].enable_rising_edge |= (1 << pin_->GetPin());
  }

  // Sets the gpio interrupt to trigger on a falling edge.
  void SetEdgeFalling()
  {
    *interrupt[interupt_port_].enable_falling_edge |= (1 << pin_->GetPin());
  }

  // Clears the gpio interrupt to no longer trigger on a rising edge.
  void ClearEdgeRising()
  {
    *interrupt[interupt_port_].enable_rising_edge &= ~(1 << pin_->GetPin());
  }

  // Clears the gpio interrupt to no longer trigger on a falling edge.
  void ClearEdgeFalling()
  {
    *interrupt[interupt_port_].enable_falling_edge &= ~(1 << pin_->GetPin());
  }

  uint8_t interupt_port_;
  PinInterface * pin_;
  Pin lpc40xx_pin_;
};
