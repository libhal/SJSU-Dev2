#pragma once

#include <cstdint>
#include <utility>

#include "platforms/targets/stm32f10x/stm32f10x.h"
#include "peripherals/adc.hpp"
#include "peripherals/stm32f10x/dma.hpp"
#include "peripherals/stm32f10x/pin.hpp"
#include "peripherals/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/math/units.hpp"

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

  /// DMA settings for the ADC
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
  explicit constexpr Adc(const Channel_t & channel) : channel_(channel) {}

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

    static constexpr PinSettings_t kADCSettings = {
      .function   = 0,
      .resistor   = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    channel_.pin.settings = kADCSettings;
    channel_.pin.Initialize();
  }

  uint32_t Read() override
  {
    return adc_storage[channel_.index] & 0xFFFF;
  }

  uint8_t GetActiveBits() override
  {
    return kActiveBits;
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
};

template <int channel>
constexpr static auto GetPinBasedOnChannel()
{
  std::array<std::pair<int, int>, 16> pins = {
    std::make_pair('A', 0),  // channel 0
    std::make_pair('A', 1),  // channel 1
    std::make_pair('A', 2),  // channel 2
    std::make_pair('A', 3),  // channel 3
    std::make_pair('A', 4),  // channel 4
    std::make_pair('A', 5),  // channel 5
    std::make_pair('A', 6),  // channel 6
    std::make_pair('A', 7),  // channel 7
    std::make_pair('B', 0),  // channel 8
    std::make_pair('B', 1),  // channel 9
    std::make_pair('C', 0),  // channel 10
    std::make_pair('C', 1),  // channel 11
    std::make_pair('C', 2),  // channel 12
    std::make_pair('C', 3),  // channel 13
    std::make_pair('C', 4),  // channel 14
    std::make_pair('C', 5),  // channel 15
  };

  return pins[channel];
}

template <int channel>
inline Adc & GetAdc()
{
  static_assert(0 <= channel && channel <= 15,
                "\n\n"
                "SJSU-Dev2 Compile Time Error:\n"
                "    STM32F10x only supports ADC channels 0 to 15!\n"
                "\n");

  constexpr int bounded_channel = std::clamp(channel, 0, 15);

  // Get pin values based on the ADC channel number
  constexpr std::pair<int, int> pin_values =
      GetPinBasedOnChannel<bounded_channel>();
  // Get pin using GetPin to generate compile time error if map is out of
  // bounds.
  static Pin & channel_pin = GetPin<pin_values.first, pin_values.second>();

  // Create a static const ADC channel info object
  static const Adc::Channel_t kChannelInfo = {
    .pin   = channel_pin,
    .index = channel,
  };

  static Adc adc(kChannelInfo);
  return adc;
}
}  // namespace stm32f10x
}  // namespace sjsu
