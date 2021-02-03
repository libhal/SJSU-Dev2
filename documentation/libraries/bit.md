# Bit Manipulation API

SJSU-Dev2 has a standalone bit manipulation utility in the directory
`library/utility/bit.hpp`. This guide will go over why it should be used and how
to use it.

## Why do we need bit manipulation

Bit manipulation is the means to changing the specific bits of a value without
effecting the other bits in that value. Bit manipulation is used a lot in the
field of embedded applications as information tends to either be mapped to
specific bits within a register or a blob of data, or when attempting to
configure a peripheral by setting specific bits or fields of bits to specific
values. Take a look at any of the peripheral drivers for the MCUs and you will
see that they all utilize bit manipulation.

Example of bit manipulation could look like this:

```C++
REG->CONTROL &= ~(1 << 15);
```

This particular block of code will clear (or also known as set to zero) the 15th
bit of this CONTROL register within this REG structure.

```C++
REG->CONTROL = (REG->CONTROL & ~(0xFFFF << 4)) | (0xABC << 4);
```

This example places the value `0xABC` at bit position 4 on to bit position 15.
The first half using the `&` operator clears those bits and the second half with
the `|` places the `0xABC` into the appropriate position.

It may be good to brush up/learn how to perform bitwise operations before
proceeding.

## Why make an API for this

Direct bitwise manipulation is a programming techniques that is:

1. Writability: Prone to mistakes
2. Readability: Hard to reason about
3. Debuggability: Can be subtly wrong

To give credence to these statements lets evaluate the following examples:

```C++
REG->CONTROL = (REG->CONTROL & (0xFFFF << 4)) | (0xABC << 4);
```

Now whats wrong with this example? If you look at the `(0xFFFF << 4)` area, you
will see that we are missing a `~`. Without this, all other bits in the
`CONTROL` register will be zeroed out. The only bits left in tact are the ones
we meant to manipulate. Looking at our list of issues how does this mistake fit?

1. **Writability**: One could imagine that forgetting to apply a `~` in the
   correct location is a reasonably easy typo to make.
2. **Readability**: For experienced embedded software developers this is a very
   familiar pattern, but at the same time, just like it is easy to forget to
   apply the `~`, it is easy to overlook the missing `~`. This increases the
   need to reason about the correctness of this line.
3. **Debuggability**: This example is a good type of failure mode because this
   is easy to detect in unit tests. At runtime there is a potential that wiping
   out the other bits doesn't crash the program but changes the configurations
   enough to make something look like it's kind of working but not quite.
   Additionally, this can lead developers to think that the issue could be with
   hardware or some other part of the code.

Now lets throw in a curve ball. Lets say we fix this and add back in the `~`.
Is there anything wrong with it?

```C++
REG->CONTROL = (REG->CONTROL & ~(0xFFFF << 4)) | (0xABC << 4);
```

It is likely most will assume no, it seems fine. But what if the actual width of
the bit field isn't 16 bits as indicated by the `0xFFFF`, but that it was 12
bits, thus the mask should be `0xFFF`. The current implementation will clear
out additional bits outside of the desired field. One could make that guess by
the value `0xABC`. But the data being put into the field can tell you nothing
about the size of the field, unless the width of the input is larger than the
clearing mask (i.e. `value = 0xABCD` and `mask = 0xFF`).

Again, this to is not easy to reason about, is easy to typo, and is hard to
notice at runtime. Unit test code could also miss this, if the other fields in
the value are not evaluated as well.

## Bit API

A wishlist of features for a bit manipulation library would be the following:

1. Easy and efficient way to construct bit masks at compile time
2. Set of APIs for performing:
    1. Bit Set: single bit set to value 1.
    2. Bit Clear: single bit set to value 0.
    3. Bit Toggle: single bit flipped from 0 to 1 or 1 to 0.
    4. Bit Read: read single bit
    5. Value Extraction: extract multiple bits from a location in a value
    6. Signed Value Extraction: extract multiple bits from a location in a value
       and sign extend it
    7. Value Insert: insert multiple bits into a specific location in a value
3. All APIs must be constexpr so that these APIs can be used at compile time
4. Expressive means to construct a value with bit masks and bit apis.

### Constructing bit::Mask(s)

