#pragma once

#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/adc.hpp"
#include "L1_Peripheral/stm32f10x/dma.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// ADC driver for implementation for the stm32f10x microcontrollers.
/// Activating a single channel will activate and enable sampling for all
/// channel.
///
/// DMA channels used are:
///    - DMA 1 Channel 1
///    - DMA 2 Channel 5
///
/// Only supports ADC1 and ADC2. ADC3 not supported.
class Adc final : public sjsu::Adc
{
 public:
  /// Number of active bits of the ADC. The ADC is a 12-bit ADC meaning that the
  /// largest value it can have is 2^12 = 4096
  static constexpr uint8_t kActiveBits = 12;

  /// Namespace containing the bitmask to manipulate the control registers1
  struct Control1  // NOLINT
  {
    /// Turn on scanning mode which allows the ADC to scan through each channel
    /// and store the converted value in the data register.
    static constexpr auto kScanMode = bit::MaskFromRange(8);

    /// Value of the control register.
    static constexpr uint32_t kDefault = bit::Value().Set(kScanMode);
  };

  /// Namespace containing the bitmask to manipulate the control registers2
  struct Control2  // NOLINT
  {
    /// Start Conversion
    static constexpr auto kSoftwareStart = bit::MaskFromRange(22);

    /// Enable external trigger for conversion
    static constexpr auto kExternalTriggerEnable = bit::MaskFromRange(20);

    /// External Trigger Select
    ///
    ///    000: Timer 1 CC1 event
    ///    001: Timer 1 CC2 event
    ///    010: Timer 1 CC3 event
    ///    011: Timer 2 CC2 event
    ///    100: Timer 3 TRGO event
    ///    101: Timer 4 CC4 event
    ///    110: EXTI line 11/TIM8_TRGO event
    ///    111: SWSTART
    static constexpr auto kExternalTriggerSelect = bit::MaskFromRange(17, 19);

    /// Turn on DMA access mode
    static constexpr auto kDmaEnable = bit::MaskFromRange(8);

    /// Start ADC Calibration process
    static constexpr auto kCalibration = bit::MaskFromRange(2);

    /// Turns on continuous conversion which, when coupled with scan mode and
    /// DMA will result in all of the channels being continuously sampled and
    /// stored in RAM.
    static constexpr auto kContinuous = bit::MaskFromRange(1);

    /// Global ADC peripheral On/Off (enable/disable)
    /// To enable or disable this, this bit must be the only bit that changes
    /// when writing to the register, otherwise it will not trigger a change in
    /// state.
    static constexpr auto kAdcOn = bit::MaskFromRange(0);

    /// Value of the control register
    static constexpr uint32_t kDefault =
        bit::Value()
            .Set(kDmaEnable)
            .Set(kContinuous)
            .Set(kExternalTriggerEnable)
            .Insert(0b111, kExternalTriggerSelect);
  };

  /// Namespace containing the bitmasks to set the ADC sequence
  struct RegularSequence  // NOLINT
  {
    /// Which ADC channel should be sampled first
    static constexpr auto kSequence1 = bit::MaskFromRange(0, 4);

    /// Which ADC channel should be sampled second
    static constexpr auto kSequence2 = bit::MaskFromRange(5, 9);

    /// Which ADC channel should be sampled third
    static constexpr auto kSequence3 = bit::MaskFromRange(10, 14);

    /// Which ADC channel should be sampled fourth
    static constexpr auto kSequence4 = bit::MaskFromRange(15, 19);

    /// Which ADC channel should be sampled fifth
    static constexpr auto kSequence5 = bit::MaskFromRange(20, 24);

    /// Which ADC channel should be sampled sixth
    static constexpr auto kSequence6 = bit::MaskFromRange(25, 29);

    /// In sequence register 1, the length indicates how many regular sequence
    /// registers to be readl
    static constexpr auto kLength = bit::MaskFromRange(20, 23);

    /// Value of the sequence register 1
    static constexpr uint32_t kRegisterSequence1 = bit::Value()
                                                       .Insert(12, kSequence1)
                                                       .Insert(13, kSequence2)
                                                       .Insert(14, kSequence3)
                                                       .Insert(15, kSequence4)
                                                       .Insert(15, kLength);

    /// Value of the sequence register 2
    static constexpr uint32_t kRegisterSequence2 = bit::Value()
                                                       .Insert(6, kSequence1)
                                                       .Insert(7, kSequence2)
                                                       .Insert(8, kSequence3)
                                                       .Insert(9, kSequence4)
                                                       .Insert(10, kSequence5)
                                                       .Insert(11, kSequence6);

    /// Value of the sequence register 3
    static constexpr uint32_t kRegisterSequence3 = bit::Value()
                                                       .Insert(0, kSequence1)
                                                       .Insert(1, kSequence2)
                                                       .Insert(2, kSequence3)
                                                       .Insert(3, kSequence4)
                                                       .Insert(4, kSequence5)
                                                       .Insert(5, kSequence6);
  };

