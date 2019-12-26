INCLUDES += $(LIBRARY_DIR)/
SYSTEM_INCLUDES +=
SOURCES +=
TESTS += $(LIBRARY_DIR)/test/config_test.cpp
TESTS += $(LIBRARY_DIR)/test/log_levels_test.cpp

LINT_FILTER += $(LIBRARY_DIR)/project_config.hpp
LINT_FILTER += $(LIBRARY_DIR)/third_party/

NO_TEST_NEEDED += $(LIBRARY_DIR)/newlib/%

include $(LIBRARY_DIR)/L0_Platform/L0.mk
include $(LIBRARY_DIR)/L1_Peripheral/L1.mk
include $(LIBRARY_DIR)/L2_HAL/L2.mk
include $(LIBRARY_DIR)/L3_Application/L3.mk
include $(LIBRARY_DIR)/L4_Testing/L4.mk
include $(LIBRARY_DIR)/newlib/newlib.mk
include $(LIBRARY_DIR)/third_party/third_party.mk
include $(LIBRARY_DIR)/utility/utility.mk
