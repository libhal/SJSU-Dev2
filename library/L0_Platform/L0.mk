INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

NO_TEST_NEEDED += $(LIBRARY_DIR)/L0_Platform/%

# FreeRTOS
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/freertos/
# Linux
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/
# Arm Cortex Core
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/arm_cortex/
# Example
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/example/example_memory_map_c.h
# LPC17xx
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc17xx/LPC17xx.h
# LPC40xx
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/lpc40xx/LPC40xx.h
# STM32F10x
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/stm32f10x/stm32f10x.h
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/stm32f10x/system_stm32f10x.h
# STM32F4xx
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/stm32f4xx/stm32f4xx.h
LINT_FILTER += $(LIBRARY_DIR)/L0_Platform/stm32f4xx/system_stm32f4xx.h

include $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/$(PLATFORM).mk
