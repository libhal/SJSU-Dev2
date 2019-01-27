#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/ssp.hpp"
#include "utility/log.hpp"

int main(void)
{
  constexpr uint8_t kBytesToRead = 4;
  constexpr uint8_t kReadDeviceId = 0x9F;
  constexpr uint8_t kPort1 = 1;
  constexpr uint8_t kPin10 = 10;
  uint8_t data[kBytesToRead];

  // This application sets up the SSP peripheral as a SPI master and will
  // interact with the external SPI flash memory chip on the SJ2 board.
  // Once the connection is established, the LPC master will request the
  // device ID from the flash memory. NOTE: the flash memory is connected
  // to SSP2. Device ID (part 1) = 40h; Manufacturer ID = 1Fh.
  LOG_INFO("SSP Application Starting...");

  // Select SSP1 peripheral
  LOG_INFO("Setting up peripheral pins: MOSI on Port 1.1.");
  LOG_INFO("MISO on Port 1.4, and SCK on Port 1.0.");
  Ssp spi2(Ssp::Peripheral::kSsp2);

  // Set SSP1 as SPI master. Note that the function SetSpiMasterDefault()
  // could be used instead of SetPeripheralMode() and SetClock().
  LOG_INFO("Set SSP1 as SPI master.");
  spi2.SetPeripheralMode(SspInterface::MasterSlaveMode::kMaster,
                             SspInterface::FrameMode::kSpi,
                             SspInterface::DataSize::kEight);

  // Set up SPI clock polarity and phase
  LOG_INFO("MOSI will read low when inactive.");
  LOG_INFO("SCK will read high when inactive.");
  spi2.SetClock(1, 1, 2, 0);

  // Initialize SSP
  LOG_INFO("SSP initialization");
  spi2.Initialize();

  // Set up chip select as GPIO pin
  Gpio chip_select(kPort1, kPin10);
  chip_select.SetAsOutput();
  chip_select.SetHigh();
  LOG_INFO("Chip select on Port 1.10 and initialized high.");

  // Get Device ID from flash memory
  LOG_INFO("Observe the logic analyzer for SPI interaction.");
  LOG_INFO("Sending command 9Fh to read device ID.");
  chip_select.SetLow();
  spi2.Transfer(kReadDeviceId);
  for (uint8_t i = 0; i < kBytesToRead; i++)
  {
    data[i] = static_cast<uint8_t>(spi2.Transfer(0));
  }
  chip_select.SetHigh();
  LOG_INFO("Device ID: 0x%.2x", data[1]);
  LOG_INFO("Manufacturer ID: 0x%.2x", data[0]);
}
