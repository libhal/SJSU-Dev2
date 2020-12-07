#pragma once

#include <memory_resource>

#include "utility/log.hpp"

namespace sjsu
{
/// The StaticAllocator is the polymorphic memory resource allocator of choice
/// for SJSU-Dev2's internal libraries when a library would like to use a C++
/// standard container such as std::pmr::vector, std::pmr::string,
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
///    StaticAllocator<sizeof(int) * 100> memory_resource;
///    std::pmr::vector<int> sampled_data(&memory_resource);
///
/// USAGE:
///
///    StaticAllocator<1024> memory_resource;
///    std::pmr::unordered_map<int, const char*> id_name_map(&memory_resource);
///
/// @tparam kBufferSizeBytes - Number of bytes to statically allocate for the
///         memory resource.
template <size_t kBufferSizeBytes>
class StaticAllocator : public std::pmr::memory_resource
{
 public:
  StaticAllocator()
      : memory_allocated_(0),
        buffer_{},
        resource_(buffer_.data(),
                  buffer_.size(),
                  std::pmr::null_memory_resource())
  {
  }

  /// @return size_t - the total number of bytes that this allocator can
  /// allocate before throwing a std::bad_alloc exception.
  constexpr size_t TotalCapacity()
  {
    return kBufferSizeBytes;
  }

  /// @return size_t - number of bytes that have already been allocated.
  size_t BytesAllocated()
  {
    return memory_allocated_;
  }

  /// @return int - Bytes that have yet to be allocated from this allocator.
  int BytesUnallocated()
  {
    return TotalCapacity() - BytesAllocated();
  }

  /// Print to STDOUT the total capcity, memory allocated and memory left in
  /// allocator.
  void Print()
  {
    LogInfo("StaticAllocator >> Capacity: %zu, Allocated: %zu, Left: %d",
            TotalCapacity(),
            BytesAllocated(),
            BytesUnallocated());
  }

 protected:
  void * do_allocate(std::size_t bytes, std::size_t alignment) override
  {
    memory_allocated_ += bytes;
    LogDebug("Allocating %zu @ alignment %zu, left: %zu\n",
             bytes,
             alignment,
             memory_allocated_);
    return resource_.allocate(bytes, alignment);
  }

  void do_deallocate(void * p,
                     std::size_t bytes,
                     std::size_t alignment) override
  {
    return resource_.deallocate(p, bytes, alignment);
  }

  bool do_is_equal(
      const std::pmr::memory_resource & other) const noexcept override
  {
    return resource_.is_equal(other);
  }

 private:
  size_t memory_allocated_;
  std::array<uint8_t, kBufferSizeBytes> buffer_;
  std::pmr::monotonic_buffer_resource resource_;
};
}  // namespace sjsu
