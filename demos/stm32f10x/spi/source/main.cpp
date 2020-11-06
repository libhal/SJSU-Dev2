#include "L1_Peripheral/stm32f10x/spi.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting SPI Application!");
  sjsu::LogInfo("To perform a loopback test connect MISO and MOSI together");
  sjsu::LogInfo("using a jumper. This will make the SPI response match the");
  sjsu::LogInfo("sent payload of \"Hello, World\"");
  sjsu::stm32f10x::Spi spi(sjsu::stm32f10x::Spi::Port::kSpi1);

  sjsu::LogInfo("SPI initialization...");
  spi.Initialize();

  sjsu::LogInfo("Set clock mode to the standard defaults for SPI...");
  spi.ConfigureClockMode();

  sjsu::LogInfo("Set frame size to 8-bits (standard)...");
  spi.ConfigureFrameSize(sjsu::Spi::FrameSize::kEightBits);

  // Set up SPI clock polarity and phase
  sjsu::LogInfo("Set clock frequency to 1 MHz...");
  spi.ConfigureFrequency(1_MHz);

  sjsu::LogInfo("Enabling SPI...");
  spi.Enable();

  std::array<uint8_t, 12> payload = { 'H', 'e', 'l', 'l', 'o', ' ',
                                      'W', 'o', 'r', 'l', 'd', '\0' };

  sjsu::LogInfo("Sending Payload: %s...", payload.data());
  spi.Transfer(payload);

  sjsu::LogInfo("Printing response!");
  sjsu::debug::Hexdump(payload.data(), payload.size());

  return 0;
}
