INCLUDES +=

SYSTEM_INCLUDES += $(LIB_DIR)/third_party/printf/

TESTS += $(LIB_DIR)/third_party/printf/printf.cpp

LIBRARY_PRINTF += $(LIB_DIR)/third_party/printf/printf.cpp

$(eval $(call BUILD_LIRBARY,libprintf,LIBRARY_PRINTF))
