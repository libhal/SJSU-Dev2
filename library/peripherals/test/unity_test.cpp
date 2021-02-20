// =============================================================================
// platform Tests
// =============================================================================

#include "platforms/utility/test/ram_test.cpp"  // NOLINT

// =============================================================================
// Interface Test
// =============================================================================

#include "peripherals/test/adc_test.cpp"               // NOLINT
#include "peripherals/test/can_test.cpp"               // NOLINT
#include "peripherals/test/gpio_test.cpp"              // NOLINT
#include "peripherals/test/hardware_counter_test.cpp"  // NOLINT
#include "peripherals/test/i2c_test.cpp"               // NOLINT
#include "peripherals/test/interrupt_test.cpp"         // NOLINT
#include "peripherals/test/pin_test.cpp"               // NOLINT
#include "peripherals/test/pwm_test.cpp"               // NOLINT
#include "peripherals/test/uart_test.cpp"              // NOLINT

// =============================================================================
// stm32f10x implemenation test
// =============================================================================

#include "peripherals/stm32f10x/test/adc_test.cpp"                // NOLINT
#include "peripherals/stm32f10x/test/gpio_test.cpp"               // NOLINT
#include "peripherals/stm32f10x/test/pin_test.cpp"                // NOLINT
#include "peripherals/stm32f10x/test/spi_test.cpp"                // NOLINT
#include "peripherals/stm32f10x/test/system_controller_test.cpp"  // NOLINT
#include "peripherals/stm32f10x/test/uart_test.cpp"               // NOLINT

// =============================================================================
// stm32f4xx implemenation test
// =============================================================================

#include "peripherals/stm32f4xx/test/gpio_test.cpp"               // NOLINT
#include "peripherals/stm32f4xx/test/pin_test.cpp"                // NOLINT
#include "peripherals/stm32f4xx/test/system_controller_test.cpp"  // NOLINT

// =============================================================================
// msp432p401r implemenation test
// =============================================================================

#include "peripherals/msp432p401r/test/gpio_test.cpp"  // NOLINT
#include "peripherals/msp432p401r/test/pin_test.cpp"   // NOLINT

// =============================================================================
// lpc40xx implemenation test
// =============================================================================

#include "peripherals/lpc40xx/test/adc_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/can_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/dac_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/eeprom_test.cpp"             // NOLINT
#include "peripherals/lpc40xx/test/gpio_test.cpp"               // NOLINT
#include "peripherals/lpc40xx/test/i2c_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/pin_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/pulse_capture_test.cpp"      // NOLINT
#include "peripherals/lpc40xx/test/pwm_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/spi_test.cpp"                // NOLINT
#include "peripherals/lpc40xx/test/system_controller_test.cpp"  // NOLINT
#include "peripherals/lpc40xx/test/timer_test.cpp"              // NOLINT
#include "peripherals/lpc40xx/test/uart_test.cpp"               // NOLINT
#include "peripherals/lpc40xx/test/watchdog_test.cpp"           // NOLINT

// =============================================================================
// lpc17xx implemenation test
// =============================================================================

#include "peripherals/lpc17xx/test/pin_test.cpp"                // NOLINT
#include "peripherals/lpc17xx/test/system_controller_test.cpp"  // NOLINT

// =============================================================================
// cortex implemenation test
// =============================================================================

#include "peripherals/cortex/test/dwt_counter_test.cpp"   // NOLINT
#include "peripherals/cortex/test/interrupt_test.cpp"     // NOLINT
#include "peripherals/cortex/test/system_timer_test.cpp"  // NOLINT
