INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

SYSTEM_INCLUDES +=

LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/lpc17xx/diskio.cpp
LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/lpc17xx/startup.cpp
LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/lpc17xx/interrupt.cpp
LIBRARY_LPC40XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/ARM_CM3/port.c

TESTS += $(LIBRARY_DIR)/L0_Platform/lpc17xx/interrupt.cpp
TESTS += $(LIBRARY_DIR)/L0_Platform/lpc17xx/test/interrupt_test.cpp
TESTS += $(LIBRARY_DIR)/L0_Platform/lpc17xx/test/system_controller_test.cpp

OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/lpc40xx/sjone.cfg

$(eval $(call BUILD_LIRBARY,liblpc17xx,LIBRARY_LPC40XX))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/m3.mk
