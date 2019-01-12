INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

TESTS += $(LIB_DIR)/L4_Testing/freertos_mocks.cpp
TESTS += $(LIB_DIR)/L4_Testing/main_test.cpp

USER_TESTS += $(LIB_DIR)/L4_Testing/freertos_mocks.cpp
USER_TESTS += $(LIB_DIR)/L4_Testing/main_test.cpp
