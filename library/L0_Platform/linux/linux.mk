SYSTEM_INCLUDES += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/

SOURCES += $(LIBRARY_DIR)/L0_Platform/linux/startup.cpp
SOURCES += $(LIBRARY_DIR)/L0_Platform/linux/freertos_posix/port.c

DEVICE_CC        ?= gcc
DEVICE_CPPC      ?= g++
DEVICE_OBJDUMP   ?= objdump
DEVICE_SIZEC     ?= size
DEVICE_OBJCOPY   ?= objcopy
DEVICE_NM        ?= nm
DEVICE_AR        ?= ar
DEVICE_RANLIB    ?= ranlib
DEVICE_ADDR2LINE ?= addr2line
DEVICE_GDB       ?= gdb

COMMON_FLAGS += -m32

LINKFLAGS = $(COMMON_FLAGS) -m32 -pthread -Wl,--gc-sections -Wl,-Map,"$(MAP)"

include $(LIBRARY_DIR)/L0_Platform/freertos/freertos.mk

platform-flash:
	@./$(EXECUTABLE)
