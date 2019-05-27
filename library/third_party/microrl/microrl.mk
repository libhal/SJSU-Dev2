INCLUDES +=
SYSTEM_INCLUDES +=

LIBRARY_MICRORL += $(LIBRARY_DIR)/third_party/microrl/microrl.cpp

TESTS +=

$(eval $(call BUILD_LIRBARY,libmicrorl,LIBRARY_MICRORL))