The bit API establishes a `bit::Mask` structure that contains the bit position
and width of a field within a value. This mask can be used with the bit
manipulation APIs in order to specify the location of the bits needed to be
modified. There are a couple of ways to construct `bit::Mask` objects.
Each area below will go over these methods and when they should be used.
The idea of when one would have to use one method or another is simple: it is
the amount of cognitive work required to verify if a bit mask is correct.

#### 1. bit::Mask{}

It is less common to need to use this, but is helpful in certain situations.
The implementation looks like this:

```C++
bit::Mask mask = bit::Mask{.position = 5, .width = 3};
// OR
auto mask      = bit::Mask{.position = 5, .width = 3};
```

In this case, we are constructing the data structure directly. This method
should be used if the reference material documents their registers or data
blocks using the starting bit position and the width of the field. This way,
cognitive load is reduced on the developer because they can simple cross
reference the bit position and width in the reference material and the code to
see if they match or not.

#### 2. bit::MaskFromRange()

This is the most used API for generating bit masks in SJSU-Dev2 because most
reference material (data sheets, user manuals, technical specifications, etc.)
use the paradigms of bit start and bit end position. With that in mind, there
are two APIs for generating masks for this.

```C++
constexpr Mask MaskFromRange(uint32_t low_bit_position,
                             uint32_t high_bit_position);
constexpr Mask MaskFromRange(uint32_t bit_position);
```

Usage looks like:

```C++
// Creates a mask that starts at bit position 3 and ends at bit position 7, or
// in other words, has a bit width of 5 bits.
bit::Mask mask = bit::MaskFromRange(3, 7);
// Creates a bit mask with a single bit position at 8, width 1.
bit::Mask single_bit_mask = bit::MaskFromRange(8);

// OR (preferred)
auto mask = bit::MaskFromRange(3, 7);
auto single_bit_mask = bit::MaskFromRange(8);
```

### Basic Bit Manipulation APIs

The bit API has the following constexpr functions:

1. bit::Read()
2. bit::Set()
3. bit::Clear()
4. bit::Toggle()
5. bit::Extract()
6. bit::SignedExtract()
7. bit::Insert()

These APIs do as you would expect. Each takes a value as an input, along with a
bit mask and will return a value with that change.

