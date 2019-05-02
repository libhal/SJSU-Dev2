LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/diskio.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/startup.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/interrupt.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/port.c

TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/interrupt.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/test/interrupt_test.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/test/system_controller_test.cpp

$(eval $(call BUILD_LIRBARY,liblpc40xx,LIBRARY_LPC40XX))

include $(LIB_DIR)/L0_Platform/arm_cortex/m4/m4.mk
