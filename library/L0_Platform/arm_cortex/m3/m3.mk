SYSTEM_INCLUDES += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m3/ARM_CM3/

CFLAGS += -mcpu=cortex-m3 -mthumb -mfloat-abi=soft \
                 -fabi-version=0 -mtpcs-frame -mtpcs-leaf-frame

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/arm_cortex.mk
