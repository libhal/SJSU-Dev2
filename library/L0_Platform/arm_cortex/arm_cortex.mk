DEVICE_CC        = $(SJARMGCC)/bin/arm-none-eabi-gcc
DEVICE_CPPC      = $(SJARMGCC)/bin/arm-none-eabi-g++
DEVICE_OBJDUMP   = $(SJARMGCC)/bin/arm-none-eabi-objdump
DEVICE_SIZEC     = $(SJARMGCC)/bin/arm-none-eabi-size
DEVICE_OBJCOPY   = $(SJARMGCC)/bin/arm-none-eabi-objcopy
DEVICE_NM        = $(SJARMGCC)/bin/arm-none-eabi-nm
DEVICE_AR        = $(SJARMGCC)/bin/arm-none-eabi-ar
DEVICE_RANLIB    = $(SJARMGCC)/bin/arm-none-eabi-ranlib
DEVICE_ADDR2LINE = $(SJARMGCC)/bin/arm-none-eabi-addr2line
DEVICE_GDB       = $(SJARMGCC)/bin/arm-none-eabi-gdb-py

include $(LIBRARY_DIR)/L0_Platform/freertos/freertos.mk