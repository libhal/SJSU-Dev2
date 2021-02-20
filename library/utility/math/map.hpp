#pragma once

#include <type_traits>

namespace sjsu
{
/// Remap a numeric value from one range to another.
///
/// Example 1:
///
///   value = 5
///   current_minimum = 0, current_maximum = 10
///   new_minimum = 0, new_maximum = 100
///   result = 50
/// ```
///                            50
///   |--------------------------------------------------|
///   0                        |                         100
///       _____________________/
///      /
///      |
///   |-----|
///   0  5  10
/// ```
///
/// Example 2:
///
///   value = 7
///   current_minimum = 0, current_maximum = 10
///   new_minimum = 100, new_maximum = 200
///   result = 170
///
/// ```
///                                                          170
///                      |--------------------------------------------------|
///                    100                                   |              200
///        __________________________________________________/
///       /
///       |
///   |-----|
///   0   7 10
/// ```
///
/// Mapping equation is as follows:
///
/// ```
///               /                    (max - min)     \_
///      value = | (value - min) x -------------------  | + new_min
///               \                (new_max - new_min) /
/// ```
///
/// @param value - the value that will be mapped to the new range
/// @param min - current minimum value that the value can reach
/// @param max - current maximum value that the value can reach
/// @param new_min - the new minimum value to scale and shift the old value to
/// @param new_max - the new maximum value to scale and shift the old value to
template <typename Input,
          typename RangeMin,
          typename RangeMax,
          typename NewRangeMin,
          typename NewRangeMax>
constexpr NewRangeMax Map(Input value,
                          RangeMin min,
                          RangeMax max,
                          NewRangeMin new_min,
                          NewRangeMax new_max)
{
  static_assert(std::is_arithmetic<Input>::value,
                "Input value variable type must be an arithmetic type (like "
                "int, char, float, etc).");
  static_assert(std::is_arithmetic<RangeMin>::value,
                "min and max variable types must be an arithmetic type (like "
                "int, char, float, etc).");
  static_assert(std::is_arithmetic<RangeMax>::value,
                "min and max variable types must be an arithmetic type (like "
                "int, char, float, etc).");
  static_assert(std::is_arithmetic<NewRangeMin>::value,
                "new_min and new_max variable types must be an arithmetic type "
                "(like int, char, float, etc).");
  static_assert(std::is_arithmetic<NewRangeMax>::value,
                "new_min and new_max variable types must be an arithmetic type "
                "(like int, char, float, etc).");

  float map_value   = static_cast<float>(value);
  float map_min     = static_cast<float>(min);
  float map_max     = static_cast<float>(max);
  float map_new_min = static_cast<float>(new_min);
  float map_new_max = static_cast<float>(new_max);
  float range_ratio = (map_new_max - map_new_min) / (map_max - map_min);
  float mapped      = ((map_value - map_min) * range_ratio) + map_new_min;

  return static_cast<NewRangeMax>(mapped);
};
}  // namespace sjsu
