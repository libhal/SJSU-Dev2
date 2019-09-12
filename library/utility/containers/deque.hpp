#pragma once

#include <deque>

#include "utility/allocator.hpp"

#if defined(__GLIBCXX__)
#define DEQUE_BUFFER_SIZE(type) std::__deque_buf_size(sizeof(T))
#else
#warning \
    "Could not determine std::deque<T> buffer size, so assuming size 512 * " \
    "sizeof(T)"
#define DEQUE_BUFFER_SIZE(type) 512 * sizeof(type)
#endif

namespace sjsu
{
/// ----------------------------------------------------------------------------
/// Using std::deque
/// ----------------------------------------------------------------------------
///
/// ```
/// sjsu::FixedAllocator<int, 8 * std::__deque_buf_size(sizeof(int))>
///       fixed_list_allocator;
/// std::deque<int, decltype(fixed_list_allocator)> deque(fixed_list_allocator);
/// ```
///
///    or
///
/// ```
///    sjsu::Deque<int, 8> list(sjsu::Deque<int, 8>::allocator_type{});
/// ```
///
/// ----------------------------------------------------------------------------
/// Deque Usage Notes
/// ----------------------------------------------------------------------------
/// The example list can store up to roughly 8 elements. Resize to your needs.
/// Deque allocates arrays in block chunks. When it allocates greatly depends on
/// how items were inserted or removed from the deque. Thus the memory allocated
/// by the allocator cannot be precise as it is with the other containers. You
/// may end up with less memory then needed or a lot of extra memory.
template <class T, const size_t length>
using Deque =
    std::deque<T, FixedAllocator<T, length * DEQUE_BUFFER_SIZE(T)>>;
}  // namespace sjsu

#undef DEQUE_BUFFER_SIZE
