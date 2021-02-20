#pragma once

#include <cstddef>
#include <memory_resource>

#include "utility/log.hpp"

namespace sjsu
{
/// The StaticMemoryResource is the polymorphic memory resource allocator of
/// choice for SJSU-Dev2's internal libraries when a library would like to use a
/// C++ standard container such as std::pmr::vector, std::pmr::string,
/// std::pmr::unordered_map, and more without breaking the style rule of "NO
/// Dynamic Allocation". It provides a simple way to create an allocator with
/// built in storage, with its size indicated by a single template parameter. It
/// also provides statistics regarding the amount of memory allocated such that
/// developers can determine if they need to reduce or increase the size of the
/// buffer. Memory statistics can also give programs the insight into how much
/// memory space is available and make decisions based on that. In the event
/// that the memory of the static allocator is exceeded, a std::bad_alloc
/// excpetion is thrown.
///
/// USAGE:
///
///    StaticMemoryResource<sizeof(int) * 100> memory_resource;
///    std::pmr::vector<int> sampled_data(&memory_resource);
///
/// USAGE:
///
///    StaticMemoryResource<1024> memory_resource;
///    std::pmr::unordered_map<int, const char*> id_name_map(&memory_resource);
///
/// @tparam kBufferSizeBytes - Number of bytes to statically allocate for the
///         memory resource.
template <size_t kBufferSizeBytes>
class StaticMemoryResource : public std::pmr::memory_resource
{
 public:
  StaticMemoryResource()
      : buffer_{},
        unallocated_memory_(buffer_.data()),
        resource_(buffer_.data(),
                  buffer_.size(),
                  std::pmr::null_memory_resource())
  {
  }

  /// @return size_t - the total number of bytes that this allocator can
  /// allocate before throwing a std::bad_alloc exception.
  constexpr std::size_t Capacity() const
  {
    return kBufferSizeBytes;
  }

  /// @return size_t - number of bytes that have already been allocated.
  std::size_t MemoryUsed() const
  {
    return unallocated_memory_ - buffer_.data();
  }

  /// @return int - Bytes that have yet to be allocated from this allocator.
  int MemoryAvailable() const
  {
    return Capacity() - MemoryUsed();
  }

  /// Print to STDOUT the total capcity, memory allocated and memory left in
  /// allocator.
  void Print() const
  {
    LogInfo("StaticMemoryResource >> Capacity: %zu, Allocated: %zu, Left: %d",
            Capacity(),
            MemoryUsed(),
            MemoryAvailable());
  }

 protected:
  /// Implemenation of the do_allocate() method for std::pmr::memory_resource
  void * do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    LogDebug("Allocating %zu @ alignment %zu, left: %zu\n",
             bytes,
             alignment,
             MemoryAvailable());

    // Request a pointer to unallocated memory from the
    // monotonic_buffer_resource buffer.
    void * allocated_address = resource_.allocate(bytes, alignment);

    // If the above call has not thrown a std::bad_alloc exception, then the
    // allocated address must contain a valid address from buffer. To get the
    // location of the unallocated memory, simply add the number of bytes to
    // allocate_address variable.
    unallocated_memory_ = static_cast<std::byte *>(allocated_address) + bytes;

    return allocated_address;
  }

  /// Implemenation of the do_deallocate() method for std::pmr::memory_resource
  void do_deallocate(void * p,
                     std::size_t bytes,
                     std::size_t alignment) override
  {
    return resource_.deallocate(p, bytes, alignment);
  }

  /// Implemenation of the do_is_equal() method for std::pmr::memory_resource
  bool do_is_equal(
      const std::pmr::memory_resource & other) const noexcept override
  {
    return resource_.is_equal(other);
  }

 private:
  std::array<std::byte, kBufferSizeBytes> buffer_;
  std::byte * unallocated_memory_;
  std::pmr::monotonic_buffer_resource resource_;
};
}  // namespace sjsu
