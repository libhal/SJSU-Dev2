#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace sjsu
{
namespace bit
{
/// Definition of a bit mask. Can be used in various bit manipulation functions
/// to specify which bits to be modified.
struct Mask  // NOLINT
{
  /// Starting position of the bitfield
  uint8_t position;
  /// Number of bits of the bitfield
  uint8_t width;
};
/// @param low_bit_position - the starting bit of the bit field.
/// @param high_bit_position - the last bit of the bit field.
/// @return constexpr Mask from the low bit position to the high bit position.
///         If the low_bit_position > high_bit_position, the result is
///         undefined.
constexpr Mask CreateMaskFromRange(uint8_t low_bit_position,
                                   uint8_t high_bit_position)
{
  return Mask({
      .position = low_bit_position,
      .width = static_cast<uint8_t>(1 + (high_bit_position - low_bit_position)),
  });
}
/// @param bit_position - bit field composed of a single bit with bit width 1.
/// @return a bit mask with of width 1 and position = bit_position.
constexpr Mask CreateMaskFromRange(uint8_t bit_position)
{
  return Mask({
      .position = bit_position,
      .width    = 1,
  });
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
[[nodiscard]] constexpr T Extract(T target,
                                  uint32_t position,
                                  uint32_t width = 1)
{
  // Check the types at compile time
  static_assert(std::numeric_limits<T>::is_integer,
                "Extract only accepts intergers.");
  // Need to use an unsigned version of the type T for the mask to make sure
  // that the shift right doesn't result in a sign extended shift.
  using UnsignedT = typename std::make_unsigned<T>::type;
  // At compile time, generate variable containing all 1s with the size of the
  // target parameter.
  constexpr UnsignedT kFieldOfOnes = std::numeric_limits<UnsignedT>::max();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask = kFieldOfOnes >> (kTargetWidth - width);
  // Shift target down to the right to get the bit position in the 0th bit
  // location.
  // Mask the value to clear any bit after the width's amount of bits.
  return static_cast<T>((target >> position) & mask);
}
template <typename T>
[[nodiscard]] constexpr T Extract(T target, Mask bitmask)
{
  return Extract(target, bitmask.position, bitmask.width);
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
[[nodiscard]] constexpr T Insert(T target,
                                 U value,
                                 uint32_t position,
                                 uint32_t width = 1)
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
  constexpr UnsignedT kFieldOfOnes = std::numeric_limits<UnsignedT>::max();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask = kFieldOfOnes >> (kTargetWidth - width);
  // Clear width's number of bits in the target value at the bit position
  // specified.
  target &= ~(mask << position);
  // AND value with mask to remove any bits beyond the specified width.
  // Shift masked value into bit position and OR with target value.
  target |= (value & mask) << position;
  return static_cast<T>(target);
}
template <typename T, typename U>
[[nodiscard]] constexpr T Insert(T target, U value, Mask bitmask)
{
  return Insert(target, value, bitmask.position, bitmask.width);
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
[[nodiscard]] constexpr T Set(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Set only accepts intergers.");
  return static_cast<T>(target | (1 << position));
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
[[nodiscard]] constexpr T Clear(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Clear only accepts intergers.");
  return static_cast<T>(target & ~(1 << position));
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
[[nodiscard]] constexpr T Toggle(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Toggle only accepts intergers.");
  return static_cast<T>(target ^ (1 << position));
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
/// return   =               true
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to toggle
template <typename T>
[[nodiscard]] constexpr bool Read(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Read only accepts intergers.");
  return static_cast<bool>(target & (1 << position));
}

/// @returns a value that is the target value with the bit set to a 1 at the bit
/// position within the bitmask. For exmaple if your bitmask has field position
/// set to 5, then this function will return the target value with the 5th bits
/// set to a 1.
template <typename T>
[[nodiscard]] constexpr T Set(T target, Mask bitmask)
{
  return Set(target, bitmask.position);
}
/// Operates the same way as the Set(T target, Mask bitmask) function except it
/// clears the bit.
template <typename T>
[[nodiscard]] constexpr T Clear(T target, Mask bitmask)
{
  return Clear(target, bitmask.position);
}
/// Operates the same way as the Set() function except it toggles the bit.
template <typename T>
[[nodiscard]] constexpr bool Toggle(T target, Mask bitmask)
{
  return Toggle(target, bitmask.position);
}
/// @returns the bit in the value at the "position" field of the bitmask. For
/// exmaple, if the passed bitmask has position set to 5, then this function
/// will return the 5th bits value, regardless of the "width" field is.
template <typename T>
[[nodiscard]] constexpr bool Read(T target, Mask bitmask)
{
  return Read(target, bitmask.position);
}
}  // namespace bit
}  // namespace sjsu
