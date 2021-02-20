SYSTEM_INCLUDES += $(LIBRARY_DIR)/platforms/processors/arm_cortex/m3/ARM_CM3/

CFLAGS += -mcpu=cortex-m3 -mthumb -mfloat-abi=soft \
                 -fabi-version=0 -mtpcs-frame -mtpcs-leaf-frame

include $(LIBRARY_DIR)/platforms/processors/arm_cortex/arm_cortex.mk
