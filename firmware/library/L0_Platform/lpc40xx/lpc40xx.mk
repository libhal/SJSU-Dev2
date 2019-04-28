INCLUDES += $(LIB_DIR)/L0_Platform/freertos

SYSTEM_INCLUDES +=

LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/diskio.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/startup.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/interrupt.cpp

TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/interrupt.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/test/interrupt_test.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/test/system_controller_test.cpp

LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/LPC40xx.h
LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/system_LPC407x_8x_177x_8x.h

$(eval $(call BUILD_LIRBARY,liblpc40xx,LIBRARY_LPC40XX))

include $(LIB_DIR)/L0_Platform/arm_cortex/m4/m4.mk
