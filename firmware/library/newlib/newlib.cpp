#include <cstdint>
#include <cstdlib>

// Implementation of sbrk for allocating and deallocating space for malloc.
extern "C" void * _sbrk(int increment)
{
    constexpr int32_t kHeapSize = 32768; 
    extern uint8_t _heap[kHeapSize];
    static uint8_t * heap_end = _heap;
    void * previous_heap_end = static_cast<void *>(heap_end);

    // Check that by allocating this space, we do not exceed the heap area.
    if ((heap_end + increment) - _heap > kHeapSize)
    {
        previous_heap_end = reinterpret_cast<void *>(0);
    }
    heap_end += increment;
    previous_heap_end = reinterpret_cast<void *>(previous_heap_end);
    return previous_heap_end;
}
