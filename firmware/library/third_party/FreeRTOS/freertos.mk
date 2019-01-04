INCLUDES +=
SYSTEM_INCLUDES += \
	$(LIB_DIR)/third_party/FreeRTOS/Source/include \
	$(LIB_DIR)/third_party/FreeRTOS/Source/trace \
	$(LIB_DIR)/third_party/FreeRTOS/Source/portable \
	$(LIB_DIR)/third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F
SOURCES += \
	$(LIB_DIR)/third_party/FreeRTOS/Source/timers.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/event_groups.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/stream_buffer.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/list.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/croutine.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/tasks.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/queue.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
	$(LIB_DIR)/third_party/FreeRTOS/Source/portable/MemMang/heap_3.c
TESTS +=
