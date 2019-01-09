// This file acts as an example of what a driver for SJSU-Dev2 should look like.
// The driver implementation is done in the .hpp file. Only a few things need to
// be done in the .cpp file.
//   Usage:
//      Give an example of how the developer can use your driver.
//      For example, include how to setup and initialize the driver, and how to
//      do something simple with the driver.
//      See library/L1_Drivers/pin.hpp for an example.
//
// Refrain at all costs from including any libary above L1. So you can include
// from L0, L1 and Utilities, but no higher
//
// The first none comment line of the driver hpp MUST be this:
#pragma once
// Include any C-header files first, in alphabetical order.
#include <sys/stat.h>
// Include any C++ headers afterwards, in alphabetical order. If there is a C++
// version of the standard library, use the C++ version and put them here. Put a
// newline between these includes and the above includes. If you have a comment,
// that will suffice as well.
#include <cstdint>
#include <cstdio>
// Include any SJ2 libraries afterwards, in alphabetical order.
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/macros.hpp"
// Every base driver MUST have a class interface.
//
// A class interface is simply a class that defines the API that the driver
// must follow. Each method defined in the interface must be declared as a pure
// virtual as seen below. Having at least one pure virtual will cause this class
// to become an abstract class, meaning it cannot be created on its own.
// Abstract classes can only beeinherited by a child class that implements its
// pure virtual methods.
//
// This allows abstract classes to act as a contract to classes that inherit
// this interface. Classes that do not implement abstract methods cannot
// compile,forcing the developer of that class to implement all of the methods
// in the interface.
//
// This step of creating a driver interface and inheriting it is also extremely
// important in allowing the use of polymorphism, which helps in
// stubs/mocks/fakes for testing and allows the different higher level device
// drivers in the HAL to work across platforms.
class ExampleInterface
{
  // Methods, variables, constants and enumerations placed in the interface
  // should be agnostic to the platform. For example, if you are writing an
  // interface for a Gpio, you would want a set of methods for setting the
  // direction (input or output) of the pin or the output voltage level
  // (high/low). You would expect any MCU to be able to do this. Lets say that
  // the gpio's of a special chip can toggle at a specific interval based on a
  // timer. This is something that not many other MCUs would support and thus,
  // shouldn't be in the interface. Refrain from putting variables of anykind in
  // the interface
 public:
  // Place any MCU agnostic enumeration and constants in the interface. If a
  // method below needs an enumeration passed to it, that enumeration should
  // most likely be defined here.
  enum class InterfaceModes
  {
    kStartupMode = 0,
    kSpecialMode,
    kErrorMode,
    kDestructorMode
  };
  // Only put in methods that you know each Example driver should have. If you
  // are not sure, put them all in and it can be figured out during code
  // review.
  virtual void Initialize()                                             = 0;
  virtual void DoSomeAction()                                           = 0;
  virtual void SendData(const uint8_t * payload, bool readback = false) = 0;
  virtual void SetMode(InterfaceModes mode)                             = 0;
  // This access region should exist if and only if each "ExampleInterface"
  // implementation driver must hold that set of methods or variables. Of this
  // interface requires
 protected:
};
// The actual driver MUST inherit, its corrisponding interface.
// If the implementation of this driver requires access to its platform's
// SystemController, then that controller MUST be inherited as well.
//
// Label all drivers final unless it is known that this particular driver will
// be inherited by another class. For example, Pin is not final but Gpio, which
// inherits pin is final. Gpio should only ever be set as not-final in the even
// that another class MUST inherit it. It is prefered that classes do not
// inherit but use composition instead. Using final has the added benefit, in
// that it devirtualizes the interface, optimizing the size and performance of
// the code.
class Example final : public ExampleInterface, protected Lpc40xxSystemController
{
  // Public section starts first
 public:
  // Define any structures or enumerations. Here
  // Implementation specific structures and enumerations should be defined here.
  SJ2_PACKED(struct) CanFrame_t
  {
    uint32_t uid;
    uint32_t message[8];
    // etc ...
  };
  // Define and declare static variables and constants first in the class
  static constexpr uint8_t kSpiFunction = 0b101;
  // If you are designing a firmware driver, you must include a static pointer
  // to the registers you plan to use. The purpose of this is to be able to
  // change out the register you are pointing to during testing. When compiled
  // into firmware, the example_driver.cpp will assign it to the appropriate
  // address, in this case LPC_USB;
  // DO NOT DECLARE THESE AS constexpr OR const, because they need to be mutable
  // for testing purposes. Define as inline to allow it to be defined in the
  // header file.
  inline static LPC_USB_TypeDef * usb = LPC_USB;
  // Make an lookup table of pointers if you are controlling multiple
  // peripherals of the same type.
  inline static LPC_CAN_TypeDef * can[2] = { LPC_CAN1, LPC_CAN2 };
  // Next is the constructor
  // Constructors should be almost always be constexpr, if possible.
  // Constexpr constructors, are constructed at compile time, so the
  // microcontroller doesn't have to spend time running the constructor, at
  // runtime. The constructed object is baked into the .data section at compile
  // time if it is statically linked. Statically linked meaning that the
  // variable was declared global or static within a function or class, i.e. not
  // created in the heap using new or on the stack as a local variable.
  constexpr Example([[maybe_unused]] uint8_t port,
                    [[maybe_unused]] uint32_t speed,
                    [[maybe_unused]] bool echo_to_usb)
      : can_rx_(&can_rx_pin_),
        can_tx_(&can_tx_pin_),
        usb_d_minus_(&usb_d_minus_pin_),
        usb_d_plus_(&usb_d_plus_pin_),
        can_rx_pin_(Pin::CreatePin<0, 1>()),
        can_tx_pin_(Pin::CreatePin<0, 2>()),
        usb_d_minus_pin_(Pin::CreatePin<2, 3>()),
        usb_d_plus_pin_(Pin::CreatePin<2, 4>())
  {
    // Do other constructor stuff here ...
  }
  // Always include a version of the driver, if it uses pins, that can take,
  // external pins through its constructor. This will allow the driver to be
  // more flexible if you do not include every single pin that can be used for
  // this peripheral. Here is an example of a constructor using externally
  // constructed can_rx, can_tx, usb_d_minus, usb_d_plus pins. This is a
  // dependency injection point and should be used in your test files to test
  // that the pins have been initialized properly.
  constexpr Example(PinInterface * can_rx, PinInterface * can_tx,
                    PinInterface * usb_d_minus, PinInterface * usb_d_plus)
      : can_rx_(can_rx),
        can_tx_(can_tx),
        usb_d_minus_(usb_d_minus),
        usb_d_plus_(usb_d_plus),
        can_rx_pin_(Pin::CreateInactivePin()),
        can_tx_pin_(Pin::CreateInactivePin()),
        usb_d_minus_pin_(Pin::CreateInactivePin()),
        usb_d_plus_pin_(Pin::CreateInactivePin())
  {
    // Do constructor stuff here ...
  }
  // Then comes class methods. Order of defintion should be the same as the
  // interface.

