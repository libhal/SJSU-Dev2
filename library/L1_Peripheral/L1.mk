include $(LIBRARY_DIR)/L1_Peripheral/cortex/L1_cortex.mk
include $(LIBRARY_DIR)/L1_Peripheral/example/L1_example.mk
include $(LIBRARY_DIR)/L1_Peripheral/lpc17xx/L1_lpc17xx.mk
include $(LIBRARY_DIR)/L1_Peripheral/lpc40xx/L1_lpc40xx.mk
include $(LIBRARY_DIR)/L1_Peripheral/stm32f10x/L1_stm32f10x.mk
include $(LIBRARY_DIR)/L1_Peripheral/stm32f4xx/L1_stm32f4xx.mk

TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/gpio_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/hardware_counter_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/i2c_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/interrupt_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/pin_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/pwm_test.cpp
TESTS += $(LIBRARY_DIR)/L1_Peripheral/test/uart_test.cpp

NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/adc.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/can.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/dac.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/example.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/inactive.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/pulse_capture.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/pwm.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/spi.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/storage.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/system_controller.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/system_timer.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/timer.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L1_Peripheral/watchdog.hpp

