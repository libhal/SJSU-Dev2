#include "utility/allocator.hpp"

#include <cstdint>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
namespace
{
template <size_t bytes>
void CheckIfPointerIsInBounds(void * ptr,
                              const StaticAllocator<bytes> & allocator)
{
  CHECK(&allocator <= ptr);
  CHECK(ptr <= (&allocator + sizeof(decltype(allocator))));
};
}  // namespace

TEST_CASE("Testing StaticAllocator")
{
  SECTION("Check that allocator returns unique incremental addresses")
  {
    // Setup
    StaticAllocator<32> allocator;

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
    StaticAllocator<32> allocator;

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
    StaticAllocator<kCapacity> allocator;

    // Exercise
    [[maybe_unused]] void * allocate_block0 = allocator.allocate(1, 1);
    [[maybe_unused]] void * allocate_block1 = allocator.allocate(1, 1);
    [[maybe_unused]] void * allocate_block2 = allocator.allocate(1, 1);

    // Verify
    CHECK(kCapacity == allocator.TotalCapacity());
    CHECK(3 == allocator.BytesAllocated());
    CHECK((kCapacity - 3) == allocator.BytesUnallocated());
  }

  SECTION("Allocate maximum capacity without exception thrown")
  {
    // Setup
    StaticAllocator<32> allocator;

    // Exercise
    void * ptr = allocator.allocate(allocator.TotalCapacity(), 1);

    // Verify
    CheckIfPointerIsInBounds(ptr, allocator);
  }

  SECTION("Throw std::bad_alloc if memory is exceeded")
  {
    // Setup
    StaticAllocator<32> allocator;

    // Exercise
    // Exercise: Allocate the entire buffer's worth, so in the next step,
    // allocating just 1 more byte will cause the allocator to throw
    // std::bad_alloc.
    void * ptr = allocator.allocate(allocator.TotalCapacity(), 1);

    // Verify
    CHECK_THROWS_AS(
        ([&allocator, &ptr]() { ptr = allocator.allocate(1, 1); })(),
        std::bad_alloc);
  }
}
}  // namespace sjsu
