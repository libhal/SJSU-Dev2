#pragma once

#include "L1_Peripheral/example/example.hpp"
#include "L2_HAL/example.hpp"

// 1. Create a structure that acts as a namespace for your board objects and
// functions. Name it after the board you are porting
struct sjtwo  // NOLINT
{
  // 2. Define L1 commonly used peripherals. Only L1s are allowed here as they
  //    will be garbage collected by the linker if no other part of the code
  //    references them.
  inline static sjsu::example::Example example0 =
      sjsu::example::Example(sjsu::example::Example::Channel::kExample0);

  // 3. Define L2 peripherals as single instance objects that can be returned
  //    from functions. This way, example0 doesn't get reference in our code
  //    unless this method is called. This allows us to keep the binary size
  //    small if we are not using this object in our project.
  inline sjsu::ExampleHal & Oled()
  {
    static sjsu::ExampleHalImplementation example_hal(example0);
    return example_hal;
  }
};
