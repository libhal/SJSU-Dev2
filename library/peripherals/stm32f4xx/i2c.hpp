#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "peripherals/cortex/interrupt.hpp"
#include "peripherals/i2c.hpp"
#include "peripherals/stm32f4xx/pin.hpp"
#include "peripherals/stm32f4xx/system_controller.hpp"
#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "utility/build_info.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"
#include "utility/time/timeout_timer.hpp"

namespace sjsu
{
namespace stm32f4xx
{
/// Implementation of the I2C peripheral for the stm32f4xx family of
/// microcontrollers.
class I2c final : public sjsu::I2c
{
 public:
  // Bringing in I2c Interface's Write and WriteThenRead methods that use
  // std::initializer_list.
  using sjsu::I2c::Read;
  using sjsu::I2c::Write;
  using sjsu::I2c::WriteThenRead;

  /// Control Register 1
  struct CR1
  {
    /// Enable I2C peripheral
    static constexpr auto kEnable = bit::MaskFromRange(0);
    /// Set to 1 to generate a START signal
    static constexpr auto kStart = bit::MaskFromRange(8);
    /// Set to 1 to generate a STOP signal
    static constexpr auto kStop = bit::MaskFromRange(9);
    /// Set to 1 to generate an acknowledge in master receiver mode
    static constexpr auto kAcknowledge = bit::MaskFromRange(10);
    /// Soft reset peripheral if it locks up
    static constexpr auto kReset = bit::MaskFromRange(15);
  };

  /// Status Register 1
  struct SR1
  {
    /// Detect timeout errors
    static constexpr auto kTimeout = bit::MaskFromRange(14);
    /// Detect acknowledge failures
    static constexpr auto kAcknowledgeFailure = bit::MaskFromRange(10);
    /// Detect when address abritation is lost for this device
    static constexpr auto kArbitrationLost = bit::MaskFromRange(9);
    /// Detect bus error (only used to clear the bit)
    static constexpr auto kBusError = bit::MaskFromRange(8);
    /// Transmit register is empty so DR register is available to be written to
    static constexpr auto kTxDataRegisterEmpty = bit::MaskFromRange(7);
    /// Receive register is NOT empty, if detected DR register has data to read
    static constexpr auto kRxDataRegisterNotEmpty = bit::MaskFromRange(6);
    /// Detect address has been sent
    static constexpr auto kAddressSent = bit::MaskFromRange(1);
    /// Detect start own signal
    static constexpr auto kStartSent = bit::MaskFromRange(0);
  };

  /// Status Register 2
  struct SR2
  {
    /// 1 if the I2C bus is busy
    static constexpr auto kBusy = bit::MaskFromRange(1);
    /// 1 if the I2C peripheral is in master mode
    static constexpr auto kIsMaster = bit::MaskFromRange(0);
  };

  /// Clock Control Register
  struct CCR
  {
    /// 0 = Std mode (<=100kHz), 1 = FM mode (<=400kHz)
    static constexpr auto kModeSelect = bit::MaskFromRange(15);
    /// 1:1 or 6:19 duty cycle
    static constexpr auto kDuty = bit::MaskFromRange(14);
    /// Clock divider
    static constexpr auto kClockControl = bit::MaskFromRange(0, 11);
  };

  /// Port_t holds all of the information for an I2C bus on the stm32f4xx
  /// platform.
  struct Port_t
  {
    /// Holds a pointer to the LPC_I2C peripheral registers
    I2C_TypeDef * registers;
    /// ResourceID of the I2C peripheral to power on at initialization.
    sjsu::SystemController::ResourceID id;
    /// Refernce to I2C data pin.
    sjsu::Pin & sda_pin;
    /// Refernce to I2C clock pin.
    sjsu::Pin & scl_pin;
  };

  /// Constructor for stm32f4xx I2c peripheral
  ///
  /// @param bus - pass a reference to a constant stm32f4xx::I2c::Port_t
  ///        definition.
  explicit I2c(const Port_t & bus) : i2c_(bus), timeout_(0ns) {}

