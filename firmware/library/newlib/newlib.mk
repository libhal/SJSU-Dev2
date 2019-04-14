INCLUDES +=
SYSTEM_INCLUDES +=

LIBRARY_NEWLIB += $(LIB_DIR)/newlib/newlib.cpp

TESTS += $(LIB_DIR)/newlib/newlib.cpp
TESTS += $(LIB_DIR)/newlib/scanf.cpp

# TODO(#435): Consider adding this library to static libraries
$(eval $(call BUILD_LIRBARY,libnewlib,LIBRARY_NEWLIB))
