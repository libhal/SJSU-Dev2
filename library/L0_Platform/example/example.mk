# Add all sources files that constitute the platform's core functions into a
# variable named LIBRARY_<platform>. Include files from outside of the platform
# if necessary. Prefer to reused files rather than making a new one.
LIBRARY_EXAMPLE += $(LIBRARY_DIR)/L0_Platform/example/diskio.cpp
LIBRARY_EXAMPLE += $(LIBRARY_DIR)/L0_Platform/example/startup.cpp
LIBRARY_EXAMPLE += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/port.c

# Give a path to the OPENOCD configuration file
OPENOCD_CONFIG = $(LIBRARY_DIR)/L0_Platform/example/example.cfg

# This calls the BUILD_LIBRARY macro that generates the the example platform
# static library.
$(eval $(call BUILD_LIRBARY,libexample,LIBRARY_EXAMPLE))

# Include any additional platform sub make files. For example, if you are using
# an ARM Cortex M4, you should include the line below.
include $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/m4.mk

# ^ If this doesn't alreayd exist you will need to supply the following:

# Add folders to SYSTEM_INCLUDES if applicable:
SYSTEM_INCLUDES += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/
# ^ Add architecture flags here
COMMON_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
                -fabi-version=0 -mtpcs-frame -mtpcs-leaf-frame
