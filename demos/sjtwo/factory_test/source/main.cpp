#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/boards/sjtwo.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"

namespace sjsu::lpc40xx
{
class FactoryTest
{
 public:
  static constexpr uint8_t kGestureAddress       = 0x39;
  static constexpr uint8_t kAccelerometerAddress = 0x1C;

  int RunFactoryTest()
  {
    i2c_.Initialize();
    i2c_.Enable();

    printf("\n=========== STARTING FACTORY TEST ===========\n\n");

    OledTest();

    bool gesture_pass       = GestureSensorTest();
    bool accelerometer_pass = AccelerometerTest();
    bool temp_pass          = TemperatureSensorTest();
    bool flash_test         = ExternalFlashTest();

    return OnBoardLedTest(
        gesture_pass, accelerometer_pass, temp_pass, flash_test);
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
    sjtwo::led0.Initialize();
    sjtwo::led1.Initialize();
    sjtwo::led2.Initialize();
    sjtwo::led3.Initialize();

    sjtwo::led0.Enable();
    sjtwo::led1.Enable();
    sjtwo::led2.Enable();
    sjtwo::led3.Enable();

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
    sjsu::Gpio & reset = sjsu::GetInactive<sjsu::Gpio>();

    Spi spi1(Spi::Bus::kSpi1);
    Ssd1306 display(spi1, cs, dc, reset);

    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting OLED Hardware Test...\n\n");

    printf("  Initializing OLED Hardware Test...\n\n");
    display.Initialize();

    printf("  Enabling OLED Hardware Test...\n\n");
    display.Enable();

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

    cs.SetAsOutput();
    cs.SetHigh();

    spi2.Initialize();
    spi2.ConfigureFrameSize(Spi::FrameSize::kEightBits);
    spi2.ConfigureFrequency(100_kHz);
    spi2.ConfigureClockMode();
    spi2.Enable();

    cs.SetLow();
    sjsu::Delay(1ms);

    // Write Manufacturer ID command and read back memory ID information
    std::array<uint8_t, 5> array = { 0x9F };
    spi2.Transfer(array);

    printf("Returned 0x%02X 0x%02X 0x%02X 0x%02X\n\n",
           array[1],
           array[2],
           array[3],
           array[4]);

    cs.SetHigh();
    sjsu::Delay(1ms);

    printf("End of External Flash Test.\n\n");

    if ((array[1] == 0x1F) && (array[2] != 0xFF) && (array[3] != 0xFF) &&
        (array[4] != 0xFF))
    {
      return true;
    }

    return false;
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
    accelerometer.Initialize();

    // Will check the ID and valid state of the device.
    accelerometer.Enable();

    printf("End of Accelerometer Test...\n\n");

    return true;
  }

  bool TemperatureSensorTest()
  {
    // ID should be 0x01 [7:4]
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Temperature Test...\n\n");

    // Possible addresses for the temperature sensor
    for (const auto & address : { 0x30, 0x31, 0x32, 0x33 })
    {
      try
      {
        printf("  Attempting I2C Address: 0x%02X\n", address);
        Si7060 temperature_sensor(i2c_, static_cast<uint8_t>(address));
        temperature_sensor.Initialize();

        printf("  -> Attached Sensor I2C Address: 0x%02X\n", address);

        auto temperature = temperature_sensor.GetTemperature();

        printf("  Current Temperature: %f C\n\n", temperature.to<double>());
        printf("End of temperature test...\n\n");

        if (-40_degC < temperature && temperature < 125_degC)
        {
          return true;
        }
      }
      catch (sjsu::Exception & e)
      {
        if (e.GetCode() == std::errc::no_such_device_or_address)
        {
          continue;
        }
      }
    }

    return false;
  }

  bool CheckDeviceId(uint8_t device_address,
                     uint8_t id_register_address,
                     uint8_t expected_id)
  {
    std::array<uint8_t, 1> readback_id = { 0x00 };
    std::array<uint8_t, 1> command     = { id_register_address };
    std::array<uint8_t, 1> expected    = { expected_id };

    i2c_.WriteThenRead(device_address, command, readback_id);
    printf("  Device ID: 0x%02X\n", readback_id[0]);

    return readback_id == expected;
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
