#pragma once

#include <cmath>

namespace sjsu
{
/// Compare two numbers
///
/// @param a - float to compare A
/// @param b - float to compare B
/// @param epsilon - The precision in which to compare to the two A and B
/// @return true if the floats equal each other within the precision of epsilon
constexpr bool Equal(float a, float b, float epsilon = 0.00001f)
{
  return fabsf(a - b) <= epsilon;
}
}  // namespace sjsu
