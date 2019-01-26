# This file is meant to show how you can add your own set of tests to the list
# of user tests when you invoke "make user-test"
# If you require a source .cpp file to be linked into the test executable as
# well, add it to the USER_TESTS list as well.
USER_TESTS += $(LIB_DIR)/utility/test/bit_test.cpp
USER_TESTS += $(LIB_DIR)/L1_Drivers/test/pin_test.cpp
