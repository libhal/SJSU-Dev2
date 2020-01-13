#pragma once

#include <cstdint>
#include <cstdlib>

#include "utility/log.hpp"

namespace sjsu
{
/// Arena class is a memory management class that takes an external buffer and
/// manages it.
class Arena
{
 public:
  /// Typical constructor
  ///
  /// @param buf - buffer to manage
  /// @param size - size of the buffer passed
  /// @param ptr - starting offset position in the buffer
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

  /// Returns a pointer to a memory region within the buffer that has not been
  /// allocated yet and fits the size requirement.
  ///
  /// @param requested_space - number of bytes requested
  /// @return nullptr if not successful
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

  /// This actually does nothing, but in the special case where the pointer to
  /// be deallocated is at the end, the memory will be deallocated.
  ///
  /// @param pointer - position in memory to be deallocated
  /// @param size - number of bytes to be deallocated
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

  /// @return size_t - size of the arena in total
  size_t size() noexcept  // NOLINT
  {
    return size_;
  }

  /// @return size_t - number of bytes currently allocated
  size_t used() const noexcept  // NOLINT
  {
    return static_cast<size_t>(*d_ptr_ - buf_);
  }

  /// Resets the arena back to its initial state freeing all allocated memory
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

/// Fixed size allocator
template <class T, size_t N, class U = T>
class FixedAllocator
{
 public:
  /// Alias for the value of type
  using value_type = T;
  /// The size of the type
  static auto constexpr size = N;  // NOLINT
  /// Alias of the arena type
  using arena_type = Arena;

 private:
  arena_type a_;
  arena_type * a_ptr_            = &a_;
  uint8_t buf_[size * sizeof(U)] = { 0 };

 public:
  //! @cond Doxygen_Suppress
  FixedAllocator(const FixedAllocator &) = default;
  FixedAllocator & operator=(const FixedAllocator &) = delete;
  FixedAllocator() noexcept : a_(buf_, sizeof(buf_)) {}
  template <class T_copy, size_t N_copy, class U_copy>
  FixedAllocator(const FixedAllocator<T_copy, N_copy, U_copy> & a)
      : a_(a.a_), a_ptr_(const_cast<arena_type *>(&a.a_))
  {
  }
  //! @endcond

  /// Required by the std library to change the type of the fixed allocator.
  /// @tparam Bind - new type to bind to the allocator
  template <class Bind>
  struct rebind  // NOLINT
  {
    /// Alias to the rebound allocator
    using other = FixedAllocator<Bind, N, Bind>;
  };

  /// Allocate object from arena.
  ///
  /// @param n - number of bytes
  /// @return T* - pointer to a type to allocate
  T * allocate(size_t n)  // NOLINT
  {
    return reinterpret_cast<T *>(a_ptr_->allocate(n * sizeof(T)));
  }

  /// Deallocate the object.
  ///
  /// @param p - pointer to the object to deallocate
  /// @param n - how large the object is
  void deallocate(T * p, size_t n) noexcept  // NOLINT
  {
    a_ptr_->deallocate(reinterpret_cast<uint8_t *>(p), n * sizeof(T));
  }

  //! @cond Doxygen_Suppress
  template <class T1, size_t N1, class U1, size_t M1>
  friend bool operator==(const FixedAllocator<T1, N1> & x,
                         const FixedAllocator<U1, M1> & y) noexcept;

  template <class T1, size_t M1, class U1>
  friend class FixedAllocator;
  //! @endcond
};

//! @cond Doxygen_Suppress
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
//! @endcond

}  // namespace sjsu
