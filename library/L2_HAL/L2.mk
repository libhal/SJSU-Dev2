INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

TESTS += $(LIBRARY_DIR)/L2_HAL/test/device_memory_map_test.cpp

TESTS += $(LIBRARY_DIR)/L2_HAL/audio/test/buzzer_test.cpp

TESTS += $(LIBRARY_DIR)/L2_HAL/communication/test/esp8266_test.cpp

# TESTS += $(LIBRARY_DIR)/L2_HAL/displays/lcd/test/st7066u_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/displays/led/test/onboard_led_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/displays/oled/test/ssd1306_test.cpp

TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/optical/test/apds9960_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature/test/si7060_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/movement/accelerometer/test/mma8452q_test.cpp

TESTS += $(LIBRARY_DIR)/L2_HAL/switches/test/button_test.cpp

TESTS += $(LIBRARY_DIR)/L2_HAL/memory/test/sd_test.cpp
