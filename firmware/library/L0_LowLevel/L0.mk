INCLUDES += $(LIB_DIR)/L0_LowLevel/SystemFiles

SYSTEM_INCLUDES +=

LIBRARY_LPC40XX += $(LIB_DIR)/L0_LowLevel/startup.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_LowLevel/interrupt.cpp

TESTS += $(LIB_DIR)/L0_LowLevel/interrupt.cpp
TESTS += $(LIB_DIR)/L0_LowLevel/test/interrupt_test.cpp
TESTS += $(LIB_DIR)/L0_LowLevel/test/system_controller_test.cpp

# TODO(#435): Consider adding this library, but also for each platform
$(eval $(call BUILD_LIRBARY,liblpc40xx,LIBRARY_LPC40XX))
