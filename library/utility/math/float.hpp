#pragma once

#include <cmath>

namespace sjsu
{
/// Compare two numbers
///
/// @param array - pointer to an array of numeric values
/// @param size - the number of elements in the array
/// @return the average as a floating point value.
constexpr bool Equal(float a, float b, float epsilon = 0.00001f)
{
  return fabsf(a - b) <= epsilon;
}
}