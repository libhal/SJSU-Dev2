SYSTEM_INCLUDES += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/include
SYSTEM_INCLUDES += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/trace

LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/timers.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/event_groups.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/stream_buffer.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/list.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/croutine.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/tasks.c
LIBRARY_FREERTOS += $(LIBRARY_DIR)/third_party/FreeRTOS/Source/queue.c
LIBRARY_FREERTOS += \
  $(LIBRARY_DIR)/third_party/FreeRTOS/Source/portable/MemMang/heap_3.c

$(eval $(call BUILD_LIRBARY,libfreertos,LIBRARY_FREERTOS))
