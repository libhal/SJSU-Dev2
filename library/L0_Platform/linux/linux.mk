SYSTEM_INCLUDES += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/

LIBRARY_LINUX += $(LIBRARY_DIR)/L0_Platform/linux/startup.cpp
LIBRARY_LINUX += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/port.c

$(eval $(call BUILD_LIBRARY,liblinux,LIBRARY_LINUX))

DEVICE_CC        := gcc-9
DEVICE_CPPC      := g++-9
DEVICE_OBJDUMP   := objdump
DEVICE_SIZEC     := size
DEVICE_OBJCOPY   := objcopy
DEVICE_NM        := nm
DEVICE_AR        := ar
DEVICE_RANLIB    := ranlib
DEVICE_ADDR2LINE := addr2line
DEVICE_GDB       := gdb

CFLAGS += -m32

LINKER_SCRIPT :=

LDFLAGS := $(CFLAGS) -pthread -Wl,--gc-sections -Wl,-Map,"$(MAP)"

include $(LIBRARY_DIR)/L0_Platform/freertos/freertos.mk

platform-flash:
	@./$(EXECUTABLE)
