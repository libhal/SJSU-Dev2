#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include "L1_Peripheral/example.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/status.hpp"

// 0. Everything in the library folder must be within the sjsu namespace.
namespace sjsu
{
namespace example
{
// 1. All L1_Peripherals must be final
// L1_Peripherals implementations should not be inherited further.
// This promotes object composition (HAS-A relationship) vs inheritance (IS-A)
class Example final : public sjsu::Example
{
 public:
  // 2. Pull in utility methods
  // Include utility methods from the parent interface class by using the
  // keyword "using" as shown below.
  using sjsu::Example::Read;
  using sjsu::Example::Write;
  // 3. Define enumerations, structures and constants here
  enum Registers
  {
    kId      = 0x45,
    kControl = 0x46,
    kPower   = 0x47,  // prefer to leave a trailing comma
  };

  struct [[gnu::packed]] ControlRegister_t
  {
    uint8_t enable : 1;
    uint8_t mode : 3;
    uint8_t reserved0 : 1;
    uint8_t speed : 2;
  };
  // 4. Predefined implementation detail structure
  // The predefined structures come with all of the information that the Example
  // driver needs in order to operate. Previously we used lookup tables for
  // this, but this had the issue of pulling in all of the information for every
  // single available instance of driver, which increases binary size.
  //
  // The predefined structures can be used to create a channel profile that
  // isn't already included in this set of Channel_t within the Channel struct.
  //
  // The structure is also an injetion point. You can have, in this case below,
  // the iocon pointer, point to a local version of LPC_IOCON_TypeDef and check
  // that it maniuplates the registers correctly.
  struct Channel_t
  {
    lpc40xx::LPC_IOCON_TypeDef * iocon;
    const sjsu::Pin & data;
    const sjsu::Pin & clock;
    uint8_t data_pin_function;
    uint8_t clock_pin_function;
  };
  struct Channel  // NOLINT
  {
    inline static const sjsu::lpc40xx::Pin kExamplePinData =
        sjsu::lpc40xx::Pin(4, 5);
    inline static const sjsu::lpc40xx::Pin kExamplePinClock =
        sjsu::lpc40xx::Pin(4, 6);

    inline static const Channel_t kExample0 = { .iocon =
                                                    sjsu::lpc40xx::LPC_IOCON,
                                                .data  = kExamplePinData,
                                                .clock = kExamplePinClock,
                                                .data_pin_function  = 0b100,
                                                .clock_pin_function = 0b100 };
    // ... Do the same with more predefined example structures
  };

  // 6. Constructors without Side effect
  // Constructors cannot cause any sort of side effect outside of the object.
  // This means that your constructor cannot and should not access hardware
  // registers or attempt to access some global resource. The class must be
  // entirely self contained.
  // Typically constructors only take 1 argument, 1 default argument or no
  // arguments if there is not something to customize.
  // If you are passing a "predefined implementation detail structure", be sure
  // to pass it as a const reference, otherwise undefined behavior will occur
  // resulting in a hard fault.
  explicit Example(const Channel_t & peripheral_details)
      : peripheral_details_(peripheral_details)
  {
  }
  // 7. Methods that override the parent's implementation must use the keyword
  // "override".
  Status Initialize() const override
  {
    // Add implementation here.
    // Example implementation where we set the functions of the pins.
    peripheral_details_.data.SetPinFunction(
        peripheral_details_.data_pin_function);
    peripheral_details_.clock.SetPinFunction(
        peripheral_details_.clock_pin_function);
    // This doesn't do anything special, but is used to illustrate test by side
    // effect in the L1_Peripheral/example/test/example_test.cpp file.
    peripheral_details_.iocon->P5_3 |= 0b1000'0000;
    // Change this line to the appropriate status.
    return Status::kNotImplemented;
  }
  bool Write(const uint8_t * data, size_t length) const override
  {
    // This is a trick to make the compiler think we used these variables.
    // Only using this for an example. Otherwise, use the [[maybe_unused]]
    // attribute.
    (void)data;
    (void)length;
    return true;
  }
  Status Read(uint8_t * data, size_t length, uint32_t timeout) const override
  {
    // This is a trick to make the compiler think we used these variables.
    // Only using this for an example. Otherwise, use the [[maybe_unused]]
    // attribute.
    (void)data;
    (void)length;
    (void)timeout;
    // Change this line to the appropriate status.
    return Status::kNotImplemented;
  }
  // 9. Set Hardware not local variables
  // Do not cache val
  void SetValue(uint32_t value) const override
  {
    (void)value;
  }
  void SetClockRate(uint32_t frequency) const override
  {
    (void)frequency;
  }
  // 9. Get From Hardware
  // The values returned from getters must come from hardware register memory.
  // Data must not be cached and retrieved from cache.
  float GetValue() const override
  {
    return 1.5f;
  }
  uint32_t GetClockRate() const override
  {
    return 1'000'000;
  }
  bool HasCompletedAction() const override
  {
    return true;
  }

 private:
  // This should be the only variable in the peripheral. If you need more
  // variables you can add them to the Channel_t type.
  const Channel_t & peripheral_details_;
};
}  // namespace example
}  // namespace sjsu
