DEVICE_CC        := $(SJARMGCC)/bin/arm-none-eabi-gcc
DEVICE_CPPC      := $(SJARMGCC)/bin/arm-none-eabi-g++
DEVICE_OBJDUMP   := $(SJARMGCC)/bin/arm-none-eabi-objdump
DEVICE_SIZEC     := $(SJARMGCC)/bin/arm-none-eabi-size
DEVICE_OBJCOPY   := $(SJARMGCC)/bin/arm-none-eabi-objcopy
DEVICE_NM        := $(SJARMGCC)/bin/arm-none-eabi-nm
DEVICE_AR        := $(SJARMGCC)/bin/arm-none-eabi-ar
DEVICE_RANLIB    := $(SJARMGCC)/bin/arm-none-eabi-ranlib
DEVICE_ADDR2LINE := $(SJARMGCC)/bin/arm-none-eabi-addr2line
DEVICE_GDB       := $(SJARMGCC)/bin/arm-none-eabi-gdb-py

CFLAGS += -D ARM_CORTEX_M_TRACE_ENABLE=1 -D OS_USE_TRACE_SEMIHOSTING_STDOUT=1

include $(LIBRARY_DIR)/platforms/operating_systems/freertos/freertos.mk