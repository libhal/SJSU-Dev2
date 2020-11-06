#pragma once

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/spi.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// Implementation of the SPI peripheral for the STM32F10x family of
/// microcontrollers.
class Spi final : public sjsu::Spi
{
 public:
  /// SPI Control Register 1
  struct Control1  // NOLINT
  {
    /// Data Frame Format bitmask
    /// 0: 8-bit data frame format for transmission/reception
    /// 1: 16-bit data frame format for transmission/reception
    static constexpr bit::Mask kDataFrameFormat = bit::MaskFromRange(11);

    /// Setting this bit to 1 will enable spi master mode
    static constexpr bit::Mask kSpiEnable = bit::MaskFromRange(6);

    /// Bitmask for setting the Baud Rate for the SPI
    static constexpr bit::Mask kBaudRateControl = bit::MaskFromRange(3, 5);

    /// Setting this bit to 1 will enable the peripheral for communication.
    static constexpr bit::Mask kMasterSelection = bit::MaskFromRange(2);

    // If bit is set to 1 SSP controller maintains the bus clock high betwen
    // frames
    static constexpr bit::Mask kClockPolarity = bit::MaskFromRange(1);

    /// If bit is set to 1 SSP controller captures serial data on the second
    /// clock transition of the frame, that is, the transition back to the
    /// transition back to the inter-frame state of the clock line.
    static constexpr bit::Mask kClockPhase = bit::MaskFromRange(0);

    /// When the SSM bit is set, the NSS pin input is replaced with the
    /// value from the SSI bit. This is used for in the software based NSS
    /// management, and will be the main mode of interating with the NSS pin in
    /// SJSU-Dev2
    static constexpr bit::Mask kSoftwareSlaveManagement = bit::MaskFromRange(9);

    /// The value of this bit is forced onto the NSS pin and the I/O value of
    /// the NSS pin is ignored.
    static constexpr bit::Mask kInternalSlaveSelect = bit::MaskFromRange(8);
  };

  /// SPI_I2S Configuration Register
  struct ConfigurationRegister  // NOLINT
  {
    // Data length to be transferred. This field controls the number of bits
    // transferred in each frame. Value 11 in not supported and should not be
    // used.
    static constexpr bit::Mask kDataLength = bit::MaskFromRange(1, 2);
  };

  /// SPI Status Register
  struct StatusRegister  // NOLINT
  {
    /// This bit is 0 if the SSPn controller is idle, or 1 if it is currently
    /// sending/receiving a frame and/or the Tx buffer is not empty.
    static constexpr bit::Mask kBusyFlag = bit::MaskFromRange(7);

    /// This bit is 0 if the SSPn controller is idle, or 1 if it is currently
    /// sending/receiving a frame and/or the Tx buffer is not empty.
    static constexpr bit::Mask kTransmitBufferEmpty = bit::MaskFromRange(1);

    /// This bit is 0 if the SSPn controller is idle, or 1 if it is currently
    /// sending/receiving a frame and/or the Tx buffer is not empty.
    static constexpr bit::Mask kReceiveBufferNotEmpty = bit::MaskFromRange(0);
  };

  /// Bus_t holds all of the information for an SPI bus on the LPC40xx platform.
  struct Port_t
  {
    /// PeripheralID of the SPI peripheral to power on at initialization
    sjsu::stm32f10x::SystemController::ResourceID id;

    /// Reference to the M.Aster-O.UT-S.LAVE-I.N (output from microcontroller)
    /// spi pin
    sjsu::Pin & mosi;

    /// Refernce to the M.ASTER-I.N-S.LAVE-O.UT (input to microcontroller) spi
    /// pin.
    sjsu::Pin & miso;

    /// Refernce to serial clock spi pin.
    sjsu::Pin & sck;

    /// Address to the SPI peripheral to use
    SPI_TypeDef * spi;

    /// Function code to set each pin to the appropriate SPI function.
    // Value of 1 will set pin to alternative function
    uint8_t pin_function;
  };

  /// SPI Port namespace holder
  struct Port  // NOLINT
  {
   private:
    // SPI1 pins
    inline static auto kMosi1 = sjsu::stm32f10x::Pin('A', 7);
    inline static auto kMiso1 = sjsu::stm32f10x::Pin('A', 6);
    inline static auto kSck1  = sjsu::stm32f10x::Pin('A', 5);

    // SPI2 pins
    inline static auto kMosi2 = sjsu::stm32f10x::Pin('B', 15);
    inline static auto kMiso2 = sjsu::stm32f10x::Pin('B', 14);
    inline static auto kSck2  = sjsu::stm32f10x::Pin('B', 13);

   public:
    /// Definition for SPI bus 1 for STM32F10x
    inline static const Port_t kSpi1 = {
      .id           = SystemController::Peripherals::kSpi1,
      .mosi         = kMosi1,
      .miso         = kMiso1,
      .sck          = kSck1,
      .spi          = SPI1,
      .pin_function = 0b01,
    };
    /// Definition for SPI bus 2 for STM32F10x
    inline static const Port_t kSpi2 = {
      .id           = SystemController::Peripherals::kSpi2,
      .mosi         = kMosi2,
      .miso         = kMiso2,
      .sck          = kSck2,
      .spi          = SPI2,
      .pin_function = 0b01,
    };
  };

