// -----------------------------------------------------------------------------
// Notes about the allocator:
// -----------------------------------------------------------------------------
// The FixedAllocator is meant for containers that are commonly used that rarely
// or never deallocate. The allocator does not perform any actions when
// deallocating to keep the logic as simple and fast as possible. This means
// that memory is never returned when deallocating, thus, if you use a container
// like std::list, every allocation and deallocation results in the more space
// being used up.
//
// This allocate is best suited for:
//    - std::vector
//    - std::basic_string
//    - std::deque
//
// -----------------------------------------------------------------------------
// Usage with std::vector:
// -----------------------------------------------------------------------------
//
//    sjsu::FixedAllocator<int, 8> fixed_allocator;
//    std::vector<int, decltype(fixed_allocator)> vec(fixed_allocator);
//    vec.reserve(decltype(vec)::allocator_type::size);
//
//    or
//
//    sjsu::Vector<int, 8> vec(sjsu::Vector<int, 8>::allocator_type{});
//    vec.reserve(decltype(vec)::allocator_type::size);
//
// -----------------------------------------------------------------------------
// Vector Usage Notes:
// -----------------------------------------------------------------------------
// The fixed allocator can hold up to exactly 8 elements in this example. Resize
// to any amount needed.
//
// The call to reserve will allocate the maximum amount from the allocator.
// This is very important as not doing so will cause vector to allocate an
// arbitary amount of data, which may overflow our fixed buffer.
// Doing this means we allocate as much space as we need. Allocator will fault
// if we attempt to allocate beyond this.
//
// -----------------------------------------------------------------------------
// Usage with std::basic_string:
// -----------------------------------------------------------------------------
//
//    sjsu::FixedAllocator<char, 128> fixed_allocator;
//    std::basic_string<char, std::char_traits<char>, decltype(fixed_allocator)>
//        str(fixed_allocator);
//    str.reserve(decltype(str)::allocator_type::size);
//
//    or
//
//    sjsu::String<128> str(sjsu::String<128>::allocator_type{});
//    str.reserve(decltype(str)::allocator_type::size - 1);
//
// -----------------------------------------------------------------------------
// String Usage Notes:
// -----------------------------------------------------------------------------
// The string can hold up to exactly 127 characters. Note that the last
// character is used up for a null cahracter.
//
// Reserve 1 minus the total size to leave space for the null character. When
// specifying a reserve space, the string object will increment the reserve
// amount by 1 to hold the null character.
//
// The rest follows the "Vector Usage Notes".
//
// -----------------------------------------------------------------------------
// Usage with std::list:
// -----------------------------------------------------------------------------
//
//   sjsu::FixedAllocator<int, 8, std::_List_node<int>> fixed_list_allocator;
//   std::list<int, decltype(fixed_list_allocator)> list(fixed_list_allocator);
//
//    or
//
//    sjsu::List<int, 8> list(sjsu::List<int, 8>::allocator_type{});
//
// -----------------------------------------------------------------------------
// List Usage Notes:
// -----------------------------------------------------------------------------
// The example list can store up to exactly 8 elements. Resize to your needs.
// Lists allocate nodes individually and never allocate more then a single
// list node size, thus no need to pre-allocate memory.
//
// The FixedAllocator's third template parameter must be
// std::_List_node<typename>. This 3rd parameter specifies the type of the
// element that will be allocated by the allocator. For each allocation,
// std::list will allocate a std::_List_node<typename> and not the original type
// of the list value.
//
// -----------------------------------------------------------------------------
// Usage with std::deque:
// -----------------------------------------------------------------------------
//
//   sjsu::FixedAllocator<int, 8 * std::__deque_buf_size(sizeof(int))>
//           fixed_list_allocator;
//   std::deque<int, decltype(fixed_list_allocator)>
//           deque(fixed_list_allocator);
//
//    or
//
//    sjsu::Deque<int, 8> list(sjsu::Deque<int, 8>::allocator_type{});
//
// -----------------------------------------------------------------------------
// Deque Usage Notes:
// -----------------------------------------------------------------------------
// The example list can store up to roughly 8 elements. Resize to your needs.
// Deque allocates arrays in block chunks. When it allocates greatly depends on
// how items were inserted or removed from the deque. Thus the memory allocated
// by the allocator cannot be precise as it is with the other containers. You
// may end up with less memory then needed, or a lot of extra memory.
//
// -----------------------------------------------------------------------------
// All other containers have not yet been tested with the allocator.
// -----------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstdlib>

#include "utility/log.hpp"

