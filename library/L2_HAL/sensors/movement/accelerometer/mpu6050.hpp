#pragma once

#include <cstdint>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/sensors/movement/accelerometer.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/map.hpp"

namespace sjsu
{
/// Driver for the Mpu6050 3-axis accelerometer
class Mpu6050 : public Accelerometer
{
 public:
  /// Map of all of the used device addresses in this driver.
  enum class RegisterMap : uint8_t
  {
    /// Register address of the the first byte of the X axis
    kXYZStartAddress = 0x3B,

    /// Device ID register address
    kWhoAmI = 0x75,

    /// Device configuration starting address
    kDataConfig = 0x1C,

    /// Control register 1 holds the enable bit
    kControlReg1 = 0x6B,
  };

  ///
  /// MPU6050 Constructor
  ///
  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param full_scale - the specification maximum detectable acceleration.
  ///        Allowed values are 2g, 4g, 8g, and 16g, where g reprsents the
  ///        gravitational constent 9.8 m/s^2 (the user defined literals for
  ///        this are 2_SG, 4_SG, 8_SG, and 16_SG). Setting this to a larger max
  ///        acceleration results in being able to detect accelerations but
  ///        loses resolution because -16g to 16g must span 12 bits of
  ///        information. Where as 2g has a lower maximum detectable
  ///        acceleration, but 12 bits of information.
  /// @param address - Mpu6050 device address.
  explicit constexpr Mpu6050(
      const I2c & i2c,
      units::acceleration::standard_gravity_t full_scale = 2_SG,
      uint8_t address                                    = 0x68)
      : i2c_(i2c), kFullScale(full_scale), kAccelerometerAddress(address)
  {
  }

  Returns<void> Initialize() override
  {
    Status status = i2c_.Initialize();
    // TODO(#1244): Migrate to using auto return macro SJ2_RETURN_VALUE_ON_ERROR
    if (!IsOk(status))
    {
      return Error(Status::kBusError, "I2C Initialize Failed!");
    }

    return {};
  }

  /// Will automatically set the device to 2g for full-scale. Use
  /// `SetFullScaleRange()` to change it.
  Returns<void> Enable() override
  {
    // Check that the device is valid before proceeding.
    SJ2_RETURN_ON_ERROR(IsValidDevice());

    // Wake up the device so we can configure the device.
    SJ2_RETURN_ON_ERROR(ActiveMode(true));

    // Set device full-scale to the value supplied by the constructor.
    SJ2_RETURN_ON_ERROR(SetFullScaleRange());

    return {};
  }

  Returns<Acceleration_t> Read() override
  {
    constexpr uint16_t kBytesPerAxis = 2;
    constexpr uint8_t kNumberOfAxis  = 3;

    Acceleration_t acceleration = {};

    uint8_t xyz_data[kBytesPerAxis * kNumberOfAxis];

    i2c_.WriteThenRead(kAccelerometerAddress,
                       { Value(RegisterMap::kXYZStartAddress) }, xyz_data,
                       sizeof(xyz_data));

    // First X-axis Byte (MSB first)
    // =========================================================================
    // Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0
    //  XD15 | XD14  |  XD13 |  XD12 |  XD11 |  XD10 |  XD9  |  XD8
    //
    // Final X-axis Byte (LSB)
    // =========================================================================
    // Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0
    //   XD7 |   XD6 |   XD5 |   XD4 |   XD3 |   XD2 |   XD1 |   XD0
    //
    // We simply shift and OR the bytes together to get them into a signed int
    // 16 value.

    int16_t x = static_cast<int16_t>(xyz_data[0] << 8 | xyz_data[1]);
    int16_t y = static_cast<int16_t>(xyz_data[2] << 8 | xyz_data[3]);
    int16_t z = static_cast<int16_t>(xyz_data[4] << 8 | xyz_data[5]);

    // Convert the 16 bit value into a floating point value m/S^2
    constexpr int16_t kMax = std::numeric_limits<int16_t>::max();
    constexpr int16_t kMin = std::numeric_limits<int16_t>::min();

    float x_ratio = sjsu::Map(x, kMin, kMax, -1.0f, 1.0f);
    float y_ratio = sjsu::Map(y, kMin, kMax, -1.0f, 1.0f);
    float z_ratio = sjsu::Map(z, kMin, kMax, -1.0f, 1.0f);

    acceleration.x = kFullScale * x_ratio;
    acceleration.y = kFullScale * y_ratio;
    acceleration.z = kFullScale * z_ratio;

    return acceleration;
  }

  Returns<void> SetFullScaleRange()
  {
    uint32_t gravity_scale = kFullScale.to<uint32_t>();

    if (gravity_scale != 2 && gravity_scale != 4 && gravity_scale != 8 &&
        gravity_scale != 16)
    {
      return Error(Status::kInvalidParameters,
                   "Invalid gravity scale. Must be 2g, 4g, 8g or 16g.");
    }
    uint8_t gravity_code;
    if (gravity_scale == 2)
    {
      gravity_code = 0x00;
    }
    else if (gravity_scale == 4)
    {
      gravity_code = 0x01;
    }
    else if (gravity_scale == 8)
    {
      gravity_code = 0x02;
    }
    else if (gravity_scale == 16)
    {
      gravity_code = 0x03;
    }

    // Write in the full scale range; but leave the self test and high pass
    // filter config untouched
    uint8_t configRegister;
    i2c_.WriteThenRead(kAccelerometerAddress,
                       { Value(RegisterMap::kDataConfig) }, &configRegister, 1);
    auto scaleMask = bit::MaskFromRange(3, 4);
    configRegister = bit::Insert(configRegister, gravity_code, scaleMask);
    Status status =
        i2c_.Write(kAccelerometerAddress,
                   { Value(RegisterMap::kDataConfig), configRegister });

    // TODO(#1244): Migrate to using auto return macro SJ2_RETURN_VALUE_ON_ERROR
    if (!IsOk(status))
    {
      return Error(Status::kBusError, "I2C transaction failure");
    }

    return {};
  }

  Returns<void> ActiveMode(bool is_active = true)
  {
    uint8_t controlRegister;
    i2c_.WriteThenRead(kAccelerometerAddress,
                       { Value(RegisterMap::kDataConfig) }, &controlRegister,
                       1);
    auto sleepMask = bit::MaskFromRange(6);

    // !is_active is required as the bit must be set to 0 in order to prevent it
    // from sleeping.
    controlRegister = bit::Insert(controlRegister, !is_active, sleepMask);

    // Write enable sequence
    Status status =
        i2c_.Write(kAccelerometerAddress,
                   { Value(RegisterMap::kControlReg1), controlRegister });

    // TODO(#1244): Migrate to using auto return macro SJ2_RETURN_VALUE_ON_ERROR
    if (!IsOk(status))
    {
      return Error(Status::kBusError, "I2C transaction failure");
    }

    return {};
  }

  Returns<void> IsValidDevice()
  {
    // Verify that the device is the correct device
    static constexpr uint8_t kExpectedDeviceID = 0x68;

    uint8_t device_id = 0;

    // Read out the identity register
    Status status = i2c_.WriteThenRead(kAccelerometerAddress,
                                       { Value(RegisterMap::kWhoAmI) },
                                       &device_id, sizeof(device_id));

    // TODO(#1244): Migrate to using auto return macro SJ2_RETURN_VALUE_ON_ERROR
    if (!IsOk(status))
    {
      return Error(Status::kBusError, "I2C transaction failure");
    }

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