  void ModuleInitialize() override
  {
    sjsu::SystemController::GetPlatformController().PowerUpPeripheral(i2c_.id);

    i2c_.sda_pin.settings.function   = 4;  // AF4
    i2c_.sda_pin.settings.open_drain = true;
    i2c_.sda_pin.settings.Floating();

    i2c_.scl_pin.settings.function   = 4;  // AF4
    i2c_.scl_pin.settings.open_drain = true;
    i2c_.scl_pin.settings.Floating();

    i2c_.sda_pin.Initialize();
    i2c_.scl_pin.Initialize();

    ConfigureClockRate();

    bit::Register(&i2c_.registers->CR1).Set(CR1::kEnable).Save();
  }

  void ModulePowerDown() override
  {
    // Disable I2C interface
    bit::Register(&i2c_.registers->CR1).Clear(CR1::kEnable).Save();
  }

  void Transaction(Transaction_t transaction) override
  {
    auto control                      = bit::Register(&i2c_.registers->CR1);
    auto status                       = bit::Register(&i2c_.registers->SR1);
    [[maybe_unused]] uint32_t status2 = 0;

    timeout_.SetNewTimeout(transaction.timeout);

    Start(transaction);

    // Step 7.a Stream out data to i2c bus if out_length > 0
    for (size_t i = 0; i < transaction.out_length; i++)
    {
      // Step 7.b. Wait for data to finish being sent
      while (!status.Read(SR1::kTxDataRegisterEmpty))
      {
        if (AcknowledgeFailure())
        {
          control.Set(CR1::kStop).Save();
          return;
        }
      }

      i2c_.registers->DR = transaction.data_out[i];
    }

    // Check if this transaction is a write-then-read operation
    if (transaction.repeated && transaction.in_length > 0)
    {
      transaction.operation = Operation::kRead;
      Start(transaction);
    }

    if (transaction.in_length > 0)
    {
      // Step 8. Read data from bus if in_length > 0
      for (size_t i = 0; i < transaction.in_length - 1; i++)
      {
        transaction.data_in[i] = GetData();
      }

      control.Clear(CR1::kAcknowledge).Set(CR1::kStop).Save();

      transaction.data_in[transaction.in_length - 1] = GetData();
    }
    else
    {
      control.Set(CR1::kStop).Save();
    }

    // Wait until the transaction is complete.
    return;
  }

  /// Special method to check if the bus is currently initialized.
  /// @returns true if this bus has been initialized.
  bool IsEnabled() const
  {
    return bit::Register(&i2c_.registers->CR1).Read(CR1::kEnable);
  }

  ~I2c()
  {
    ModulePowerDown();
  }

 private:
  void Start(Transaction_t & transaction)
  {
    auto control = bit::Register(&i2c_.registers->CR1);
    auto status  = bit::Register(&i2c_.registers->SR1);
    auto status2 = bit::Register(&i2c_.registers->SR2);

    [[maybe_unused]] uint32_t read_value = 0;

    // Step 1. Start the transaction
    control.Set(CR1::kStart).Set(CR1::kAcknowledge).Save();

    // Step 2. Wait for start sent
    while (!status.Read(SR1::kStartSent))
    {
      ThrowErrorIfPresent();
    }

    // Step X.
    while (!status2.Read(SR2::kBusy) && !status2.Read(SR2::kIsMaster))
    {
      ThrowErrorIfPresent();
    }

    // Step 3. Set data register to address
    i2c_.registers->DR = transaction.GetProperAddress();

    // Step 4. Wait for address to be sent
    while (!status.Read(SR1::kAddressSent))
    {
      ThrowErrorIfPresent();
      if (AcknowledgeFailure())
      {
        throw CommonErrors::kDeviceNotFound;
      }
    }

    read_value = i2c_.registers->SR2;
  }

  bool AcknowledgeFailure()
  {
    auto status  = bit::Register(&i2c_.registers->SR1);
    bool failure = status.Read(SR1::kAcknowledgeFailure);
    if (failure)
    {
      status.Clear(SR1::kAcknowledgeFailure).Save();
    }
    return failure;
  }

  void ThrowErrorIfPresent()
  {
    ThrowOnArbitrationLost();
    ThrowOnBusError();
    if (timeout_.HasExpired())
    {
      bit::Register(&i2c_.registers->CR1).Set(CR1::kReset).Save();
      throw CommonErrors::kTimeout;
    }
  }

