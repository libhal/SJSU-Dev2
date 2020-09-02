#include <cstdint>

#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

const uint8_t kLongText[512] = R"(
Do you want to build a snowman?
Come on, let's go and play!
I never see you anymore
Come out the door
It's like you've gone away!
)";

const uint8_t kHelloWorld[] = "Hello World!";

namespace sjsu
{
class DualGpios : public sjsu::Gpio
{
 public:
  DualGpios(sjsu::Gpio & master, sjsu::Gpio & slave)
      : master_(master), slave_(slave)
  {
  }

  void SetDirection(Direction direction) const override
  {
    master_.SetDirection(direction);
    slave_.SetDirection(direction);
  }

  void Set(State output) const override
  {
    master_.Set(output);
    slave_.Set(output);
  }

  void Toggle() const override
  {
    master_.Toggle();
    slave_.Toggle();
  }

  bool Read() const override
  {
    return master_.Read();
  }

  const sjsu::Pin & GetPin() const override
  {
    return master_.GetPin();
  }

  Returns<void> AttachInterrupt(InterruptCallback callback, Edge edge) override
  {
    return master_.AttachInterrupt(callback, edge);
  }

  Returns<void> DetachInterrupt() const override
  {
    return master_.DetachInterrupt();
  }

 private:
  sjsu::Gpio & master_;
  sjsu::Gpio & slave_;
};
}  // namespace sjsu

int main()
{
  sjsu::LogInfo("BEGIN SD Card Driver Example...");

  sjsu::lpc40xx::Spi spi2(sjsu::lpc40xx::Spi::Bus::kSpi2);
  sjsu::lpc40xx::Gpio sd_chip_select_actual(1, 8);
  // This pin is not necessary for operation, but because the SD card's chip
  // select not accessable, it is useful for debugging the SD card to have an
  // extra GPIO that mirrors the proper SD Card chip select.
  sjsu::lpc40xx::Gpio sd_chip_select_mirror(0, 6);
  sjsu::DualGpios sd_chip_select(sd_chip_select_actual, sd_chip_select_mirror);
  sjsu::lpc40xx::Gpio sd_card_detect(1, 9);

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

  auto enable_status = card.Enable();
  if (!enable_status)
  {
    sjsu::LogError("Failed to mount SD Card!");
    return -2;
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
  SJ2_RETURN_VALUE_ON_ERROR(card.Erase(0, 1), -3);
  sjsu::Delay(1s);

  sjsu::LogInfo("Writing Hello World to block 0");
  SJ2_RETURN_VALUE_ON_ERROR(card.Write(0, kHelloWorld, sizeof(kHelloWorld)),
                            -4);
  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 0");
  uint8_t buffer[512];
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(0, buffer, sizeof(buffer)), -5);
  sjsu::debug::Hexdump(buffer, sizeof(buffer));
  sjsu::Delay(1s);

  sjsu::LogInfo("Deleting block 5");
  SJ2_RETURN_VALUE_ON_ERROR(card.Erase(5, 1), -6);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after delete");
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(5, buffer, sizeof(buffer)), -6);
  sjsu::debug::Hexdump(buffer, sizeof(buffer));

  sjsu::LogInfo("Writing to block 5 after delete");
  SJ2_RETURN_VALUE_ON_ERROR(card.Write(5, kLongText, sizeof(kLongText)), -7);

  sjsu::Delay(1s);

  sjsu::LogInfo("Reading block 5 after write");
  SJ2_RETURN_VALUE_ON_ERROR(card.Read(5, buffer, sizeof(buffer)), -8);
  sjsu::debug::Hexdump(buffer, sizeof(buffer));

  sjsu::LogInfo("End SD Card Driver Example...");
  return 0;
}
