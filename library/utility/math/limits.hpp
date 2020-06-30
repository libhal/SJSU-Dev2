#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace sjsu
{
/// BitLimits defines compile-time functions that provides Min() and Max()
/// functions which calculate the minimum and maximum possible values for an
/// integer of some number of bits that can fit within the integer type
/// supplied.
///
/// @tparam bitwidth - the number of bits represented in the value.
/// @tparam IntType - the container integer for the type.
template <uint8_t bitwidth, typename IntType>
class BitLimits
{
 public:
  // Check that the IntType is actually an integer.
  static_assert(
      std::is_integral_v<IntType>,
      "Type must be an integer type such as int8_t, int16_t, uint32_t, etc.");

  // Check that the bitwidth is less than or equal to the size of the IntType.
  static_assert(
      bitwidth <= sizeof(IntType) * 8,
      "The bitwidth exceed the number of bitwidth in the integer type.");

  // Check that bitwidth is not zero.
  static_assert(bitwidth != 0, "The bitwidth cannot be 0.");

  /// @return constexpr IntType - returns the maximum value available for an
  /// integer of `bitwidth` size and that can be stored within `IntType`.
  /// The final value of the function depends also on the sign of the int type.
  static constexpr IntType Max()
  {
    if constexpr (bitwidth == 64)
    {
      return std::numeric_limits<IntType>::max();
    }
    else if constexpr (std::is_signed_v<IntType>)
    {
      IntType max = static_cast<IntType>(((1ULL << bitwidth) / 2ULL) - 1ULL);
      return max;
    }
    else
    {
      IntType max = static_cast<IntType>((1ULL << bitwidth) - 1ULL);
      return max;
    }
  }

  /// @return constexpr IntType - returns the minimum value available for an
  /// integer of `bitwidth` size and that can be stored within `IntType`.
  /// The final value of the function depends also on the sign of the int type.
  /// Unsigned ints simply return zero.
  static constexpr IntType Min()
  {
    if constexpr (bitwidth == 64)
    {
      return std::numeric_limits<IntType>::min();
    }
    else if constexpr (std::is_signed_v<IntType>)
    {
      IntType min = static_cast<IntType>(((1ULL << bitwidth) / 2ULL) * -1ULL);
      return min;
    }
    else
    {
      return 0U;
    }
  }
};
}  // namespace sjsu
