# ==============================================================================
# Task Scheduler
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L3_Application/test/task_scheduler_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/periodic_scheduler_test.cpp

# ==============================================================================
# FILE I/O
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L3_Application/file_io/test/fatfs_test.cpp

SOURCES += $(LIBRARY_DIR)/L3_Application/file_io/diskio.cpp
