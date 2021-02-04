INCLUDES += $(LIBRARY_DIR)/platforms/operating_systems/freertos

NO_TEST_NEEDED += $(LIBRARY_DIR)/platforms/%

include $(LIBRARY_DIR)/platforms/targets/$(PLATFORM)/$(PLATFORM).mk
