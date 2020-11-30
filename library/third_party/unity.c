#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#include "third_party/fatfs/source/ff.c"         // NOLINT
#include "third_party/fatfs/source/ffsystem.c"   // NOLINT
#include "third_party/fatfs/source/ffunicode.c"  // NOLINT

#include "third_party/FreeRTOS/Source/timers.c"                   // NOLINT
#include "third_party/FreeRTOS/Source/event_groups.c"             // NOLINT
#include "third_party/FreeRTOS/Source/stream_buffer.c"            // NOLINT
#include "third_party/FreeRTOS/Source/list.c"                     // NOLINT
#include "third_party/FreeRTOS/Source/croutine.c"                 // NOLINT
#include "third_party/FreeRTOS/Source/tasks.c"                    // NOLINT
#include "third_party/FreeRTOS/Source/queue.c"                    // NOLINT
#include "third_party/FreeRTOS/Source/portable/MemMang/heap_3.c"  // NOLINT

#pragma GCC diagnostic pop
