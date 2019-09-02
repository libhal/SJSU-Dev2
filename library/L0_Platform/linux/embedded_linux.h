#pragma once

// If you are building for a linux operating system
// then include this set of peripheral and utility headers from the linux
// OS.
//
// If a peripheral is missing from this for a L1 driver, add to this list rather
// than adding the include directly to the peripheral header.
#if defined(__unix__)
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
// #include <linux/gpio.h>
#endif
