INCLUDES += $(LIBRARY_DIR)/L0_Platform/freertos

NO_TEST_NEEDED += $(LIBRARY_DIR)/L0_Platform/%

include $(LIBRARY_DIR)/L0_Platform/$(PLATFORM)/$(PLATFORM).mk
