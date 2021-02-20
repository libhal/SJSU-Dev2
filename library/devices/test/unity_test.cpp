
// =============================================================================
// Example HAL Tests
// =============================================================================
#include "devices/test/memory_access_protocol_test.cpp"  // NOLINT

// =============================================================================
// Audio
// =============================================================================
#include "devices/audio/test/buzzer_test.cpp"  // NOLINT

// =============================================================================
// Communication
// =============================================================================
#include "devices/communication/test/tsop752_test.cpp"  // NOLINT

// =============================================================================
// Displays
// =============================================================================
#include "devices/displays/lcd/test/st7066u_test.cpp"   // NOLINT
// #include "devices/displays/oled/test/ssd1306_test.cpp"  // NOLINT

// =============================================================================
// I/O
// =============================================================================
#include "devices/io/test/parallel_bus_test.cpp"                // NOLINT
#include "devices/io/parallel_bus/test/parallel_gpio_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Optical
// =============================================================================
// #include "devices/sensors/optical/test/apds9960_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Environment
// =============================================================================
#include "devices/sensors/environment/light/test/temt6000x01_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Temperature
// =============================================================================
#include "devices/sensors/environment/temperature/test/si7060_test.cpp"  // NOLINT
#include "devices/sensors/environment/temperature/test/tmp102_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Movement
// =============================================================================
#include "devices/sensors/movement/accelerometer/test/mma8452q_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Distance
// =============================================================================
#include "devices/sensors/distance/time_of_flight/test/tfmini_test.cpp"  // NOLINT

// =============================================================================
// Sensor/Signal
// =============================================================================
#include "devices/sensors/signal/test/frequency_counter_test.cpp"  // NOLINT

// =============================================================================
// Switches
// =============================================================================
#include "devices/switches/test/button_test.cpp"  // NOLINT

// =============================================================================
// Memory
// =============================================================================
#include "devices/memory/test/sd_test.cpp"  // NOLINT

// =============================================================================
// Actuators
// =============================================================================
#include "devices/actuators/servo/test/servo_test.cpp"  // NOLINT
#include "devices/actuators/servo/test/rmd_x_test.cpp"  // NOLINT

// =============================================================================
// Battery
// =============================================================================
#include "devices/sensors/battery/test/ltc4150_test.cpp"  // NOLINT
