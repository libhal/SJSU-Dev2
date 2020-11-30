// =============================================================================
// L0 Platform Tests
// =============================================================================
#include "L0_Platform/test/ram_test.cpp"  // NOLINT

// =============================================================================
// Interface Test
// =============================================================================
#include "L1_Peripheral/test/adc_test.cpp"               // NOLINT
#include "L1_Peripheral/test/gpio_test.cpp"              // NOLINT
#include "L1_Peripheral/test/hardware_counter_test.cpp"  // NOLINT
#include "L1_Peripheral/test/i2c_test.cpp"               // NOLINT
#include "L1_Peripheral/test/interrupt_test.cpp"         // NOLINT
#include "L1_Peripheral/test/pin_test.cpp"               // NOLINT
#include "L1_Peripheral/test/pwm_test.cpp"               // NOLINT
#include "L1_Peripheral/test/uart_test.cpp"              // NOLINT

// =============================================================================
// stm32f10x implemenation test
// =============================================================================
#include "L1_Peripheral/stm32f10x/test/adc_test.cpp"                // NOLINT
#include "L1_Peripheral/stm32f10x/test/gpio_test.cpp"               // NOLINT
#include "L1_Peripheral/stm32f10x/test/pin_test.cpp"                // NOLINT
#include "L1_Peripheral/stm32f10x/test/system_controller_test.cpp"  // NOLINT
#include "L1_Peripheral/stm32f10x/test/uart_test.cpp"               // NOLINT

// =============================================================================
// stm32f4xx implemenation test
// =============================================================================
#include "L1_Peripheral/stm32f4xx/test/gpio_test.cpp"               // NOLINT
#include "L1_Peripheral/stm32f4xx/test/pin_test.cpp"                // NOLINT
#include "L1_Peripheral/stm32f4xx/test/system_controller_test.cpp"  // NOLINT

// =============================================================================
// msp432p401r implemenation test
// =============================================================================
#include "L1_Peripheral/msp432p401r/test/gpio_test.cpp"  // NOLINT
#include "L1_Peripheral/msp432p401r/test/pin_test.cpp"   // NOLINT

// =============================================================================
// lpc40xx implemenation test
// =============================================================================
#include "L1_Peripheral/lpc40xx/test/adc_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/can_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/dac_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/eeprom_test.cpp"             // NOLINT
#include "L1_Peripheral/lpc40xx/test/gpio_test.cpp"               // NOLINT
#include "L1_Peripheral/lpc40xx/test/i2c_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/pin_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/pulse_capture_test.cpp"      // NOLINT
#include "L1_Peripheral/lpc40xx/test/pwm_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/spi_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc40xx/test/system_controller_test.cpp"  // NOLINT
#include "L1_Peripheral/lpc40xx/test/timer_test.cpp"              // NOLINT
#include "L1_Peripheral/lpc40xx/test/uart_test.cpp"               // NOLINT
#include "L1_Peripheral/lpc40xx/test/watchdog_test.cpp"           // NOLINT

// =============================================================================
// lpc17xx implemenation test
// =============================================================================
#include "L1_Peripheral/lpc17xx/test/pin_test.cpp"                // NOLINT
#include "L1_Peripheral/lpc17xx/test/system_controller_test.cpp"  // NOLINT

// =============================================================================
// cortex implemenation test
// =============================================================================
#include "L1_Peripheral/cortex/test/dwt_counter_test.cpp"   // NOLINT
#include "L1_Peripheral/cortex/test/interrupt_test.cpp"     // NOLINT
#include "L1_Peripheral/cortex/test/system_timer_test.cpp"  // NOLINT
