#pragma once

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/uart.hpp"

namespace sjsu::stm32f10x
{
/// Uart Driver for the stm32f10x platform.
/// Usart 1 will occupy DMA1 channel 5
/// Usart 2 will occupy DMA1 channel 6
/// Usart 3 will occupy DMA1 channel 3
class UartBase : public sjsu::Uart
{
 public:
  using sjsu::Uart::Read;
  using sjsu::Uart::Write;

  /// Uart port definition object.
  /// Defines all of the elements needed to enable uart.
  struct Port_t
  {
    /// Reference to the UART TX pin.
    /// This driver will not handle remapping (muxing) the pin. So for example,
    /// For USART3 the default TX is PB10 and the default RX is PB11. But PC10
    /// and PC11 can be remapped to USART3. In order to use those other pins,
    /// the AFIO->MAPR register must be configured for this before using this
    /// library, otherwise, the default mapping will be used.
    sjsu::Pin & tx;

    /// Reference to the UART RX pin
    /// This driver will not handle remapping (muxing) the pin. So for example,
    /// For USART3 the default TX is PB10 and the default RX is PB11. But PC10
    /// and PC11 can be remapped to USART3. In order to use those other pins,
    /// the AFIO->MAPR register must be configured for this before using this
    /// library, otherwise, the default mapping will be used.
    sjsu::Pin & rx;

    /// Address to the UART peripheral to use
    USART_TypeDef * uart;

    /// The ID of the UART peripheral to be used. This is used to enable the
    /// peripheral clock of th UART peripheral.
    SystemController::ResourceID id;

    /// Address of the DMA channel for this UART. The STM32F10x UART does not
    /// have a FIFO or buffer of any sort, thus DMA is required for reasonable
    /// usage.
    DMA_Channel_TypeDef * dma;
  };

  /// Namespace for the predefined UART configuration objects
  struct Port  // NOLINT
  {
   private:
    inline static auto tx1 = Pin('A', 9);
    inline static auto rx1 = Pin('A', 10);

    inline static auto rx2 = Pin('A', 3);
    inline static auto tx2 = Pin('A', 2);

    inline static auto tx3 = Pin('B', 10);
    inline static auto rx3 = Pin('B', 11);

   public:
    /// Predefined port for UART1
    const inline static Port_t kUart1 = {
      .tx   = tx1,
      .rx   = rx1,
      .uart = USART1,
      .id   = SystemController::Peripherals::kUsart1,
      .dma  = DMA1_Channel5,
    };

    /// Predefined port for UART2
    const inline static Port_t kUart2 = {
      .tx   = tx2,
      .rx   = rx2,
      .uart = USART2,
      .id   = SystemController::Peripherals::kUsart2,
      .dma  = DMA1_Channel6,
    };

    /// Predefined port for UART3
    const inline static Port_t kUart3 = {
      .tx   = tx3,
      .rx   = rx3,
      .uart = USART3,
      .id   = SystemController::Peripherals::kUsart3,
      .dma  = DMA1_Channel3,
    };
  };

  /// Namespace for the status registers (SR) bit masks
  struct StatusReg  // NOLINT
  {
    /// Indicates if the transmit data register is empty and can be loaded with
    /// another byte.
    static constexpr auto kTransitEmpty = bit::MaskFromRange(7);
  };

  /// Namespace for the control registers (CR1, CR3) bit masks and predefined
  /// settings constants.
  struct ControlReg  // NOLINT
  {
    /// When this bit is cleared the USART prescalers and outputs are stopped
    /// and the end of the current byte transfer in order to reduce power
    /// consumption. (CR1)
    static constexpr auto kUsartEnable = bit::MaskFromRange(13);

    /// Enables DMA receiver (CR3)
    static constexpr auto kDmaReceiverEnable = bit::MaskFromRange(6);

    /// This bit enables the transmitter. (CR1)
    static constexpr auto kTransmitterEnable = bit::MaskFromRange(3);

    /// This bit enables the receiver. (CR1)
    static constexpr auto kReceiveEnable = bit::MaskFromRange(2);

    /// Enable USART + Enable Receive + Enable Transmitter
    static constexpr uint16_t kControlSettings1 =
        bit::Value<uint16_t>()
            .Set(ControlReg::kUsartEnable)
            .Set(ControlReg::kReceiveEnable)
            .Set(ControlReg::kTransmitterEnable);

    /// Make sure that DMA is enabled for receive only
    static constexpr uint16_t kControlSettings3 =
        bit::Value<uint16_t>().Set(ControlReg::kDmaReceiverEnable);
  };

  /// Namespace for the baud rate (BRR) registers bit masks
  struct BaudRateReg  // NOLINT
  {
    /// Mantissa of USART DIV
    static constexpr auto kMantissa = bit::MaskFromRange(4, 15);

