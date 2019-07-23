#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/uart.hpp"
#include "utility/status.hpp"
#include "utility/time.hpp"

using sjsu::lpc17xx::LPC_UART0_TypeDef;

namespace sjsu
{
namespace lpc40xx
{
namespace uart
{
constexpr float kThreshold = 0.05f;

struct UartCalibration_t
{
  uint32_t divide_latch = 0;
  uint32_t divide_add   = 0;
  uint32_t multiply     = 1;
};

constexpr UartCalibration_t FindClosestFractional(float decimal)
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

constexpr float DividerEstimate(float baud_rate,
                                float fraction_estimate       = 1,
                                uint32_t peripheral_frequency = 1)
{
  float clock_frequency = static_cast<float>(peripheral_frequency);
  return clock_frequency / (16.0f * baud_rate * fraction_estimate);
}

constexpr float FractionalEstimate(float baud_rate,
                                   float divider,
                                   uint32_t peripheral_frequency)
{
  float clock_frequency = static_cast<float>(peripheral_frequency);
  return clock_frequency / (16.0f * baud_rate * divider);
}

constexpr float RoundFloat(float value)
{
  return static_cast<float>(static_cast<int>(value + 0.5f));
}

constexpr bool IsDecmial(float value)
{
  bool result         = false;
  float rounded_value = RoundFloat(value);
  float error         = value - rounded_value;
  if (-kThreshold <= error && error <= kThreshold)
  {
    result = true;
  }
  return result;
}

enum class States
{
  kCalculateIntegerDivideLatch,
  kCalculateDivideLatchWithDecimal,
  kDecimalFailedGenerateNewDecimal,
  kGenerateFractionFromDecimal,
  kDone
};

constexpr static UartCalibration_t GenerateUartCalibration(
    uint32_t baud_rate, uint32_t peripheral_frequency)
{
  States state = States::kCalculateIntegerDivideLatch;
  UartCalibration_t uart_calibration;
  float baud_rate_float = static_cast<float>(baud_rate);
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
        divide_estimate =
            DividerEstimate(baud_rate_float, 1, peripheral_frequency);

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
        divide_estimate = RoundFloat(
            DividerEstimate(baud_rate_float, decimal, peripheral_frequency));
        decimal = FractionalEstimate(
            baud_rate_float, divide_estimate, peripheral_frequency);
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
}  // namespace uart

class Uart final : public sjsu::Uart
{
 public:
  using sjsu::Uart::Read;
  using sjsu::Uart::Write;

  static constexpr uint8_t kStandardUart = 0b011;
  struct Port_t
  {
    LPC_UART_TypeDef * registers;
    sjsu::SystemController::PeripheralID power_on_id;
    const sjsu::Pin & tx;
    const sjsu::Pin & rx;
    uint8_t tx_function_id : 3;
    uint8_t rx_function_id : 3;
  };

  struct Port  // NOLINT
  {
   private:
    inline static const Pin kUart0Tx = Pin(0, 2);
    inline static const Pin kUart0Rx = Pin(0, 3);

    inline static const Pin kUart2Tx = Pin(2, 8);
    inline static const Pin kUart2Rx = Pin(2, 9);

    inline static const Pin kUart3Tx = Pin(4, 28);
    inline static const Pin kUart3Rx = Pin(4, 29);

    inline static const Pin kUart4Tx = Pin(1, 29);
    inline static const Pin kUart4Rx = Pin(2, 9);

   public:
    inline static const Port_t kUart0 = {
      // NOTE: required since LPC_UART0 is of type LPC_UART0_TypeDef in lpc17xx
      // and LPC_UART_TypeDef in lpc40xx causing a "useless cast" warning when
      // compiled for, some odd reason, for either one being compiled, which
      // would make more sense if it only warned us with lpc40xx.
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART0_BASE),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart0,
      .tx             = kUart0Tx,
      .rx             = kUart0Rx,
      .tx_function_id = 0b001,
      .rx_function_id = 0b001,
    };

    inline static const Port_t kUart2 = {
      .registers      = LPC_UART2,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart2,
      .tx             = kUart2Tx,
      .rx             = kUart2Rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    inline static const Port_t kUart3 = {
      .registers      = LPC_UART3,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart3,
      .tx             = kUart3Tx,
      .rx             = kUart3Rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    inline static const Port_t kUart4 = {
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART4),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart4,
      .tx             = kUart4Tx,
      .rx             = kUart4Rx,
      .tx_function_id = 0b101,
      .rx_function_id = 0b011,
    };
  };
  static constexpr sjsu::lpc40xx::SystemController kLpc40xxSystemController =
      sjsu::lpc40xx::SystemController();

  explicit constexpr Uart(const Port_t & port,
                          const sjsu::SystemController & system_controller =
                              kLpc40xxSystemController)
      : port_(port), system_controller_(system_controller)
  {
  }

  Status Initialize(uint32_t baud_rate) const override
  {
    constexpr uint8_t kFIFOEnableAndReset = 0b111;
    system_controller_.PowerUpPeripheral(port_.power_on_id);

    SetBaudRate(baud_rate);

    port_.rx.SetPinFunction(port_.rx_function_id);
    port_.tx.SetPinFunction(port_.tx_function_id);
    port_.rx.SetPull(sjsu::Pin::Resistor::kPullUp);
    port_.tx.SetPull(sjsu::Pin::Resistor::kPullUp);
    port_.registers->FCR |= kFIFOEnableAndReset;

    return Status::kSuccess;
  }

  bool SetBaudRate(uint32_t baud_rate) const override
  {
    uart::UartCalibration_t calibration = uart::GenerateUartCalibration(
        baud_rate,
        system_controller_.GetPeripheralFrequency(port_.power_on_id));

    constexpr uint8_t kDlabBit = (1 << 7);

    uint8_t dlm = static_cast<uint8_t>((calibration.divide_latch >> 8) & 0xFF);
    uint8_t dll = static_cast<uint8_t>(calibration.divide_latch & 0xFF);
    uint8_t fdr = static_cast<uint8_t>((calibration.multiply & 0xF) << 4 |
                                       (calibration.divide_add & 0xF));

    port_.registers->LCR = kDlabBit;
    port_.registers->DLM = dlm;
    port_.registers->DLL = dll;
    port_.registers->FDR = fdr;
    port_.registers->LCR = kStandardUart;
    return true;
  }

  void Write(const uint8_t * data, size_t size) const override
  {
    for (size_t i = 0; i < size; i++)
    {
      port_.registers->THR = data[i];
      while (!TransmissionComplete())
      {
        continue;
      }
    }
  }

  Status Read(
      uint8_t * data,
      size_t size,
      uint32_t timeout = std::numeric_limits<uint32_t>::max()) const override
  {
    // NOTE: Consider changing this to using a Wait() call.
    Status status       = Status::kTimedOut;
    uint64_t start_time = Milliseconds();
    size_t position     = 0;
    while (Milliseconds() < (start_time + timeout))
    {
      if (HasData())
      {
        data[position++] = static_cast<uint8_t>(port_.registers->RBR);
      }
      if (position >= size)
      {
        status = Status::kSuccess;
        break;
      }
    }
    return status;
  }
  bool HasData() const override
  {
    return bit::Read(port_.registers->LSR, 0);
  }

 private:
  bool TransmissionComplete() const
  {
    return bit::Read(port_.registers->LSR, 5);
  }

  const Port_t & port_;
  const sjsu::SystemController & system_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
