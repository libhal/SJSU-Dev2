#include <cstdint>
#include <iterator>

#include "L1_Peripheral/lpc40xx/eeprom.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  sjsu::LogInfo("Starting EEPROM Example");
  constexpr size_t kPayloadSize = 128;
  constexpr uint32_t kAddress   = 0b0110'0011'1000;

  sjsu::LogInfo("Initializing button");
  sjsu::lpc40xx::Gpio button_gpio3(0, 29);
  sjsu::Button button3(button_gpio3);
  button3.Initialize();

  sjsu::LogInfo("Initializing EEPROM");
  sjsu::lpc40xx::Eeprom eeprom;
  eeprom.Initialize();

  // Generating Payload
  std::array<uint8_t, kPayloadSize> list;

  for (size_t i = 0; i < list.size(); i++)
  {
    list[i] = static_cast<uint8_t>(i * 2);
  }

  while (true)
  {
    sjsu::LogInfo("Starting Write");
    eeprom.Write(kAddress, list.data(), kPayloadSize);

    sjsu::LogInfo("Starting Read");
    std::array<uint8_t, kPayloadSize> results;
    eeprom.Read(kAddress, results.data(), kPayloadSize);

    sjsu::LogInfo("Results:");
    for (size_t i = 0; i < list.size(); i++)
    {
      if (results[i] == i * 2)
      {
        sjsu::LogInfo("%i: %u", i, results[i]);
      }
      else
      {
        sjsu::LogInfo("%i: %u ERROR", i, results[i]);
      }
    }
    printf("\n");

    sjsu::LogInfo("Please press button 3 to restart demo");
    while (true)
    {
      if (button3.Pressed())
      {
        break;
      }
    }
  }

  return 0;
}
