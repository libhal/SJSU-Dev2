#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "L2_HAL/boards/sjtwo.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu::lpc40xx
{
class FactoryTest
{
 public:
  static constexpr uint8_t kGestureAddress                    = 0x39;
  static constexpr uint8_t kAccelerometerAddress              = 0x1C;
  static constexpr std::array<uint8_t, 4> kTemperatureAddress = { 0x30, 0x31,
                                                                  0x32, 0x33 };

  int RunFactoryTest()
  {
    i2c_.Initialize();

    OledTest();

    bool gesture_pass       = GestureSensorTest();
    bool accelerometer_pass = AccelerometerTest();
    bool temp_pass          = TemperatureSensorTest();
    bool flash_test         = ExternalFlashTest();

    return OnBoardLedTest(gesture_pass, accelerometer_pass, temp_pass,
                          flash_test);
  }

 private:
  const char * BoolToSuccess(bool was_successful)
  {
    return (was_successful) ? "SUCCESS" : "FAILED";
  }

  int OnBoardLedTest(bool gesture_passed,
                     bool accel_passed,
                     bool temp_passed,
                     bool flash_passed)
  {
    // Turn on all LEDs if all tests pass else none.
    // LED Test
    sjtwo::led0.SetAsOutput();
    sjtwo::led1.SetAsOutput();
    sjtwo::led2.SetAsOutput();
    sjtwo::led3.SetAsOutput();

    sjtwo::led0.SetHigh();
    sjtwo::led1.SetHigh();
    sjtwo::led2.SetHigh();
    sjtwo::led3.SetHigh();

    if (gesture_passed)
    {
      sjtwo::led0.SetLow();
    }
    if (accel_passed)
    {
      sjtwo::led1.SetLow();
    }
    if (temp_passed)
    {
      sjtwo::led2.SetLow();
    }
    if (flash_passed)
    {
      sjtwo::led3.SetLow();
    }

    printf("\n=========== TEST RESULTS ===========\n\n");
    printf("Gesture:       '%s'\n", BoolToSuccess(gesture_passed));
    printf("Accelerometer: '%s'\n", BoolToSuccess(accel_passed));
    printf("Temperature:   '%s'\n", BoolToSuccess(temp_passed));
    printf("Flash:         '%s'\n", BoolToSuccess(flash_passed));

    // Sum together all of the tests that FAILED by inverting them with !. This
    // way, we get zero is they all passed and a negative value for the number
    // of tests that failed.
    return -(!gesture_passed + !accel_passed + !temp_passed + !flash_passed);
  }

  void OledTest()
  {
    sjsu::lpc40xx::Spi ssp1(sjsu::lpc40xx::Spi::Bus::kSpi1);
    sjsu::lpc40xx::Gpio cs(1, 22);
    sjsu::lpc40xx::Gpio dc(1, 25);

    Spi spi1(Spi::Bus::kSpi1);
    Ssd1306 display(spi1, cs, dc, sjsu::GetInactive<sjsu::Gpio>());

    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting OLED Hardware Test...\n\n");

    printf("  Initializing OLED Hardware Test...\n\n");
    display.Initialize();

    printf("  Filling internal screen bitmap...\n\n");
    display.Fill();

    printf("  Updating Screen...\n\n");
    display.Update();

    printf("  Clearing internal screen bitmap...\n\n");
    display.Clear();

    printf("  Updating Screen...\n\n");
    display.Update();
    sjsu::Delay(1s);

    printf("  Inverting screen from black to white...\n\n");
    display.InvertScreenColor();

    printf(
        "Manually check that the screen is white with no black pixels...\n\n");
    printf("If the screen looks very spotty, try this test 4 more times.\n\n");

    printf("End of OLED Hardware Test.\n\n");
  }

  bool ExternalFlashTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting External Flash Test...\n\n");
    Spi spi2(Spi::Bus::kSpi2);
    Gpio cs(1, 10);
    bool result = false;
    cs.SetAsOutput();
    cs.SetHigh();

    spi2.Initialize();
    spi2.SetDataSize(Spi::DataSize::kEight);
    spi2.SetClock(100_kHz);

    uint8_t array[5];
    cs.SetLow();
    sjsu::Delay(1ms);

    // Read Manufacturer ID
    spi2.Transfer(0x9F);

    // Read back flash memory ID information
    array[0] = static_cast<uint8_t>(spi2.Transfer(0x00));
    array[1] = static_cast<uint8_t>(spi2.Transfer(0x00));
    array[2] = static_cast<uint8_t>(spi2.Transfer(0x00));
    array[3] = static_cast<uint8_t>(spi2.Transfer(0x00));
    printf("Returned 0x%02X 0x%02X 0x%02X 0x%02X\n\n", array[0], array[1],
           array[2], array[3]);

    cs.SetHigh();
    sjsu::Delay(1ms);

    printf("End of External Flash Test.\n\n");

    if ((array[0] == 0x1F) && (array[1] != 0xFF) && (array[2] != 0xFF) &&
        (array[3] != 0xFF))
    {
      result = true;
    }
    return result;
  }

  // TODO(#1314): Use refactored apds9960.hpp library for this test
  bool GestureSensorTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Gesture Sensor Test...\n\n");
    constexpr uint8_t kIdAddress = 0x92;

    bool result1 = CheckDeviceId(kGestureAddress, kIdAddress, 0xA8);
    bool result2 = CheckDeviceId(kGestureAddress, kIdAddress, 0xAB);

    printf("End of Gesture Sensor Test...\n\n");

    return result1 || result2;
  }

  bool AccelerometerTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Accelerometer Test...\n\n");
    Mma8452q accelerometer(i2c_);

    // Verify that initialization of peripherals works
    SJ2_RETURN_VALUE_ON_ERROR(accelerometer.Initialize(), false);
    // Will check the ID and valid state of the device.
    SJ2_RETURN_VALUE_ON_ERROR(accelerometer.Enable(), false);

    printf("End of Accelerometer Test...\n\n");

    return true;
  }

  bool TemperatureSensorTest()
  {
    // ID should be 0x01 [7:4]
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Temperature Test...\n\n");

    for (auto address : kTemperatureAddress)
    {
      printf("  Attempting I2C Address: 0x%02X\n", address);
      Si7060 temperature_sensor(i2c_, address);
      auto status = temperature_sensor.Initialize();

      if (!status)
      {
        continue;
      }

      printf("  -> Attached Sensor I2C Address: 0x%02X\n", address);

      units::temperature::celsius_t current_temperature =
          SJ2_RETURN_VALUE_ON_ERROR(temperature_sensor.GetTemperature(), false);

      printf("  Current Temperature: %f C\n\n",
             current_temperature.to<double>());
      printf("End of temperature test...\n\n");

      if (-40_degC < current_temperature && current_temperature < 125_degC)
      {
        return true;
      }
    }

    return false;
  }

  bool CheckDeviceId(uint8_t device_address,
                     uint8_t id_register_address,
                     uint8_t expected_id)
  {
    uint8_t device_id = 0x00;
    i2c_.WriteThenRead(device_address, &id_register_address, 1, &device_id, 1);
    printf("  Device ID: 0x%02X\n\n", device_id);
    return (device_id == expected_id);
  }

 private:
  I2c i2c_ = I2c(I2c::Bus::kI2c2);
};
}  // namespace sjsu::lpc40xx

int main()
{
  sjsu::lpc40xx::FactoryTest ft;
  return ft.RunFactoryTest();
}
