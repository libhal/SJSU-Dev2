#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

#include "peripherals/cortex/interrupt.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "peripherals/uart.hpp"
#include "platforms/targets/lpc17xx/LPC17xx.h"
#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "third_party/ring_span/ring_span.hpp"
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
constexpr float kThreshold = 0.015f;

/// Contains the float fraction value as well as the immediate byte code to
/// generate it when assigned to the FDR register.
struct UartFractionals
{
  /// Fraction value
  float fraction;
  /// FDR value
  uint8_t byte_code;
};

/// Simply holds an oversized version of the baud rate table to be shrunk down
/// in a later step.
struct TableIntermediate
{
  /// Oversized array of UartFractionals
  std::array<UartFractionals, 1000> element{};
  /// How many entries within element contain a fraction and byte_code
  int valid_entries = 0;
};

/// Generate the table using and returning the TableIntermediate which will be
/// truncated by GenerateBaudRateTable()
static constexpr auto GenerateOversizedBaudTable() noexcept
{
  constexpr uint32_t numerator_min   = 1;
  constexpr uint32_t denominator_min = 1;
  constexpr uint32_t denominator_max = 15;

  TableIntermediate table{};

  // First element is the fractional 1.0f value. This allows baud rates that
  // have dividers that are already integers, on the first iteration of the
  // fractional value, this will match and the loop will stop.
  table.element[table.valid_entries++] = UartFractionals{
    .fraction  = 1,
    .byte_code = (1 << 4) | (0),
  };

  // Skip zero numerator
  for (uint32_t denominator = denominator_min; denominator < denominator_max;
       denominator++)
  {
    for (uint32_t numerator = numerator_min; numerator < denominator;
         numerator++)
    {
      float fraction = 1.0f + static_cast<float>(numerator) /
                                  static_cast<float>(denominator);
      if (1.1f <= fraction && fraction <= 1.9f)
      {
        table.element[table.valid_entries++] = UartFractionals{
          .fraction  = fraction,
          .byte_code = static_cast<uint8_t>((denominator << 4) | (numerator)),
        };
      }
    }
  }

  return table;
}

/// Generate the baudrate table.
constexpr auto GenerateBaudRateTable()
{
  constexpr auto kLargeTable = GenerateOversizedBaudTable();
  std::array<UartFractionals, kLargeTable.valid_entries> table{};

  for (size_t i = 0; i < table.size(); i++)
  {
    table[i] = kLargeTable.element[i];
  }

  return table;
}

constexpr auto kBaudTable = GenerateBaudRateTable();

/// Structure containing all of the information that a lpc40xx UART needs to
/// achieve its desired baud rate.
struct UartCalibration_t
{
  /// Main clock divider, default to minimum of 2
  uint32_t divide_latch = 0;
  /// Fractional divisor to trim the UART baud rate into the proper rate
  uint8_t fraction = 0;
};

/// @param decimal - the number to approximate.
/// @return Will generate a UartCalibration_t that attempts to find a fractional
/// value that closely matches the input decimal number as much as possible.
constexpr UartCalibration_t FindClosestFractional(float decimal)
{
  UartCalibration_t result{};

  for (const auto & fraction : kBaudTable)
  {
    if (fraction.fraction - kThreshold <= decimal &&
        decimal <= fraction.fraction + kThreshold)
    {
      result.fraction = fraction.byte_code;
      break;
    }
  }

  return result;
}

