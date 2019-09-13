#pragma once

#include <vector>

#include "utility/allocator.hpp"
namespace sjsu
{
/// ----------------------------------------------------------------------------
/// Using std::vector
/// ----------------------------------------------------------------------------
///
/// ```
/// sjsu::FixedAllocator<int, 8> fixed_allocator;
/// std::vector<int, decltype(fixed_allocator)> vec(fixed_allocator);
/// vec.reserve(decltype(vec)::allocator_type::size);
/// ```
///
///    or
///
/// ```
/// sjsu::Vector<int, 8> vec(sjsu::Vector<int, 8>::allocator_type{});
/// vec.reserve(decltype(vec)::allocator_type::size);
/// ```
///
/// ----------------------------------------------------------------------------
/// Vector Usage Notes
/// ----------------------------------------------------------------------------
/// The fixed allocator can hold up to exactly 8 elements in this example.
/// Resize to any amount needed.
///
/// The call to reserve will allocate the maximum amount from the allocator.
/// This is very important as not doing so will cause vector to allocate an
/// arbitary amount of data, which may overflow our fixed buffer.
/// Doing this means we allocate as much space as we need. Allocator will fault
/// if we attempt to allocate beyond this.
template <class T, const size_t length>
using Vector = std::vector<T, FixedAllocator<T, length>>;
}  // namespace sjsu
