#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

#include "platforms/targets/lpc17xx/LPC17xx.h"
#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "peripherals/uart.hpp"
#include "utility/error_handling.hpp"
#include "utility/time/time.hpp"

using sjsu::lpc17xx::LPC_UART0_TypeDef;

namespace sjsu
{
namespace lpc40xx
{
namespace uart
{
/// UART baud error threshold. Used to check if a fractional value is reasonable
/// close to the desired value.
constexpr float kThreshold = 0.01f;
/// Structure containing all of the information that a lpc40xx UART needs to
/// achieve its desired baud rate.
struct UartCalibration_t
{
  /// Main clock divider
  uint32_t divide_latch = 0;
  /// Fractional divisor to trim the UART baud rate into the proper rate
  uint32_t divide_add = 0;
  /// Fractional numerator to trim the UART baud rate into the proper rate.
  uint32_t multiply = 1;
};
/// @param decimal - the number to approximate.
/// @return Will generate a UartCalibration_t that attempts to find a fractional
/// value that closely matches the input decimal number as much as possible.
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
/// @param baud_rate - desired baud rate.
/// @param fraction_estimate - corrissponds to the result of UartCalibration_t
///        divide_add/multiply.
/// @param peripheral_frequency - input source frequency.
/// @return an estimate for the baud rate divider
constexpr float DividerEstimate(float baud_rate,
                                float fraction_estimate       = 1,
                                uint32_t peripheral_frequency = 1)
{
  float clock_frequency = static_cast<float>(peripheral_frequency);
  return clock_frequency / (16.0f * baud_rate * fraction_estimate);
}
/// @param baud_rate - desired baud rate.
/// @param divider - clock divider for the baud rate.
/// @param peripheral_frequency - input source frequency.
/// @return a fraction that would get the baud rate as close to desired baud
///         rate, given the input divider.
constexpr float FractionalEstimate(float baud_rate,
                                   float divider,
                                   uint32_t peripheral_frequency)
{
  float clock_frequency = static_cast<float>(peripheral_frequency);
  return clock_frequency / (16.0f * baud_rate * divider);
}
/// @param value - value to round
/// @return rounded up and truncated value
constexpr float RoundFloat(float value)
{
  return static_cast<float>(static_cast<int>(value + 0.5f));
}
/// @param value input float value.
/// @return true if value is within our threshold of a decimal number, false
///         otherwise.
constexpr bool IsDecimal(float value)
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
/// States for the uart calibration state machine.
enum class States
{
  kCalculateIntegerDivideLatch,
  kCalculateDivideLatchWithDecimal,
  kDecimalFailedGenerateNewDecimal,
  kGenerateFractionFromDecimal,
  kDone
};
/// @param baud_rate - desire baud rate
/// @param peripheral_frequency - input clock source frequency
/// @return UartCalibration_t that will get the output baud rate as close to the
///         desired baud_rate as possible.
constexpr static UartCalibration_t GenerateUartCalibration(
    uint32_t baud_rate,
    units::frequency::hertz_t peripheral_frequency)
{
  uint32_t integer_peripheral_frequency =
      units::unit_cast<uint32_t>(peripheral_frequency);
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
            DividerEstimate(baud_rate_float, 1, integer_peripheral_frequency);

        if (divide_estimate < 1.0f)
        {
          uart_calibration.divide_latch = 0;
          state                         = States::kDone;
        }
        else if (IsDecimal(divide_estimate))
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
        divide_estimate = RoundFloat(DividerEstimate(
            baud_rate_float, decimal, integer_peripheral_frequency));
        decimal         = FractionalEstimate(
            baud_rate_float, divide_estimate, integer_peripheral_frequency);
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
      case States::kDone:
      default: break;
    }
  }
  return uart_calibration;
}
}  // namespace uart

/// Implementation of the UART peripheral for the LPC40xx family of
/// microcontrollers.
class Uart final : public sjsu::Uart
{
 public:
  using sjsu::Uart::Read;
  using sjsu::Uart::Write;

  /// Bit code for enabling standard uart mode.
  static constexpr uint8_t kStandardUart = 0b011;
  /// Bit code for resetting UART FIFO and enabling peripheral
  static constexpr uint8_t kEnableAndResetFIFO = 0b111;
  /// Bit code to power down UART driver
  static constexpr uint8_t kPowerDown = ~kEnableAndResetFIFO;

  /// Port contains all of the information that the lpc40xx uart port needs to
  /// operate.
  struct Port_t
  {
    /// Address of the LPC_UART peripheral in memory
    LPC_UART_TypeDef * registers;

    /// ResourceID of the UART peripheral to power on at initialization.
    sjsu::SystemController::ResourceID power_on_id;

    /// Refernce to a uart transmitter pin
    sjsu::Pin & tx;

    /// Refernce to a uart receiver pin
    sjsu::Pin & rx;

    /// Function code to set the transmit pin to uart transmitter
    uint8_t tx_function_id : 3;

    /// Function code to set the receive pin to uart receiver
    uint8_t rx_function_id : 3;
  };

  /// @param port - a reference to a constant lpc40xx::Uart::Port_t definition
  explicit constexpr Uart(const Port_t & port) : port_(port) {}