  static constexpr uint32_t kDmaSettings =
      bit::Value{}
          .Clear(Dma::Reg::kTransferCompleteInterruptEnable)
          .Clear(Dma::Reg::kHalfTransferInterruptEnable)
          .Clear(Dma::Reg::kTransferErrorInterruptEnable)
          .Clear(Dma::Reg::kDataTransferDirection)  // Read from peripheral
          .Set(Dma::Reg::kCircularMode)
          .Clear(Dma::Reg::kPeripheralIncrementEnable)
          .Set(Dma::Reg::kMemoryIncrementEnable)
          .Insert(0b10, Dma::Reg::kPeripheralSize)  // size = 32 bits
          .Insert(0b10, Dma::Reg::kMemorySize)      // size = 32 bits
          .Insert(0b10,
                  Dma::Reg::kChannelPriority)  // Low Medium [High] Very_High
          .Clear(Dma::Reg::kMemoryToMemory)
          .Set(Dma::Reg::kEnable);

  /// Structure that defines a channel's pin, pin's function code and channel
  /// number.
  ///
  /// Usage:
  ///
  /// ```
  /// sjsu::stm32f10x::Pin pin(/* adc port number */, /* adc pin number */);
  /// const sjsu::stm32f10x::Adc::Channel_t kCustomChannelX = {
  ///   .pin      = pin,
  ///   .index    = /* insert correct channel here */,
  ///   .pin_function = 0b101,
  /// };
  /// sjsu::stm32f10x::Adc channelX_adc(kCustomChannelX);
  /// ```
  struct Channel_t
  {
    /// Reference to the pin associated with the adc channel.
    sjsu::Pin & pin;

    /// Channel number
    uint8_t index;
  };

  /// Namespace containing predefined Channel_t description objects. These
  /// objects can be passed directly to the constructor of an stm32f10x::Adc
  /// object.
  ///
  /// Usage:
  ///
  /// ```
  /// sjsu::stm32f10x::Adc adc(sjsu::stm32f10x::Adc::Channel::kChannel0);
  /// ```
  struct Channel  // NOLINT
  {
   private:
    inline static auto channel0_pin  = Pin('A', 0);
    inline static auto channel1_pin  = Pin('A', 1);
    inline static auto channel2_pin  = Pin('A', 2);
    inline static auto channel3_pin  = Pin('A', 3);
    inline static auto channel4_pin  = Pin('A', 4);
    inline static auto channel5_pin  = Pin('A', 5);
    inline static auto channel6_pin  = Pin('A', 6);
    inline static auto channel7_pin  = Pin('A', 7);
    inline static auto channel8_pin  = Pin('B', 0);
    inline static auto channel9_pin  = Pin('B', 1);
    inline static auto channel10_pin = Pin('C', 0);
    inline static auto channel11_pin = Pin('C', 1);
    inline static auto channel12_pin = Pin('C', 2);
    inline static auto channel13_pin = Pin('C', 3);
    inline static auto channel14_pin = Pin('C', 4);
    inline static auto channel15_pin = Pin('C', 5);

   public:
    /// Channel 0 definition.
    inline static const Channel_t kChannel0 = {
      .pin   = channel0_pin,
      .index = 0,
    };

    /// Channel 1 definition.
    inline static const Channel_t kChannel1 = {
      .pin   = channel1_pin,
      .index = 1,
    };

    /// Channel 2 definition.
    inline static const Channel_t kChannel2 = {
      .pin   = channel2_pin,
      .index = 2,
    };

    /// Channel 3 definition.
    inline static const Channel_t kChannel3 = {
      .pin   = channel3_pin,
      .index = 3,
    };

    /// Channel 4 definition.
    inline static const Channel_t kChannel4 = {
      .pin   = channel4_pin,
      .index = 4,
    };

    /// Channel 5 definition.
    inline static const Channel_t kChannel5 = {
      .pin   = channel5_pin,
      .index = 5,
    };

    /// Channel 6 definition.
    inline static const Channel_t kChannel6 = {
      .pin   = channel6_pin,
      .index = 6,
    };

    /// Channel 7 definition.
    inline static const Channel_t kChannel7 = {
      .pin   = channel7_pin,
      .index = 7,
    };

    /// Channel 8 definition.
    inline static const Channel_t kChannel8 = {
      .pin   = channel8_pin,
      .index = 8,
    };

    /// Channel 9 definition.
    inline static const Channel_t kChannel9 = {
      .pin   = channel9_pin,
      .index = 9,
    };

    /// Channel 10 definition.
    inline static const Channel_t kChannel10 = {
      .pin   = channel10_pin,
      .index = 10,
    };

    /// Channel 11 definition.
    inline static const Channel_t kChannel11 = {
      .pin   = channel11_pin,
      .index = 11,
    };