    /// Fraction of USART DIV
    static constexpr auto kFraction = bit::MaskFromRange(0, 3);
  };

  /// Namespace for the control registers (DMA->CCR) bit masks and predefined
  /// settings constants.
  struct DmaReg  // NOLINT
  {
    /// Declare this channel for Memory to memory mode
    static constexpr auto kMemoryToMemory = bit::MaskFromRange(14);

    /// Configure the channel priority for this channel.
    /// 0b00: Low
    /// 0b01: Medium
    /// 0b10: High
    /// 0b11: Very high
    static constexpr auto kChannelPriority = bit::MaskFromRange(12, 13);

    /// The size of each element of the memory.
    /// 0b00: 8-bits
    /// 0b01: 16-bits
    /// 0b10: 32-bits
    /// 0b11: Reserved
    static constexpr auto kMemorySize = bit::MaskFromRange(10, 11);

    /// The peripheral register size.
    /// 0b00: 8-bits
    /// 0b01: 16-bits
    /// 0b10: 32-bits
    /// 0b11: Reserved
    static constexpr auto kPeripheralSize = bit::MaskFromRange(8, 9);

    /// Activate memory increment mode, which will increment the memory address
    /// with each transfer
    static constexpr auto kMemoryIncrementEnable = bit::MaskFromRange(7);

    /// Activate memory increment mode, which will increment the peripheral
    /// address with each transfer
    static constexpr auto kPeripheralIncrementEnable = bit::MaskFromRange(6);

    /// DMA will continuous load bytes into the buffer supplied in a circular
    /// buffer manner.
    static constexpr auto kCircularMode = bit::MaskFromRange(5);

    /// Data transfer direction
    /// 0: Read from peripheral
    /// 1: Read from memory
    static constexpr auto kDataTransferDirection = bit::MaskFromRange(4);

    /// Enable interrupt on transfer error
    static constexpr auto kTransferErrorInterruptEnable = bit::MaskFromRange(3);

    /// Enable interrupt on half of data transferred
    static constexpr auto kHalfTransferInterruptEnable = bit::MaskFromRange(2);

    /// Enable interrupt on complete transfer
    static constexpr auto kTransferCompleteInterruptEnable =
        bit::MaskFromRange(1);

    /// Enable this DMA channel
    static constexpr auto kEnable = bit::MaskFromRange(0);

    /// Setup the DMA channel with all of the options specific to handling UART
    static constexpr uint32_t kDmaSettings =
        bit::Value{}
            .Clear(kTransferCompleteInterruptEnable)
            .Clear(kHalfTransferInterruptEnable)
            .Clear(kTransferErrorInterruptEnable)
            .Clear(kDataTransferDirection)  // Read from peripheral
            .Set(kCircularMode)
            .Clear(kPeripheralIncrementEnable)
            .Set(kMemoryIncrementEnable)
            .Insert(0b00, kPeripheralSize)   // size = 8 bits
            .Insert(0b00, kMemorySize)       // size = 8 bits
            .Insert(0b10, kChannelPriority)  // Low Medium [High] Very_High
            .Clear(kMemoryToMemory)
            .Set(kEnable);
  };

  /// @tparam size - size of the array
  /// @param port - reference to the port configuration object
  /// @param buffer - reference to the array buffer to hold the received bytes
  template <size_t size>
  constexpr UartBase(const Port_t & port, std::array<uint8_t, size> & buffer)
      : port_(port),
        read_pointer_(0),
        queue_(buffer.data()),
        queue_size_(buffer.size())
  {
  }

  /// @param port - reference to the port configuration object
  /// @param buffer - pointer to the array buffer to hold the received bytes
  /// @param size - size of the buffer in bytes
  constexpr UartBase(const Port_t & port, uint8_t * buffer, size_t size)
      : port_(port), read_pointer_(0), queue_(buffer), queue_size_(size)
  {
  }

  /// @tparam size - size of the array
  /// @param port - reference to the port configuration object
  /// @param buffer - reference to the array buffer to hold the received bytes
  template <size_t size>
  constexpr UartBase(const Port_t & port, uint8_t (&buffer)[size])
      : port_(port), read_pointer_(0), queue_(buffer), queue_size_(size)
  {
  }

  /// Disables DMA channel, USART DMA receiver mode, and the UART peripheral
  ~UartBase()
  {
    // It is important to disable the DMA control of the UART peripheral after
    // destruction, as the queue_ member variable's address location may no
    // longer be a valid and thus should not be accessed.
    // Without doing this, the enabled DMA will continue to use up port cycles.

    // Disable DMA channel for this UART
    bit::Register(&port_.dma->CCR).Clear(DmaReg::kEnable).Save();

    // Disable DMA flag Receive flag in UART control register
    bit::Register(&port_.uart->CR3)
        .Clear(ControlReg::kDmaReceiverEnable)
        .Save();

    // Disable UART peripheral
    bit::Register(&port_.uart->CR1).Clear(ControlReg::kUsartEnable).Save();
  }

