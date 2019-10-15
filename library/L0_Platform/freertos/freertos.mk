LIBRARY_FREERTOS_COMMON += \
    $(LIBRARY_DIR)/L0_Platform/freertos/freertos_common.cpp

$(eval $(call BUILD_LIRBARY,libfreertoscommon,LIBRARY_FREERTOS_COMMON))

include $(LIBRARY_DIR)/third_party/FreeRTOS/freertos.mk