    /// Channel 12 definition.
    inline static const Channel_t kChannel12 = {
      .pin   = channel12_pin,
      .index = 12,
    };

    /// Channel 13 definition.
    inline static const Channel_t kChannel13 = {
      .pin   = channel13_pin,
      .index = 13,
    };

    /// Channel 14 definition.
    inline static const Channel_t kChannel14 = {
      .pin   = channel14_pin,
      .index = 14,
    };

    /// Channel 15 definition.
    inline static const Channel_t kChannel15 = {
      .pin   = channel15_pin,
      .index = 15,
    };
  };

  /// The default and highest frequency that the ADC can operate at.
  static constexpr units::frequency::hertz_t kClockFrequency = 1_MHz;

  /// A pointer holding the address to the stm32f10x ADC peripheral.
  /// This variable is a dependency injection point for unit testing thus it is
  /// public and mutable. This is needed to perform the "test by side effect"
  /// technique for this class.
  inline static ADC_TypeDef * adc1 = ADC1;

  /// DMA pointer
  inline static DMA_Channel_TypeDef * dma = DMA1_Channel1;

  /// RAM storage location for ADC conversion values
  inline static std::array<uint32_t, 16> adc_storage = {};

  /// @param channel - Passed channel descriptor object. See Channel_t and
  ///        Channel documentation for more details about how to use this.
  /// @param reference_voltage - Voltage applied to Vref (ADC reference voltage)
  explicit constexpr Adc(const Channel_t & channel,
                         units::voltage::microvolt_t reference_voltage = 3.3_V)
      : channel_(channel), kReferenceVoltage(reference_voltage)
  {
  }

  void ModuleInitialize() override
  {
    auto & system = sjsu::SystemController::GetPlatformController();

    // Step 0. Check if the peripheral is already powered on. If it is, then
    //         this peripheral has already been initialized and should not be
    //         initialized again.
    if (system.IsPeripheralPoweredUp(SystemController::Peripherals::kAdc1))
    {
      return;
    }

    // Step 1. Power on Peripherals
    system.PowerUpPeripheral(SystemController::Peripherals::kAdc1);
    system.PowerUpPeripheral(SystemController::Peripherals::kDma1);

    // Step 2. Set Power on ADC bit
    bit::Register(&adc1->CR2).Set(Control2::kAdcOn).Save();
    sjsu::Delay(1ms);

    // Step 3. Run Calibration on each peripheral
    CalibrateADC();

    // Step 4. Setup DMA for collecting ADC samples
    SetupDMA();

    // Step 5. Set ADC control registers and sequence registers
    adc1->SQR3 = RegularSequence::kRegisterSequence3;
    adc1->SQR2 = RegularSequence::kRegisterSequence2;
    adc1->SQR1 = RegularSequence::kRegisterSequence1;
    adc1->CR1  = Control1::kDefault;
    adc1->CR2  = Control2::kDefault;

    // Step 6. Turn on ADC
    bit::Register(&adc1->CR2).Set(Control2::kAdcOn).Save();

    // Step 7. Start the conversion process
    bit::Register(&adc1->CR2).Set(Control2::kSoftwareStart).Save();

    // Give some time for samples to be collected before leaving initialize
    Delay(1ms);
  }

  void ModuleEnable(bool enable = true) override
  {
    channel_.pin.Initialize();
    channel_.pin.ConfigureAsAnalogMode(enable);
    channel_.pin.Enable(enable);
  }

  uint32_t Read() override
  {
    return adc_storage[channel_.index] & 0xFFFF;
  }

  uint8_t GetActiveBits() override
  {
    return kActiveBits;
  }

  units::voltage::microvolt_t ReferenceVoltage() override
  {
    return kReferenceVoltage;
  }

 private:
  void CalibrateADC()
  {
    // Start calibration process
    bit::Register(&adc1->CR2).Set(Control2::kCalibration).Save();

    // Wait 1s for the calibration bit to go from 1 to zero. The hardware sets
    // the calibration bit to 0 when it is finished calibrating.
    while (bit::Register(&adc1->CR2).Read(Control2::kCalibration))
    {
      continue;
    }
  }

  void SetupDMA()
  {
    const auto kPeripheralDataAddress = reinterpret_cast<intptr_t>(&adc1->DR);
    const auto kRamAddress = reinterpret_cast<intptr_t>(adc_storage.data());

    dma->CNDTR = adc_storage.size();
    dma->CPAR  = static_cast<uint32_t>(kPeripheralDataAddress);
    dma->CMAR  = static_cast<uint32_t>(kRamAddress);

    // Set DMA Channel Settings
    dma->CCR = kDmaSettings;
  }

  const Channel_t & channel_;
  const units::voltage::microvolt_t kReferenceVoltage;
};
}  // namespace stm32f10x
}  // namespace sjsu
