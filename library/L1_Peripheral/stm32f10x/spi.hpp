
#pragma once

#include <unordered_map>
#include "L1_Peripheral/spi.hpp"
#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/status.hpp"

/*TODO:

Add disable SPI to change data size
replace umap with lookuptable
refactor setDataSize if else to be a one liner (Done)
formatting(Done)
Document
Test readback from slave with breadboard
*/

namespace sjsu
{
namespace stm32f10x
{
/// Implementation of the SPI peripheral for the STM32F10x family of
/// microcontrollers.
class Spi final : public sjsu::Spi
{
 public:
  // SPI Control Register 1
  struct ControlRegister1  // NOLINT
  {
    /// Data Frame Format bitmask
    /// 0: 8-bit data frame format for transmission/reception
    /// 1: 16-bit data frame format for transmission/reception
    static constexpr bit::Mask kDataFrameFormat = bit::MaskFromRange(11);
    /// Setting this bit to 1 will enable spi master mode
    static constexpr bit::Mask kSpiEnable = bit::MaskFromRange(6);
    /// Bitmask for setting the Baud Rate for the SPI
    static constexpr bit::Mask kBaudRateControl = bit::MaskFromRange(3, 5);
    /// Setting this bit to 1 will enbable the peripheral for communication.
    static constexpr bit::Mask kMasterSelection = bit::MaskFromRange(2);
    // If bit is set to 1 SSP controller maintains the bus clock high betwen
    // frames
    static constexpr bit::Mask kClockPolarity = bit::MaskFromRange(1);
    /// If bit is set to 1 SSP controller captures serial data on the second
    /// clock transtion of the frame, that is, the transition back to the
    /// transition back to the inter-frame state of the clock line.
    static constexpr bit::Mask kClockPhase = bit::MaskFromRange(0);
    /// When the SSM bit is set, the NSS pin input is replaced with the 
    /// value from the SSI bit. This is used for in the software based NSS management,
    /// and will be the main mode of interating with the NSS pin in SJSU-Dev2
    static constexpr bit::Mask kSoftwareSlaveManagement = bit::MaskFromRange(9);
    /// The value of this bit is forced onto the NSS pin and the I/O value of the
    /// NSS pin is ignored.
    static constexpr bit::Mask kInternalSlaveSelect = bit::MaskFromRange(8);
  };

  /// SPI_I2S Configuration Register
  struct ConfigurationRegister  // NOLINT
  {
    // Data length to be transfered. This field controls the number of bits
    // transfered in each frame. Value 11 in not supported and should not be
    // used.
    static constexpr bit::Mask kDataLength = bit::MaskFromRange(1, 2);
  };
  // SPI Status Register
  struct StatusRegister  // NOLINT
  {
    /// This bit is 0 if the SSPn controller is idle, or 1 if it is currently
    /// sending/receiving a frame and/or the Tx buffer is not empty.
    static constexpr bit::Mask kBusyFlag = bit::MaskFromRange(7);
  };
   

   /// Bus_t holds all of the information for an SPI bus on the LPC40xx platform.
   struct Port_t
  {
    /// PeripheralID of the SPI peripheral to power on at initialization
    sjsu::stm32f10x::SystemController::ResourceID id;
    /// Reference to the M.Aster-O.UT-S.LAVE-I.N (output from microcontroller)
    /// spi pin
    const sjsu::Pin & mosi;
    /// Refernce to the M.ASTER-I.N-S.LAVE-O.UT (input to microcontroller) spi
    /// pin.
    const sjsu::Pin & miso;
    /// Refernce to serial clock spi pin.
    const sjsu::Pin & sck;
    /// Address to the SPI peripheral to use
    SPI_TypeDef * spi;
    /// Function code to set each pin to the appropriate SPI function.
    // Value of 1 will set pin to alternative function
    uint8_t pin_function;
  };

  struct Port  // NOLINT
  {
   private:
    // SPI1 pins
    inline static const auto kMosi1 = sjsu::stm32f10x::Pin('A', 7);
    inline static const auto kMiso1 = sjsu::stm32f10x::Pin('A', 6);
    inline static const auto kSck1  = sjsu::stm32f10x::Pin('A', 5);
    // SPI2 pins
    inline static const auto kMosi2 = sjsu::stm32f10x::Pin('B', 15);
    inline static const auto kMiso2 = sjsu::stm32f10x::Pin('B', 14);
    inline static const auto kSck2  = sjsu::stm32f10x::Pin('B', 13);

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

