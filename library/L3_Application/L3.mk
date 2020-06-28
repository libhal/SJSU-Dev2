# ==============================================================================
# Task Scheduler
# ==============================================================================
SOURCES += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/task_scheduler.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/periodic_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_test.cpp

# ==============================================================================
# FILE I/O
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L3_Application/file_io/test/fatfs_test.cpp

SOURCES += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp
USER_TESTS += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp
# Test is handled by fatfs_test.cpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp

# ==============================================================================
# Graphics
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L3_Application/test/graphics_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/graphical_terminal_test.cpp

# ==============================================================================
# Command line
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/arm_system_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/rtos_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/i2c_command_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/commands/test/common_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/commandline_test.cpp

# ==============================================================================
# Unity Test
# ==============================================================================
UNITY_TESTS += $(LIBRARY_DIR)/L3_Application/test/unity_test_l3.cpp
UNITY_TESTS += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp
