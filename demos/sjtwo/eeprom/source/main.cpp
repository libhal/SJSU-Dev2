#include <cstdint>
#include <numeric>

#include "peripherals/lpc40xx/eeprom.hpp"
#include "utility/log.hpp"

int main(void)
{
  sjsu::LogInfo("Starting EEPROM Example");

  constexpr size_t kPayloadSize = 128;
  constexpr uint32_t kAddress   = 0b0110'0011'1000;

  sjsu::LogInfo("Initializing & Enabling EEPROM");
  sjsu::lpc40xx::Eeprom & eeprom = sjsu::lpc40xx::GetEeprom<0>();
  eeprom.Initialize();

  // Payload array
  std::array<uint8_t, kPayloadSize> list;
  // Fill list with number starting from 1.
  std::iota(list.begin(), list.end(), 0);

  sjsu::LogInfo("Write to EEPROM...");
  eeprom.Write(kAddress, list);

  sjsu::LogInfo("Reading back from EEPROM...");
  std::array<uint8_t, kPayloadSize> read;
  eeprom.Read(kAddress, read);

  sjsu::LogInfo("Print out contents:");

  bool all_data_valid = true;
  for (size_t i = 0; i < list.size(); i++)
  {
    if (read[i] == i)
    {
      sjsu::LogInfo("Expected: %zu == Actual: %u", i, read[i]);
    }
    else
    {
      sjsu::LogError("Expected: %zu != Actual: %u ", i, read[i]);
      all_data_valid = false;
    }
  }

  if (all_data_valid)
  {
    sjsu::LogInfo("All of the data written to EEPROM was read by correctly!");
  }
  else
  {
    sjsu::LogInfo("An error occurred when reading back data from EEPROM.");
  }

  sjsu::LogInfo("Exiting! To restart demo, reset device.");

  return 0;
}
