INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/freertos/
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/arm_cortex/

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/example/example_memory_map_c.h

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc17xx/LPC17xx.h
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc17xx/LPC17xx_c.h

LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc40xx/LPC40xx.h
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc40xx/LPC40xx_c.h
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc40xx/system_LPC407x_8x_177x_8x.h

include $(LIBRARY_DIR)/L0_Platform/freertos/freertos.mk
include $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/$(PLATFORM).mk
