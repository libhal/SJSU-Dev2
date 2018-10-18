#include <cmath>
#include <cstdint>

#include "config.hpp"
#include "LPC40xx.h"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/uart0.hpp"
#include "L1_Drivers/gpio.hpp"
#include "L2_Utilities/macros.hpp"

namespace uart0
{
Pin rx_pin(0, 3);
PinInterface * rx = &rx_pin;

Pin tx_pin(0, 2);
PinInterface * tx = &tx_pin;

LPC_UART_TypeDef * uart0_register = LPC_UART0;
LPC_SC_TypeDef * sc               = LPC_SC;

UartCalibration_t FindClosestFractional(float decimal)
{
  UartCalibration_t result;
  bool finished = false;

  for (int div = 0; div < 15 && !finished; div++)
  {
    for (int mul = div + 1; mul < 15 && !finished; mul++)
    {
      float divf         = static_cast<float>(div);
      float mulf         = static_cast<float>(mul);
      float test_decimal = 1.0f + divf / mulf;
      if (decimal <= test_decimal + kThreshold &&
          decimal >= test_decimal - kThreshold)
      {
        result.divide_add = div;
        result.multiply   = mul;
        finished          = true;
      }
    }
  }
  return result;
}

float DividerEstimate(float baud_rate, float fraction_estimate)
{
  return config::kSystemClockRate / (16.0f * baud_rate * fraction_estimate);
}

float FractionalEstimate(float baud_rate, float divider)
{
  return config::kSystemClockRate / (16.0f * baud_rate * divider);
}

bool IsDecmial(float value)
{
  bool result         = false;
  float rounded_value = roundf(value);
  float error         = value - rounded_value;
  if (-kThreshold <= error && error <= kThreshold)
  {
    result = true;
  }
  return result;
}

UartCalibration_t GenerateUartCalibration(float baud_rate)
{
  States state = States::kCalculateIntegerDivideLatch;
  UartCalibration_t uart_calibration;
  float divide_estimate = 0;
  float decimal         = 1.5;
  float div             = 1;
  float mul             = 2;
  while (state != States::kDone)
  {
    switch (state)
    {
      case States::kCalculateIntegerDivideLatch:
      {
        divide_estimate = DividerEstimate(baud_rate);
        if (divide_estimate < 1.0f)
        {
          uart_calibration.divide_latch = 0;
          state                         = States::kDone;
        }
        else if (IsDecmial(divide_estimate))
        {
          uart_calibration.divide_latch =
              static_cast<uint32_t>(divide_estimate);
          state = States::kDone;
        }
        else
        {
          state = States::kCalculateDivideLatchWithDecimal;
        }
        break;
      }
      case States::kCalculateDivideLatchWithDecimal:
      {
        divide_estimate = roundf(DividerEstimate(baud_rate, decimal));
        decimal         = FractionalEstimate(baud_rate, divide_estimate);
        if (1.1f <= decimal && decimal <= 1.9f)
        {
          state = States::kGenerateFractionFromDecimal;
        }
        else
        {
          state = States::kDecimalFailedGenerateNewDecimal;
        }
        break;
      }
      case States::kDecimalFailedGenerateNewDecimal:
      {
        mul += 1;

        if (div > 15)
        {
          state = States::kDone;
          break;
        }
        else if (mul > 15)
        {
          div += 1;
          mul = div + 1;
        }
        decimal = div / mul;
        state   = States::kCalculateDivideLatchWithDecimal;
        break;
      }
      case States::kGenerateFractionFromDecimal:
      {
        uart_calibration              = FindClosestFractional(decimal);
        uart_calibration.divide_latch = static_cast<uint32_t>(divide_estimate);
        state                         = States::kDone;
        break;
      }
      case States::kDone: { break;
      }
      default: { break;
      }
    }
  }
  return uart_calibration;
}

void Init(uint32_t baud_rate)
{
  // Adding 0.5 to perform rounding correctly since we do not want
  // 1.9 to round down to 1, but we want it to round-up to 2.
  float baud_rate_f          = static_cast<float>(baud_rate);
  UartCalibration_t dividers = GenerateUartCalibration(baud_rate_f);
  // Power on UART0
  sc->PCONP |= (1 << 3);

  constexpr uint8_t kEightBitDataLength = 3;
  constexpr uint8_t kUartFunction       = 1;
  constexpr uint8_t kDlabBit            = (1 << 7);

  tx->SetMode(PinInterface::Mode::kPullUp);
  rx->SetMode(PinInterface::Mode::kPullUp);
  tx->SetPinFunction(kUartFunction);
  rx->SetPinFunction(kUartFunction);

  uint8_t dlm = static_cast<uint8_t>((dividers.divide_latch >> 8) & 0xFF);
  uint8_t dll = static_cast<uint8_t>(dividers.divide_latch & 0xFF);
  uint8_t fdr = static_cast<uint8_t>((dividers.multiply & 0xF) << 4 |
                                     (dividers.divide_add & 0xF));

  uart0_register->LCR = kDlabBit;  // Set DLAB bit to access DLM & DLL
  uart0_register->DLM = dlm;
  uart0_register->DLL = dll;
  uart0_register->FDR = fdr;
  uart0_register->LCR = kEightBitDataLength;  // DLAB is reset back to zero
  uart0_register->FCR |= 1;                   // Enable FIFO
}

int GetChar()
{
  return GetChar(0xFFFFFFFF);
}

char GetChar(uint32_t timeout)
{
  uint64_t timeout_time = Milliseconds() + timeout;
  uint64_t current_time = Milliseconds();
  char result;
  while (!(uart0_register->LSR & 0x1) && current_time < timeout_time)
  {
    current_time = Milliseconds();
  }
  if (!(uart0_register->LSR & 0x1) && current_time >= timeout_time)
  {
    result = '\xFF';
  }
  else
  {
    result = uart0_register->RBR;
  }
  return result;
}

int PutChar(int out)
{
  return PutChar(static_cast<char>(out));
}

char PutChar(char out)
{
  uart0_register->THR = static_cast<uint8_t>(out);

  while (!(uart0_register->LSR & (0x1 << 6)))
  {
    continue;
  }
  return 1;
}

void Puts(const char * c_string)
{
  for (uint32_t i = 0; c_string[i] != '\0'; i++)
  {
    PutChar(c_string[i]);
  }
}

}  // namespace uart0