  void ModuleInitialize() override
  {
    auto & system = SystemController::GetPlatformController();

    if (queue_size_ > 65'535)
    {
      throw Exception(
          std::errc::invalid_argument,
          "UART Receive queue size must not exceed the DMA transfer limit of "
          "65,535.");
    }

    // Supply clock to UART
    system.PowerUpPeripheral(port_.id);
    system.PowerUpPeripheral(SystemController::Peripherals::kDma1);
  }

  void ModuleEnable(bool = true) override
  {
    // Setup TX as output with alternative control (TXD)
    port_.tx.ConfigureFunction(1);
    // Set pin as input with internal PULL UP (RXD)
    port_.rx.ConfigurePullUp();

    // Setup RX DMA channel
    const auto kDataAddress  = reinterpret_cast<intptr_t>(&port_.uart->DR);
    const auto kQueueAddress = reinterpret_cast<intptr_t>(queue_);

    port_.dma->CNDTR = queue_size_;
    port_.dma->CPAR  = static_cast<uint32_t>(kDataAddress);
    port_.dma->CMAR  = static_cast<uint32_t>(kQueueAddress);

    // Setup DMA Channel Settings
    port_.dma->CCR = DmaReg::kDmaSettings;

    // Setup UART Control Settings 1
    port_.uart->CR1 = ControlReg::kControlSettings1;

    // NOTE: We leave control settings 2 alone as it is for features beyond
    //       basic UART such as USART clock, USART port network (LIN), and other
    //       things.

    // Setup UART Control Settings 3
    port_.uart->CR3 = ControlReg::kControlSettings3;
  }

  void ConfigureFormat(FrameSize = FrameSize::kEightBits,
                       StopBits  = StopBits::kSingle,
                       Parity    = Parity::kNone) override
  {
    sjsu::LogWarning("Not supported currently");
  }

  void ConfigureBaudRate(uint32_t baud_rate) override
  {
    auto & system  = SystemController::GetPlatformController();
    auto frequency = system.GetClockRate(port_.id);

    float float_baud_rate = static_cast<float>(baud_rate);
    float usart_divider =
        static_cast<float>(frequency) / (16.0f * float_baud_rate);

    // Truncate off the decimal values
    uint16_t mantissa = static_cast<uint16_t>(usart_divider);

    // Subtract the whole number to leave just the decimal
    float fraction = static_cast<float>(usart_divider - mantissa);

    uint16_t fractional_int = static_cast<uint16_t>(std::roundf(fraction * 16));

    if (fractional_int >= 16)
    {
      mantissa       = static_cast<uint16_t>(mantissa + 1U);
      fractional_int = 0;
    }

    port_.uart->BRR = bit::Value<uint16_t>()
                          .Insert(mantissa, BaudRateReg::kMantissa)
                          .Insert(fractional_int, BaudRateReg::kFraction);
  }

  void Write(std::span<const uint8_t> data) override
  {
    for (const auto & byte : data)
    {
      while (!bit::Read(port_.uart->SR, StatusReg::kTransitEmpty))
      {
        continue;
      }

      // Load the next byte into the data register
      port_.uart->DR = byte;
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
      byte          = queue_[read_pointer_++];
      read_pointer_ = read_pointer_ % queue_size_;
      index++;
    }

    return index;
  }

  bool HasData() override
  {
    // When write and read position are equal, the receive queue is empty.
    // WARNING: if DMA overruns the queue, the information will be lost.
    return !(DmaWritePosition() == read_pointer_);
  }

  void Flush() override
  {
    read_pointer_ = DmaWritePosition();
  }

 private:
  size_t DmaWritePosition() const
  {
    size_t write_position = queue_size_ - port_.dma->CNDTR;
    return write_position % queue_size_;
  }

  const Port_t & port_;
  /// This is made mutable
  mutable size_t read_pointer_ = 0;
  uint8_t * queue_;
  size_t queue_size_;
};

/// Uart Driver for the stm32f10x platform.
///
/// @tparam - defaults to 32 bytes for the queue size. You can configure this
///           for a higher or lower number of bytes. Note: that the larger this
///           value, the larger this object's size is.
template <const size_t kQueueSize = 32>
class Uart : public sjsu::stm32f10x::UartBase
{
 public:
  using sjsu::Uart::Read;
  using sjsu::Uart::Write;

  explicit constexpr Uart(const sjsu::stm32f10x::UartBase::Port_t & port)
      : sjsu::stm32f10x::UartBase(port, queue_), queue_{}
  {
  }

 private:
  std::array<uint8_t, kQueueSize> queue_;
};
}  // namespace sjsu::stm32f10x
