// @ingroup SJSU-Dev2
// @defgroup Bit manipulation library
// @brief This library contains helper methods for manipulating or extracting
// bits from a numeric values.
// @{
#pragma once

#include <cstdint>

namespace bit
{
/// Extract a set of contiguous bits from a target value.
///
/// target   =        0x00FE'DCBA
///                            ^
///                           /
///                          /
/// payload  = 4 -----------+
/// width    = 8
///
/// return   = 0xCB
///
/// @param target the target containing the bits to be extracted.
/// @param position the starting position of the bits to extracted.
/// @param width the number of bits from the starting position to be extracted.
template<typename T>
inline T Extract(T target, uint8_t position, uint8_t width = 1);
/// Insert a set of continguous bits into a target value.
///
/// target   =        0xXXXX'XXXX
///                        ^
///                       /
///                      /
/// payload  = 0xABCD --+
/// position = 16
/// width    = 16
///
/// return   =        0xABCD'XXXX
///
/// @param target the target that will have bits inserted into it.
/// @param payload the bits to be inserted into the target
/// @param position the position in the target to insert the payload of bits.
/// @param width the length of bits that will be overwritten in the target.
template<typename T>
inline T Insert(T target, uint8_t payload, uint8_t position,
                       uint8_t width = 1);
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
template<typename T>
inline T Set(T target, uint8_t position);
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
template<typename T>
inline T Clear(T target, uint8_t position);
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
template<typename T>
inline T Toggle(T target, uint8_t position);
}  // namespace bit
// @}