  // You must put "override" for methods that you override from the interface.
  // If you don't expect someone to inherit and override this class's
  // implementation declare it "final".
  void Initialize() override
  {
    // Typically you will need to power up your peripheral. Use the inherited
    // method rather than directly using the LPC_SC register.
    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kCan2);
    // Refrain from manipulating the CPU clock speed or peripheral clock speed
    // at this point. It would be very confusing for the user if using, lets say
    // the PWM driver, results in the CPU speed dropping by half or the
    // peripheral clock doubling.
  }
  void DoSomeAction() override
  {
    // Do some action stuff here ...
  }
  void SendData([[maybe_unused]] const uint8_t * payload,
                [[maybe_unused]] bool readback = false) override
  {
    // Ignore these two lines, these are here to suppress the "unused"
    // variable warning.
    // Send data here ...
  }
  void SetMode(InterfaceModes mode) override
  {
    // Set modes of stuff here ...
  }

  // Member variables must be suffixed with an underscore. For example
  //     int a_;
  //     GpioInterface & chip_select_;
  //     /* etc... */

  // Then protected region, if you need one.
 protected:
  // Then private region, if you need one.
 private:
  // Interface objects come first in declaration order
  PinInterface * can_rx_;
  PinInterface * can_tx_;
  PinInterface * usb_d_minus_;
  PinInterface * usb_d_plus_;
  // Then objects ...
  // NOTE: Prefer to use the generic Pin class rather than the pin for your
  // particular platform.
  Pin can_rx_pin_;
  Pin can_tx_pin_;
  Pin usb_d_minus_pin_;
  Pin usb_d_plus_pin_;
  // Then primatives ...
};