  // Currently this sets the clock and data size. Looking at the datasheet clock
  // can be set after the SPI is enabled, but the datasize shouldn't.
  sjsu::Returns<void> Initialize() const override
  {
    auto & system = SystemController::GetPlatformController();

    system.PowerUpPeripheral(port_.id);
    // Enable SPI pins
    port_.mosi.SetPinFunction(port_.pin_function);
    port_.miso.SetPinFunction(port_.pin_function);
    port_.sck.SetPinFunction(port_.pin_function);

    // set clock to be a default value of 1_MHz
    SetClock(1_MHz, false, false);
    // set data size to be 8 bits as default.
    SetDataSize(DataSize::kEight);
    // set software control of NSS pin
    port_.spi->CR1 =
        bit::Set(port_.spi->CR1, ControlRegister1::kSoftwareSlaveManagement);
    // set the SSI bit to emulate a high signal on the NSS pin.
    port_.spi->CR1 = 
        bit::Set(port_.spi->CR1, ControlRegister1::kInternalSlaveSelect);
    // set to master mode
    port_.spi->CR1 =
        bit::Set(port_.spi->CR1, ControlRegister1::kMasterSelection);
    // Enable SPI
    port_.spi->CR1 = bit::Set(port_.spi->CR1, ControlRegister1::kSpiEnable);
    //GetControlRegister1();
    return {};
  }

  /// An easy way to sets up an SPI peripheral as SPI master with default clock
  /// rate at 1Mhz.
  void SetSpiDefault() const
  {
    constexpr bool kPositiveClockOnIdle                   = false;
    constexpr bool kReadMisoOnRising                      = false;
    constexpr units::frequency::hertz_t kDefaultFrequency = 1_MHz;

    SetDataSize(DataSize::kEight);
    SetClock(kDefaultFrequency, kPositiveClockOnIdle, kReadMisoOnRising);
  }

  /// Checks if the SSP controller is idle.
  /// @returns true if the controller is sending or receiving a data frame and
  /// false if it is idle.
  bool IsBusBusy() const
  {
    return bit::Read(port_.spi->SR, StatusRegister::kBusyFlag);
  }

  /// Transfers a data frame to an external device using the SSP
  /// data register. This functions for both transmitting and
  /// receiving data. It is recommended this region be protected
  /// by a mutex.
  /// @param data - information to be placed in data register
  /// @return - received data from external device
  uint16_t Transfer(uint16_t data) const override
  {
    // Place data to be transfered into the Data Register
    port_.spi->DR = data;
    while (IsBusBusy())
    {
      continue;
    }
    return static_cast<uint16_t>(port_.spi->DR);
  }

