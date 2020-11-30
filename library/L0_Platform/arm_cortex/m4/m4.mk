SYSTEM_INCLUDES += $(LIBRARY_DIR)/L0_Platform/arm_cortex/m4/ARM_CM4F/

CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
                -fabi-version=0 -mtpcs-frame -mtpcs-leaf-frame \
                -D ARM_MATH_CM4=1 -D __FPU_PRESENT=1U

include $(LIBRARY_DIR)/L0_Platform/arm_cortex/arm_cortex.mk
