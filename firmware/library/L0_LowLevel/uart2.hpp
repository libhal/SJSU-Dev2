// Minimum required implementations of uart2 to make print statements work
#pragma once

#include <cmath>
#include <cstdint>

#include "config.hpp"
#include "LPC40xx.h"
#include "L1_Drivers/gpio.hpp"

namespace uart2
{
extern PinInterface * rx;
extern PinInterface * tx;

extern LPC_UART_TypeDef * uart2_register;
extern LPC_SC_TypeDef * sc;

struct UartCalibration_t
{
  constexpr UartCalibration_t() : divide_latch(0), divide_add(0), multiply(1) {}
  uint32_t divide_latch;
  uint32_t divide_add;
  uint32_t multiply;
};

enum class States
{
  kCalculateIntegerDivideLatch,
  kCalculateDivideLatchWithDecimal,
  kDecimalFailedGenerateNewDecimal,
  kGenerateFractionFromDecimal,
  kDone
};

constexpr float kThreshold = 0.05f;

UartCalibration_t FindClosestFractional(float decimal);
float DividerEstimate(float baud_rate, float fraction_estimate = 1);
float FractionalEstimate(float baud_rate, float divider);
bool IsDecmial(float value);
UartCalibration_t GenerateUartCalibration(float baud_rate);

void Init(uint32_t baud_rate);
int GetChar();
char GetChar(uint32_t timeout = 0x7FFFFFFF);
int PutChar(int out);
char PutChar(char out);
void Puts(const char c_string[]);

}  // namespace uart2
