NO_TEST_NEEDED += $(LIBRARY_DIR)/L4_Testing/%

TESTS += $(LIBRARY_DIR)/L4_Testing/freertos_mocks.cpp
TESTS += $(LIBRARY_DIR)/L4_Testing/main_test.cpp
TESTS += $(LIBRARY_DIR)/newlib/stdio.cpp

USER_TESTS += $(LIBRARY_DIR)/L4_Testing/freertos_mocks.cpp
USER_TESTS += $(LIBRARY_DIR)/L4_Testing/main_test.cpp
USER_TESTS += $(LIBRARY_DIR)/newlib/stdio.cpp