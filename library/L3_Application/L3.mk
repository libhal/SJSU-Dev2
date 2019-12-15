INCLUDES +=
SYSTEM_INCLUDES +=

SOURCES += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp

TESTS += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/commandline_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/graphics_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/graphical_terminal_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/periodic_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/arm_system_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/rtos_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/i2c_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/common_test.cpp
