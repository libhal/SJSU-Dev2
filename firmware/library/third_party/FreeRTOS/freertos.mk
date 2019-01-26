SYSTEM_INCLUDES += $(LIB_DIR)/third_party/FreeRTOS/Source/include
SYSTEM_INCLUDES += $(LIB_DIR)/third_party/FreeRTOS/Source/trace
SYSTEM_INCLUDES += $(LIB_DIR)/third_party/FreeRTOS/Source/portable
SYSTEM_INCLUDES += $(LIB_DIR)/third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F

SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/timers.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/event_groups.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/stream_buffer.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/list.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/croutine.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/tasks.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/queue.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
SOURCES += $(LIB_DIR)/third_party/FreeRTOS/Source/portable/MemMang/heap_3.c
