#include <cstdint>

#include "peripherals/lpc40xx/gpio.hpp"
#include "peripherals/lpc40xx/spi.hpp"
#include "devices/memory/sd.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

std::string_view long_text = R"(
Do you want to build a snowman?
Come on, let's go and play!
I never see you anymore
Come out the door
It's like you've gone away!
)";

std::string_view hello_world = "Hello World!";

namespace sjsu
{
class DualGpios : public sjsu::Gpio
{
 public:
  DualGpios(sjsu::Gpio & lead, sjsu::Gpio & follower)
      : lead_(lead), follower_(follower)
  {
  }

  void ModuleInitialize() override
  {
    lead_.Initialize();
    lead_.Initialize();
  }

  void SetDirection(Direction direction) override
  {
    lead_.SetDirection(direction);
    follower_.SetDirection(direction);
  }

  void Set(State output) override
  {
    lead_.Set(output);
    follower_.Set(output);
  }

  void Toggle() override
  {
    lead_.Toggle();
    follower_.Toggle();
  }

  bool Read() override
  {
    return lead_.Read();
  }

  sjsu::Pin & GetPin() override
  {
    return lead_.GetPin();
  }

  void AttachInterrupt(InterruptCallback callback, Edge edge) override
  {
    return lead_.AttachInterrupt(callback, edge);
  }

  void DetachInterrupt() override
  {
    return lead_.DetachInterrupt();
  }

 private:
  sjsu::Gpio & lead_;
  sjsu::Gpio & follower_;
};
}  // namespace sjsu

int main()
{
  sjsu::LogInfo("BEGIN SD Card Driver Example...");

  sjsu::lpc40xx::Spi & spi2                   = sjsu::lpc40xx::GetSpi<2>();
  sjsu::lpc40xx::Gpio & sd_card_detect        = sjsu::lpc40xx::GetGpio<1, 9>();
  sjsu::lpc40xx::Gpio & sd_chip_select_actual = sjsu::lpc40xx::GetGpio<1, 8>();
  sjsu::lpc40xx::Gpio & sd_chip_select_mirror = sjsu::lpc40xx::GetGpio<0, 6>();

  // This pin is not necessary for operation, but because the SD card's chip
  // select not accessable, it is useful for debugging the SD card to have an
  // extra GPIO that mirrors the proper SD Card chip select.
  sjsu::DualGpios sd_chip_select(sd_chip_select_actual, sd_chip_select_mirror);

  sjsu::Sd card(spi2, sd_chip_select, sd_card_detect);

  // Must always initialize the hardware before usage.
  card.Initialize();

  if (card.IsMediaPresent())
  {
    sjsu::LogInfo("Card is" SJ2_HI_BOLD_GREEN " present!");
  }
  else
  {
    sjsu::LogInfo("Card is" SJ2_HI_BOLD_RED " NOT present!");
    sjsu::LogInfo("Please insert an SD card and restart the system.");
    return -1;
  }

  sjsu::LogInfo("Mounting of SD Card was" SJ2_HI_BOLD_GREEN " successful!");

  sjsu::LogInfo("Gather SD Card information...");
  units::data::gigabyte_t capacity = card.GetCapacity();
  units::data::byte_t block_size   = card.GetBlockSize();
  bool is_read_only                = card.IsReadOnly();

  sjsu::LogInfo("SD Card Capacity   = " SJ2_HI_BOLD_GREEN "%f GB",
                capacity.to<double>());
  sjsu::LogInfo("SD Card Block Size = " SJ2_HI_BOLD_GREEN "%d bytes",
                block_size.to<int>());
  sjsu::LogInfo("Is Read Only       = " SJ2_HI_BOLD_BLUE "%d", is_read_only);

  // Beyond this point, information on the SD card will be overwritten
  sjsu::LogInfo("Deleting blocks");
  card.Erase(0, 1);
  sjsu::Delay(1s);

  sjsu::LogInfo("Writing Hello World to block 0");
  std::span<const uint8_t> payload(
      reinterpret_cast<const uint8_t *>(hello_world.data()),
      hello_world.size());

  card.Write(0, payload);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 0");
  std::array<uint8_t, 512> buffer;
  card.Read(0, buffer);
  sjsu::debug::Hexdump(buffer.data(), buffer.size());
  sjsu::Delay(1s);

  sjsu::LogInfo("Deleting block 5");
  card.Erase(5, 1);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after delete");
  card.Read(5, buffer);
  sjsu::debug::Hexdump(buffer.data(), buffer.size());

  sjsu::LogInfo("Writing to block 5 after delete");
  std::span<const uint8_t> long_text_span(
      reinterpret_cast<const uint8_t *>(long_text.data()), long_text.size());
  card.Write(5, long_text_span);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after write");
  card.Read(5, buffer);
  sjsu::debug::Hexdump(buffer.data(), buffer.size());

  sjsu::LogInfo("End SD Card Driver Example...");
  return 0;
}