  /// Constructor for STM32F10x Spi peripheral
  ///
  /// @param port - pass a reference to a constant stm32f10x::Spi::Port_t
  ///        definition.
  explicit constexpr Spi(const Port_t & port) : port_(port) {}

  void ModuleInitialize() override
  {
    // Power on peripheral
    SystemController::GetPlatformController().PowerUpPeripheral(port_.id);

    // Enable SPI pins
    port_.mosi.ConfigureFunction(port_.pin_function);
    port_.miso.ConfigureFunction(port_.pin_function);
    port_.sck.ConfigureFunction(port_.pin_function);

    // Enable Master mode SPI
    // Enable software slave management (gpio must be used for slave select)
    // Disable (by setting to 1) the internal slave select NSS pin
    bit::Register(&port_.spi->CR1)
        .Set(Control1::kSoftwareSlaveManagement)
        .Set(Control1::kInternalSlaveSelect)
        .Set(Control1::kMasterSelection)
        .Save();
  }

  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      bit::Register(&port_.spi->CR1).Set(Control1::kSpiEnable).Save();
    }
    else
    {
      while (!TransmitBufferEmpty())
      {
        continue;
      }
      while (BusBusy())
      {
        continue;
      }
      bit::Register(&port_.spi->CR1).Clear(Control1::kSpiEnable).Save();
    }
  }

  bool BusBusy()
  {
    return bit::Read(port_.spi->SR, StatusRegister::kBusyFlag);
  }

  bool TransmitBufferEmpty()
  {
    return bit::Read(port_.spi->SR, StatusRegister::kTransmitBufferEmpty);
  }

  bool ReceiveBufferNotEmpty()
  {
    return bit::Read(port_.spi->SR, StatusRegister::kReceiveBufferNotEmpty);
  }

  template <typename T>
  void UniversalTransfer(std::span<T> buffer)
  {
    // Follows the instructions provided by RM0008.pdf page 710 to get the
    // maximum performance.

    // Step 1. Load the first byte into the data register
    port_.spi->DR = buffer[0];

    for (size_t i = 1; i < buffer.size(); i++)
    {
      // Step 2.1 Wait until the transmit buffer is empty, then...
      while (!TransmitBufferEmpty())
      {
        continue;
      }

      // Step 2.2 Load it with the next byte.
      port_.spi->DR = buffer[i];

      // Step 2.3 Wait until the receive buffer has data.
      while (!ReceiveBufferNotEmpty())
      {
        continue;
      }

      // Step 2.4 Read byte from DR register and place it into the previous byte
      // location. This is important as the bytes written are head of the bytes
      // received by 1.
      buffer[i - 1] = static_cast<T>(port_.spi->DR);
    }

    // Step 3.1 Wait for the last byte to come in on the receive buffer.
    while (!ReceiveBufferNotEmpty())
    {
      continue;
    }

    // Step 3.2 Read the final byte from the receive buffer and place it in the
    // end of the span.
    buffer.end()[-1] = static_cast<T>(port_.spi->DR);
  }

  void Transfer(std::span<uint8_t> buffer) override
  {
    UniversalTransfer(buffer);
  }

  void Transfer(std::span<uint16_t> buffer) override
  {
    UniversalTransfer(buffer);
  }

  void ConfigureFrameSize(FrameSize size) override
  {
    if (size == FrameSize::kEightBits)
    {
      bit::Register(&port_.spi->CR1).Clear(Control1::kDataFrameFormat).Save();
    }
    else if (size == FrameSize::kSixteenBits)
    {
      bit::Register(&port_.spi->CR1).Set(Control1::kDataFrameFormat).Save();
    }
    else
    {
      throw Exception(std::errc::invalid_argument,
                      "STM32F10x SPI Peripheral only supports 8-bit and 16-bit "
                      "frame sizes!");
    }
  }

  /// The STM32F10x family of chips only supports base 2 dividers for SPI from 2
  /// to 256, meaning that the output clock frequency will be either
  /// peripheral_frequency /2, /4, /8, /16, /32, /64, /128, or /256.
  void ConfigureFrequency(units::frequency::hertz_t frequency) override
  {
    auto & system = sjsu::SystemController::GetPlatformController();
    const auto kPeripheralFrequency = system.GetClockRate(port_.id);

    float divider        = (kPeripheralFrequency / frequency).to<float>();
    float base_2_divider = std::clamp(std::log2f(divider) - 1.0f, 0.0f, 7.0f);

    bit::Register(&port_.spi->CR1)
        .Insert(static_cast<uint16_t>(base_2_divider),
                Control1::kBaudRateControl)
        .Save();
  }

  void ConfigureClockMode(Polarity polarity = Polarity::kIdleLow,
                          Phase phase       = Phase::kSampleLeading) override
  {
    bit::Register(&port_.spi->CR1)
        .Insert(Value(polarity), Control1::kClockPolarity)
        .Insert(Value(phase), Control1::kClockPhase)
        .Save();
  }

 private:
  const Port_t & port_;
};
}  // namespace stm32f10x
}  // namespace sjsu
