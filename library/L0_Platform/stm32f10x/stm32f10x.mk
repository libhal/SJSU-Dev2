LIBRARY_STM32F10X += $(LIBRARY_DIR)/L0_Platform/stm32f10x/startup.cpp
LIBRARY_STM32F10X += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/ARM_CM3/port.c
LIBRARY_STM32F10X += $(LIBRARY_DIR)/L0_Platform/arm_cortex/exceptions.cpp

OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/stm32f10x/stm32f10x.cfg

$(eval $(call BUILD_LIBRARY,libstm32f10x,LIBRARY_STM32F10X))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/m3.mk

$(eval $(call DEFAULT_PLATFORM_FLASH))
