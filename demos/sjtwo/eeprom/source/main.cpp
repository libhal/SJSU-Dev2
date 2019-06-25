#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L1_Peripheral/lpc40xx/eeprom.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  LOG_INFO("Starting EEPROM Example");
  size_t count = 128;
  uint32_t address = 0b0110'0011'1000;
  uint8_t * results = new uint8_t[count];

  LOG_INFO("Initializing EEPROM");
  sjsu::lpc40xx::Eeprom eeprom;
  eeprom.Initialize();

  // Generating Payload
  uint8_t * list = new uint8_t[count];

  for (size_t i = 0; i < count; i++)
  {
    list[i] = static_cast<uint8_t>(i*2);
  }

  while (true)
  {
    LOG_INFO("Starting Write");
    eeprom.Write(list, address, count);

    LOG_INFO("Starting Read");
    eeprom.Read(results, address, count);

    LOG_INFO("Results:");
    for (size_t i = 0; i < count; i++)
    {
      LOG_INFO("%i: %u", i, results[i]);
    }
    LOG_INFO("");

    sjsu::Delay(1000);
  }

  return 0;
}
