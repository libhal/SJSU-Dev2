INCLUDES += $(LIB_DIR)/L0_Platform/freertos

LINT_FILTER += $(LIB_DIR)/L0_Platform/freertos/
LINT_FILTER += $(LIB_DIR)/L0_Platform/arm_cortex/

LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc17xx/

LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/LPC40xx.h
LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/LPC40xx_c.h
LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/system_LPC407x_8x_177x_8x.h

include $(LIB_DIR)/L0_Platform/$(PLATFORM)/$(PLATFORM).mk
