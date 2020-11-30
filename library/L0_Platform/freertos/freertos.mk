LINT_FILTER += library/L0_Platform/freertos/

LIBRARY_FREERTOS_COMMON += \
    $(LIBRARY_DIR)/L0_Platform/freertos/freertos_common.cpp

$(eval $(call BUILD_LIBRARY,libfreertoscommon,LIBRARY_FREERTOS_COMMON))

include $(LIBRARY_DIR)/third_party/FreeRTOS/freertos.mk
