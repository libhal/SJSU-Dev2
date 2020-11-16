#pragma once

#include <cstdint>
#include <limits>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/memory_access_protocol.hpp"
#include "L2_HAL/sensors/battery/battery_charge.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/map.hpp"

namespace sjsu
{
/// Driver for the max17043 battery fuel gauge
class Max170343 : public BatteryCharge
{
 public:
  static constexpr MemoryAccessProtocol::Specification_t<
      MemoryAccessProtocol::AddressWidth::kByte1,
      std::endian::big>
      kSpec{};
  /// Map of all of the used device addresses in this driver.
  /// All registers are 2 bytes and must be completely r/w for success
  struct Map  // NOLINT
  {
    /// 12 bit A/D measurement of battery voltage
    static constexpr auto kCellVoltage =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x02, .width = 2 });

    /// 16 bit State of charge result
    static constexpr auto kSoc =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x04, .width = 2 });

    /// Device Mode configuration, Send quick start command to the IC
    static constexpr auto kMode =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x06, .width = 2 });

    /// Config bytes, set sleep and alert threshold
    static constexpr auto kConfig =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x0C, .width = 2 });

    /// Command bytes allows power-on-reset
    static constexpr auto kCommand =
        MemoryAccessProtocol::Address(kSpec, { .address = 0xFE, .width = 2 });

    static_assert(
        NoRegistersOverlap({ kCellVoltage, kSoc, kMode, kConfig, kCommand }),
        "Memory map for MAX17043 is not valid. Register "
        "addresses/sizes overlap with each other");
  };

  /// Max17043 Constructor
  ///
  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param alert_pin - pin that the devices alert pin is connected to.
  /// @param callback - InterruptCallback that is used when the alert_pin goes
  /// low.
  /// @param address - max17043 device address.
  explicit constexpr Max170343(I2c & i2c,
                               sjsu::Gpio & alert_pin,
                               const sjsu::InterruptCallback & callback,
                               uint8_t address = 0b0110110)
      : i2c_(i2c),
        i2c_memory_(address, i2c),
        memory_(i2c_memory_),
        alert_pin_(alert_pin),
        callback_(callback)
  {
  }

  explicit constexpr Max170343(MemoryAccessProtocol & external_map_protocol,
                               I2c & i2c,
                               sjsu::Gpio & alert_pin,
                               sjsu::InterruptCallback & callback,
                               uint8_t address = 0b0110110)
      : i2c_(i2c),
        i2c_memory_(address, i2c),
        memory_(external_map_protocol),
        alert_pin_(alert_pin),
        callback_(callback)
  {
  }

  void ModuleInitialize() override
  {
    i2c_.Initialize();

    if (i2c_.RequiresConfiguration())
    {
      i2c_.ConfigureClockRate();
    }

    i2c_.Enable();
  }

  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      // Wake up the device from sleep mode
      ActiveMode(true);
      alert_pin_.SetAsInput();
      alert_pin_.GetPin().ConfigurePullDown();
      alert_pin_.AttachInterrupt(callback_, sjsu::Gpio::Edge::kFalling);
    }
    else
    {
      ActiveMode(false);
    }
  }

  float Read() override
  {
    std::array<int8_t, 2> soc_data = memory_[Map::kSoc];

    // First Byte(MSB first) contains the SOC at an integer resolution, Second
    // Byte contains resolution in 1/256%
    float charge_percent = ((static_cast<float>(soc_data[0]))) +
                           ((static_cast<float>(soc_data[1])) / 256);
    return charge_percent / 100.0f;
  }

  units::voltage::volt_t GetVoltage()
  {
    uint16_t volt_data = memory_[Map::kCellVoltage];
    volt_data          = volt_data >> 4;
    // voltage is a 12 bit number then represents the voltage when multiplied
    // by 1.25mV
    float multiplier = (static_cast<float>(volt_data)) * 0.00125f;

    units::voltage::volt_t voltage(multiplier);
    return voltage;
  }

  void ActiveMode(bool is_active = true)
  {
    std::array<int8_t, 2> config_register = memory_[Map::kConfig];
    auto sleep_mask                       = bit::MaskFromRange(8);

    // !is_active is required as the bit must be set to 0 in order to prevent it
    // from sleeping.
    config_register[1] =
        bit::Insert(config_register[1], !is_active, sleep_mask);

    // Write enable sequence
    memory_[Map::kConfig] = config_register;
  }

  void QuickStart()
  {
    /// restart fuel-gauge calculations in the same manner as initial power-up
    /// of the IC. Use if start up is noisy to reduce error.
    memory_[Map::kMode] = 0x4000;
  }

  void Reset()
  {
    /// Completely reset the max17043 as if power had been removed. does not
    /// i2c_ ACK
    memory_[Map::kCommand] = 0x5400;
  }

  void SetAlertThreshold(float threshold)
  {
    // Sets the threshold for when the IC alerts that the battery% is running
    // low by setting the alert byte to 1 and alert pin low.
    if (threshold > 0.32f)
      threshold = 0.32f;
    uint8_t threshold_code = 32 - static_cast<uint8_t>(threshold * 100);

    std::array<int8_t, 2> config_register = memory_[Map::kConfig];
    config_register[1] = bit::Insert(config_register[1], threshold_code, 0, 5);
    // Write the alert threshold
    memory_[Map::kConfig] = config_register;
  }

  void ResetAlert()
  {
    std::array<int8_t, 2> config_register = memory_[Map::kConfig];
    bit::Clear(config_register[1], 6);
    // Reset the low battery alert
    memory_[Map::kConfig] = config_register;
  }

 private:
  I2c & i2c_;
  I2cProtocol<1> i2c_memory_;
  MemoryAccessProtocol & memory_;
  sjsu::Gpio & alert_pin_;
  const InterruptCallback & callback_;
};
}  // namespace sjsu
