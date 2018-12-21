#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L2_Utilities/log.hpp"
#include "L2_Utilities/status.hpp"
#include "L3_HAL/onboard_led.hpp"
#include "L3_HAL/ssd1306.hpp"

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
  void OledTest()
  {
    Ssd1306 display;
    LOG_INFO("Starting OLED Hardware Test...");

    LOG_INFO("Initializing OLED Hardware Test...");
    display.Initialize();

    LOG_INFO("Clearing Screen...");
    display.Clear();

    LOG_INFO("Clearing Screen finished.");

    display.Update();
    LOG_INFO("Inverting screen...");
    display.InvertScreenColor();
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
    LOG_INFO("End of External Flash Test.");
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
    printf("Test Results:\n\n");
    printf("\tI2C = { Gesture: %d; Accelerometer: %d; Temperature: %d; }\n",
           gesture_test, accelerometer_test, temp_test);
    printf("\tFlash:%d\n", flash_test);

    Delay(1000);
  }

  bool GestureSensorTest()
  {
    // Gesture Sensor Test
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Gesture Sensor Test...\n\n");
    constexpr uint8_t kApds9960IdRegisterAddress = 0x92;
    bool result                                  = false;

    result =
        CheckDeviceId(i2c_, kGestureAddress, kApds9960IdRegisterAddress, 0xAB);
    LOG_INFO("End of Gesture Sensor Test...");

    return result;
  }

  bool AccelerometerTest()
  {
    // Accelerometer Test
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Accelerometer Test...\n\n");
    constexpr uint8_t kMMAIdRegisterAddress = 0x0D;
    bool result                             = false;

    result =
        CheckDeviceId(i2c_, kAccelerometerAddress, kMMAIdRegisterAddress, 0x2A);
    LOG_INFO("End of Accelerometer Test...");

    return result;
  }

  bool TemperatureSensorTest()
  {
    // Temperature Test
    // ID should be 0x01 [7:4]
    printf("++++++++++++++++++++++++++++++++++++++\n\n");
    printf("Starting Temperature Test...\n\n");
    constexpr uint8_t kTemperatureIdAddress = 0xC0;
    bool id_test_result                     = false;

    id_test_result =
        CheckDeviceId(i2c_, kTemperatureAddress, kTemperatureIdAddress, 0x14);

    int actual_temp       = CalculateTemperature(i2c_);
    bool temp_test_result = (-40 < actual_temp && actual_temp < 125);
    LOG_INFO("Temperature is: %u", actual_temp);
    LOG_INFO("End of temperature test...");

    return (id_test_result && temp_test_result);
  }

  bool CheckDeviceId(I2c i2c_peripheral, uint8_t device_address,
                     uint8_t id_register_address, uint8_t expected_id)
  {
    uint8_t device_id = 0x00;
    i2c_peripheral.WriteThenRead(device_address, &id_register_address, 1,
                                 &device_id, 1);
    LOG_INFO("Device ID:%02X", device_id);
    return (device_id == expected_id);
  }

  int CalculateTemperature(I2c temperature_i2c)
  {
    uint8_t control_sequence[] = { 0xC4, 0x00, 0xC9, 0x01 };
    temperature_i2c.Write(kTemperatureAddress, control_sequence,
                          sizeof(control_sequence));

    // MSB, LSB for temperature data
    uint8_t temperature_ms = 0xC1;
    uint8_t temperature_ls = 0xC2;
    int temperature_data   = 0;
    temperature_i2c.WriteThenRead(kTemperatureAddress, &temperature_ms, 1,
                                  &temperature_ms, 1);
    temperature_i2c.WriteThenRead(kTemperatureAddress, &temperature_ls, 1,
                                  &temperature_ls, 1);

    temperature_data = ((temperature_ms & 0x7F) << 8) | temperature_ls;
    LOG_INFO("temperature_data is %02X%02X, %04X", temperature_ms,
             temperature_ls, temperature_data);
    // Compute the actual temperature in Celsius
    return (55 + ((temperature_data - 16384) / 160));
  }

  I2c i2c_;
};
