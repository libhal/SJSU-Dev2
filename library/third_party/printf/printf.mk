INCLUDES +=

SYSTEM_INCLUDES += $(LIBRARY_DIR)/third_party/printf/

TESTS += $(LIBRARY_DIR)/third_party/printf/printf.cpp

LIBRARY_PRINTF += $(LIBRARY_DIR)/third_party/printf/printf.cpp

$(eval $(call BUILD_LIRBARY,libprintf,LIBRARY_PRINTF))
