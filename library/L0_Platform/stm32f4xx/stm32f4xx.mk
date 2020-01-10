INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

SYSTEM_INCLUDES +=

LIBRARY_STM32F4XX += $(LIBRARY_DIR)/L0_Platform/stm32f4xx/diskio.cpp
LIBRARY_STM32F4XX += $(LIBRARY_DIR)/L0_Platform/stm32f4xx/startup.cpp
LIBRARY_STM32F4XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/port.c
LIBRARY_STM32F4XX += $(LIBRARY_DIR)/L0_Platform/arm_cortex/exceptions.cpp

OPENOCD_CONFIG := $(or $(OPENOCD_CONFIG), \
                       $(LIBRARY_DIR)/L0_Platform/stm32f4xx/stm32f4xx.cfg)

$(eval $(call BUILD_LIBRARY,libstm32f4xx,LIBRARY_STM32F4XX))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/m4.mk

platform-flash:
	echo -n "Factory bootloader flashing support is not currently supported for "
	echo "stm32f4xx."
	echo "Please use 'make jtag-flash' instead"
