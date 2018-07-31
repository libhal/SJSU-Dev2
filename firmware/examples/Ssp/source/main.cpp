#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"
// #include "L1_Drivers/pin_configure.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L2_Utilities/debug_print.hpp"

int main(void)
{
    uint16_t data;
    // static constexpr read_id = 0x9F;
    // This application sets up the SSP peripheral as a SPI master and will
    // interact with the external SPI flash memory chip (AT25DN. Once the
    // connection is established, the LPC master will request the device ID
    // from the flash memory.
    Delay(5000);
    DEBUG_PRINT("SSP Application Starting...");

    // Select SSP1 peripheral
    Ssp ssp1_spi(SspInterface::Peripheral::kSsp1);
    DEBUG_PRINT("Setting up peripheral pins: MOSI on Port 0.9.");
    DEBUG_PRINT("MISO on Port 0.8, and SCK on Port 0.7.");
    // Enable SSP1 peripheral power
    ssp1_spi.SetPeripheralPower();
    DEBUG_PRINT("Set power for SSP1 peripheral");
    // Set SSP1 as SPI master
    ssp1_spi.SetPeripheralMode(SspInterface::MasterSlaveMode::kMaster,
                               SspInterface::FrameMode::kSpi,
                               SspInterface::DataSize::kEight);
    DEBUG_PRINT("Set SSP1 as SPI master.");
    // Set up SPI clock polarity and phase
    ssp1_spi.SetClock(1, 1, 2, 0);
    DEBUG_PRINT("SCK will read high when inactive.");
    DEBUG_PRINT("MOSI will read low when inactive.");
    // Initialize SSP
    DEBUG_PRINT("SSP initialization status: %d", ssp1_spi.Initialize());

    // Set up chip select as GPIO pin
    Gpio chip_select(0, 6);
    chip_select.SetAsOutput();
    chip_select.SetHigh();
    Delay(20);
    chip_select.SetLow();
    Delay(20);
    chip_select.SetHigh();
    Delay(20);
    DEBUG_PRINT("Chip select on Port 0.6 and initialized high.");

    // Get Device ID from flash memory, read id command = 0x9f
    DEBUG_PRINT("Observe the logic analyzer for SPI interaction.");
    DEBUG_PRINT("Sending command D7h to read status register.");
    chip_select.SetLow();
    ssp1_spi.Transfer(0xD7);
    data = ssp1_spi.Transfer(0);
    chip_select.SetHigh();
    DEBUG_PRINT("Flash Status Reg: 0x%.2x", data);
    DEBUG_PRINT("Ready/Busy Status: %d", (data >> 7));
    DEBUG_PRINT("Device ID: %.1x", ((data >> 2) & 0b1111));
}
