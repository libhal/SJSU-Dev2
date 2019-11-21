include $(LIBRARY_DIR)/L1_Peripheral/cortex/L1_cortex.mk
include $(LIBRARY_DIR)/L1_Peripheral/example/L1_example.mk
include $(LIBRARY_DIR)/L1_Peripheral/lpc17xx/L1_lpc17xx.mk
include $(LIBRARY_DIR)/L1_Peripheral/lpc40xx/L1_lpc40xx.mk

TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/interrupt_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/hardware_counter_test.cpp
