#pragma once

#include <cstdint>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"
#include "L2_HAL/device_memory_map.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/math/limits.hpp"
#include "utility/map.hpp"

namespace sjsu
{
/// Driver for the MMA8452Q 3-axis accelerometer
class Mma8452q : public Accelerometer
{
 public:
  static constexpr DeviceMemoryMap::MemorySpecification kSpec = {
    .address_width = DeviceMemoryMap::AddressWidth::Byte1,
    .endianness    = std::endian::big,
  };

  /// Map of all of the used device addresses in this driver.
  struct Map
  {
    /// Device status register address
    static constexpr auto kStatus =
        DeviceMemoryMap::Register<kSpec>({ .address = 0x00, .width = 1 });

    /// Register address of the the first byte of the X axis
    static constexpr auto kXYZStartAddress =
        DeviceMemoryMap::Register<kSpec>({ .address = 0x01, .width = 6 });

    /// Device ID register address
    static constexpr auto kWhoAmI =
        DeviceMemoryMap::Register<kSpec>({ .address = 0x0D, .width = 1 });

    /// Device configuration starting address
    static constexpr auto kDataConfig =
        DeviceMemoryMap::Register<kSpec>({ .address = 0x0E, .width = 1 });

    /// Control register 1 holds the enable bit
    static constexpr auto kControlReg1 =
        DeviceMemoryMap::Register<kSpec>({ .address = 0x2A, .width = 1 });

    // static_assert(NoRegistersOverlap(kSpec,
    //                                  { kStatus, kXYZStartAddress, kWhoAmI,
    //                                    kDataConfig, kControlReg1 }),
    //               "Memory map for MMA8452 is not valid. Register "
    //               "addresses/sizes overlap with each other");
  };

  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param full_scale - the specification maximum detectable acceleration.
  ///        Allowed values are 2g, 4g, and 8g, where g reprsents the
  ///        gravitational constent 9.8 m/s^2 (the user defined literals for
  ///        this are 2_SG, 4_SG, and 8_SG). Setting this to a larger max
  ///        acceleration results in being able to detect accelerations but
  ///        loses resolution because -8g to 8g must span 12 bits of
  ///        information. Where as 2g has a lower maximum detectable
  ///        acceleration, but 12 bits of information.
  /// @param address - Mma8452q device address.
  explicit constexpr Mma8452q(
      const I2c & i2c,
      units::acceleration::standard_gravity_t full_scale = 2_SG,
      uint8_t address                                    = 0x1c)
      : i2c_(i2c),
        kFullScale(full_scale),
        kAccelerometerAddress(address),
        i2c_memory_(kAccelerometerAddress, i2c),
        device_(i2c_memory_)
  {
  }

  explicit constexpr Mma8452q(
      DeviceMemoryMap & external_map_protocol,
      const I2c & i2c,
      units::acceleration::standard_gravity_t full_scale = 2_SG,
      uint8_t address                                    = 0x1c)
      : i2c_(i2c),
        kFullScale(full_scale),
        kAccelerometerAddress(address),
        i2c_memory_(kAccelerometerAddress, i2c),
        device_(external_map_protocol)
  {
  }

  Returns<void> Initialize() override
  {
    return i2c_.Initialize();
  }

  /// Will automatically set the device to 2g for full-scale. Use
  /// `SetFullScaleRange()` to change it.
  Returns<void> Enable() override
  {
    // Check that the device is valid before proceeding.
    SJ2_RETURN_ON_ERROR(IsValidDevice());

    // Put device into standby so we can configure the device.
    SJ2_RETURN_ON_ERROR(ActiveMode(false));

    // Set device full-scale to the value supplied by the constructor.
    SJ2_RETURN_ON_ERROR(SetFullScaleRange());

    // Activate device to allow full-scale and configuration to take effect.
    SJ2_RETURN_ON_ERROR(ActiveMode(true));

    return {};
  }

  Returns<Acceleration_t> Read() override
  {
    Acceleration_t acceleration     = {};
    std::array<int16_t, 3> xyz_data = device_[Map::kXYZStartAddress];

    // SJ2_CHECK_DEVICE_ERROR(device_);

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
    // be multiplied by the kFullScale in order to get the true acceleration.
    constexpr int16_t kMin = sjsu::BitLimits<12, int16_t>::Min();
    constexpr int16_t kMax = sjsu::BitLimits<12, int16_t>::Max();

    float x_axis_ratio = sjsu::Map(x, kMin, kMax, -1.0f, 1.0f);
    float y_axis_ratio = sjsu::Map(y, kMin, kMax, -1.0f, 1.0f);
    float z_axis_ratio = sjsu::Map(z, kMin, kMax, -1.0f, 1.0f);

    acceleration.x = kFullScale * x_axis_ratio;
    acceleration.y = kFullScale * y_axis_ratio;
    acceleration.z = kFullScale * z_axis_ratio;

    return acceleration;
  }

  Returns<void> SetFullScaleRange()
  {
    uint32_t gravity_scale = kFullScale.to<uint32_t>();

    if (gravity_scale != 2 && gravity_scale != 4 && gravity_scale != 8)
    {
      return Error(Status::kInvalidParameters,
                   "Invalid gravity scale. Must be 2g, 4g, or 8g.");
    }

    // device_[Map::kDataConfig] = static_cast<uint8_t>(gravity_scale >> 2);
    SJ2_CHECK_DEVICE_ERROR(device_);

    return {};
  }

  Returns<void> ActiveMode(bool is_active = true)
  {
    // Write enable sequence
    device_[Map::kControlReg1] = is_active;

    // Check and return any errors found by the device during the transaction
    SJ2_CHECK_DEVICE_ERROR(device_);

    return {};
  }

  Returns<void> IsValidDevice()
  {
    // Verify that the device is the correct device
    static constexpr uint8_t kExpectedDeviceID = 0x2A;

    // Read out the identity register
    uint8_t device_id = device_[Map::kWhoAmI];
    SJ2_CHECK_DEVICE_ERROR(device_);

    if (device_id != kExpectedDeviceID)
    {
      sjsu::LogDebug("device_id = 0x%02X", device_id);
      return Error(Status::kDeviceNotFound,
                   "Invalid device id from device, expected 0x2A.");
    }

    return {};
  }

 private:
  const I2c & i2c_;
  const units::acceleration::standard_gravity_t kFullScale;
  const uint8_t kAccelerometerAddress;
  I2cMemory<1> i2c_memory_;
  DeviceMemoryMap & device_;
};  // namespace sjsu
}  // namespace sjsu
