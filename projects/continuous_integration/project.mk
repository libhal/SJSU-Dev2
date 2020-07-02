PLATFORM = lpc40xx

# ==============================================================================
# UNIT TESTING
# ==============================================================================

# Unity Files
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/L3_Application/test/unity_test.cpp
TESTS += $(LIBRARY_DIR)/utility/test/unity_test.cpp
