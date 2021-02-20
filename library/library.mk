INCLUDES += $(LIBRARY_DIR)/

LINT_FILTER += library/third_party/

include $(LIBRARY_DIR)/platforms/L0.mk
include $(LIBRARY_DIR)/peripherals/L1.mk
include $(LIBRARY_DIR)/devices/L2.mk
include $(LIBRARY_DIR)/systems/L3.mk
include $(LIBRARY_DIR)/testing/L4.mk
include $(LIBRARY_DIR)/newlib/newlib.mk
include $(LIBRARY_DIR)/third_party/third_party.mk
include $(LIBRARY_DIR)/utility/utility.mk
