INCLUDES +=
SYSTEM_INCLUDES +=

LIBRARY_NEWLIB += $(LIBRARY_DIR)/newlib/stdio.cpp

ifneq ($(PLATFORM), linux)
LIBRARY_NEWLIB += $(LIBRARY_DIR)/newlib/newlib.cpp
TESTS += $(LIBRARY_DIR)/newlib/newlib.cpp
USER_TESTS += $(LIBRARY_DIR)/newlib/newlib.cpp
endif

TESTS += $(LIBRARY_DIR)/newlib/stdio.cpp
USER_TESTS += $(LIBRARY_DIR)/newlib/stdio.cpp

$(eval $(call BUILD_LIRBARY,libnewlib,LIBRARY_NEWLIB))
