INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

TESTS += $(LIBRARY_DIR)/L4_Testing/freertos_mocks.cpp
TESTS += $(LIBRARY_DIR)/L4_Testing/main_test.cpp

USER_TESTS += $(LIBRARY_DIR)/L4_Testing/freertos_mocks.cpp
USER_TESTS += $(LIBRARY_DIR)/L4_Testing/main_test.cpp
