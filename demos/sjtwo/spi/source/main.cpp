#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "utility/log.hpp"

int main()
{
  // This application sets up the SPI peripheral as a SPI master and will
  // interact with the external SPI flash memory chip on the SJ2 board.
  // Once the connection is established, the LPC master will request the
  // device ID from the flash memory. NOTE: the flash memory is connected
  // to SPI2. Device ID (part 1) = 40h; Manufacturer ID = 1Fh.
  sjsu::LogInfo("SPI Application Starting...");
  sjsu::LogInfo("Pins for SPI2 are: MOSI = P1[1], MISO = P1[4], SCK = P1[0]");

  sjsu::lpc40xx::Spi spi2(sjsu::lpc40xx::Spi::Bus::kSpi2);

  sjsu::LogInfo("SPI initialization");
  spi2.Initialize();

  sjsu::LogInfo("Set clock mode to the standard defaults for SPI.");
  spi2.ConfigureClockMode();

  sjsu::LogInfo("Set frame size to 8-bits (standard)");
  spi2.ConfigureFrameSize(sjsu::Spi::FrameSize::kEightBits);

  // Set up SPI clock polarity and phase
  sjsu::LogInfo("Set clock frequency to 1 MHz");
  spi2.ConfigureFrequency(1_MHz);

  sjsu::LogInfo("Enabling SPI2");
  spi2.Enable();

  // Set up chip select as GPIO pin
  sjsu::lpc40xx::Gpio chip_select(1, 10);
  chip_select.Initialize();
  chip_select.Enable();
  chip_select.SetAsOutput();
  chip_select.SetHigh();
  sjsu::LogInfo("Chip select on Port 1.10 and initialized high.");

  // Creating an array with 3 elements where the first is the command register
  // The Spi::Transfer() method will write the contents of the array to the bus
  // and will overwrite the array with the data received from bus.
  std::array<uint8_t, 3> transfer_buffer = { 0x9F };

  sjsu::LogInfo("Observe the logic analyzer for SPI interaction.");
  sjsu::LogInfo("Sending command 9Fh to read device ID.");

  // Assert chip select
  chip_select.SetLow();

  // Begin transfer of buffer.
  spi2.Transfer(transfer_buffer);

  // De-assert chip select
  chip_select.SetHigh();

  sjsu::LogInfo("Manufacturer ID: 0x%02x", transfer_buffer[1]);
  sjsu::LogInfo("      Device ID: 0x%02x", transfer_buffer[2]);

  return 0;
}
