#pragma once

#include <cstdint>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"
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
  /// Map of all of the used device addresses in this driver.
  enum class RegisterMap : uint8_t
  {
    /// Device status register address
    kStatus = 0x00,

    /// Register address of the the first byte of the X axis
    kXYZStartAddress = 0x01,

    /// Device ID register address
    kWhoAmI = 0x0D,

    /// Device configuration starting address
    kDataConfig = 0x0E,

    /// Control register 1 holds the enable bit
    kControlReg1 = 0x2A,
  };

  ///
  ///
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
      : i2c_(i2c), kFullScale(full_scale), kAccelerometerAddress(address)
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
    constexpr uint16_t kBytesPerAxis = 2;
    constexpr uint8_t kNumberOfAxis  = 3;

    Acceleration_t acceleration = {};

    uint8_t xyz_data[kBytesPerAxis * kNumberOfAxis];

    SJ2_RETURN_ON_ERROR(i2c_.WriteThenRead(
        kAccelerometerAddress, { Value(RegisterMap::kXYZStartAddress) },
        xyz_data, sizeof(xyz_data)));

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
    int16_t x = static_cast<int16_t>(xyz_data[0] << 8 | xyz_data[1]);
    int16_t y = static_cast<int16_t>(xyz_data[2] << 8 | xyz_data[3]);
    int16_t z = static_cast<int16_t>(xyz_data[4] << 8 | xyz_data[5]);

    // Each axis is left shifted by 4 bits in order to place the signed bit in
    // the 16th bit position for the 16 bit integer. This allows us to right
    // shift to get rid of the right most zeros and to sign extend the value,
    // allowing us to return the correct value of the axis.
    x = static_cast<int16_t>(x >> 4);
    y = static_cast<int16_t>(y >> 4);
    z = static_cast<int16_t>(z >> 4);

    // Convert the 12-bit signed value into a value from -1.0 to 1.0f so it can
    // be multiplied by the kFullScale in order to get the true acceleration.
    constexpr int16_t kMin = sjsu::BitLimits<12, int16_t>::Min();
    constexpr int16_t kMax = sjsu::BitLimits<12, int16_t>::Max();

    float x_axis_ratio = sjsu::Map(x, kMin, kMax, -1.0f, 1.0f);
    float y_axis_ratio = sjsu::Map(y, kMin, kMax, -1.0f, 1.0f);
    float z_axis_ratio = sjsu::Map(z, kMin, kMax, -1.0f, 1.0f);

    sjsu::LogDebug("x = %d :: y = %d :: z = %d", x, y, z);

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

    uint8_t gravity_code = static_cast<uint8_t>(gravity_scale >> 2);
    SJ2_RETURN_ON_ERROR(
        i2c_.Write(kAccelerometerAddress,
                   { Value(RegisterMap::kDataConfig), gravity_code }));

    return {};
  }

  Returns<void> ActiveMode(bool is_active = true)
  {
    uint8_t state = is_active;

    // Write enable sequence
    SJ2_RETURN_ON_ERROR(i2c_.Write(
        kAccelerometerAddress, { Value(RegisterMap::kControlReg1), state }));

    return {};
  }

  Returns<void> IsValidDevice()
  {
    // Verify that the device is the correct device
    static constexpr uint8_t kExpectedDeviceID = 0x2A;

    uint8_t device_id = 0;

    // Read out the identity register
    SJ2_RETURN_ON_ERROR(i2c_.WriteThenRead(kAccelerometerAddress,
                                           { Value(RegisterMap::kWhoAmI) },
                                           &device_id, sizeof(device_id)));

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
};
}  // namespace sjsu