  void ModuleInitialize() override
  {
    sjsu::SystemController::GetPlatformController().PowerUpPeripheral(
        port_.power_on_id);

    ConfigureBaudRate();
    ConfigureFormat();

    port_.rx.settings.function = port_.rx_function_id;
    port_.tx.settings.function = port_.tx_function_id;
    port_.rx.settings.PullUp();
    port_.tx.settings.PullUp();

    port_.rx.Initialize();
    port_.tx.Initialize();

    port_.registers->FCR |= kEnableAndResetFIFO;
  }

  void ModulePowerDown() override
  {
    port_.registers->FCR &= kPowerDown;
  }

  void Write(std::span<const uint8_t> data) override
  {
    for (const auto & byte : data)
    {
      port_.registers->THR = byte;
      while (!TransmissionComplete())
      {
        continue;
      }
    }
  }

  size_t Read(std::span<uint8_t> data) override
  {
    size_t index = 0;

    for (auto & byte : data)
    {
      if (!HasData())
      {
        break;
      }
      byte = port_.registers->RBR;
      index++;
    }

    return index;
  }

  bool HasData() override
  {
    return bit::Read(port_.registers->LSR, 0);
  }

 private:
  void ConfigureFormat()
  {
    // To be continued...
  }

  void ConfigureBaudRate()
  {
    constexpr uint8_t kDlabBit = (1 << 7);
    auto & system             = sjsu::SystemController::GetPlatformController();
    auto peripheral_frequency = system.GetClockRate(port_.power_on_id);

    uart::UartCalibration_t calibration =
        uart::GenerateUartCalibration(settings.baud_rate, peripheral_frequency);

    uint8_t dlm = static_cast<uint8_t>((calibration.divide_latch >> 8) & 0xFF);
    uint8_t dll = static_cast<uint8_t>(calibration.divide_latch & 0xFF);
    uint8_t fdr = static_cast<uint8_t>((calibration.multiply & 0xF) << 4 |
                                       (calibration.divide_add & 0xF));

    port_.registers->LCR = kDlabBit;
    port_.registers->DLM = dlm;
    port_.registers->DLL = dll;
    port_.registers->FDR = fdr;
    port_.registers->LCR = kStandardUart;
  }

  /// @return true if port is still sending the byte.
  bool TransmissionComplete()
  {
    return bit::Read(port_.registers->LSR, 5);
  }

  /// const reference to lpc40xx::Uart::Port_t definition
  const Port_t & port_;
};

template <int port>
inline Uart & GetUart()
{
  if constexpr (port == 0)
  {
    static Pin & uart0_tx = sjsu::lpc40xx::GetPin<0, 2>();
    static Pin & uart0_rx = sjsu::lpc40xx::GetPin<0, 3>();

    /// Definition for uart port 0 for lpc40xx.
    static const Uart::Port_t kUart0 = {
      // NOTE: required since LPC_UART0 is of type LPC_UART0_TypeDef in lpc17xx
      // and LPC_UART_TypeDef in lpc40xx causing a "useless cast" warning when
      // compiled for, some odd reason, for either one being compiled, which
      // would make more sense if it only warned us with lpc40xx.
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART0_BASE),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart0,
      .tx             = uart0_tx,
      .rx             = uart0_rx,
      .tx_function_id = 0b001,
      .rx_function_id = 0b001,
    };

    static Uart uart0(kUart0);
    return uart0;
  }
  else if constexpr (port == 2)
  {
    static Pin & uart2_tx = sjsu::lpc40xx::GetPin<2, 8>();
    static Pin & uart2_rx = sjsu::lpc40xx::GetPin<2, 9>();

    /// Definition for uart port 1 for lpc40xx.
    static const Uart::Port_t kUart2 = {
      .registers      = LPC_UART2,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart2,
      .tx             = uart2_tx,
      .rx             = uart2_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart uart2(kUart2);
    return uart2;
  }
  else if constexpr (port == 3)
  {
    static Pin & uart3_tx = sjsu::lpc40xx::GetPin<4, 28>();
    static Pin & uart3_rx = sjsu::lpc40xx::GetPin<4, 29>();

    /// Definition for uart port 2 for lpc40xx.
    static const Uart::Port_t kUart3 = {
      .registers      = LPC_UART3,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart3,
      .tx             = uart3_tx,
      .rx             = uart3_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart uart3(kUart3);
    return uart3;
  }
  else if constexpr (port == 4)
  {
    static Pin & uart4_tx = sjsu::lpc40xx::GetPin<1, 29>();
    static Pin & uart4_rx = sjsu::lpc40xx::GetPin<2, 9>();

    /// Definition for uart port 3 for lpc40xx.
    static const Uart::Port_t kUart4 = {
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART4),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart4,
      .tx             = uart4_tx,
      .rx             = uart4_rx,
      .tx_function_id = 0b101,
      .rx_function_id = 0b011,
    };

    static Uart uart4(kUart4);
    return uart4;
  }
  else
  {
    static_assert(
        InvalidOption<port>,
        SJ2_ERROR_MESSAGE_DECORATOR("Support UART ports for LPC40xx are UART0, "
                                    "UART2, UART3, and UART4."));
    return GetUart<0>();
  }
}
}  // namespace lpc40xx
}  // namespace sjsu
