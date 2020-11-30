INCLUDES += $(LIBRARY_DIR)/

LINT_FILTER += library/third_party/

include $(LIBRARY_DIR)/L0_Platform/L0.mk
include $(LIBRARY_DIR)/L1_Peripheral/L1.mk
include $(LIBRARY_DIR)/L2_HAL/L2.mk
include $(LIBRARY_DIR)/L3_Application/L3.mk
include $(LIBRARY_DIR)/L4_Testing/L4.mk
include $(LIBRARY_DIR)/newlib/newlib.mk
include $(LIBRARY_DIR)/third_party/third_party.mk
include $(LIBRARY_DIR)/utility/utility.mk
