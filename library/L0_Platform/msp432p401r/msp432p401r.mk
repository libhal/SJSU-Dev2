LIBRARY_MSP432P401R += $(LIBRARY_DIR)/L0_Platform/msp432p401r/startup.cpp
LIBRARY_MSP432P401R += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/port.c
LIBRARY_MSP432P401R += $(LIBRARY_DIR)/L0_Platform/arm_cortex/exceptions.cpp

OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/msp432p401r/msp432p401r.cfg

$(eval $(call BUILD_LIBRARY,libmsp432p401r,LIBRARY_MSP432P401R))

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/m4.mk

$(eval $(call DEFAULT_PLATFORM_FLASH))
