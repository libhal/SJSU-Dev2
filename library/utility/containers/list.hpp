#pragma once

// Serves the purpose of defining _LIBCPP_VERSION and literally nothing else.
// _LIBCPP_VERSION is used to detect if we are being compiled for clang or gcc
#include <ciso646>
#include <list>

#include "utility/allocator.hpp"

namespace sjsu
{
/// Fallback list Node for compilers not defined in the macro below.
template <class T>
struct FallbackListNode_t
{
  /// Pointer to the previous value.
  void * a;
  /// Pointer to the next value.
  void * b;
  /// Node value
  T c;
};

#if defined(__GLIBCXX__)
#define LIST_NODE_TYPE(type) std::_List_node<type>
#elif defined(_LIBCPP_VERSION)
#define LIST_NODE_TYPE(type) std::__list_node<type, void *>
#else
#warning \
    "Could not determine std::list<T> node size, assuming sizeof(T)+2*sizeof(void*)"  // NOLINT
#define LIST_NODE_TYPE(type) FallbackListNode_t<type>
#endif
/// ----------------------------------------------------------------------------
/// Using std::list
/// ----------------------------------------------------------------------------
///
/// ```
/// sjsu::FixedAllocator<int, 8, std::_List_node<int>> fixed_list_allocator;
/// std::list<int, decltype(fixed_list_allocator)> list(fixed_list_allocator);
/// ```
///
///    or
///
/// ```
/// sjsu::List<int, 8> list(sjsu::List<int, 8>::allocator_type{});
/// ```
///
/// ----------------------------------------------------------------------------
/// List Usage Notes
/// ----------------------------------------------------------------------------
/// The example list can store up to exactly 8 elements. Resize to your needs.
/// Lists allocate nodes individually and never allocate more then a single
/// list node size, thus no need to pre-allocate memory.
///
/// The FixedAllocator's third template parameter must be
/// std::_List_node<typename>. This 3rd parameter specifies the type of the
/// element that will be allocated by the allocator. For each allocation,
/// std::list will allocate a std::_List_node<typename> and not the original
/// type of the list value.
template <class T, const size_t length>
using List = std::list<T, FixedAllocator<T, length, LIST_NODE_TYPE(T)>>;
}  // namespace sjsu

#undef LIST_NODE_TYPE
