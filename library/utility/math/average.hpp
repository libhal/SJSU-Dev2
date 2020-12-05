// Average value returned can only go up to 7 numbers.
// Please ensure your average value does not exceed this limit.

#pragma once

#include <initializer_list>
#include <iterator>

namespace sjsu
{
/// Calculates the average of an array of numeric values of the same type.
/// This function can be used in a constexpr (compile time) context.
///
/// @tparam T - numeric type of the array
/// @param array - pointer to an array of numeric values
/// @param size - the number of elements in the array
/// @return the average as a floating point value.
template <typename T>
constexpr float Average(const T * array, size_t size)
{
  float sum    = 0;
  float length = static_cast<float>(size);

  for (size_t i = 0; i < size; i++)
  {
    sum += static_cast<float>(array[i]);
  }

  return sum / length;
}

/// Calculates the average of an array literal of numeric types.
///
/// @tparam T - numeric type of the array
/// @param array - array literal to convert into an average
/// @return the average as a floating point value.
template <typename T>
constexpr float Average(std::initializer_list<T> array)
{
  return Average(array.begin(), array.size());
}

/// Calculates the average of a C-style array where the length is knowable at
/// compile time. No need to pass in the size of the array.
///
/// Usage:
///
///    int array[] = { 1, 2, 3, 4, 5 };
///    Average(array);
///
/// This function can be used in a constexpr (compile time) context.
///
/// @tparam T - numeric type of the array
/// @tparam size - size of the array passed, (this will be inferred from the
/// passed array and should never be assigned manually)
/// @return constexpr float
template <typename T, size_t size>
constexpr float Average(const T (&array)[size])
{
  return Average(array, size);
}
}  // namespace sjsu
