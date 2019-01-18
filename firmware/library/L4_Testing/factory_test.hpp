#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

class FactoryTest
{
 public:
  static constexpr uint8_t kGestureAddress       = 0x39;
  static constexpr uint8_t kAccelerometerAddress = 0x1C;
  static constexpr uint8_t kTemperatureAddress   = 0x31;

  void RunFactoryTest()
  {
    i2c_.Initialize();

    OledTest();
    bool gesture_pass       = GestureSensorTest();
    bool accelerometer_pass = AccelerometerTest();
    bool temp_pass          = TemperatureSensorTest();
    bool flash_test         = ExternalFlashTest();
    OnBoardLedTest(gesture_pass, accelerometer_pass, temp_pass, flash_test);
  }

 private:
  const char * BoolToSuccess(bool was_successful)
  {
    return (was_successful) ? "SUCCESS" : "FAILED";
  }
  void OledTest()
  {
    Ssd1306 display;

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
    Delay(1000);

    printf("  Inverting screen from black to white...\n\n");
    display.InvertScreenColor();

    printf(
        "Manually check that the screen is white with no black pixels...\n\n");
    printf(
        "If the screen looks very spotty, try this test 4 more times.\n\n");

    printf("End of OLED Hardware Test.\n\n");
  }

  bool ExternalFlashTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting External Flash Test...\n\n");
    Ssp ssp2(Ssp::Peripheral::kSsp2);
    Gpio cs(1, 10);
    bool result = false;
    cs.SetAsOutput();
    cs.SetHigh();

    ssp2.SetPeripheralMode(Ssp::MasterSlaveMode::kMaster, Ssp::FrameMode::kSpi,
                           Ssp::DataSize::kEight);
    ssp2.SetClock(false, false, 100, 48);
    ssp2.Initialize();

    uint8_t array[5];
    cs.SetLow();
    Delay(1);
    // Read Manufacturer ID
    ssp2.Transfer(0x9F);
    array[0] = static_cast<uint8_t>(ssp2.Transfer(0x00));
    array[1] = static_cast<uint8_t>(ssp2.Transfer(0x00));
    array[2] = static_cast<uint8_t>(ssp2.Transfer(0x00));
    array[3] = static_cast<uint8_t>(ssp2.Transfer(0x00));
    LOG_INFO("Returned 0x%02X 0x%02X 0x%02X 0x%02X", array[0], array[1],
             array[2], array[3]);
    cs.SetHigh();
    Delay(1);
    printf("End of External Flash Test.\n\n");
    if ((array[0] == 0x1F) && (array[1] == 0x40) && (array[2] == 0x00) &&
        (array[3] == 0x00))
    {
      result = true;
    }
    return result;
  }

  void OnBoardLedTest(bool gesture_test, bool accelerometer_test,
                      bool temp_test, bool flash_test)
  {
    // Turn on all LEDs if all tests pass else none.
    // LED Test
    OnBoardLed leds;
    leds.Initialize();
    if (gesture_test)
    {
      leds.Set(0);
    }
    if (accelerometer_test)
    {
      leds.Set(1);
    }
    if (temp_test)
    {
      leds.Set(2);
    }
    if (flash_test)
    {
      leds.Set(3);
    }

    printf("\n=========== TEST RESULTS ===========\n\n");
    printf("I2C: { \n");
    printf("  Gesture: '%s', \n", BoolToSuccess(gesture_test));
    printf("  Accelerometer: '%s',\n", BoolToSuccess(accelerometer_test));
    printf("  Temperature: '%s',\n", BoolToSuccess(temp_test));
    printf("}\n");
    printf("Flash: '%s'\n\n", BoolToSuccess(flash_test));
  }

  bool GestureSensorTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Gesture Sensor Test...\n\n");
    constexpr uint8_t kApds9960IdRegisterAddress = 0x92;
    bool result                                  = false;

    result = CheckDeviceId(kGestureAddress, kApds9960IdRegisterAddress, 0xAB);
    printf("End of Gesture Sensor Test...\n\n");

    return result;
  }

  bool AccelerometerTest()
  {
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Accelerometer Test...\n\n");
    constexpr uint8_t kMMAIdRegisterAddress = 0x0D;
    bool result                             = false;

    result = CheckDeviceId(kAccelerometerAddress, kMMAIdRegisterAddress, 0x2A);
    printf("End of Accelerometer Test...\n\n");

    return result;
  }

  bool TemperatureSensorTest()
  {
    // ID should be 0x01 [7:4]
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Temperature Test...\n\n");
    constexpr uint8_t kTemperatureIdAddress = 0xC0;
    bool id_test_result                     = false;

    id_test_result =
        CheckDeviceId(kTemperatureAddress, kTemperatureIdAddress, 0x14);

    int actual_temp       = CalculateTemperature(i2c_);
    bool temp_test_result = (-40 < actual_temp && actual_temp < 125);
    printf("  Temperature is: %u\n\n", actual_temp);
    printf("End of temperature test...\n\n");

    return (id_test_result && temp_test_result);
  }

  bool CheckDeviceId(uint8_t device_address, uint8_t id_register_address,
                     uint8_t expected_id)
  {
    uint8_t device_id = 0x00;
    i2c_.WriteThenRead(device_address, &id_register_address, 1, &device_id, 1);
    printf("  Device ID: 0x%02X\n\n", device_id);
    return (device_id == expected_id);
  }

  int CalculateTemperature(I2c temperature_i2c)
  {
    // Enables one burst mode (on-demand) temperature sampling
    // by setting control register 0xC4 oneburst bit 3 to 1
    // and stop bit 2 to 0. Stop bit is set to 1 upon successful
    // measurement.
    temperature_i2c.Write(kTemperatureAddress, { 0xC4, 0x04 });

    // Enable the auto increment of the i2c register address pointer
    // by setting control register 0xC5 to autoinc bit to 1
    temperature_i2c.Write(kTemperatureAddress, { 0xC5, 0x01 });
    // MSB, LSB for temperature data
    uint8_t temperature_ms = 0xC1;
    uint8_t temperature_ls = 0xC2;
    int temperature_data   = 0;
    temperature_i2c.WriteThenRead(kTemperatureAddress, &temperature_ms, 1,
                                  &temperature_ms, 1);
    temperature_i2c.Read(kTemperatureAddress, &temperature_ls, 1);

    temperature_data = ((temperature_ms & 0x7F) << 8) | temperature_ls;
    printf("  Temperature Data: 0x%02X, 0x%02X, 0x%04X\n\n", temperature_ms,
           temperature_ls, temperature_data);
    // Compute the actual temperature in Celsius
    return (55 + ((temperature_data - 16384) / 160));
  }

  I2c i2c_;
};
