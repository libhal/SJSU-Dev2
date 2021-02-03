#pragma once

#include <cstdint>
#include <limits>

#include "peripherals/gpio.hpp"
#include "peripherals/i2c.hpp"
#include "devices/memory_access_protocol.hpp"
#include "devices/sensors/battery/battery_charge.hpp"
#include "utility/math/bit.hpp"
#include "utility/enum.hpp"
#include "utility/math/map.hpp"

namespace sjsu
{
/// Driver for the max17043 battery fuel gauge
class Max170343 : public BatteryCharge
{
 public:
  /// Memory map specification for the Max170343
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

  /// Max17043 Constructor for debugging or custom external map protocol
  ///
  /// @param external_map_protocol - alternative map protocol.
  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param alert_pin - pin that the devices alert pin is connected to.
  /// @param callback - InterruptCallback that is used when the alert_pin goes
  /// low.
  /// @param address - max17043 device address.
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
    // Initialize using previously set settings
    i2c_.Initialize();

    // Wake up the device from sleep mode
    ActiveMode(true);
    alert_pin_.SetAsInput();
    alert_pin_.GetPin().settings.PullDown();
    alert_pin_.GetPin().Initialize();
    alert_pin_.AttachInterrupt(callback_, sjsu::Gpio::Edge::kFalling);
  }

  void ModulePowerDown() override
  {
    ActiveMode(false);
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

  /// @return units::voltage::volt_t - measured voltage of the battery
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

  /// Restart fuel-gauge calculations in the same manner as initial power-up
  /// of the IC. Use if start up is noisy to reduce error.
  void QuickStart()
  {
    memory_[Map::kMode] = 0x4000;
  }

  /// Completely reset the max17043 as if power had been removed. Device will
  /// not ACK the I2C transaction.
  void Reset()
  {
    memory_[Map::kCommand] = 0x5400;
  }

  /// Sets the threshold battery % for when the the ALERT signal is asserted
  /// (pulled LOW)
  ///
  /// @param threshold - Battery percentage to alert host about. Must be between
  /// 0.0f to 0.32f (presenting 0% and 32% respectively).
  void SetAlertThreshold(float threshold)
  {
    threshold              = std::clamp(threshold, 0.0f, 0.32f);
    uint8_t threshold_code = 32 - static_cast<uint8_t>(threshold * 100);

    std::array<int8_t, 2> config_register = memory_[Map::kConfig];
    config_register[1] = bit::Insert(config_register[1], threshold_code, 0, 5);
    // Write the alert threshold
    memory_[Map::kConfig] = config_register;
  }

  /// Clear alert signal
  void ResetAlert()
  {
    std::array<int8_t, 2> config_register = memory_[Map::kConfig];
    bit::Clear(config_register[1], 6);
    // Reset the low battery alert
    memory_[Map::kConfig] = config_register;
  }

 private:
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

  I2c & i2c_;
  I2cProtocol<1> i2c_memory_;
  MemoryAccessProtocol & memory_;
  sjsu::Gpio & alert_pin_;
  const InterruptCallback & callback_;
};
}  // namespace sjsu
