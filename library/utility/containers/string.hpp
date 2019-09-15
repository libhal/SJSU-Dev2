#pragma once

#include <string>

#include "utility/allocator.hpp"

namespace sjsu
{
/// ----------------------------------------------------------------------------
/// Using std::basic_string
/// ----------------------------------------------------------------------------
///
/// ```
/// sjsu::FixedAllocator<char, 128> fixed_allocator;
/// std::basic_string<char, std::char_traits<char>, decltype(fixed_allocator)>
///    str(fixed_allocator);
/// str.reserve(decltype(str)::allocator_type::size);
/// ```
///    or
///
/// ```
/// sjsu::String<128> str(sjsu::String<128>::allocator_type{});
/// str.reserve(decltype(str)::allocator_type::size - 1);
/// ```
///
/// ----------------------------------------------------------------------------
/// String Usage Notes
/// ----------------------------------------------------------------------------
/// The string in the above example can hold up to exactly 127 characters. Note
/// that the last character is used up for a null cahracter.
///
/// Reserve 1 minus the total size to leave space for the null character. When
/// specifying a reserve space, the string object will increment the reserve
/// amount by 1 to hold the null character.
///
/// The rest follows the "Vector Usage Notes".
template <size_t length>
using String = std::
    basic_string<char, std::char_traits<char>, FixedAllocator<char, length>>;
}  // namespace sjsu
