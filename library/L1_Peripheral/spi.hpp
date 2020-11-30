#pragma once

#include <array>
#include <span>

#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for hardware that implements the Serial Peripheral
/// Interface (SPI) communication protocol.
/// @ingroup l1_peripheral
class Spi : public Module
{
 public:
  // ===========================================================================
  // Interface Definitions
  // ===========================================================================

  /// SPI Data Frame bitwidths
  enum class FrameSize : uint8_t
  {
    kFourBits = 0,  // The smallest standard frame sized allowed for SJSU-Dev2
    kFiveBits,
    kSixBits,
    kSevenBits,
    kEightBits,
    kNineBits,
    kTenBits,
    kElevenBits,
    kTwelveBits,
    kThirteenBits,
    kFourteenBits,
    kFifteenBits,
    kSixteenBits,  // The largest standard frame sized allowed for SJSU-Dev2
  };

  /// Determins the polarity of the SPI clock
  enum class Polarity : uint8_t
  {
    // Start the clock LOW then each cycle consists of a pulse of HIGH
    kIdleLow = 0,

    // Start the clock HIGH then each cycle consists of a pulse of LOW
    kIdleHigh,
  };

  /// Determins the phase of the SPI clock
  enum class Phase : uint8_t
  {
    // Data is valid on the LEADING edge of SPI clock
    kSampleLeading = 0,

    // Data is valid on the TRAILING edge of SPI clock
    kSampleTrailing,
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// Set the serial clock signal/sample frequency.
  ///
  /// @param frequency - the clock rate in hertz to set the serial clock to.
  /// Will attempt to get as close to the desired frequency as possible.
  ///
  /// @throw sjsu::Exception - if frequency is above what is possible for this
  /// SPI peripheral. May be an indication that the input clock frequency of the
  /// peripheral is not high enough to reach the desired frequency.
  virtual void ConfigureFrequency(units::frequency::hertz_t frequency) = 0;

  /// Set the behaviour/mode of the clock signal.
  ///
  /// @param polarity - clock polarity (see sjsu::Spi::Polarity for details)
  /// @param phase - clock phase (see sjsu::Spi::Phase for details)
  virtual void ConfigureClockMode(Polarity polarity = Polarity::kIdleLow,
                                  Phase phase = Phase::kSampleLeading) = 0;

  /// Set the serial clock signal/sample frequency as well as the polarity and
  /// phase characteristics of the clock and data.
  ///
  /// @param size - the number of bits the SPI data frame size should be. Note
  /// that many SPI implementations only support size 8 and size 16.
  ///
  /// @throw sjsu::Exception - if data size is not achievable with this SPI
  /// implementation.
  virtual void ConfigureFrameSize(FrameSize size = FrameSize::kEightBits) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// Write 8-bit data to the SPI bus and read back the data response on the
  /// bus.
  ///
  /// @param buffer - buffer of data to write to the spi bus. The contents of
  /// the buffer will be modified to the results of the response.
  virtual void Transfer(std::span<uint8_t> buffer) = 0;

  /// Write 16-bit data to the SPI bus and read back the data response on the
  /// bus.
  ///
  /// @param buffer - buffer of data to write to the spi bus. The contents of
  /// the buffer will be modified to the results of the response.
  virtual void Transfer(std::span<uint16_t> buffer) = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================
  void DefaultConfiguration()
  {
    // Use standard clock mode
    ConfigureClockMode();
    ConfigureFrameSize(FrameSize::kEightBits);
    ConfigureFrequency(100_kHz);
  }

  /// Transfer a single byte
  ///
  /// @param data - byte to send
  /// @return uint8_t - byte read back from bus
  uint8_t Transfer(uint8_t data)
  {
    std::array<uint8_t, 1> buffer = { data };
    Transfer(buffer);
    return buffer[0];
  }

  /// Transfer a 16-bit int
  ///
  /// @param data - 16-bit int to send
  /// @return uint16_t - byte read back from bus
  uint16_t Transfer(uint16_t data)
  {
    std::array<uint16_t, 1> buffer = { data };
    Transfer(buffer);
    return buffer[0];
  }

  /// Transfer a const array of data and receive an array back.
  /// This function should be used only in cases where the array to be
  /// transferred is const. This method must perform a copy of the data into a
  /// mutable array before performing the transfer. This is typically optimized
  /// away if the output of the method is not stored in a variable.
  ///
  /// Usage:
  ///
  ///    const std::array<uint8_t, 4> to_device = {1, 2, 3, 4};
  ///    auto from_device = spi.ConstTransfer(data);
  ///
  /// @tparam T - deduced data type of the array. Must be less than or equal to
  ///             uint16_t.
  /// @tparam length - deduced length of the array.
  ///
  /// @param data - the array to be sent via SPI.
  /// @return std::array<T, length> - the results of the tranfer. The result can
  ///         be ignored with little cost to the program. C++20 performs copy
  ///         ellision, preventing a memcpy from occuring when the result is
  ///         returned.
  template <typename T, size_t length>
  std::array<T, length> ConstTransfer(const std::array<T, length> & data)
  {
    // Compile time check that the datatype used is equal to or smaller than
    // datatype for Transfer. This will produce a better error message than the
    // generic template error message generated by the compiler.
    static_assert(sizeof(T) <= sizeof(uint16_t),
                  "Array datatype must be uint16_t or smaller.");

    // Create a mutable buffer with a copy of the const array data.
    std::array<T, length> buffer = data;

    // Transfer the data
    Transfer(buffer);

    // Return the data read back from the bus.
    return buffer;
  }
};

/// Template specialization that generates an inactive sjsu::Spi.
template <>
inline sjsu::Spi & GetInactive<sjsu::Spi>()
{
  class InactiveSpi : public sjsu::Spi
  {
   public:
    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}
    void ConfigureFrequency(units::frequency::hertz_t) override {}
    void ConfigureClockMode(Polarity = Polarity::kIdleLow,
                            Phase    = Phase::kSampleLeading) override
    {
    }
    void ConfigureFrameSize(FrameSize = FrameSize::kEightBits) override {}
    void Transfer(std::span<uint8_t>) override {}
    void Transfer(std::span<uint16_t>) override {}
  };

  static InactiveSpi inactive;
  return inactive;
}
}  // namespace sjsu
