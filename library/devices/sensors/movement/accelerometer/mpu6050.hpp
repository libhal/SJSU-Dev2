#pragma once

#include <cstdint>
#include <limits>

#include "peripherals/i2c.hpp"
#include "devices/sensors/movement/accelerometer.hpp"
#include "utility/math/bit.hpp"
#include "utility/enum.hpp"
#include "utility/math/map.hpp"

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

  /// MPU6050 Constructor
  ///
  /// @param i2c - i2c peripheral used to commnicate with device.
  /// @param address - Mpu6050 device address.
  explicit constexpr Mpu6050(I2c & i2c, uint8_t address = 0x68)
      : i2c_(i2c), kAddress(address)
  {
  }

  void ModuleInitialize() override
  {
    i2c_.Initialize();

    // Check that the device is valid before proceeding.
    IsValidDevice();

    // Wake up the device so we can configure the device.
    ActiveMode(true);
  }

  /// Will automatically set the device to 2g for full-scale. Use
  /// `SetFullScaleRange()` to change it.
  void ModulePowerDown() override
  {
    // Put device into standby so we can configure the device.
    ActiveMode(false);
  }

  Acceleration_t Read() override
  {
    constexpr uint16_t kBytesPerAxis = 2;
    constexpr uint8_t kNumberOfAxis  = 3;

    Acceleration_t acceleration = {};

    uint8_t xyz_data[kBytesPerAxis * kNumberOfAxis];

    i2c_.WriteThenRead(kAddress,
                       { Value(RegisterMap::kXYZStartAddress) },
                       xyz_data,
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

    acceleration.x = CurrentSettings().gravity * x_ratio;
    acceleration.y = CurrentSettings().gravity * y_ratio;
    acceleration.z = CurrentSettings().gravity * z_ratio;

    return acceleration;
  }

 private:
  void ConfigureFullScale()
  {
    uint32_t gravity_scale = settings.gravity.to<uint32_t>();
    uint8_t gravity_code;

    switch (gravity_scale)
    {
      case 2: gravity_code = 0x00; break;
      case 4: gravity_code = 0x01; break;
      case 8: gravity_code = 0x02; break;
      case 16: gravity_code = 0x03; break;
      default:
        throw Exception(std::errc::invalid_argument,
                        "Invalid gravity scale. Must be 2g, 4g, 8g or 16g.");
    }

    // Write in the full scale range; but leave the self test and high pass
    // filter config untouched
    constexpr auto kScaleMask = bit::MaskFromRange(3, 4);
    uint8_t config;
    i2c_.WriteThenRead(
        kAddress, { Value(RegisterMap::kDataConfig) }, &config, 1);

    config = bit::Insert(config, gravity_code, kScaleMask);

    i2c_.Write(kAddress, { Value(RegisterMap::kDataConfig), config });
  }

  void ActiveMode(bool is_active = true)
  {
    constexpr auto kSleepMask = bit::MaskFromRange(6);

    uint8_t control;
    i2c_.WriteThenRead(
        kAddress, { Value(RegisterMap::kDataConfig) }, &control, 1);

    // !is_active is required as the bit must be set to 0 in order to prevent it
    // from sleeping.
    control = bit::Insert(control, !is_active, kSleepMask);

    // Write enable sequence
    i2c_.Write(kAddress, { Value(RegisterMap::kControlReg1), control });
  }

  void IsValidDevice()
  {
    // Verify that the device is the correct device
    static constexpr uint8_t kExpectedDeviceID = 0x68;

    uint8_t device_id = 0;

    // Read out the identity register
    i2c_.WriteThenRead(kAddress,
                       { Value(RegisterMap::kWhoAmI) },
                       &device_id,
                       sizeof(device_id));

    if (device_id != kExpectedDeviceID)
    {
      sjsu::LogDebug("device_id = 0x%02X", device_id);
      throw Exception(std::errc::no_such_device, "Expected Device ID: 0x2A");
    }
  }

  I2c & i2c_;
  const uint8_t kAddress;
};
}  // namespace sjsu