  // Debuging only
  void CheckStatusFlags() const
  {
    int iBusyFlag                = bit::Read(port_.spi->SR, 7);
    int iOverRunFlag             = bit::Read(port_.spi->SR, 6);
    int iModeFaultFlag           = bit::Read(port_.spi->SR, 5);
    int iCrcErrorFlag            = bit::Read(port_.spi->SR, 4);
    int iUnderRunFlag            = bit::Read(port_.spi->SR, 3);
    int iTransmitBufferEmptyFlag = bit::Read(port_.spi->SR, 1);
    int iReceiveBufferNotEmpty   = bit::Read(port_.spi->SR, 0);

    sjsu::LogDebug("Busy Flag: %d", iBusyFlag);
    sjsu::LogDebug("OverRun Flag: %d", iOverRunFlag);
    sjsu::LogDebug("ModeFaultFlag: %d", iModeFaultFlag);
    sjsu::LogDebug("CRC Error Flag: %d", iCrcErrorFlag);
    sjsu::LogDebug("UnderRun Flag: %d", iUnderRunFlag);
    sjsu::LogDebug("Transmit Buffer Empty Flag: %d", iTransmitBufferEmptyFlag);
    sjsu::LogDebug("Receive Buffer Not Empty: %d", iReceiveBufferNotEmpty);
  }
  // Debuging only
  void GetControlRegister1() const
  {
    int iBidiMode               = bit::Read(port_.spi->CR1, 15);
    int iBidioe                 = bit::Read(port_.spi->CR1, 14);
    int iCrcEnable              = bit::Read(port_.spi->CR1, 13);
    int iCrcNext                = bit::Read(port_.spi->CR1, 12);
    int iDataFrameFormat        = bit::Read(port_.spi->CR1, 11);
    int iReceiveOnly            = bit::Read(port_.spi->CR1, 10);
    int iSoftwareSlaveManagment = bit::Read(port_.spi->CR1, 9);
    int iInternalSlaveSelect    = bit::Read(port_.spi->CR1, 8);
    int iFrameFormat            = bit::Read(port_.spi->CR1, 7);
    int iSpiEnable              = bit::Read(port_.spi->CR1, 6);
    int iBaudeRateControl2      = bit::Read(port_.spi->CR1, 5);
    int iBaudeRateControl1      = bit::Read(port_.spi->CR1, 4);
    int iBaudeRateControl0      = bit::Read(port_.spi->CR1, 3);
    int iMasterSelection        = bit::Read(port_.spi->CR1, 2);
    int iClockPolarity          = bit::Read(port_.spi->CR1, 1);
    int iClockPhase             = bit::Read(port_.spi->CR1, 0);

    sjsu::LogDebug("BIDIMODE: %d", iBidiMode);
    sjsu::LogDebug("BIDI output enabled: %d", iBidioe);
    sjsu::LogDebug("Hardware CRC calculation enabled: %d", iCrcEnable);
    sjsu::LogDebug("CRC transfer next: %d", iCrcNext);
    sjsu::LogDebug("Data frame format: %d", iDataFrameFormat);
    sjsu::LogDebug("Receive only: %d", iReceiveOnly);
    sjsu::LogDebug("Software slave management: %d", iSoftwareSlaveManagment);
    sjsu::LogDebug("Internal slave select: %d", iInternalSlaveSelect);
    sjsu::LogDebug("Frame format: %d", iFrameFormat);
    sjsu::LogDebug("SPI enabled: %d", iSpiEnable);
    sjsu::LogDebug("Baude rate control: %d%d%d", iBaudeRateControl2,
                   iBaudeRateControl1, iBaudeRateControl0);
    sjsu::LogDebug("Master selected: %d", iMasterSelection);
    sjsu::LogDebug("Clock polarity: %d", iClockPolarity);
    sjsu::LogDebug("Clock phase: %d", iClockPhase);
  }

  /// Sets the various modes for the Peripheral
  /// @param size - number of bits per frame
  void SetDataSize(DataSize size) const override
  {
    port_.spi->CR1 = bit::Insert(port_.spi->CR1, (size != DataSize::kEight),
                                ControlRegister1::kDataFrameFormat);
  }

  sjsu::Returns<void> SetClock(units::frequency::hertz_t frequency,
                               bool positive_clock_on_idle = false,
                               bool read_miso_on_rising = false) const override
  {
    port_.spi->CR1 = bit::Insert(port_.spi->CR1, read_miso_on_rising,
                                 ControlRegister1::kClockPolarity);

    port_.spi->CR1 = bit::Insert(port_.spi->CR1, positive_clock_on_idle,
                                 ControlRegister1::kClockPhase);

    auto & system         = sjsu::SystemController::GetPlatformController();
    auto system_frequency = system.GetClockRate(port_.id);
    uint32_t integer_peripheral_frequency =
        units::unit_cast<uint32_t>(system_frequency);
    uint32_t integer_target_frequency = units::unit_cast<uint32_t>(frequency);

    // Replica of bit_ceil method from C++20
    uint32_t divider =
        (integer_peripheral_frequency / integer_target_frequency);
    divider |= divider >> 1;
    divider |= divider >> 2;
    divider |= divider >> 4;
    divider = divider + 1;

    // Find the best divider to get as close to the target frequency as
    // possible.
    if (std::abs(static_cast<int>(integer_target_frequency -
                                  (integer_peripheral_frequency / divider))) <
        std::abs(
            static_cast<int>(integer_target_frequency -
                             (integer_peripheral_frequency / (divider >> 1)))))
    {
      port_.spi->CR1 = bit::Insert(port_.spi->CR1, static_cast<uint16_t>(std::log2f(divider) - 1),
                                   ControlRegister1::kBaudRateControl);
    }
    else
    {
      port_.spi->CR1 = bit::Insert(port_.spi->CR1, static_cast<uint16_t>(std::log2f(divider >> 1) - 1),
                                   ControlRegister1::kBaudRateControl);
    }
    return {};
  }

 private:
  const Port_t & port_;
};
}  // namespace stm32f10x

}  // namespace sjsu