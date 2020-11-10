# Math Utilities

## BitLimits

BitLimits defines compile-time functions that provides Min() and Max() functions
which calculate the minimum and maximum possible values for an integer of some
number of bits that can fit within the integer type supplied.

For example, lets say you want to get the maximum possible 12 bit signed  value.
To get this using BitLimits, you would do the following:

```C++
// First template parameter is the number of bits, 12, and the second is the
// integer container type to return the value in.
//
// A compile time error will be produced if the number of bits cannot fit within
// the passed integer type.
//
// The return type need not be `auto`.
constexpr auto kMax = sjsu::BitLimits<12, int16_t>::Max();
```

Example of getting the minimum value of a 23 bit signed integer:

```C++
constexpr auto kMax = sjsu::BitLimits<23, int32_t>::Min();
```

Also works with unsigned values

```C++
constexpr auto kMax = sjsu::BitLimits<22, uint32_t>::Max();
```

If you use an unsigned value and ask for its minimum you will always get zero.

```C++
constexpr auto kMin = sjsu::BitLimits<22, uint32_t>::Min();
```
