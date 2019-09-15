INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

# ==============================================================================
# Example HAL Tests
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/test/device_memory_map_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/test/example_test.cpp
# ==============================================================================
# Audio
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/audio/test/buzzer_test.cpp
# ==============================================================================
# Communication
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/communication/test/esp8266_test.cpp
# ==============================================================================
# Displays
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/displays/led/test/onboard_led_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/displays/oled/test/ssd1306_test.cpp
# TESTS += $(LIBRARY_DIR)/L2_HAL/displays/lcd/test/st7066u_test.cpp
# ==============================================================================
# I/O
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/io/test/parallel_bus_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/io/parallel_bus/test/parallel_gpio_test.cpp
# ==============================================================================
# Sensor/Optical
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/optical/test/apds9960_test.cpp
# ==============================================================================
# Sensor/Environment
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/light/test/temt6000x01_test.cpp
# ==============================================================================
# Sensor/Temperature
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature/test/si7060_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature/test/tmp102_test.cpp
# ==============================================================================
# Sensor/Movement
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/movement/accelerometer/test/mma8452q_test.cpp
#==============================================================================
# Sensor/Distance
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/distance/time_of_flight/test/tfmini_test.cpp
# ==============================================================================
# Switches
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/switches/test/button_test.cpp
# ==============================================================================
# Memory
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/memory/test/sd_test.cpp
# ==============================================================================
# Actuators
# ==============================================================================
TESTS += $(LIBRARY_DIR)/L2_HAL/actuators/servo/test/servo_test.cpp
