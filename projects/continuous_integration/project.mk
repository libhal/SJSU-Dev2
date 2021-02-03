PLATFORM = lpc40xx

# ==============================================================================
# UNIT TESTING
# ==============================================================================

# Unity Files
TESTS += $(LIBRARY_DIR)/peripherals/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/devices/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/systems/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/unity_test.cpp
