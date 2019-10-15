#include <iterator>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "utility/log.hpp"

int main()
{
  constexpr uint8_t kBytesToRead  = 4;
  constexpr uint8_t kReadDeviceId = 0x9F;
  constexpr uint8_t kPort1        = 1;
  constexpr uint8_t kPin10        = 10;
  uint8_t data[kBytesToRead];

  // This application sets up the SPI peripheral as a SPI master and will
  // interact with the external SPI flash memory chip on the SJ2 board.
  // Once the connection is established, the LPC master will request the
  // device ID from the flash memory. NOTE: the flash memory is connected
  // to SPI2. Device ID (part 1) = 40h; Manufacturer ID = 1Fh.
  LOG_INFO("SPI Application Starting...");

  sjsu::lpc40xx::Spi spi2(sjsu::lpc40xx::Spi::Bus::kSpi2);
  // Initialize SPI
  LOG_INFO("SPI initialization");
  LOG_INFO(
      "Pins enabled for SPI2 are: MOSI on Port 1.1, MISO on Port 1.4, and SCK "
      "on Port 1.0.");
  spi2.Initialize();

  // Set SPI1 as SPI master. Note that the function SetSpiDefault()
  // could be used instead of SetPull() and SetClock().
  LOG_INFO("Set SPI1 as SPI master.");
  spi2.SetDataSize(sjsu::Spi::DataSize::kEight);

  // Set up SPI clock polarity and phase
  LOG_INFO("Set SPI2 clock.");
  spi2.SetClock(1_MHz);

  // Set up chip select as GPIO pin
  sjsu::lpc40xx::Gpio chip_select(kPort1, kPin10);
  chip_select.SetAsOutput();
  chip_select.SetHigh();
  LOG_INFO("Chip select on Port 1.10 and initialized high.");

  // Get Device ID from flash memory
  LOG_INFO("Observe the logic analyzer for SPI interaction.");
  LOG_INFO("Sending command 9Fh to read device ID.");
  chip_select.SetLow();
  spi2.Transfer(kReadDeviceId);
  for (uint8_t i = 0; i < std::size(data); i++)
  {
    data[i] = static_cast<uint8_t>(spi2.Transfer(0));
  }
  chip_select.SetHigh();

  LOG_INFO("Manufacturer ID: 0x%.2x", data[0]);
  LOG_INFO("Device ID: 0x%.2x", data[1]);
  LOG_INFO("Program halting...");
  sjsu::Halt();
  return 0;
}