namespace sjsu
{
class Arena
{
 public:
  Arena(uint8_t * buf, size_t size, uint8_t * ptr = nullptr) noexcept
      : buf_(buf), ptr_(ptr), size_(size), id_(arena_id++), d_ptr_(&ptr_)
  {
    if (ptr_ == nullptr)
    {
      ptr_ = buf_;
    }
  }
  /// NOTE: Arena's should never be shared between threads. This constructor
  /// should only be used by std::containers that need to copy an arena.
  Arena(const Arena & a)
      : buf_(a.buf_),
        ptr_(a.ptr_),
        size_(a.size_),
        id_(arena_id++),
        d_ptr_(const_cast<uint8_t **>(&a.ptr_))
  {
  }

  uint8_t * allocate(size_t requested_space)  // NOLINT
  {
    bool inside_of_allocatable_space = WithinAllocatableSpace(requested_space);
    if (inside_of_allocatable_space)
    {
      uint8_t * r = *d_ptr_;
      *d_ptr_ += requested_space;
      return r;
    }

    SJ2_ASSERT_FATAL(inside_of_allocatable_space,
                     R"(
(%d) Memory overflow when attempting to allocate memory for container.
buffer address     = %p
buffer_size        = %zu
allocation pointer = %p
requested space    = %zu
)",
                     id_,
                     buf_,
                     size_,
                     *d_ptr_,
                     requested_space);
    return nullptr;
  }
  /// Only moves back pointer if it is at the end
  void deallocate(uint8_t * pointer, size_t size) noexcept  // NOLINT
  {
    if (PointerInBuffer(pointer))
    {
      if (pointer + size == *d_ptr_)
      {
        *d_ptr_ = pointer;
      }
    }
  }

  size_t size() noexcept  // NOLINT
  {
    return size_;
  }
  size_t used() const noexcept  // NOLINT
  {
    return static_cast<size_t>(*d_ptr_ - buf_);
  }
  void reset() noexcept  // NOLINT
  {
    *d_ptr_ = buf_;
  }

  template <class T, size_t N, class U>
  friend class FixedAllocator;

 private:
  static inline int arena_id = 0;

  bool PointerInBuffer(uint8_t * p) noexcept
  {
    return buf_ <= p && p <= buf_ + size_;
  }
  bool WithinAllocatableSpace(size_t requested_space) noexcept
  {
    return (buf_ + size_) >= (*d_ptr_ + requested_space);
  }

  uint8_t * buf_    = nullptr;
  uint8_t * ptr_    = nullptr;
  size_t size_      = 0;
  int id_           = 0;
  uint8_t ** d_ptr_ = nullptr;
};

template <class T, size_t N, class U = T>
class FixedAllocator
{
 public:
  using value_type           = T;
  static auto constexpr size = N;  // NOLINT
  using arena_type           = Arena;

 private:
  arena_type a_;
  arena_type * a_ptr_            = &a_;
  uint8_t buf_[size * sizeof(U)] = { 0 };

 public:
  FixedAllocator(const FixedAllocator &) = default;
  FixedAllocator & operator=(const FixedAllocator &) = delete;

  FixedAllocator() noexcept : a_(buf_, sizeof(buf_))
  {
    LOG_DEBUG(
        "FixedAllocator Constructor<size = %zu :: sizeof(T) = %zu :: "
        "sizeof(buf_) = %zu>\n",
        size,
        sizeof(T),
        sizeof(buf_));
  }
  // Used by node containers
  template <class T_copy, size_t N_copy, class U_copy>
  FixedAllocator(const FixedAllocator<T_copy, N_copy, U_copy> & a)
      : a_(a.a_), a_ptr_(const_cast<arena_type *>(&a.a_))
  {
  }

  template <class Tbind>
  struct rebind  // NOLINT
  {
    using other = FixedAllocator<Tbind, N, Tbind>;
  };

  T * allocate(size_t n)  // NOLINT
  {
    return reinterpret_cast<T *>(a_ptr_->allocate(n * sizeof(T)));
  }
  void deallocate(T * p, size_t n) noexcept  // NOLINT
  {
    a_ptr_->deallocate(reinterpret_cast<uint8_t *>(p), n * sizeof(T));
  }

  template <class T1, size_t N1, class U1, size_t M1>
  friend bool operator==(const FixedAllocator<T1, N1> & x,
                         const FixedAllocator<U1, M1> & y) noexcept;

  template <class T1, size_t M1, class U1>
  friend class FixedAllocator;
};

template <class T, size_t N, class U, class V, size_t M, class W>
inline bool operator==(const FixedAllocator<T, N, U> & x,
                       const FixedAllocator<V, M, W> & y) noexcept
{
  return N == M && &x.a_ == &y.a_;
}

template <class T, size_t N, class U, class V, size_t M, class W>
inline bool operator!=(const FixedAllocator<T, N, U> & x,
                       const FixedAllocator<V, M, W> & y) noexcept
{
  return !(x == y);
}
}  // namespace sjsu