/// @param baud_rate - desired baud rate.
/// @param fraction_estimate - corresponds to the result of UartCalibration_t
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
  float decimal         = kBaudTable[0].fraction;
  size_t table_position = 0;
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
        if (table_position >= kBaudTable.size())
        {
          state = States::kDone;
        }
        decimal = kBaudTable[table_position++].fraction;
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
class UartBase : public sjsu::Uart
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

    /// IRQ number for this I2C port.
    sjsu::cortex::IRQn_Type irq_number;

    /// Reference to a uart transmitter pin
    sjsu::Pin & tx;

    /// Reference to a uart receiver pin
    sjsu::Pin & rx;

    /// Function code to set the transmit pin to uart transmitter
    uint8_t tx_function_id : 3;

    /// Function code to set the receive pin to uart receiver
    uint8_t rx_function_id : 3;
  };

  /// Interrupt enable bit fields
  struct InterruptEnable
  {
    /// RBR Interrupt Enable. Enables the Receive Data Available interrupt for
    /// UARTn: Reset 0 It also controls the Character Receive Time-out
    /// interrupt.
    /// - 0 Disable the RDA interrupts.
    /// - 1 Enable the RDA interrupts.
    static constexpr auto kReceiveInterrupt = bit::MaskFromRange(0);
  };

  /// Interrupt ID bit fields
  struct InterruptID
  {
    /// Interrupt identification. UnIER[3:1] identifies an interrupt
    /// corresponding to the UARTn Rx or TX FIFO. All other combinations of
    /// UnIER[3:1] not listed below are reserved (000,100,101,111).
    /// - 0x3 1 - Receive Line Status (RLS).
    /// - 0x2 2a - Receive Data Available (RDA).
    /// - 0x6 2b - Character Time-out Indicator (CTI).
    /// - 0x1 3 - THRE Interrupt
    static constexpr auto kID = bit::MaskFromRange(1, 3);
  };

  /// FIFO control bit fields
  struct FIFOControl
  {
    /// FIFO Enable: Reset 0
    /// - 0 UARTn FIFOs are disabled. Must not be used in the application.
    /// - 1 Active high enable for both UARTn Rx and TX FIFOs and UnFCR[7:1]
    /// access. This bit must be set for proper UART operation. Any transition
    /// on this bit will automatically clear the related UART FIFOs.
    static constexpr auto kFifoEnable = bit::MaskFromRange(0);
    /// RX FIFO Reset: Reset 0
    /// - 0 No impact on either of UARTn FIFOs.
    /// - 1 Writing a logic 1 to UnFCR[1] will clear all bytes in UARTn Rx FIFO,
    /// reset the pointer logic. This bit is self-clearing.
    static constexpr auto kRxFifoClear = bit::MaskFromRange(1);
    /// TX FIFO Reset: Reset 0
    /// - 0 No impact on either of UARTn FIFOs.
    /// - 1 Writing a logic 1 to UnFCR[2] will clear all bytes in UARTn TX FIFO,
    /// reset the pointer logic. This bit is self-clearing.
    static constexpr auto kTxFifoClear = bit::MaskFromRange(2);
    /// RX Trigger Level. These two bits determine how many receiver UARTn FIFO
    /// characters must be written before an interrupt or DMA request is
    /// activated: Reset 0
    /// - 0x0 Trigger level 0 (1 character or 0x01).
    /// - 0x1 Trigger level 1 (4 characters or 0x04).
    /// - 0x2 Trigger level 2 (8 characters or 0x08).
    /// - 0x3 Trigger level 3 (14 characters or 0x0E).
    static constexpr auto kRxTriggerLevel = bit::MaskFromRange(6, 7);
  };

  /// @param port - reference to the port specification object
  /// @param buffer - pointer to the array buffer to hold the received bytes
  UartBase(const Port_t & port, std::span<uint8_t> buffer)
      : port_(port), receive_buffer_(buffer.begin(), buffer.end())
  {
  }

  /// @tparam size - size of the array
  /// @param port - reference to the port specification object
  /// @param buffer - reference to the array buffer to hold the received bytes
  template <size_t size>
  UartBase(const Port_t & port, uint8_t (&buffer)[size])
      : port_(port), receive_buffer_(buffer, size)
  {
  }

  void ModuleInitialize() override
  {
    sjsu::SystemController::GetPlatformController().PowerUpPeripheral(
        port_.power_on_id);

    port_.registers->FCR =
        bit::Value(0).Set(FIFOControl::kFifoEnable).To<uint8_t>();

    ConfigureFormat();
    ConfigureBaudRate();

    port_.rx.settings.function = port_.rx_function_id;
    port_.tx.settings.function = port_.tx_function_id;
    port_.rx.settings.PullUp();
    port_.tx.settings.PullUp();

    port_.rx.Initialize();
    port_.tx.Initialize();

    SetupReceiveInterrupt();
    Flush();
    ResetUartQueue();
  }

  void ModulePowerDown() override
  {
    port_.registers->FCR = kPowerDown;
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
    int count = 0;

    for (auto & byte : data)
    {
      if (receive_buffer_.empty())
      {
        break;
      }

      byte = receive_buffer_.pop_front();
      count++;
    }

    return count;
  }

  bool HasData() override
  {
    return receive_buffer_.size();
  }

  void Flush() noexcept override
  {
    while (!receive_buffer_.empty())
    {
      receive_buffer_.pop_back();
    }
  }

  ~UartBase()
  {
    sjsu::InterruptController::GetPlatformController().Disable(
        port_.irq_number);
  }

 private:
  bool FifoContainsData()
  {
    return bit::Read(port_.registers->LSR, 0);
  }
  void Interrupt()
  {
    [[maybe_unused]] auto line_status_value = port_.registers->LSR;
    auto interrupt_type = bit::Extract(port_.registers->IIR, InterruptID::kID);
    if (interrupt_type == 0x2 || interrupt_type == 0x6)
    {
      while (FifoContainsData())
      {
        uint8_t byte{ port_.registers->RBR };
        if (!receive_buffer_.full())
        {
          receive_buffer_.push_back(byte);
        }
      }
    }
  }

  void ResetUartQueue()
  {
    // 0x3 = 14 bytes in fifo before triggering a receive interrupt.
    // 0x2 = 8
    // 0x1 = 4
    // 0x0 = 1
    port_.registers->FCR = bit::Value(0)
                               .Set(FIFOControl::kRxFifoClear)
                               .Set(FIFOControl::kTxFifoClear)
                               .To<uint8_t>();
  }

  void SetupReceiveInterrupt()
  {
    // Enable interrupt service routine.
    sjsu::InterruptController::GetPlatformController().Enable({
        .interrupt_request_number = port_.irq_number,
        .interrupt_handler        = [this]() { Interrupt(); },
    });

    // Enable uart interrupt signal
    bit::Register(&port_.registers->IER)
        .Set(InterruptEnable::kReceiveInterrupt)
        .Save();

    port_.registers->FCR =
        bit::Value(0)
            .Insert(std::uint8_t{ 0x3 }, FIFOControl::kRxTriggerLevel)
            .To<uint8_t>();
  }

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
    uint8_t fdr = calibration.fraction;

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
  nonstd::ring_span<std::uint8_t> receive_buffer_;
};

