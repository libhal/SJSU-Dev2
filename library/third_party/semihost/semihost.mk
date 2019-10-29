INCLUDES +=

SYSTEM_INCLUDES += $(LIBRARY_DIR)/third_party/semihost/

LIBRARY_SEMIHOST += $(LIBRARY_DIR)/third_party/semihost/trace.cpp
LIBRARY_SEMIHOST += $(LIBRARY_DIR)/third_party/semihost/trace_impl.cpp

$(eval $(call BUILD_LIRBARY,libsemihost,LIBRARY_SEMIHOST))