For more details about how each works, see
[bit.hpp](https://kammce.github.io/SJSU-Dev2/api/html/d3/d51/bit_8hpp.html).

A quick example to demonstrate how each could be used:

```C++
static constexpr auto kReady = bit::MaskFromRange(10);
while(!bit::Read(REG->STATUS, kReady))
{
  continue;
}
```

```C++
static constexpr auto kEnablePeripheral = bit::MaskFromRange(2);
REG->CONTROL = bit::Set(REG->CONTROL, kEnablePeripheral);
```

```C++
static constexpr auto kInterruptFlag = bit::MaskFromRange(4);
REG->INTERRUPT = bit::Clear(REG->INTERRUPT, kInterruptFlag);
```

```C++
// Here we are creating a mask at runtime based on the pin number returned from
// the gpio_pin object.
GPIO->STATE = bit::Toggle(GPIO->STATE, bit::MaskFromRange(gpio_pin.GetPin()));
```

```C++
static constexpr auto kOperatingMode = bit::MaskFromRange(4, 7);
REG->STATE = bit::Extract(REG->STATE, kOperatingMode);
```

```C++
static constexpr auto kPllMultiply = bit::MaskFromRange(15, 21);
PLL->CONFIG = bit::Insert(PLL->CONFIG, 0x5, kPllMultiply);
```

### bit::StreamExtract(), Bit Extract from an Array

`bit::StreamExtract()` works the same way as `bit::Extract()` except that an
array of bytes can passed to `bit::StreamExtract()`.

See the
[sd card](https://github.com/SJSU-Dev2/SJSU-Dev2/blob/master/library/devices/memory/sd.hpp)
implementation source code for an example of its usage.

## bit::Register & bit::Value Class

Along with the functional APIs, SJSU-Dev2 also provides classes for manipulating
and constructing bit values in an expressive and efficient manner.

### bit::Register

One of the issues with using the functional bit APIs is that it is easy to
produce a typo by doing the following:

```C++
static constexpr auto kPllMultiply = bit::MaskFromRange(15, 21);
PLL->CONFIG = bit::Insert(REG->STATE, 0x5, kPllMultiply);
```

The input register is not the same as the output register. This is not a bug or
mistake in the API. A developer may want to use to a different input value,
such as a local variable, as the input to `Insert()` rather than reading from
the register directly. But when the source and destination should be the same,
this posses a problem.

The `Register` class comes in to prevent this. It has the same functional APIs
as the bit API but all nested within a single class. For example:

```C++
bit::Register(&PLL->CONFIG)
    .Insert(0x5, kPllMultiply)
    .Insert(0x2, kPllDivide)
    .Set(kPllEnable)
    .Save();
```

Here you can see that we are performing method chaining in order to manipulate
the contents of `PLL->CONFIG`, but note the `Save()` method at the end. Here is
what is happening.

1. `bit::Register` is constructed with a pointer to the register you want to
   manipulate
2. `bit::Register` will store the address of the register and will copy its
   contents to an internal cache variable.
3. When `Insert()` and `Set()` are used, they manipulate the cached variable.
4. Once all of the modifications are in place, the final step is to `Save()` the
   contents of the cache variable into register.

!!! WARNING
    The use of a cache variable is a very important optimization. Repeatedly
    modifying the register directly will require that each modification generate
    a bus cycle. A bus cycle is where data must leave the CPU in order to talk
    to hardware outside of itself and this takes longer than simply modifying
    registers or CPU internal cache memory. Multiple bus cycles results in a
    lower performance code and requires more instructions to be emitted bloating
    the size of the binary. `Save()` should be used as few times as possible to
    improve runtime performance and to reduce binary size.

Register objects can be saved into a variable for multiple uses. This is a good
practice if the register needs to be used multiple times within a function or
class method.

```C++
auto config = bit::Register(&PLL->CONFIG);
```

!!! NOTE
    In order to keep RAM usage low, only store `Register` objects as local
    variables within functions. The performance cost of constructing a
    `Register` is very low and does not warrant wasting space by storing them
    as member variables or as static variables.

### bit::Value

Value is used to generate fully to mostly constructed register values in an
expressive way. Works in the same way as the `bit::Register` class, except
that it only takes an initial value. It can also be implicitly converted from a
`bit::Value` object into its underlying type (defaults to `uint32_t`).

```C++
// Setup the DMA channel with all of the options specific to handling UART
static constexpr uint32_t kDmaSettings =
  bit::Value(0)
      .Clear(DmaReg::kTransferCompleteInterruptEnable)
      .Clear(DmaReg::kHalfTransferInterruptEnable)
      .Clear(DmaReg::kTransferErrorInterruptEnable)
      .Clear(DmaReg::kDataTransferDirection)  // Read from peripheral
      .Set(DmaReg::kCircularMode)
      .Clear(DmaReg::kPeripheralIncrementEnable)
      .Set(DmaReg::kMemoryIncrementEnable)
      .Insert(0b00, DmaReg::kPeripheralSize)  // size = 8 bits
      .Insert(0b00, DmaReg::kMemorySize)      // size = 8 bits
      .Insert(0b10,
              DmaReg::kChannelPriority)  // Low Medium [High] Very_High
      .Clear(DmaReg::kMemoryToMemory)
      .Set(DmaReg::kEnable);

// Only a single assignment needed to set the DMA settings for the peripheral
port_.dma->CCR = DmaReg::kDmaSettings;
```

Here we construct the DMA settings for a UART peripheral on the stm32f10x MCU.
Since the settings do not change between UART peripherals, this particular value
can be created at compile time in this expressive manner. It will only be made
a real variable when used, and because it is static and constexpr, the compiler
will tend to not even statically allocate space for the value, but instead,
embed the value into the instructions of the code. No ram, no stack, and
nearly no ROM usage.

It is a best practice to use this for peripheral settings that can be known at
compile time.

## Finding examples

The entire codebase of SJSU-Dev2 uses the bit APIs. Some of the older codes and
tests still uses direct bit manipulation, but going forward everything will go
through the bit API. A good place to find usage of the bit APIs are in the
`peripherals/<insert mcu name here>` directories. Each MCU implementation of a
peripheral will need to perform bit manipulation, thus they are a hot spot its
usage.
