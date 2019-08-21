#include <cstdint>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/allocator.hpp"

namespace sjsu
{
TEST_CASE("Testing Arena", "[arena]")
{
  uint8_t buffer[1024];

  constexpr size_t kBlockSize = 32;
  SECTION("Allocate")
  {
    // Setup ...
    Arena test_arena(buffer, sizeof(buffer));

    // Exercise ...
    // Should return the start of the buffer
    uint8_t * allocate_block0 = test_arena.allocate(kBlockSize);
    // Should return the address (start_of_buffer + kBlockSize)
    uint8_t * allocate_block1 = test_arena.allocate(kBlockSize);

    // Verify ...
    CHECK(allocate_block0 == &buffer[0]);
    CHECK(allocate_block1 == &buffer[kBlockSize]);
  }

  SECTION("Deallocate")
  {
    // Setup ...
    Arena test_arena(buffer, sizeof(buffer));

    // Exercise ...
    uint8_t * allocate_block0 = test_arena.allocate(kBlockSize);
    uint8_t * allocate_block1 = test_arena.allocate(kBlockSize);
    test_arena.deallocate(allocate_block1, kBlockSize);
    test_arena.deallocate(allocate_block0, kBlockSize);
    allocate_block0 = test_arena.allocate(kBlockSize);

    // Verify ...
    // The allocated block should be set back to the beginning of the buffer
    CHECK(allocate_block0 == &buffer[0]);
  }
  SECTION("Deallocate in the middle does nothing")
  {
    // Setup ...
    Arena test_arena(buffer, sizeof(buffer));

    // Exercise ...
    [[maybe_unused]] uint8_t * allocate_block0 =
        test_arena.allocate(kBlockSize);
    [[maybe_unused]] uint8_t * allocate_block1 =
        test_arena.allocate(kBlockSize);
    [[maybe_unused]] uint8_t * allocate_block2 =
        test_arena.allocate(kBlockSize);

    test_arena.deallocate(allocate_block1, kBlockSize);

    uint8_t * allocate_block3 = test_arena.allocate(kBlockSize);

    // Verify ...
    // We deallocated block1, which is in the middle, so another allocation
    // should progress us forward in the buffer.
    CHECK(allocate_block3 == &buffer[kBlockSize * 3]);
  }
  SECTION("Copy Construct")
  {
    // Setup ...
    Arena test_arena(buffer, sizeof(buffer));

    // Exercise ...
    [[maybe_unused]] uint8_t * allocate_block0 =
        test_arena.allocate(kBlockSize);
    [[maybe_unused]] uint8_t * allocate_block1 =
        test_arena.allocate(kBlockSize);
    [[maybe_unused]] uint8_t * allocate_block2 =
        test_arena.allocate(kBlockSize);

    Arena test_arena_copy(test_arena);

    uint8_t * allocate_block3 = test_arena_copy.allocate(kBlockSize);
    uint8_t * allocate_block4 = test_arena.allocate(kBlockSize);

    // Verify ...
    // test_arena_copy should allocate from the position that "test_arena" left
    // off at.
    CHECK(allocate_block3 == &buffer[kBlockSize * 3]);
    // The origiinal arena's ptr should have been moved by the copy's
    // allocation, meaning we can continue to allocate memory using the original
    // without issue.
    CHECK(allocate_block4 == &buffer[kBlockSize * 4]);
  }
  SECTION("Buffer Overflow returns nullptr")
  {
    // Setup ...
    Arena test_arena(buffer, sizeof(buffer));

    // Exercise ...
    [[maybe_unused]] uint8_t * allocate_block0 =
        test_arena.allocate(sizeof(buffer));
    [[maybe_unused]] uint8_t * allocate_block1 = test_arena.allocate(1);

    // Verify ...
    // test_arena_copy should allocate from the position that "test_arena" left
    // off at.
    CHECK(allocate_block0 == &buffer[0]);
    // The origiinal arena's ptr should have been moved by the copy's
    // allocation, meaning we can continue to allocate memory using the original
    // without issue.
    CHECK(allocate_block1 == nullptr);
  }
}
TEST_CASE("Testing FixedAllocator", "[fixed-allocator]")
{
  SECTION("FixedAllocator<int, 32>")
  {
    // Setup ...
    using Type = int;
    FixedAllocator<Type, 32> fixed_allocator;

    // Exercise ...
    Type * allocated_int0 = fixed_allocator.allocate(1);
    Type * allocated_int1 = fixed_allocator.allocate(1);

    // Verify ...
    CHECK(allocated_int0 != allocated_int1);
    // The next allocated object should be + 1 offset from the first allocated
    // object.
    CHECK(allocated_int1 == &allocated_int0[1]);
  }
  SECTION("Deallocate")
  {
    // Setup ...
    using Type = int;
    FixedAllocator<Type, 32> fixed_allocator;

    // Exercise ...
    Type * allocated_int0 = fixed_allocator.allocate(1);
    Type * allocated_int1 = fixed_allocator.allocate(1);

    fixed_allocator.deallocate(allocated_int1, 1);
    fixed_allocator.deallocate(allocated_int0, 1);

    Type * allocated_int_again0 = fixed_allocator.allocate(1);
    Type * allocated_int_again1 = fixed_allocator.allocate(1);

    // Verify ...
    CHECK(allocated_int_again0 != allocated_int_again1);
    CHECK(allocated_int_again0 == allocated_int0);
    // The next allocated object should be + 1 offset from the first allocated
    // object.
    CHECK(allocated_int_again1 == allocated_int1);
  }
  SECTION("Copy Construct")
  {
    // Setup ...
    using Type = int;
    FixedAllocator<Type, 32> fixed_allocator;

    // Exercise ...
    [[maybe_unused]] Type * allocate_block0 = fixed_allocator.allocate(1);
    [[maybe_unused]] Type * allocate_block1 = fixed_allocator.allocate(1);
    [[maybe_unused]] Type * allocate_block2 = fixed_allocator.allocate(1);

    FixedAllocator<Type, 32> fixed_allocator_copy(fixed_allocator);

    Type * allocate_block3 = fixed_allocator_copy.allocate(1);
    Type * allocate_block4 = fixed_allocator.allocate(1);

    // Verify ...
    CHECK(&allocate_block0[1] == allocate_block1);
    CHECK(&allocate_block1[1] == allocate_block2);
    CHECK(&allocate_block2[1] == allocate_block3);
    CHECK(&allocate_block3[1] == allocate_block4);
  }

  SECTION("FixedAllocator<int, 32> return nullptr if out of memory")
  {
    using Type = int;
    FixedAllocator<Type, 32> fixed_allocator;
    Type * allocated_int0 =
        fixed_allocator.allocate(decltype(fixed_allocator)::size);
    Type * allocate_to_nullptr = fixed_allocator.allocate(1);

    CHECK(allocated_int0 != allocate_to_nullptr);
    // The next allocated object should be + 1 offset from the first allocated
    // object.
    CHECK(allocate_to_nullptr == nullptr);
  }
}
}  // namespace sjsu
