#include "utility/memory_resource.hpp"

#include <cstdint>

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
namespace
{
template <size_t bytes>
void CheckIfPointerIsInBounds(void * ptr,
                              const StaticMemoryResource<bytes> & allocator)
{
  CHECK(&allocator <= ptr);
  CHECK(ptr <= (&allocator + sizeof(decltype(allocator))));
};
}  // namespace

TEST_CASE("Testing StaticMemoryResource")
{
  SECTION("Check that allocator returns unique incremental addresses")
  {
    // Setup
    StaticMemoryResource<32> allocator;

    // Exercise
    void * ptr0 = allocator.allocate(1, 1);
    void * ptr1 = allocator.allocate(1, 1);

    // Verify
    CHECK(ptr0 != ptr1);
    CheckIfPointerIsInBounds(ptr0, allocator);
    CheckIfPointerIsInBounds(ptr1, allocator);
  }

  SECTION("Deallocate should do nothing")
  {
    // Setup
    StaticMemoryResource<32> allocator;

    // Exercise
    void * ptr0 = allocator.allocate(1, 1);
    void * ptr1 = allocator.allocate(1, 1);

    allocator.deallocate(ptr1, 1, 1);
    allocator.deallocate(ptr0, 1, 1);

    void * ptr_again0 = allocator.allocate(1, 1);
    void * ptr_again1 = allocator.allocate(1, 1);

    // Verify
    CHECK(ptr0 != ptr1);
    CHECK(ptr_again0 != ptr0);
    CHECK(ptr_again1 != ptr1);
    CHECK(ptr_again0 != ptr_again1);

    CheckIfPointerIsInBounds(ptr0, allocator);
    CheckIfPointerIsInBounds(ptr1, allocator);
    CheckIfPointerIsInBounds(ptr_again0, allocator);
    CheckIfPointerIsInBounds(ptr_again1, allocator);
  }

  SECTION("Memory Metrics")
  {
    // Setup
    constexpr size_t kCapacity = 32;
    StaticMemoryResource<kCapacity> allocator;

    // Exercise
    [[maybe_unused]] void * allocate_block0 = allocator.allocate(1, 1);
    [[maybe_unused]] void * allocate_block1 = allocator.allocate(1, 1);
    [[maybe_unused]] void * allocate_block2 = allocator.allocate(1, 1);

    // Verify
    CHECK(kCapacity == allocator.Capacity());
    CHECK(3 == allocator.MemoryUsed());
    CHECK((kCapacity - 3) == allocator.MemoryAvailable());
  }

  SECTION("Memory Metrics with alignment")
  {
    // Setup
    constexpr size_t kCapacity = 32;
    StaticMemoryResource<kCapacity> allocator;

    // Exercise
    // Exercise: Memory Allocation --> [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
    [[maybe_unused]] void * allocate_block0 = allocator.allocate(1, 1);
    // Exercise: Memory Allocation --> [x] [x] [ ] [ ] [ ] [ ] [ ] [ ]
    [[maybe_unused]] void * allocate_block1 = allocator.allocate(1, 1);
    // Exercise: Memory Allocation --> [x] [x] [-] [-] [x] [ ] [ ] [ ]
    //           Notice how 2 of the bytes get skipped? this is due to the fact
    //           that the alignment needs to be in sections of 4 bytes, or word,
    //           chunks. Putting the pointer at index 3 (starting from 1), would
    //           cause the address to be misaligned.
    [[maybe_unused]] void * allocate_block2 = allocator.allocate(1, 4);

    // Verify
    CHECK(kCapacity == allocator.Capacity());
    CHECK(5 == allocator.MemoryUsed());
    CHECK((kCapacity - 5) == allocator.MemoryAvailable());
  }

  SECTION("Allocate maximum capacity without exception thrown")
  {
    // Setup
    StaticMemoryResource<32> allocator;

    // Exercise
    void * ptr = allocator.allocate(allocator.Capacity(), 1);

    // Verify
    CheckIfPointerIsInBounds(ptr, allocator);
  }

  SECTION("Throw std::bad_alloc if memory is exceeded")
  {
    // Setup
    StaticMemoryResource<32> allocator;

    // Exercise
    // Exercise: Allocate the entire buffer's worth, so in the next step,
    // allocating just 1 more byte will cause the allocator to throw
    // std::bad_alloc.
    void * ptr = allocator.allocate(allocator.Capacity(), 1);

    // Verify
    CHECK_THROWS_AS(
        ([&allocator, &ptr]() { ptr = allocator.allocate(1, 1); })(),
        std::bad_alloc);
  }

  SECTION("Test .is_equal()")
  {
    // Setup
    StaticMemoryResource<32> allocator1;
    StaticMemoryResource<32> allocator2;

    // Exercise
    bool are_not_equal = allocator1.is_equal(allocator2);
    bool are_equal = allocator1.is_equal(allocator1);

    // Verify
    CHECK(!are_not_equal);
    CHECK(!are_equal);
  }
}
}  // namespace sjsu
