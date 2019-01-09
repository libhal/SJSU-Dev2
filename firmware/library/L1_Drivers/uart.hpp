#pragma once

#include <array>
#include <cmath>
#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/time.hpp"

class UartInterface
{
  virtual void SetBaudRate(uint32_t baud_rate) = 0;
  virtual bool Initialize(uint32_t baud_rate)  = 0;
  virtual void Send(uint8_t out)               = 0;
  virtual uint8_t Receive(uint32_t timeout)    = 0;
};

class Uart final : public UartInterface, protected Lpc40xxSystemController
{
 public:
  enum Pins : uint8_t
  {
    kTx = 0,
    kRx = 1
  };

  enum class Channels : uint8_t
  {
    kUart0 = 0,
    kUart2 = 1,
    kUart3 = 2,
    kUart4 = 3
  };

  enum class States
  {
    kCalculateIntegerDivideLatch,
    kCalculateDivideLatchWithDecimal,
    kDecimalFailedGenerateNewDecimal,
    kGenerateFractionFromDecimal,
    kDone
  };

  struct UartCalibration_t
  {
    constexpr UartCalibration_t() : divide_latch(0), divide_add(0), multiply(1)
    {
    }
    uint32_t divide_latch;
    uint32_t divide_add;
    uint32_t multiply;
  };

  // Each port ony has 1 usable set of pins
  // UART0 pins Tx = 0,0; Rx = 0,1
  // UART2 pins Tx = 2,8; Rx = 2,9
  // UART3 pins Tx = 4,28; Rx = 4,29
  // UART4 pins Tx = 1,29; Rx = 2,9
  static constexpr uint8_t kStandardUart = 0b011;
  static constexpr float kThreshold      = 0.05f;

  static constexpr uint8_t kTxUartPortFunction[4] = { 0b001, 0b010, 0b010,
                                                      0b101 };
  static constexpr uint8_t kRxUartPortFunction[4] = { 0b001, 0b010, 0b010,
                                                      0b011 };
  static constexpr Lpc40xxSystemController::PeripheralID kPowerbit[] = {
    Lpc40xxSystemController::Peripherals::kUart0,
    Lpc40xxSystemController::Peripherals::kUart2,
    Lpc40xxSystemController::Peripherals::kUart3,
    Lpc40xxSystemController::Peripherals::kUart4
  };

  inline static LPC_UART_TypeDef * uart[4] = {
    [0] = LPC_UART0,
    [1] = LPC_UART2,
    [2] = LPC_UART3,
    [3] = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART4)
  };

  inline static Pin pairs[4][2] = { { Pin(0, 2), Pin(0, 3) },
                                    { Pin(2, 8), Pin(2, 9) },
                                    { Pin(4, 28), Pin(4, 29) },
                                    { Pin(1, 29), Pin(2, 9) } };
  // Not using a default constructor. User must define the Uart channel in order
  // to properly define pins. User defined constructor. Must be commented out to
  // use unit testing constructor below
  explicit constexpr Uart(Channels mode)
      : channel_(static_cast<uint8_t>(mode)),
        tx_(&pairs[channel_][Pins::kTx]),
        rx_(&pairs[channel_][Pins::kRx])
  {
  }
  // Pass you own preconfigured pins through this constructor
  constexpr Uart(Channels mode, PinInterface * tx_pin, PinInterface * rx_pin)
      : channel_(static_cast<uint8_t>(mode)), tx_(tx_pin), rx_(rx_pin)
  {
  }

  void SetBaudRate(uint32_t baud_rate) override
  {
    constexpr uint8_t kDlabBit = (1 << 7);
    float baudrate             = static_cast<float>(baud_rate);
    UartCalibration_t dividers = GenerateUartCalibration(baudrate);

    uint8_t dlm = static_cast<uint8_t>((dividers.divide_latch >> 8) & 0xFF);
    uint8_t dll = static_cast<uint8_t>(dividers.divide_latch & 0xFF);
    uint8_t fdr = static_cast<uint8_t>((dividers.multiply & 0xF) << 4 |
                                       (dividers.divide_add & 0xF));

    // Set baud rate
    uart[channel_]->LCR = kDlabBit;
    uart[channel_]->DLM = dlm;
    uart[channel_]->DLL = dll;
    uart[channel_]->FDR = fdr;
    uart[channel_]->LCR = kStandardUart;
  }

  bool Initialize(uint32_t baud_rate) override
  {
    constexpr uint8_t kFIFOEnableAndReset = 0b111;
    // Powering the port
    PowerUpPeripheral(kPowerbit[channel_]);
    // Setting the pin functions and modes
    rx_->SetPinFunction(kRxUartPortFunction[channel_]);
    tx_->SetPinFunction(kTxUartPortFunction[channel_]);
    rx_->SetMode(PinInterface::Mode::kPullUp);
    tx_->SetMode(PinInterface::Mode::kPullUp);
    // Baud rate setting
    SetBaudRate(baud_rate);
    uart[channel_]->FCR |= kFIFOEnableAndReset;
    return true;
  }

  void Send(uint8_t data) override
  {
    uart[channel_]->THR              = data;
    auto wait_for_transfer_to_finish = [this]() -> bool {
      return (uart[channel_]->LSR & (1 << 5));
    };
    Wait(kMaxWait, wait_for_transfer_to_finish);
  }

  uint8_t Receive([[maybe_unused]] uint32_t timeout = 0x7FFFFFFF) override
  {
    uint8_t receiver   = '\xFF';
    auto byte_recieved = [this]() -> bool {
      return (uart[channel_]->LSR & (1 << 0));
    };

    Status status = Wait(timeout, byte_recieved);

    if (status == Status::kSuccess)
    {
      receiver = static_cast<uint8_t>(uart[channel_]->RBR);
    }
    return receiver;
  }

 private:
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

  float DividerEstimate(float baud_rate, float fraction_estimate = 1)
  {
    float clock_frequency = static_cast<float>(GetPeripheralFrequency());
    return clock_frequency / (16.0f * baud_rate * fraction_estimate);
  }

  float FractionalEstimate(float baud_rate, float divider)
  {
    float clock_frequency = static_cast<float>(GetPeripheralFrequency());
    return clock_frequency / (16.0f * baud_rate * divider);
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
          uart_calibration = FindClosestFractional(decimal);
          uart_calibration.divide_latch =
              static_cast<uint32_t>(divide_estimate);
          state = States::kDone;
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

  uint8_t channel_;
  PinInterface * tx_;
  PinInterface * rx_;
};

inline Uart uart0(Uart::Channels::kUart0);
