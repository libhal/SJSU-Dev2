INCLUDES += $(LIB_DIR)/L0_Platform/freertos

SYSTEM_INCLUDES +=

LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc17xx/diskio.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc17xx/startup.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc17xx/interrupt.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/arm_cortex/m3/ARM_CM3/port.c

TESTS += $(LIB_DIR)/L0_Platform/lpc17xx/interrupt.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc17xx/test/interrupt_test.cpp
TESTS += $(LIB_DIR)/L0_Platform/lpc17xx/test/system_controller_test.cpp

$(eval $(call BUILD_LIRBARY,liblpc17xx,LIBRARY_LPC40XX))

include $(LIB_DIR)/L0_Platform/arm_cortex/m3/m3.mk
