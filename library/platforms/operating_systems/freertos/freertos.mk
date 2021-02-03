LINT_FILTER += $(LIBRARY_DIR)/platforms/operating_systems/freertos/

LIBRARY_FREERTOS_COMMON += \
    $(LIBRARY_DIR)/platforms/operating_systems/freertos/freertos_common.cpp

$(eval $(call BUILD_LIBRARY,libfreertoscommon,LIBRARY_FREERTOS_COMMON))

include $(LIBRARY_DIR)/third_party/FreeRTOS/freertos.mk
