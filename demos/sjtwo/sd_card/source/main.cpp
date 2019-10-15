#include <cstdint>

#include "L2_HAL/memory/sd.hpp"
#include "utility/debug.hpp"  // sjsu::debug::Hexdump()
#include "utility/log.hpp"
#include "utility/time.hpp"  // Halt()

const uint8_t kLongText[512] = R"(Lorem ipsum dolor sit amet, consectetur
adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna
aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi
ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in
voluptate velit es se cillum dolore eu fugiat nulla pariatur. Excepteur sint
occaecat cupidatat no n proident, sunt in culpa qui officia deserunt mollit
anim id est laborum.)";
const uint8_t kHelloWorld[512] = "Hello World!";

int main()
{
    LOG_INFO("BEGIN SD Card Driver Example...");

    sjsu::lpc40xx::Spi spi2(sjsu::lpc40xx::Spi::Bus::kSpi2);
    sjsu::lpc40xx::Gpio sd_cs(1, 8);

    sjsu::Sd card(spi2, sd_cs);
    sjsu::Sd::CardInfo_t sd_card_info;

    uint8_t buffer[512];
    card.Initialize();
    SJ2_ASSERT_FATAL(card.Mount(&sd_card_info), "Failed to mount SD Card");

    sjsu::Delay(1s);

    LOG_INFO("Deleting blocks");
    card.DeleteBlock(0x00000000, 0x0000A000);

    sjsu::Delay(1s);

    LOG_INFO("Writing Hello World to block 0 (0x00000000)");
    card.WriteBlock(0x00000000, kHelloWorld);

    sjsu::Delay(1s);

    LOG_INFO("Reading block 0 (0x00000000)");
    card.ReadBlock(0x00000000, buffer);
    sjsu::debug::Hexdump(buffer, sizeof(buffer));

    sjsu::Delay(1s);

    LOG_INFO("Deleting blocks again");
    card.DeleteBlock(0x00000000, 0x0000A000);

    sjsu::Delay(1s);

    LOG_INFO("Reading block 0 after delete (0x00000000)");
    card.ReadBlock(0x00000000, buffer);
    sjsu::debug::Hexdump(buffer, sizeof(buffer));

    LOG_INFO("Writing to block 1 after delete (0x00000200)");
    card.WriteBlock(0x00000200, kLongText);

    sjsu::Delay(1s);

    LOG_INFO("Reading block 1 after write (0x00000200)");
    card.ReadBlock(0x00000200, buffer);
    sjsu::debug::Hexdump(buffer, sizeof(buffer));

    LOG_INFO("END SD Card Driver Example...");
    sjsu::Halt();
    return 0;
}
