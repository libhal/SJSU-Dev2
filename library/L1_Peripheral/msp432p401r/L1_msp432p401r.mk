TESTS += $(LIBRARY_DIR)/L1_Peripheral/msp432p401r/test/gpio_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/msp432p401r/test/pin_test.cpp

# TODO(#1271): Add this back when we figure out a solution to GENERATE and
#              subcases
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/msp432p401r/system_controller.hpp
