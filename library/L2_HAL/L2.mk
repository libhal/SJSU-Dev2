INCLUDES +=
SYSTEM_INCLUDES +=
SOURCES +=

NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/boards/%

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
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/communication/infrared_receiver.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/communication/internet_socket.hpp
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/communication/esp8266.hpp
TESTS += $(LIBRARY_DIR)/L2_HAL/communication/test/tsop752_test.cpp
# ==============================================================================
# Displays
# ==============================================================================
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/displays/pixel_display.hpp
# TODO: Remove this when st7066u_test.cpp doesn't take 15 minutes to compile
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/displays/lcd/st7066u.hpp
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
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/sensors/environment/light_sensor.hpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/light/test/temt6000x01_test.cpp
# ==============================================================================
# Sensor/Temperature
# ==============================================================================
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature_sensor.hpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature/test/si7060_test.cpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/environment/temperature/test/tmp102_test.cpp
# ==============================================================================
# Sensor/Movement
# ==============================================================================
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/sensors/movement/accelerometer.hpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/movement/accelerometer/test/mma8452q_test.cpp
#==============================================================================
# Sensor/Distance
# ==============================================================================
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/sensors/distance/distance_sensor.hpp
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
# ==============================================================================
# Battery
# ==============================================================================
NO_TEST_NEEDED += $(LIBRARY_DIR)/L2_HAL/sensors/battery/coulomb_counter.hpp
TESTS += $(LIBRARY_DIR)/L2_HAL/sensors/battery/test/ltc4150_test.cpp
