// This file acts as an example of what a driver for SJSU-Dev2 should look like.
// The driver implementation is done in the .hpp file. Only a few things need to
// be done in the .cpp file.
//   Usage:
//      In this area here, you should give some examples of how the developer
//      can use your driver. For example you should include how to initilize the
//      driver, and how you would do some basic setup. You don't have to show
//      everything, just enough to get the developer started.
//      See library/L1_Drivers/pin_configure.hpp for an example.
// The first none comment line of the driver hpp should be this.
#pragma once
// Include any C-header files first, in alphabetical order.
#include <sys/stat.h>
// Include any C++ headers afterwards, in alphabetical order. If there is a C++ version of the standard
// library, use the C++ version and put them here. Put a newline between these
// includes and the above includes.
#include <cstdio>
#include <cstdint>
// Include any SJ2 libraries afterwards, in alphabetical order.
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin_configure.hpp"
#include "L2_Utilities/macros.hpp"
// Every base driver you develop must start with an class interface.
// A class interface is simply a class that defines which methods you can expect
// from an implementation of this driver. Each method defined in the interface
// must be declared as a pure virtual as seen below, making it an abstract
// class. An abstract class can never be constructed as a stand alone object,
// but it can be used as a contract to classes that inheritate (they are forced
// to implement this class), as a reference to its children classes
// (polymorphasism), and it can be used to generate stubs/mocks/fakes etc. for
// testing
class ExampleInterface
{
   public:
    // Only put in methods that you know each Example driver should have. If you
    // are not sure, put them all in and it can be figured out during code
    // review.
    virtual void Init() = 0;
    virtual void DoSomeAction() = 0;
    virtual void SendData(const uint8_t * payload, bool readback = false) = 0;
    virtual void SetMode() = 0;
};
// Your actual driver should inherit, the interface.
class Example : public ExampleInterface
{
   // Public starts first
   public:
    // Define and declare static variables and constants first in the class
    static constexpr uint8_t kSpiFunction = 0b101;
    // If you are designing a firmware driver, you must include a static pointer
    // to the registers you plan to use. The purpose of this is to be able to
    // change out the register you are pointing to during testing. When compiled
    // into firmware, the example-driver.cpp will assign it to the appropriate
    // address, in this case LPC_USB;
    // DO NOT DECLARE THESE AS constexpr OR const, they need to be mutable
    static LPC_USB_TypeDef * usb;
    // Make an array of pointers if you are controlling multiple perpherials of
    // the same type.
    static LPC_CAN_TypeDef * can[2];
    // Define any structures or enumerations.
    SJ2_PACKED(struct) CanFrame {
        uint32_t uid;
        uint32_t message[8];
        // etc ...
    };
    // Then comes any static factory methods. A factory is a design pattern,
    // wherein a function or object builds an object. If there are many steps to
    // build an object a factory helps to reduce it down to a single function.
    //
    // In this example, the parameters are passed via template arguments.
    // Template arguments must be known at compile time, thus we can use
    // "static_assert" to prompt the developer with a compiler error and error
    // message if they use the factory inappropriately.
    template<unsigned can_port, unsigned can_speed>
    static Example createExample()
    {
        static_assert(1 <= can_port && can_port <= 2,
            "Only CAN ports 0 and 1 are available");
        static_assert(can_speed <= 500'000,
            "Invalid CAN speed, CAN frequency cannot exceed 500Kbps");
        return Example(can_port, can_speed, false);
    }
    // Them comes your constructors
    // This constructor is constexpr, but it does not have to be. The nice thing
    // about using a constexpr constructor, is that the microcontroller doesn't
    // have to spend time running the constructor, it is known at compile time,
    // constructed at compile time and baked into the .data section.
    constexpr Example(uint8_t port, uint32_t speed, bool echo_to_usb):
        can_rx_(can_rx_pin_), can_tx_(can_tx_pin_),
        usb_d_minus_(usb_d_minus_pin_), usb_d_plus_(usb_d_plus_pin_),
        can_rx_pin_(PinConfigure::CreatePinConfigure<0,1>()),
        can_tx_pin_(PinConfigure::CreatePinConfigure<0,2>()),
        usb_d_minus_pin_(PinConfigure::CreatePinConfigure<2,3>()),
        usb_d_plus_pin_(PinConfigure::CreatePinConfigure<2,4>())
    {
        (void)port;
        (void)speed;
        (void)echo_to_usb;
        // Do other constructor stuff here ...
    }
    // Construction using externally constructed mosi, miso, and sck pins.
    // This is a dependency injection point
    constexpr Example(PinConfigureInterface & can_rx,
        PinConfigureInterface & can_tx,
        PinConfigureInterface & usb_d_minus,
        PinConfigureInterface & usb_d_plus):
        can_rx_(can_rx), can_tx_(can_tx),
        usb_d_minus_(usb_d_minus), usb_d_plus_(usb_d_plus),
        can_rx_pin_(PinConfigure::CreateInactivePin()),
        can_tx_pin_(PinConfigure::CreateInactivePin()),
        usb_d_minus_pin_(PinConfigure::CreateInactivePin()),
        usb_d_plus_pin_(PinConfigure::CreateInactivePin())
    {
        // Do constructor stuff here ...
    }
    // Then comes class methods. Order of defintion should be the same as the
    // interface.

    // You must put "override" for methods that you override from the interface.
    // If you don't expect someone to inherit and override this class's
    // implementation declare it "final".
    void Init() final override
    {
        // Do init stuff here ...
    }
    void DoSomeAction() final override
    {
        // Do some action stuff here ...
    }
    void SendData(const uint8_t * payload, bool readback = false) final override
    {
        // Ignore these two lines, these are here to suppress the "unused"
        // variable warning.
        (void)payload;
        (void)readback;
        // Send data here ...
    }
    void SetMode() final override
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
    PinConfigureInterface & can_rx_;
    PinConfigureInterface & can_tx_;
    PinConfigureInterface & usb_d_minus_;
    PinConfigureInterface & usb_d_plus_;
    // Then objects ...
    PinConfigure can_rx_pin_;
    PinConfigure can_tx_pin_;
    PinConfigure usb_d_minus_pin_;
    PinConfigure usb_d_plus_pin_;
    // Then primatives ...
};
