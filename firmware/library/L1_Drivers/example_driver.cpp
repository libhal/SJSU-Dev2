// Don't forget to import this
#include "L1_Drivers/example_driver.hpp"
// This file should only be used to initialize/link things that otherwise
// could not be defined in the header. Cpp and C files provide hard links to
// variables where as header files do not (as much).

// This part here is very important, since it links the static usb variable to
// the register LPC_USB. During testing you would swap this out with your own
// LPC_USB_TypeDef object like so:
//
//      LPC_USB_TypeDef local_usb = {0};
//      Example::usb = &local_usb;
LPC_USB_TypeDef * Example::usb = LPC_USB;
LPC_CAN_TypeDef * Example::can[2] = { LPC_CAN1, LPC_CAN2 };
