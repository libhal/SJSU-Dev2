INCLUDES +=
SYSTEM_INCLUDES +=

LIBRARY_NEWLIB += $(LIBRARY_DIR)/newlib/stdio.cpp

ifneq ($(PLATFORM), linux)
LIBRARY_NEWLIB += $(LIBRARY_DIR)/newlib/newlib.cpp
TESTS += $(LIBRARY_DIR)/newlib/newlib.cpp
endif

$(eval $(call BUILD_LIBRARY,libnewlib,LIBRARY_NEWLIB))