  void ThrowOnBusError()
  {
    if (bit::Register(&i2c_.registers->SR1).Read(SR1::kBusError))
    {
      bit::Register(&i2c_.registers->SR1).Clear(SR1::kBusError);
    }
  }

  void ThrowOnArbitrationLost()
  {
    if (bit::Register(&i2c_.registers->SR1).Read(SR1::kArbitrationLost))
    {
      throw CommonErrors::kArbitrationLost;
    }
  }

  void ConfigureClockRate()
  {
    // Calculating and setting the I2C Clock rate
    // Weight the high side duty cycle more than the lower side by 30% in
    // order to give more time for the bus to charge up.
    const auto kPeripheralFrequency =
        sjsu::SystemController::GetPlatformController().GetClockRate(i2c_.id);

    if (kPeripheralFrequency <= 4_MHz)
    {
      throw I2c::CommonErrors::kClockRateNotPossible;
    }

    // Cut clock divider in half for HIGH and LOW time
    const float kClockDivider = kPeripheralFrequency / (settings.frequency * 3);

    bit::Register(&i2c_.registers->CCR)
        .Insert(0, CCR::kDuty)
        .Insert(1, CCR::kModeSelect)
        .Insert(static_cast<uint16_t>(kClockDivider), CCR::kClockControl)
        .Save();

    i2c_.registers->TRISE =
        static_cast<uint16_t>(((kPeripheralFrequency * 300) / 1'000_Hz) + 1);
  }

  uint8_t GetData()
  {
    auto status = bit::Register(&i2c_.registers->SR1);
    // Step 7.b. Wait for data to finish being sent
    while (!status.Read(SR1::kRxDataRegisterNotEmpty))
    {
      ThrowErrorIfPresent();
    }

    return static_cast<uint8_t>(i2c_.registers->DR);
  }

  const Port_t & i2c_;
  TimeoutTimer timeout_;
};

template <int port>
inline I2c & GetI2c()
{
  // UM10562: Chapter 7: LPC408x/407x I/O configuration page 13
  if constexpr (port == 1)
  {
    static auto & i2c1_sda_pin = GetPin<'B', 6>();
    static auto & i2c1_scl_pin = GetPin<'B', 7>();
    /// Definition for I2C bus 0 for stm32f4xx.
    static const I2c::Port_t kI2c1 = {
      .registers = I2C1,
      .id        = sjsu::stm32f4xx::SystemController::Peripherals::kI2c1,
      .sda_pin   = i2c1_sda_pin,
      .scl_pin   = i2c1_scl_pin,
    };

    static I2c i2c1(kI2c1);
    return i2c1;
  }
  else if constexpr (port == 2)
  {
    static auto & i2c2_sda_pin = GetPin<'B', 10>();
    static auto & i2c2_scl_pin = GetPin<'B', 11>();
    /// Definition for I2C bus 1 for stm32f4xx.
    static const I2c::Port_t kI2c2 = {
      .registers = I2C2,
      .id        = sjsu::stm32f4xx::SystemController::Peripherals::kI2c2,
      .sda_pin   = i2c2_sda_pin,
      .scl_pin   = i2c2_scl_pin,
    };

    static I2c i2c2(kI2c2);
    return i2c2;
  }
  else if constexpr (port == 3)
  {
    static auto & i2c3_sda_pin = GetPin<'A', 8>();
    static auto & i2c3_scl_pin = GetPin<'C', 9>();
    /// Definition for I2C bus 2 for stm32f4xx.
    static const I2c::Port_t kI2c3 = {
      .registers = I2C3,
      .id        = sjsu::stm32f4xx::SystemController::Peripherals::kI2c3,
      .sda_pin   = i2c3_sda_pin,
      .scl_pin   = i2c3_scl_pin,
    };

    static I2c i2c3(kI2c3);
    return i2c3;
  }
  else
  {
    static_assert(InvalidOption<port>,
                  "Support UART ports for stm32f4xx are I2C1, I2C2, and I2C3.");
    return GetI2c<1>();
  }
}
}  // namespace stm32f4xx
}  // namespace sjsu
