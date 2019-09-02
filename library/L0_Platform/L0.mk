INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/freertos/
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/arm_cortex/

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/example/example_memory_map_c.h

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc17xx/LPC17xx.h

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc40xx/LPC40xx.h

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/

include $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/$(PLATFORM).mk
