INCLUDES += $(LIB_DIR)/L0_LowLevel/SystemFiles

SYSTEM_INCLUDES +=

SOURCES += $(LIB_DIR)/L0_LowLevel/startup.cpp
SOURCES += $(LIB_DIR)/L0_LowLevel/interrupt.cpp

TESTS += $(LIB_DIR)/L0_LowLevel/interrupt.cpp
TESTS += $(LIB_DIR)/L0_LowLevel/test/interrupt_test.cpp
TESTS += $(LIB_DIR)/L0_LowLevel/test/system_controller_test.cpp
