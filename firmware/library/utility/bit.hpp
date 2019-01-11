// @ingroup SJSU-Dev2
// @defgroup Bit manipulation library
// @brief This library contains helper methods for manipulating or extracting
// bits from a numeric values.
// @{
#pragma once

#include <cstdint>
#include <limits>

namespace bit
{
template <typename T>
constexpr T GenerateFieldOfOnes()
{
  T result = 0;
  for (size_t i = 0; i < sizeof(T); i++)
  {
    result |= 0xFF << (i * 8);
  }
  return result;
}
/// Extract a set of contiguous bits from a target value.
///
/// target   =        0x00FE'DCBA
///                            ^
///                           /
///                          /
/// value  = 4 -----------+
/// width    = 8
///
/// return   = 0xCB
///
/// @param target the target containing the bits to be extracted.
/// @param position the starting position of the bits to extracted.
/// @param width the number of bits from the starting position to be extracted.
template <typename T>
[[gnu::always_inline]][[nodiscard]]
constexpr T Extract(T target, uint32_t position, uint32_t width = 1)
{
  // Check the types at compile time
  static_assert(std::numeric_limits<T>::is_integer,
                "Extract only accepts intergers.");
  // Need to use an unsigned version of the type T for the mask to make sure
  // that the shift right doesn't result in a sign extended shift.
  using UnsignedT = typename std::make_unsigned<T>::type;
  // At compile time, generate variable containing all 1s with the size of the
  // target parameter.
  constexpr UnsignedT kFieldOfOnes = GenerateFieldOfOnes<T>();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth     = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask   = kFieldOfOnes >> (kTargetWidth - width);
  // Shift target down to the right to get the bit position in the 0th bit
  // location.
  // Mask the value to clear any bit after the width's amount of bits.
  return (target >> position) & mask;
}
/// Insert a set of continguous bits into a target value.
///
/// target   =        0xXXXX'XXXX
///                        ^
///                       /
///                      /
/// value   = 0xABCD --+
/// position = 16
/// width    = 16
///
/// return   =        0xABCD'XXXX
///
/// @param target the target that will have bits inserted into it.
/// @param value the bits to be inserted into the target
/// @param position the position in the target to insert the value of bits.
/// @param width the length of bits that will be overwritten in the target.
template <typename T, typename U>
[[gnu::always_inline]][[nodiscard]]
constexpr T Insert(T target, U value, uint32_t position, uint32_t width = 1)
{
  // Check the types at compile time
  static_assert(
      std::numeric_limits<T>::is_integer,
      "1st parameter (target) of Insert function must be an interger.");
  static_assert(
      std::numeric_limits<U>::is_integer,
      "2nd parameter (value) of Insert function must be an interger.");
  static_assert(sizeof(T) >= sizeof(U),
                "2nd parameter (value) must be smaller than or equal to the "
                "size of the target value.");
  // Need to use an unsigned version of the type T for the mask to make sure
  // that the shift right doesn't result in a sign extended shift.
  using UnsignedT = typename std::make_unsigned<T>::type;
  // At compile time, generate variable containing all 1s with the size of the
  // target parameter.
  constexpr UnsignedT kFieldOfOnes = GenerateFieldOfOnes<T>();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth     = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask   = kFieldOfOnes >> (kTargetWidth - width);
  // Clear width's number of bits in the target value at the bit position
  // specified.
  target &= ~(mask << position);
  // AND value with mask to remove any bits beyond the specified width.
  // Shift masked value into bit position and OR with target value.
  target |= (value & mask) << position;
  return target;
}
/// Set a bit in the target value at the position specifed to a 1 and return
///
/// target   =        0b0000'1001
///                        ^
///                       /
///                      /
/// position = 4 -------+
///
/// return   =        0b0001'1001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to change to 1
template <typename T>
[[gnu::always_inline]] [[nodiscard]]
constexpr T Set(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Set only accepts intergers.");
  return target | (1 << position);
}
/// Set a bit in the target value at the position specifed to a 0 and return
///
/// target   =        0b0000'1001
///                          ^
///                         /
///                        /
/// position = 3 ---------+
///
/// return   =        0b0000'0001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to change to 0
template <typename T>
[[gnu::always_inline]][[nodiscard]] constexpr T Clear(T target,
                                                      uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Clear only accepts intergers.");
  return target & ~(1 << position);
}
/// Toggle a bit in the target value at the position specifed.
/// If the bit was a 1, it will be changed to a 0.
/// If the bit was a 0, it will be changed to a 1.
///
/// target   =        0b0000'1001
///                          ^
///                         /
///                        /
/// position = 3 ---------+
///
/// return   =        0b0000'0001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to toggle
template <typename T>
[[gnu::always_inline]][[nodiscard]] constexpr T Toggle(T target,
                                                       uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Toggle only accepts intergers.");
  return target ^ (1 << position);
}

/// Read a bit from the target value at the position specifed.
/// If the bit is 1 at the position given, return true.
/// If the bit is 0 at the position given, return false.
///
/// target   =        0b0000'1001
///                          |
///                         /|
///                        / |
/// position = 3 ---------+  |
///                         \|/
///
/// return   =               1
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to toggle
template <typename T>
[[gnu::always_inline]][[nodiscard]] constexpr bool Read(T target,
                                                        uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Read only accepts intergers.");
  return static_cast<bool>(target & (1 << position));
}
}  // namespace bit
// @}