/// Uart Driver for the lpc40xx platform.
///
/// @tparam - defaults to 2048 bytes for the queue size. You can configure this
///           for a higher or lower number of bytes. Note: that the larger this
///           value, the larger this object's size is.
template <size_t queue_size = 2048>
class Uart : public sjsu::lpc40xx::UartBase
{
 public:
  using sjsu::Uart::Read;
  using sjsu::Uart::Write;

  /// @param port - reference to the port specification object
  explicit constexpr Uart(const sjsu::lpc40xx::UartBase::Port_t & port)
      : sjsu::lpc40xx::UartBase(port, queue_), queue_{}
  {
  }

 private:
  std::array<uint8_t, queue_size> queue_;
};

template <int port, size_t queue_size = 2048>
inline Uart<queue_size> & GetUart()
{
  if constexpr (port == 0)
  {
    static Pin & uart0_tx = sjsu::lpc40xx::GetPin<0, 2>();
    static Pin & uart0_rx = sjsu::lpc40xx::GetPin<0, 3>();

    /// Definition for uart port 0 for lpc40xx.
    static const UartBase::Port_t kUart0 = {
      // NOTE: required since LPC_UART0 is of type LPC_UART0_TypeDef in lpc17xx
      // and LPC_UART_TypeDef in lpc40xx causing a "useless cast" warning when
      // compiled for, some odd reason, for either one being compiled, which
      // would make more sense if it only warned us with lpc40xx.
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART0_BASE),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart0,
      .irq_number     = 5,
      .tx             = uart0_tx,
      .rx             = uart0_rx,
      .tx_function_id = 0b001,
      .rx_function_id = 0b001,
    };

    static Uart<queue_size> uart0(kUart0);
    return uart0;
  }
  else if constexpr (port == 2)
  {
    static Pin & uart2_tx = sjsu::lpc40xx::GetPin<2, 8>();
    static Pin & uart2_rx = sjsu::lpc40xx::GetPin<2, 9>();

    /// Definition for uart port 1 for lpc40xx.
    static const UartBase::Port_t kUart2 = {
      .registers      = LPC_UART2,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart2,
      .irq_number     = sjsu::lpc40xx::IRQn::UART2_IRQn,
      .tx             = uart2_tx,
      .rx             = uart2_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart<queue_size> uart2(kUart2);
    return uart2;
  }
  else if constexpr (port == 3)
  {
    static Pin & uart3_tx = sjsu::lpc40xx::GetPin<4, 28>();
    static Pin & uart3_rx = sjsu::lpc40xx::GetPin<4, 29>();

    /// Definition for uart port 2 for lpc40xx.
    static const UartBase::Port_t kUart3 = {
      .registers      = LPC_UART3,
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart3,
      .irq_number     = sjsu::lpc40xx::IRQn::UART3_IRQn,
      .tx             = uart3_tx,
      .rx             = uart3_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart<queue_size> uart3(kUart3);
    return uart3;
  }
  else if constexpr (port == 4)
  {
    static Pin & uart4_tx = sjsu::lpc40xx::GetPin<1, 29>();
    static Pin & uart4_rx = sjsu::lpc40xx::GetPin<2, 9>();

    /// Definition for uart port 3 for lpc40xx.
    static const UartBase::Port_t kUart4 = {
      .registers      = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART4),
      .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart4,
      .irq_number     = sjsu::lpc40xx::IRQn::UART4_IRQn,
      .tx             = uart4_tx,
      .rx             = uart4_rx,
      .tx_function_id = 0b101,
      .rx_function_id = 0b011,
    };

    static Uart<queue_size> uart4(kUart4);
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
