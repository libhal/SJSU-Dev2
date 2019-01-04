INCLUDES += $(LIB_DIR)
SYSTEM_INCLUDES +=
SOURCES +=
TESTS +=

include $(LIB_DIR)/L0_LowLevel/L0.mk
include $(LIB_DIR)/L1_Drivers/L1.mk
include $(LIB_DIR)/L2_HAL/L2.mk
include $(LIB_DIR)/L3_Application/L3.mk
include $(LIB_DIR)/L4_Testing/L4.mk
include $(LIB_DIR)/newlib/newlib.mk
include $(LIB_DIR)/third_party/third_party.mk
include $(LIB_DIR)/utility/utility.mk
