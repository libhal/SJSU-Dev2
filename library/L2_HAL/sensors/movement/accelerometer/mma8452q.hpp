#pragma once

#include <cstdint>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/memory_access_protocol.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/map.hpp"
#include "utility/math/limits.hpp"

namespace sjsu
{
/// Driver for the MMA8452Q 3-axis accelerometer
class Mma8452q : public Accelerometer
{
 public:
  static constexpr MemoryAccessProtocol::Specification_t<
      MemoryAccessProtocol::AddressWidth::kByte1,
      std::endian::big>
      kSpec{};

  /// Map of all of the used device addresses in this driver.
  struct Map  // NOLINT
  {
    /// Device status register address
    static constexpr auto kStatus =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x00, .width = 1 });

    /// Register address of the the first byte of the X axis
    static constexpr auto kXYZStartAddress =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x01, .width = 6 });

    /// Device ID register address
    static constexpr auto kWhoAmI =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x0D, .width = 1 });

    /// Device configuration starting address
    static constexpr auto kDataConfig =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x0E, .width = 1 });

    /// Control register 1 holds the enable bit
    static constexpr auto kControlReg1 =
        MemoryAccessProtocol::Address(kSpec, { .address = 0x2A, .width = 1 });

    static_assert(
        NoRegistersOverlap(
            { kStatus, kXYZStartAddress, kWhoAmI, kDataConfig, kControlReg1 }),
        "Memory map for MMA8452 is not valid. Register "
        "addresses/sizes overlap with each other");
  };

  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param address - Mma8452q device address.
  explicit constexpr Mma8452q(I2c & i2c, uint8_t address = 0x1c)
      : i2c_(i2c), i2c_memory_(address, i2c), memory_(i2c_memory_)
  {
  }

  explicit constexpr Mma8452q(MemoryAccessProtocol & external_map_protocol,
                              I2c & i2c,
                              uint8_t address = 0x1c)
      : i2c_(i2c), i2c_memory_(address, i2c), memory_(external_map_protocol)
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
      // Check that the device is valid before proceeding.
      IsValidDevice();

      // Activate device to allow full-scale and configuration to take effect.
      ActiveMode(true);
    }
    else
    {
      // Put device into standby so we can configure the device.
      ActiveMode(false);
    }
  }

  void ConfigureFullScale(
      units::acceleration::standard_gravity_t gravity) override
  {
    const uint32_t kGravityScale = gravity.to<uint32_t>();

    if (kGravityScale != 2 && kGravityScale != 4 && kGravityScale != 8)
    {
      throw Exception(std::errc::invalid_argument,
                      "Gravity scale must be 2g, 4g, or 8g.");
    }

    // Convert Gs to gravity scale code for the device.
    const uint8_t kNewGravityScale = static_cast<uint8_t>(kGravityScale >> 2);

    // Write gravity scale to memory
    memory_[Map::kDataConfig] = kNewGravityScale;

    // Set the full_scale_ value for use in Read()
    full_scale_ = gravity;
  }

  Acceleration_t Read() override
  {
    Acceleration_t acceleration   = {};
    std::array<int16_t, 3> result = memory_[Map::kXYZStartAddress];
    auto xyz_data                 = result;

    // First X-axis Byte (MSB first)
    // =========================================================================
    // Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0
    //  XD11 | XD10  |  XD9  |  XD8  |  XD7  |  XD6  |  XD5  |  XD4
    //
    // Final X-axis Byte (LSB)
    // =========================================================================
    // Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0
    //   XD3 |   XD2 |   XD1 |   XD0 |     0 |     0 |     0 |     0
    //
    // We simply shift and OR the bytes together to get them into a signed int
    // 16 value. We do not shift yet because we want to get the signed bit in
    // the most significant bit position to allow for sign extension when we
    // shift to the right later.

    int16_t x = static_cast<int16_t>(xyz_data[0] >> 4);
    int16_t y = static_cast<int16_t>(xyz_data[1] >> 4);
    int16_t z = static_cast<int16_t>(xyz_data[2] >> 4);

    // Convert the 12-bit signed value into a value from -1.0 to 1.0f so it can
    // be multiplied by the full_scale in order to get the true acceleration.
    constexpr int16_t kMin = sjsu::BitLimits<12, int16_t>::Min();
    constexpr int16_t kMax = sjsu::BitLimits<12, int16_t>::Max();

    float x_axis_ratio = sjsu::Map(x, kMin, kMax, -1.0f, 1.0f);
    float y_axis_ratio = sjsu::Map(y, kMin, kMax, -1.0f, 1.0f);
    float z_axis_ratio = sjsu::Map(z, kMin, kMax, -1.0f, 1.0f);

    acceleration.x = full_scale_ * x_axis_ratio;
    acceleration.y = full_scale_ * y_axis_ratio;
    acceleration.z = full_scale_ * z_axis_ratio;

    return acceleration;
  }

  void ActiveMode(bool is_active = true)
  {
    // Write enable sequence
    memory_[Map::kControlReg1] = is_active;
  }

  void IsValidDevice()
  {
    // Verify that the device is the correct device
    static constexpr uint8_t kExpectedDeviceID = 0x2A;

    // Read out the identity register
    uint8_t memory_id = memory_[Map::kWhoAmI];

    if (memory_id != kExpectedDeviceID)
    {
      LogDebug("ID = 0x%02X", memory_id);
      throw Exception(std::errc::no_such_device, "Expected Device ID: 0x2A.");
    }
  }

 private:
  I2c & i2c_;
  I2cProtocol<1> i2c_memory_;
  MemoryAccessProtocol & memory_;
  units::acceleration::standard_gravity_t full_scale_ = 2_SG;
};  // namespace sjsu
}  // namespace sjsu
